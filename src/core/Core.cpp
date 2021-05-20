#include "Core.h"
#include "Log.h"
#include "Crc32.h"
#include "Communicator.h"
#include "Configurator.h"
#include "CmdMsgs.h"

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
        bool bConnected = g_pComm->Update();
        if(bConnected ^ bWasConnected)
        {
            QCoreApplication::postEvent(m_pUi, new QConnectionEvent(bConnected));
            if(!bConnected)
            {
                QCoreApplication::postEvent(m_pUi, new QDroneStateEvent(SDroneState()));
            }
            bWasConnected = bConnected;
        }

        if(bConnected)
        {
            m_mutex.lock();
            SDroneState state = g_pComm->GetState();
            QCoreApplication::postEvent(m_pUi, new QDroneStateEvent(state));

            // Dispatch command events from GUI
            if(m_eventsTriggers.m_bArmDisarm)
            {
                m_eventsTriggers.m_bArmDisarm = false;
                if(!state.bArmed)
                {
                    g_pComm->Send(SCmdArm());
                }
                else
                {
                    g_pComm->Send(SCmdDisarm());
                }
            }

            if(m_eventsTriggers.m_bStartStop)
            {
                m_eventsTriggers.m_bStartStop = false;
                if(state.systemState == SDroneState::ST_IDLE)
                {
                    if(state.missionHash == m_missionData.hash && m_missionData.hash != -1)
                    {
                        g_pComm->Send(SCmdStart());
                        QCoreApplication::postEvent(m_pUi, new QMissionStartedEvent());
                    }
                }
                else if(state.systemState > SDroneState::ST_IDLE)
                {
                    g_pComm->Send(SCmdStop());
                }
            }

            if(m_eventsTriggers.m_bSendMission)
            {
                m_eventsTriggers.m_bSendMission = false;
                if( m_missionData.hash != -1 && state.systemState <= SDroneState::ST_IDLE)
                {
                    g_pComm->SendMission(m_missionData);
                }
            }

            if(m_eventsTriggers.m_bSendHeight)
            {
                m_eventsTriggers.m_bSendHeight = false;
                SCmdHeight cmd;
                cmd.height = m_flightHeight;
                g_pComm->Send(cmd);
            }

            if(m_eventsTriggers.m_bSendTolerance)
            {
                m_eventsTriggers.m_bSendTolerance = false;
                SCmdTolerance cmd;
                cmd.tolerance = m_flightTolerance;
                g_pComm->Send(cmd);
            }
            m_mutex.unlock();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(m_delayMs));
    }
}

void CCore::Invalidate()
{
    m_bValid.store(false);
}

void CCore::SetMissionPath(CLinePath2D path)
{
    m_mutex.lock();
    m_missionData.path.clear();
    for(Vec2 pt : path.GetPoints())
    {
        m_missionData.path.push_back({pt.x, pt.y});
    }

    if(m_missionData.path.size() > 0)
    {
        uint32_t len = m_missionData.path.size() * sizeof(SMissionData::Point);
        uint8_t* buf = new uint8_t[len];
        memcpy(buf, &m_missionData.path[0], len);
        m_missionData.hash = GetCrc32(buf, len);
    }
    else
    {
        // Invalid mission
        m_missionData.hash = -1;
    }

    bool bWorking = g_pComm->GetState().systemState > SDroneState::ST_IDLE;
    QCoreApplication::postEvent(m_pUi, new QMissionChangedEvent(m_missionData, bWorking));
    m_mutex.unlock();
}

void CCore::SetFlightHeight(float height)
{
    if(height >= 1.5f && m_flightHeight != height)
    {
        m_flightHeight = height;
        QCoreApplication::postEvent(m_pUi, new QHeightChangedEvent());
    }
}

void CCore::SetFlightTolerance(float tolerance)
{
    if(tolerance > 0.f && m_flightTolerance != tolerance)
    {
        m_flightTolerance = tolerance;
        QCoreApplication::postEvent(m_pUi, new QToleranceChangedEvent());
    }
}

void CCore::RequestArmDisarm()
{
    m_mutex.lock();
    m_eventsTriggers.m_bArmDisarm = true;
    m_mutex.unlock();
}

void CCore::RequestStartStop()
{
    m_mutex.lock();
    m_eventsTriggers.m_bStartStop = true;
    m_mutex.unlock();
}

void CCore::RequestSendMission()
{
    m_mutex.lock();
    m_eventsTriggers.m_bSendMission = true;
    m_mutex.unlock();
}

void CCore::RequestSendHeight()
{
    m_mutex.lock();
    m_eventsTriggers.m_bSendHeight = true;
    m_mutex.unlock();
}

void CCore::RequestSendTolerance()
{
    m_mutex.lock();
    m_eventsTriggers.m_bSendTolerance = true;
    m_mutex.unlock();
}