#pragma once

#include <Singleton.h>
#include <string>

using namespace std;

#define g_pConf (&CConfigurator::Instance())

struct SConfig
{
    double coreRate = 5.f;
    string droneIp = "127.0.0.1";
    uint16_t dronePort = 54001;
    uint32_t packetLen = 1024;
};

class CConfigurator : public TSingleton<CConfigurator>
{
public:
    CConfigurator();
    
    bool UpdateConfig();
    SConfig GetConfig();

private:
    SConfig m_cfg;
};