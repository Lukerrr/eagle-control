#include "Core.h"
#include "Log.h"
#include "Crc32.h"
#include "Communicator.h"
#include "Configurator.h"

#include "InterfaceEvents.h"

#include <chrono>
#include <thread>

#define CORE_RATE_HZ 5.f

CCore::~CCore()
{
    if(m_pUi)
    {
        m_pUi->hide();
        delete m_pUi;
    }
}

bool CCore::Init()
{
    m_pUi = new QUserInterface();

    if(m_pUi)
    {
        m_pUi->show();
        m_delayMs = uint64_t(1000.0 / g_pConf->GetConfig().coreRate);
        m_bValid.store(true);

        CLog::Log(LOG_INFO, "CCore has been initialized");

        return true;
    }
    else
    {
        CLog::Log(LOG_ERROR, "CCore: cannot initialize UI");

        return false;
    }
}

void CCore::Run()
{
    CLog::Log(LOG_INFO, "CCore: entering working loop");

    bool bWasConnected = false;

    while(m_bValid.load() == true)
    {
        m_mutex.lock();
        bool bConnected = g_pComm->Update();
        if(bConnected ^ bWasConnected)
        {
            QCoreApplication::postEvent(m_pUi, new QConnectionEvent(bConnected));
            if(!bConnected)
            {
                QCoreApplication::postEvent(m_pUi, new QDroneStateEvent(SDroneState()));
                StopDownloadManager();
            }
            bWasConnected = bConnected;
        }

        if(bConnected && g_pComm->HasValidState())
        {
            SDroneState state = g_pComm->GetState();
            QCoreApplication::postEvent(m_pUi, new QDroneStateEvent(state));
        }
        m_mutex.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(m_delayMs));
    }
}

void CCore::Invalidate()
{
    m_bValid.store(false);
}

CDownloadManager* CCore::GetDownloadManager()
{
    return m_pDownloadManager;
}

void CCore::UpdateCloudPercent()
{
    if(m_pDownloadManager)
    {
        int percent = m_pDownloadManager->GetPercent();
        QCoreApplication::postEvent(m_pUi, new QGetCloudPercentEvent(percent));
    }
}

void CCore::StopDownloadManager()
{
    if(m_pDownloadManager)
    {
        delete m_pDownloadManager;
        m_pDownloadManager = NULL;
        QCoreApplication::postEvent(m_pUi, new QGetCloudStopEvent());
    }
}

void CCore::SetMissionPath(CLinePath2D path)
{
    m_mutex.lock();
    m_missionData.pathSize = path.GetSize();
    if(m_missionData.pathSize > m_missionData.pathMaxSize)
    {
        m_missionData.pathSize = m_missionData.pathMaxSize;
    }

    for(uint32_t i = 0; i < m_missionData.pathSize; ++i)
    {
        m_missionData.path[i].x = path.GetPoint(i).x;
        m_missionData.path[i].y = path.GetPoint(i).y;
    }

    if(m_missionData.pathSize > 0)
    {
        uint32_t len = m_missionData.pathSize * sizeof(SMissionData::Point);
        m_missionData.hash = GetCrc32((uint8_t*)m_missionData.path, len);
    }
    else
    {
        // Invalid mission
        m_missionData.hash = -1;
    }

    bool bWorking = g_pComm->GetState().systemState > ST_IDLE;
    QCoreApplication::postEvent(m_pUi, new QMissionChangedEvent(m_missionData, bWorking));
    m_mutex.unlock();
}

void CCore::RequestArmDisarm()
{
    m_mutex.lock();
    SDroneState state = g_pComm->GetState();
    if(!state.bArmed)
    {
        g_pComm->Send(SCmdArm());
    }
    else
    {
        g_pComm->Send(SCmdDisarm());
    }
    m_mutex.unlock();
}

void CCore::RequestStartStop()
{
    m_mutex.lock();
    SDroneState state = g_pComm->GetState();
    if(state.systemState == ST_IDLE)
    {
        if(state.missionHash == m_missionData.hash && m_missionData.hash != -1)
        {
            g_pComm->Send(SCmdStart());
            QCoreApplication::postEvent(m_pUi, new QMissionStartedEvent());
        }
    }
    else if(state.systemState > ST_IDLE)
    {
        g_pComm->Send(SCmdStop());
    }
    m_mutex.unlock();
}

void CCore::RequestSendMission()
{
    m_mutex.lock();
    SDroneState state = g_pComm->GetState();
    if(m_missionData.hash != -1 && state.systemState <= ST_IDLE)
    {
        g_pComm->Send(m_missionData);
    }
    m_mutex.unlock();
}

void CCore::RequestSendHeight(float height)
{
    m_mutex.lock();
    SCmdHeight cmd;
    cmd.height = height;
    g_pComm->Send(cmd);
    m_mutex.unlock();
}

void CCore::RequestSendTolerance(float tolerance)
{
    m_mutex.lock();
    SCmdTolerance cmd;
    cmd.tolerance = tolerance;
    g_pComm->Send(cmd);
    m_mutex.unlock();
}

void CCore::RequestSendDensity(float density)
{
    m_mutex.lock();
    SCmdDensity cmd;
    cmd.density = density;
    g_pComm->Send(cmd);
    m_mutex.unlock();
}

void CCore::RequestGetCloud(string fileName)
{
    m_mutex.lock();

    if(m_pDownloadManager)
    {
        // Already downloading
        return;
    }

    SDroneState state = g_pComm->GetState();
    if(state.cloudSize > 0)
    {
        m_pDownloadManager = new CDownloadManager(state.cloudSize, fileName);
        SCmdGetCloud cmd;
        g_pComm->Send(cmd);

        QCoreApplication::postEvent(m_pUi, new QGetCloudStartEvent());
    }
    m_mutex.unlock();
}

void CCore::RequestStopGetCloud()
{
    m_mutex.lock();
    StopDownloadManager();
    m_mutex.unlock();
}