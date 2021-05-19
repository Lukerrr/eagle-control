#include "Log.h"
#include "Communicator.h"
#include "Configurator.h"

#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <unistd.h>

CCommunicator::CCommunicator()
{
    Reset();
}

CCommunicator::~CCommunicator()
{
    Invalidate();
}

void CCommunicator::Invalidate()
{
    CLog::Log(LOG_INFO, "CCommunicator: invalidating...");
    if(m_gsSocket != -1)
    {
        close(m_gsSocket);
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
        fcntl(m_gsSocket, F_SETFL, fcntl(m_gsSocket, F_GETFL, 0) | O_NONBLOCK);
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

    return m_bConnected;
}

bool CCommunicator::SendMission(SMissionData data)
{
    ECmdType type = CMD_MISSION;
    int pathSize = data.path.size();
    int pathDataLen = pathSize * sizeof(SMissionData::Point);

    int len =
    sizeof(type) +
    sizeof(pathSize) +
    pathDataLen +
    sizeof(data.hash);

    char* buf = new char[len];
    char* bufTmp = buf;

    memcpy(bufTmp, &type, sizeof(type));
    bufTmp += sizeof(type);

    memcpy(bufTmp, &pathSize, sizeof(pathSize));
    bufTmp += sizeof(pathSize);

    memcpy(bufTmp, &data.path[0], pathDataLen);
    bufTmp += pathDataLen;

    memcpy(bufTmp, &data.hash, sizeof(data.hash));
    
    bool bResult = SendInternal(buf, len);
    delete[] buf;
    return bResult;
}

bool CCommunicator::SendInternal(char* pData, int len)
{
    if (send(m_gsSocket, pData, len, 0) == -1)
    {
        CLog::Log(LOG_WARNING, "CCommunicator: cannot send a packet");
        Reset();
        return false;
    }
    return true;
}

bool CCommunicator::Update()
{
    if(!m_bConnected && !TryConnect())
    {
        return false;
    }

    // Read all received packets
    int dataLen = 0;
    int maxDataLen = g_pConf->GetConfig().packetLen;
    do
    {
        char *data = new char[maxDataLen];
        memset(data, 0, maxDataLen);
        dataLen = recv(m_gsSocket, data, maxDataLen, 0);

        if (dataLen == 0)
        {
            // Disconnected
            CLog::Log(LOG_INFO, "CCommunicator: disconnected");
            m_bConnected = false;
            m_droneState = SDroneState();
        }
        else if (dataLen > 0)
        {
            // Save drone state
            m_droneState = *((SDroneState*)data);
        }

        delete[] data;

    } while (dataLen > 0);

    return m_bConnected;
}

SDroneState CCommunicator::GetState()
{
    return m_droneState;
}