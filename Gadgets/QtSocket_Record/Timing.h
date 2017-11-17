
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
boost::posix_time::ptime pt = Timing::Now();
std::string ts0 = Timing::GetDateTimeString( pt);

std::string ts = Timing::ToString(pt);
boost::posix_time::ptime  pt1 = Timing::GetDateTime(ts);
std::string ts1 = Timing::ToString(pt1);
*/

#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include <ctime>
#include "StdStringFcn.h"

namespace Timing
{
inline boost::posix_time::ptime Now()
{
    return boost::posix_time::microsec_clock::local_time();;
}
inline void Sleep(long milliseconds_to_sleep)
{
    // convert seconds to nanoseconds
    double nanosec = milliseconds_to_sleep * 1E06;
    boost::chrono::nanoseconds nanosleep( (long long) nanosec);
    boost::this_thread::sleep_for(nanosleep);
}
inline std::string ToString(boost::posix_time::ptime ts)
{
    namespace pt = boost::posix_time;
    std::stringstream msg;
    pt::time_facet*const f = new pt::time_facet("%Y-%m-%d");
    msg.imbue(std::locale(msg.getloc(),f));
    long hours = ((int)ts.time_of_day().hours());
    long minutes = ((int)ts.time_of_day().minutes());
    long seconds = ((int)ts.time_of_day().seconds());
    long milliseconds = ((int)ts.time_of_day().total_milliseconds())%1000;

    msg << ts << StrFormat("T%02d:%02d:%02d.%03d",hours, minutes, seconds, milliseconds);
    return msg.str();
}
/**
    * \brief Return string using  date and time in MS format .
    */
inline std::string GetDateTimeString(boost::posix_time::ptime ts)
{
    boost::gregorian::date d = ts.date();
    long hours = ((int)ts.time_of_day().hours());
    long minutes = ((int)ts.time_of_day().minutes());
    long seconds = ((int)ts.time_of_day().seconds());
    long milliseconds = ((int)ts.time_of_day().total_milliseconds())%1000;
    long year = d.year();
    long month = d.month();
    long day = d.day();
    return StrFormat( "%4d-%02d-%02dT%02d:%02d:%02d.%3d", year, month, day, hours, minutes, seconds,milliseconds);
}
inline boost::posix_time::ptime GetDateTime(std::string s)
{
    using namespace boost::posix_time;
    // parse 2012-02-03T17:31:51.0968Z
    int Year, Month, Day, Hour, Minute, Second, Millisecond=0;
    if(sscanf(s.c_str(), "%d-%d-%dT%d:%d:%d.%d", &Year, &Month, &Day, &Hour, &Minute,&Second, &Millisecond)==7){}
    //12/2/2009 2:42:25 PM
    else if(sscanf(s.c_str(), "%d/%d/%4d%d:%d:%d", &Month, &Day, &Year,  &Hour, &Minute,&Second)==6){}
    else
    {
        ::MessageBox(NULL, StrFormat("Unrecognized date-time format -%s\n", s.c_str()).c_str(), "Error", MB_OK);
        throw std::runtime_error(StrFormat("Unrecognized date-time format -%s\n", s.c_str()));

    }

    return boost::posix_time::ptime(boost::gregorian::date(Year, Month, Day),
                                    time_duration(hours(Hour)+minutes(Minute)+seconds(Second)+millisec(Millisecond)));
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
