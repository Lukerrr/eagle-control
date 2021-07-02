#pragma once

#include "Singleton.h"
#include "TimeUtils.h"
#include "CmdMsgs.h"
#include "RspMsgs.h"

#include <string.h>
#include <vector>

#define g_pComm (&CCommunicator::Instance())

class CCommunicator : public TSingleton<CCommunicator>
{
    struct SRawPacket
    {
        ~SRawPacket();

        ERspType type = RSP_MAX;
        uint8_t* payload = NULL;
        size_t requiredSize = 0;
        size_t curSize = 0;
    };

public:
    CCommunicator();
    ~CCommunicator();

    bool Update();
    bool IsConnected();
    bool HasValidState();
    SDroneState GetState();

    template<typename T>
    void Send(T msg);

private:
    void Reset();
    void Invalidate();

    bool TryConnect();
    bool ConstructPacket();

    int RecvInternal(int socket, void* buf, size_t len);
    void SendInternal(char* pData, int len);

    SDroneState m_droneState;
    bool m_bStateValid = false;

    int m_gsSocket = -1;
    bool m_bConnected = false;
    TTime m_lastDataStamp = -1;

    SRawPacket m_curPacket;
};

template<typename T>
void CCommunicator::Send(T msg)
{
    int len = sizeof(T) + sizeof(EMsgType);
    char* buf = new char[len];
    memcpy(buf, &T::s_type, sizeof(EMsgType));
    memcpy(buf + sizeof(EMsgType), &msg, sizeof(T));
    SendInternal(buf, len);
    delete[] buf;
}