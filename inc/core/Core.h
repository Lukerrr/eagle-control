#pragma once

#include "Singleton.h"
#include "Communicator.h"
#include "UserInterface.h"
#include <atomic>
#include <mutex>

using namespace std;

#define g_pCore (&CCore::Instance())

class CCore : public TSingleton<CCore>
{
    struct SEvents
    {
        bool m_bArmDisarm = false;
        bool m_bStartStop = false;
        bool m_bSendMission = false;
        bool m_bSendHeight = false;
        bool m_bSendTolerance = false;
        bool m_bGetCloud = false;
    };

public:
    ~CCore();

    bool Init();
    void Run();

    void Invalidate();

    void SetMissionPath(CLinePath2D path);
    void SetFlightHeight(float height);
    void SetFlightTolerance(float tolerance);

    void RequestArmDisarm();
    void RequestStartStop();
    void RequestSendMission();
    void RequestSendHeight();
    void RequestSendTolerance();
    void RequestGetCloud();

private:
    QUserInterface* m_pUi = NULL;
    uint64_t m_delayMs = 0;

    SMissionData m_missionData;
    float m_flightHeight;
    float m_flightTolerance;

    SEvents m_eventsTriggers;

    atomic_bool m_bValid;
    mutex m_mutex;
};