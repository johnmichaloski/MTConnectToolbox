
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

/**
C11 translation of boost date time stuff.
Both unscrutible.

double interval = ToNanoseconds(time_in - last_time) / 1E9; // in seconds

*/

#include <thread>
#include <chrono>
#include <sstream>
#include <ctime>
#include <string.h>  // strlen
#include <stdarg.h>
#include <assert.h>

namespace Timing
{

template<class Rep, class Period>
inline double ToNanoseconds (std::chrono::duration<Rep, Period> d)
{
    return static_cast<double>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(d).count( ) );
}
template<class Rep, class Period>
inline double ToSeconds (std::chrono::duration<Rep, Period> d)
{
    return static_cast<double>(
        std::chrono::duration_cast<std::chrono::seconds>(d).count( ) );
}

inline std::string StrFormat(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);

    int m;
    size_t n= strlen(format) + 1028;
    std::string tmp(n,'0');


    // Kind of a bogus way to insure that we don't
    // exceed the limit of our buffer
    while((m=_vsnprintf(&tmp[0], n-1, format, ap))<0)
    {
        n=n+1028;
        tmp.resize(n,'0');
    }
    va_end(ap);
    return tmp.substr(0,m);

}
inline  std::chrono::high_resolution_clock::time_point Now()
{
    return std::chrono::high_resolution_clock::now( );
}
//inline void Sleep(long milliseconds_to_sleep)
//{
//    // convert seconds to nanoseconds
//    double nanosec = milliseconds_to_sleep * 1E06;
//    boost::chrono::nanoseconds nanosleep( (long long) nanosec);
//    boost::this_thread::sleep_for(nanosleep);
//}
inline void esleep (double seconds_to_sleep)
{
    if ( seconds_to_sleep <= 0.0 )
    {
        return;
    }

    // convert seconds to nanoseconds
    double                   nanosec = seconds_to_sleep * 1E09;
    std::chrono::nanoseconds nanosleep((long long) nanosec);
    std::this_thread::sleep_for(nanosleep);
}
inline std::string ToString(std::chrono::high_resolution_clock::time_point tp)
{
    namespace pt = std::chrono;

    auto         ttime_t = pt::system_clock::to_time_t(tp);
    auto         tp_sec  = pt::system_clock::from_time_t(ttime_t);
    pt::milliseconds ms      = pt::duration_cast<pt::milliseconds>(tp - tp_sec);

    std::tm *ttm = localtime(&ttime_t);

    char date_time_format[] = "%Y/%m/%d-%H:%M:%S";

    char time_str[] = "yyyy/mm/dd HH:MM:SS.fff";

    strftime(time_str, strlen(time_str), date_time_format, ttm);

    std::string result(time_str);
    result.append(".");
    result.append(std::to_string(ms.count( )));
    return result;
}
/**
    * \brief Return string using  date and time in MS format .
    */
inline std::string GetDateTimeString(std::chrono::high_resolution_clock::time_point  ts)
{
    using namespace std;
    using namespace std::chrono;

    std::time_t t = std::chrono::high_resolution_clock::to_time_t(ts);
    struct tm * tmp = localtime(&t);
    long hours = ((int) tmp->tm_hour);
    long minutes = ((int)tmp->tm_min);
    long seconds = ((int)tmp->tm_sec);
    std::chrono::system_clock::duration tp = ts.time_since_epoch();
    tp -= std::chrono::duration_cast<std::chrono::seconds>(tp);
    long milliseconds = static_cast<long>(tp / std::chrono::milliseconds(1));
    long year = tmp->tm_year+1900;
    long month = tmp->tm_mon+1;
    long day = tmp->tm_mday;
    return StrFormat( "%4d-%02d-%02dT%02d:%02d:%02d.%3d", year, month, day, hours, minutes, seconds,milliseconds);
}
inline std::chrono::high_resolution_clock::time_point GetDateTime(std::string s)
{
    using namespace std::chrono;
    // parse 2012-02-03T17:31:51.0968Z
    int Year, Month, Day, Hour, Minute, Second, Millisecond=0;
    if(sscanf(s.c_str(), "%d-%d-%dT%d:%d:%d.%d", &Year, &Month, &Day, &Hour, &Minute,&Second, &Millisecond)==7){}
    else if(sscanf(s.c_str(), "%d-%d-%d %d:%d:%d.%d", &Year, &Month, &Day, &Hour, &Minute,&Second, &Millisecond)==7){}
    //12/2/2009 2:42:25 PM
    else if(sscanf(s.c_str(), "%d/%d/%4d%d:%d:%d", &Month, &Day, &Year,  &Hour, &Minute,&Second)==6){}
    else
    {
         throw std::runtime_error(StrFormat("Unrecognized date-time format -%s\n", s.c_str()));

    }
    // This gets the time
    std::tm tm{0};
    tm.tm_year = Year - 1900;
    tm.tm_mon = Month - 1;
    tm.tm_mday = Day;
    // Fill in the time
    tm.tm_hour = Hour;
    tm.tm_min = Minute;
    tm.tm_sec = Second;


    high_resolution_clock::time_point timePoint  = {};
    timePoint += std::chrono::seconds(std::mktime(&tm))+
            std::chrono::milliseconds(Millisecond);

    return timePoint;
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
