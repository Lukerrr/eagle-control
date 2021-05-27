#pragma once

#include "Singleton.h"
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
    bool Send(T msg);

private:
    void Reset();
    void Invalidate();

    bool TryConnect();
    bool SendInternal(char* pData, int len);

    SDroneState m_droneState;

    int m_gsSocket = -1;
    bool m_bConnected = false;
};

template<typename T>
bool CCommunicator::Send(T msg)
{
    int len = sizeof(T) + sizeof(EMsgType);
    char* buf = new char[len];
    memcpy(buf, &T::s_type, sizeof(EMsgType));
    memcpy(buf + sizeof(EMsgType), &msg, sizeof(T));
    bool bResult = SendInternal(buf, len);
    delete[] buf;
    return bResult;
}