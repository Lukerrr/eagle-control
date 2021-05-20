#include "Log.h"
#include "TimeUtils.h"

#include <mutex>
#include <sstream>
#include <fstream>

#define LOG_FILE_DIR    "logs/"
#define LOG_FILE_EXT    "txt"

#define LOG_FILE_NAME   "EagleLog"

using namespace std;

static const CLog s_log;
static mutex s_logMutex;

string GetTimestamp()
{
    TTime millis = Millis();
    time_t seconds = millis / 1000;

    tm* date = localtime(&seconds);

    if (!date)
    {
        return "00.00.00-00.00.00";
    }

    ostringstream timestamp;

    char timeBuf[32];
    strftime(timeBuf, 32, "%d.%m.%Y-%H.%M.%S", date);

    timestamp << timeBuf << "." << millis % 1000;

    return timestamp.str();
}

CLog::CLog()
{
    ostringstream filePath;
    filePath << LOG_FILE_DIR << LOG_FILE_NAME << "_" << GetTimestamp() << "" << "." << LOG_FILE_EXT;

    m_filePath = filePath.str();

#ifdef __linux__
    system("mkdir -p " LOG_FILE_DIR);
#elif _WIN32
    system("mkdir " LOG_FILE_DIR);
#endif
}

void CLog::LogInternal(ELogLevel level, const char* szInfo)
{
    s_logMutex.lock();

    FILE* pLogFile = fopen(s_log.m_filePath.c_str(), "a");

    if (pLogFile != NULL)
    {
        const char* levelPrefix = "";
        switch (level)
        {
        case LOG_INFO:
            levelPrefix = "INFO";
            break;
        case LOG_WARNING:
            levelPrefix = "WARNING";
            break;
        case LOG_ERROR:
            levelPrefix = "ERROR";
            break;
        default:
            break;
        }

        ostringstream data;
        data << "[" << GetTimestamp() << "] " << levelPrefix << ": " << szInfo << "\n";

        string dataStr = data.str().c_str();

        fwrite(dataStr.c_str(), sizeof(char), dataStr.size(), pLogFile);
        fclose(pLogFile);

#ifdef DEBUG_BUILD
        printf(dataStr.c_str());
#endif
    }
    s_logMutex.unlock();
}