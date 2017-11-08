// Logging.h

/*
 * DISCLAIMER:
 * This software was produced by the National Institute of Standards
 * and Technology (NIST), an agency of the U.S. government, and by statute is
 * not subject to copyright in the United States.  Recipients of this software
 * assume all responsibility associated with its operation, modification,
 * maintenance, and subsequent redistribution.
 *
 * See NIST Administration Manual 4.09.07 b and Appendix I.
 */

#ifndef LOGONCE
#define LOGONCE    static long nLog ## __LINE__ = 0; if ( 0 == nLog ## __LINE__++ )
#endif

inline std::string LineName(std::string file, int line)
{ 
    char buffer[32];
    sprintf(buffer,"%d", line);
    return file+buffer;
}
#define LOGNAME(F,L) LineName(F, L)
#include <map>

class ALogger
{
public:
    ALogger( )
    {
        Timestamp( )  = true;
        DebugLevel( ) = 0;
        this->filename.clear( );
    }
    std::map<std::string, std::string> properties;
    void Close ( )
    {
        this->filename.clear( );
        DebugFile.close( );
    }

    void Open (std::string filename, int bAppend = false)
    {
        std::ios_base::openmode opMode = std::fstream::out;

        if ( bAppend )
        {
            opMode |= std::fstream::app;
        }
        this->filename = filename;
        DebugFile.open(filename.c_str( ), opMode); // , OF_SHARE_DENY_NONE);
    }

    int LogMessage (std::string filename, std::string msg, int level = -1)
    {
        if ( this->filename.empty( ) ) { Open(this->filename, 0); }
        LogMessage(msg, level);
    }

    int LogMessage (std::string msg, int level = -1)
    {
        static char * levels[] = {"[FATAL]","[ERROR]","[WARNING]","[INFO]","[DEBUG]","[STATUS]"};
        char * mylevel;
        if(level<0)
            mylevel="[STATUS]";
        else if(level>=4)
            mylevel="[DEBUG]";
        else
            mylevel=levels[level];
        
        if ( level > DebugLevel( ) )
        {
            return level;
        }
#if 0
        if ( OutputConsole( ) )
        {
            OutputDebugString(msg.c_str( ) );
        }
#endif

        if ( !DebugFile.is_open( ) )
        {
            return level;
        }

        if ( Timestamping( ) )
        {
            DebugFile << Timestamp( );
        }
        DebugFile << mylevel << " " ;
        DebugFile << msg;
        DebugFile.flush( );
        return level;
    }

    unsigned int LogFormatMessage(const char *fmt, ...) {
        va_list argptr;

        va_start(argptr, fmt);
        std::string str = FormatString(fmt, argptr);
        va_end(argptr);
        return LogMessage(str);
    }
    static inline std::string StrFormat (const char *fmt, ...)
    {
        va_list argptr;

        va_start(argptr, fmt);
        std::string str = FormatString(fmt, argptr);
        va_end(argptr);
        return str;
    }

    static inline std::string FormatString (const char *fmt, va_list ap)
    {
        int         m, n = (int) strlen(fmt) + 1028;
        std::string tmp(n, '0');

        while ( ( m = vsnprintf(&tmp[0], n - 1, fmt, ap) ) < 0 )
        {
            n = n + 1028;
            tmp.resize(n, '0');
        }

        return tmp.substr(0, m);
    }

    static std::string Timestamp ( )
    {
        time_t     ltime;
        struct tm *Tm;

        ltime = time(NULL);
        Tm    = localtime(&ltime);
        std::string stime = StrFormat("%4d-%02d-%0dT%02d:%02d:%02d",
#ifndef  _WINDOWS
                                      Tm->tm_year+1900,Tm->tm_mon+1,
#else
                                      Tm->tm_year, Tm->tm_mon,
#endif
                                      Tm->tm_mday, Tm->tm_hour, Tm->tm_min, Tm->tm_sec);
#ifndef  WIN32
        // Add milliseconds to end of time
        struct timeval detail_time;
        gettimeofday(&detail_time, NULL);
        stime += StrFormat(":%04d ", detail_time.tv_usec / 1000);
#endif
        return stime;
    }

    int Fatal (std::string msg)
    {
        return LogMessage(msg, 0);
    }

    int Error (std::string msg)
    {
        return LogMessage(msg, 1);
    }

    int Warning (std::string msg)
    {
        return LogMessage(msg, 2);
    }

    int Info (std::string msg)
    {
        return LogMessage(msg, 3);
    }

    int Debug (std::string msg)
    {
        return LogMessage(msg, 4);
    }

    int Status (std::string msg)
    {
        return LogMessage(msg, -1);
    }

    int & DebugLevel ( )
    {
        return _debuglevel;
    }

    bool & Timestamping ( )
    {
        return _bTimestamp;
    }

    int & OutputConsole ( )
    {
        return _nOutputConsole;
    }

    operator std::ostream & ( ) {
        return DebugFile;
    }

    // private:
    int           _debuglevel;
    bool          _bTimestamp;
    std::ofstream DebugFile;
    int           _nOutputConsole;
    std::string   filename;
};
//__declspec(selectany) ALogger Logger;
//
//inline std::ostream & operator << (std::ostream & os, const std::string & str)
//{
//    Logger.LogMessage(str);
//    return os;
//}
extern ALogger Logger;
