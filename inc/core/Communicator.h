#pragma once

#include "Singleton.h"
#include "TimeUtils.h"
#include "CmdMsgs.h"
#include "RspMsgs.h"

#include <string.h>

#define g_pComm (&CCommunicator::Instance())

class CCommunicator : public TSingleton<CCommunicator>
{
public:
    CCommunicator();
    ~CCommunicator();

    bool Update();
    bool IsConnected();
    SDroneState GetState();

    template<typename T>
    void Send(T msg);

private:
    void Reset();
    void Invalidate();

    bool TryConnect();
    void Disconnect();

    int RecvInternal(int socket, void* buf, size_t len);
    void SendInternal(char* pData, int len);

    SDroneState m_droneState;

    int m_gsSocket = -1;
    bool m_bConnected = false;
    TTime m_lastDataStamp = -1;
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