#pragma once

#include "Singleton.h"
#include "Communicator.h"
#include "UserInterface.h"
#include "DownloadManager.h"
#include <atomic>
#include <mutex>

using namespace std;

#define g_pCore (&CCore::Instance())

class CCore : public TSingleton<CCore>
{
public:
    ~CCore();

    bool Init();
    void Run();

    void Invalidate();

    CDownloadManager* GetDownloadManager();
    void UpdateCloudPercent();
    void StopDownloadManager();

    void SetMissionPath(CLinePath2D path);

    void RequestArmDisarm();
    void RequestStartStop();
    void RequestSendMission();
    void RequestSendHeight(float height);
    void RequestSendTolerance(float tolerance);
    void RequestSendDensity(float density);
    void RequestGetCloud(string fileName);
    void RequestStopGetCloud();

private:
    QUserInterface* m_pUi = NULL;
    CDownloadManager* m_pDownloadManager = NULL;

    uint64_t m_delayMs = 0;

    SMissionData m_missionData;
    uint32_t m_missionHash = -1;

    atomic_bool m_bValid;
    mutex m_mutex;
};