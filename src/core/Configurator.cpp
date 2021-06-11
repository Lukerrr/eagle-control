#include "Configurator.h"
#include "Log.h"

#include <map>
#include <fstream>
#include <sstream>

#define CONFIG_NAME "config.cfg"

using namespace std;

CConfigurator::CConfigurator()
{
    CLog::Log(LOG_INFO, "CConfigurator: initializing...");
    UpdateConfig();
}

bool CConfigurator::UpdateConfig()
{
    CLog::Log(LOG_INFO, "CConfigurator: updating config...");

    ifstream config(CONFIG_NAME);

    if (!config.is_open())
    {
        CLog::Log(LOG_ERROR, "CConfigurator: cannot open config file");
        return false;
    }

    string entry;
    map<string, string> entries;
    while(getline(config, entry))
    {
        string key, value;
        istringstream entryStream = istringstream(entry);
        getline(entryStream, key, '=');
        getline(entryStream, value, '=');
        entries[key] = value;

        CLog::Log(LOG_INFO, "CConfigurator: load '%s' = '%s'", key.c_str(), value.c_str());
    }

    CLog::Log(LOG_INFO, "CConfigurator was initialized with %d entries", entries.size());

    m_cfg = SConfig();

    entry = entries["core-rate-hz"];
    if(entry.size() > 0)
    {
        m_cfg.coreRate = atof(entry.c_str());
    }

    entry = entries["drone-ip"];
    if(entry.size() > 0)
    {
        m_cfg.droneIp = entry;
    }

    entry = entries["drone-port"];
    if(entry.size() > 0)
    {
        m_cfg.dronePort = atoi(entry.c_str());
    }

    entry = entries["auto-disconnect-time"];
    if(entry.size() > 0)
    {
        m_cfg.autoDisconnectTime = atoi(entry.c_str());
    }

    return true;
}

SConfig CConfigurator::GetConfig()
{
    return m_cfg;
}