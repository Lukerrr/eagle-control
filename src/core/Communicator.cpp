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

void CCommunicator::Disconnect()
{
    CLog::Log(LOG_INFO, "CCommunicator: disconnected");
    m_bConnected = false;
    m_droneState = SDroneState();
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
        CLog::Log(LOG_WARNING, "CCommunicator: requested receive more bytes than available (%lu > %lu)", len, availBytes);
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
    int dataLen;
    bool bNoData = true;
    do
    {
        ERspType msgType;
        dataLen = RecvInternal(m_gsSocket, (char*)&msgType, sizeof(msgType));

        if (dataLen == 0)
        {
            // Disconnected
            Disconnect();
            return false;
        }
        else if(dataLen > 0)
        {
            bNoData = false;
            switch(msgType)
            {
            case RSP_DRONE_STATE:
            {
                // Save drone state
                RecvInternal(m_gsSocket, (char*)&m_droneState, sizeof(m_droneState));
                break;
            }
            case RSP_CLOUD_CHUNK:
            {
                // Save point cloud chunk
                SPointCloud cloud;
                RecvInternal(m_gsSocket, (char*)&cloud, sizeof(cloud));
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
        }
    } while (dataLen > 0);

    if(bNoData)
    {
        if(Millis() - m_lastDataStamp > g_pConf->GetConfig().autoDisconnectTime)
        {
            CLog::Log(LOG_INFO, "CCommunicator: disconnecting on timeout...");
            Disconnect();
            return false;
        }
    }
    else
    {
        m_lastDataStamp = Millis();
    }

    return true;
}

bool CCommunicator::IsConnected()
{
    return m_bConnected;
}

SDroneState CCommunicator::GetState()
{
    return m_droneState;
}