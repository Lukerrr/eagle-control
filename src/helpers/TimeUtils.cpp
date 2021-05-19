#include "TimeUtils.h"

#include <ctime>
#include <chrono>

using namespace std::chrono;

TTime Millis()
{
    system_clock::time_point ts = system_clock::now();
    long long millis = duration_cast<milliseconds>(ts.time_since_epoch()).count();

    return millis;
}