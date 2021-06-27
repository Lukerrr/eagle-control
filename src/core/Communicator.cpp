#include "Log.h"
#include "Communicator.h"
#include "Configurator.h"

#ifdef __linux__
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#elif _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#endif

#include "Core.h"

size_t GetRspDataSize(ERspType type)
{
    switch(type)
    {
    case RSP_DRONE_STATE:       return sizeof(SRspDroneState);
    case RSP_CLOUD_CHUNK:       return sizeof(SRspCloudChunk);
    case RSP_CLOUD_END:         return sizeof(SRspCloudEnd);
    default:
        return 0;
    }
}

CCommunicator::CCommunicator()
{
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    Reset();
}

CCommunicator::~CCommunicator()
{
    Invalidate();
#ifdef _WIN32
    WSACleanup();
#endif
}

CCommunicator::SRawPacket::~SRawPacket()
{
    if(payload != NULL)
    {
        delete[] payload;
    }
}

void CCommunicator::Invalidate()
{
    CLog::Log(LOG_INFO, "CCommunicator: invalidating...");
    if(m_gsSocket != -1)
    {
#ifdef __linux__
        shutdown(m_gsSocket, SHUT_RDWR);
        close(m_gsSocket);
#elif _WIN32
        shutdown(m_gsSocket, SD_BOTH);
        closesocket(m_gsSocket);
#endif
        m_gsSocket = -1;
    }
    m_bConnected = false;
    m_droneState = SDroneState();
}

void CCommunicator::Reset()
{
    Invalidate();
    m_gsSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_gsSocket == -1)
    {
        CLog::Log(LOG_ERROR, "CCommunicator: cannot create a socket");
    }
    else
    {
        CLog::Log(LOG_INFO, "CCommunicator: socket created successfully");
    }
}

bool CCommunicator::TryConnect()
{
    SConfig cfg = g_pConf->GetConfig();

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(cfg.dronePort);
    inet_pton(AF_INET, cfg.droneIp.c_str(), &hint.sin_addr);

    m_bConnected = connect(m_gsSocket, (sockaddr*)&hint, sizeof(hint)) != -1;

    if(m_bConnected)
    {
        m_lastDataStamp = Millis();
    }

    return m_bConnected;
}

bool CCommunicator::ConstructPacket()
{
    if(m_curPacket.type == RSP_MAX)
    {
        // Begin construct packet
        int dataLen = RecvInternal(m_gsSocket, (char*)&m_curPacket.type, sizeof(m_curPacket.type));
        if(dataLen <= 0)
        {
            // No data or an error occured
            return false;
        }

        m_curPacket.requiredSize = GetRspDataSize(m_curPacket.type);
        if(m_curPacket.requiredSize == 0)
        {
            // No payload required
            return true;
        }
        m_curPacket.payload = new uint8_t[m_curPacket.requiredSize];
    }

    // Continue construct packet
    size_t remainSize = m_curPacket.requiredSize - m_curPacket.curSize;
    int dataLen = RecvInternal(m_gsSocket, (m_curPacket.payload + m_curPacket.curSize), remainSize);

    if(dataLen == -1)
    {
        // An error occured
        return false;
    }

    m_curPacket.curSize += dataLen;

    return m_curPacket.curSize == m_curPacket.requiredSize;
}

void CCommunicator::Disconnect()
{
    CLog::Log(LOG_INFO, "CCommunicator: disconnected");
    m_bConnected = false;
    m_droneState = SDroneState();
    m_bStateValid = false;
    m_curPacket = SRawPacket();
    Reset();
}

int CCommunicator::RecvInternal(int socket, void* buf, size_t len)
{
    u_long availBytes = 0;
#ifdef __linux__
    int ioctlRes = ioctl(socket, FIONREAD, &availBytes);
#elif _WIN32
    int ioctlRes = ioctlsocket(socket, FIONREAD, &availBytes);
#endif

    if(ioctlRes == -1 || availBytes == 0)
    {
        return -1;
    }

    if(len > availBytes)
    {
        len = availBytes;
    }

    return recv(socket, (char*)buf, len, 0);
}

void CCommunicator::SendInternal(char* pData, int len)
{
    if(!m_bConnected)
    {
        return;
    }

    if (send(m_gsSocket, pData, len, 0) == -1)
    {
        CLog::Log(LOG_WARNING, "CCommunicator: cannot send a packet");
        Reset();
    }
}

bool CCommunicator::Update()
{
    if(!m_bConnected && !TryConnect())
    {
        return false;
    }

    // Read all received packets
    while(ConstructPacket())
    {
        m_lastDataStamp = Millis();
        switch(m_curPacket.type)
        {
        case RSP_DRONE_STATE:
        {
            // Save drone state
            m_droneState = *(SDroneState*)m_curPacket.payload;
            if(!m_bStateValid)
            {
                m_bStateValid = true;
            }
            break;
        }
        case RSP_CLOUD_CHUNK:
        {
            // Save point cloud chunk
            SPointCloud cloud = *(SPointCloud*)m_curPacket.payload;
            CDownloadManager* pDownloadManager = g_pCore->GetDownloadManager();
            if(pDownloadManager)
            {
                pDownloadManager->AppendChunk(cloud);
                g_pCore->UpdateCloudPercent();
            }
            break;
        }
        case RSP_CLOUD_END:
        {
            // Stop saving cloud
            g_pCore->StopDownloadManager();
            break;
        }
        default:
            break;
        }
        m_curPacket = SRawPacket();
    }

    if(Millis() - m_lastDataStamp > g_pConf->GetConfig().autoDisconnectTime)
    {
        CLog::Log(LOG_INFO, "CCommunicator: disconnecting on timeout...");
        Disconnect();
        return false;
    }

    return true;
}

bool CCommunicator::IsConnected()
{
    return m_bConnected;
}

bool CCommunicator::HasValidState()
{
    return m_bStateValid;
}

SDroneState CCommunicator::GetState()
{
    return m_droneState;
}