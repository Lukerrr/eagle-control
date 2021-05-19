#pragma once

#include <stdio.h>
#include <string>

enum ELogLevel
{
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

class CLog
{
public:
    CLog();

    template<typename ... Args>
    static void Log(ELogLevel level, const char* szFormat, Args ... args);

private:
    static void LogInternal(ELogLevel level, const char* szInfo);

    std::string m_filePath;
};

template<typename ...Args>
void CLog::Log(ELogLevel level, const char* szFormat, Args ...args)
{
    if (szFormat == NULL)
    {
        return;
    }

    int size = snprintf(nullptr, size_t(0), szFormat, args ...) + 1;
    if (size > 0)
    {
        char* buf = new char[size];
        snprintf(buf, size, szFormat, args ...);
        LogInternal(level, buf);
        delete[] buf;
    }
}