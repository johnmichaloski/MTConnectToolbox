
//
// Timer.h
//

/*
DISCLAIMER:
This software was produced by the National Institute of Standards
and Technology (NIST), an agency of the U.S. government, and by statute is
not subject to copyright in the United States.  Recipients of this software
assume all responsibility associated with its operation, modification,
maintenance, and subsequent redistribution.

See NIST Administration Manual 4.09.07 b and Appendix I.
*/

#pragma once

#include <chrono>
#include <ctime>

#include <sstream>
#include <ctime>
#include <stdarg.h>
#include "globals.h"

using namespace Globals;

//https://stackoverflow.com/questions/15957805/extract-year-month-day-etc-from-stdchronotime-point-in-c
// http://www.informit.com/articles/article.aspx?p=1881386&seqNum=2

namespace Timing
{

// THis changes.
typedef std::chrono::system_clock::time_point MyTime;
typedef std::chrono::system_clock::duration MyDuration;

inline std::chrono::system_clock::time_point Now()
{
    return  std::chrono::system_clock::now();
}

/**
* \brief Converts the value of the current time point object to its equivalent date and time  with millisecond string representation.
* \param now C++11 std::chrono::system_clock::time_point
*/

inline std::string ToString(std::chrono::system_clock::time_point now)
{
    using namespace std::chrono;
    time_t tt = system_clock::to_time_t(now);
    system_clock::duration tp = now.time_since_epoch();
    tp -= duration_cast<seconds>(tp); // now only milliseconds
    tm t = *localtime(&tt);
    return StrFormat("T%02d:%02d:%02d.%03d",t.tm_hour, t.tm_min, t.tm_sec,
                     static_cast<unsigned>(tp / milliseconds(1)));
}
inline std::string GetDateTimeString(std::chrono::system_clock::time_point ts)
{
    using namespace std::chrono;
    time_t tt = system_clock::to_time_t(ts);
    system_clock::duration tp = ts.time_since_epoch();
    tp -= duration_cast<seconds>(tp); // now only milliseconds
    tm t = *localtime(&tt);
    return StrFormat("%4d-%02d-%02dT%02d:%02d:%02d.%3dZ", t.tm_year + 1900,
                     t.tm_mon +1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec,
                     static_cast<unsigned>(tp / milliseconds(1)));
}

inline std::chrono::system_clock::time_point GetDateTime(std::string s)
{
    using namespace std::chrono;
    //std::cout << "in time is: " << s.c_str() << "\n";

     int Year, Month, Day, Hour, Minute, Second, Millisecond=0;

    if(sscanf(s.c_str(), "%d-%d-%dT%d:%d:%d.%d", &Year, &Month, &Day, &Hour, &Minute,&Second, &Millisecond)==7){}
    //12/2/2009 2:42:25 PM
    else if(sscanf(s.c_str(), "%d/%d/%4d%d:%d:%d", &Month, &Day, &Year,  &Hour, &Minute,&Second)==6){}
    else throw std::runtime_error(StrFormat("Unrecognized date-time format -%s\n", s.c_str()));

    // years since 1900
    // months since January – [0, 11]
    std::tm tm = { Second, Minute, Hour-1, Day, Month-1, Year-1900, Day%7,0,0};

    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    tp=tp+milliseconds(Millisecond);
    //std::cout << "The time is:" << GetDateTimeString(tp) << "\n";
    return tp;
}


#ifndef WIN32
enum TimeFormat
{
    HUM_READ,
    GMT,
    GMT_UV_SEC,
    LOCAL
};

std::string GetTimeStamp (TimeFormat format)
{
    char            timeBuffer[50];
    struct tm *     timeinfo;
    struct timeval  tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);
    timeinfo = ( format == LOCAL ) ? localtime(&tv.tv_sec) : gmtime(&tv.tv_sec);

    switch ( format )
    {
    case HUM_READ:
    {
        strftime(timeBuffer, 50, "%a, %d %b %Y %H:%M:%S %Z", timeinfo);
    }
        break;

    case GMT:
    {
        strftime(timeBuffer, 50, "%Y-%m-%dT%H:%M:%SZ", timeinfo);
    }
        break;

    case GMT_UV_SEC:
    {
        strftime(timeBuffer, 50, "%Y-%m-%dT%H:%M:%S", timeinfo);
    }
        break;

    case LOCAL:
    {
        strftime(timeBuffer, 50, "%Y-%m-%dT%H:%M:%S%z", timeinfo);
    }
        break;
    }

    return std::string(timeBuffer);

}
#endif
}
