// **************************************************************************

// Logger.h
//
// Description:
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.
// **************************************************************************
#pragma once
#include <atlcomtime.h>
#include <fstream>
#include <iostream>
#include <stdarg.h>
#include <string>

// Based on ROS Console
// https://github.com/ros/console_bridge/blob/master/include/console_bridge/console.h
// Interesting C++ Logging filtering macros here:
// https://github.com/ros/ros_comm/blob/4383f8fad9550836137077ed1a7120e5d3e745de/tools/rosconsole/include/ros/console.h

namespace Logging
{
/** \brief The set of priorities for message logging */
    enum LogLevel
    {
        LOG_DEBUG = 4,
        LOG_INFO  = 3,
        LOG_WARN  = 2,
        LOG_ERROR = 1,
        LOG_NONE  = 0,
        LOG_FATAL = -1
    };
    struct CLogger
    {
        std::string        ExeDirectory ( )
        {
            char buf[1000];

            GetModuleFileName(NULL, buf, 1000);
            std::string path(buf);
            path = path.substr(0, path.find_last_of('\\') + 1);
            return path;
        }

        static std::string Timestamp ( )
        {
            SYSTEMTIME st;

            GetLocalTime(&st);
            char buffer[256];
            sprintf(buffer, "%4d-%02d-%02d %02d:%02d:%02d.%04d ", st.wYear, st.wMonth,
                    st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
            return buffer;
        }

        CLogger( )
        {
            DebugLevel( ) = 7;
            filename      = ExeDirectory( ) + "debug.txt";

            // std::cout.rdbuf(&_outputdebugstream);
            // std::cerr.rdbuf(&_outputdebugstream);
            OutputConsole( ) = 0;
            Timestamping( )  = 0;
            sDebugString     = "";
            _nCounter        = 1;
        }

        ~CLogger( )
        {
            if ( DebugFile.is_open( ) )
            {
                DebugFile.close( );
            }
        }

        int &              DebugLevel ( ) { return _debuglevel; }
        bool &             Timestamping ( ) { return bTimestamp; }
        std::string &      DebugString ( ) { return sDebugString; }
        int &              OutputConsole ( ) { return nOutputConsole; }
        std::ofstream & operator () (void) { return this->DebugFile; }
        void               Close ( ) { DebugFile.close( ); }
        void               Open (std::string filename, int bAppend = false)
        {
            this->filename = filename;
            Open(bAppend);
        }

        void               Open (int bAppend = 0)
        {
            int opMode = std::fstream::out;

            if ( bAppend )
            {
                opMode |= std::fstream::app;
            }

            DebugFile.open(filename.c_str( ), opMode, OF_SHARE_DENY_NONE);
        }

        void               Message (std::string msg)
        {
            if ( OutputConsole( ) )
            {
                OutputDebugString(msg.c_str( ));
            }

            if ( !DebugFile.is_open( ) )
            {
                return;
            }

            if ( Timestamping( ) )
            {
                DebugFile << Timestamp( );
            }
            DebugFile << msg;
            DebugFile.flush( );
        }

        void               logmessage (const char *file, int line, LogLevel level, const char *fmt,
                                       ...)
        {
            if ( level > DebugLevel( ) )
            {
                return;
            }

            va_list ap;
            va_start(ap, fmt);

            int         m;
            int         n = strlen(fmt) + 1028;
            std::string tmp(n, '0');

            // Kind of a bogus way to insure that we don't
            // exceed the limit of our buffer
            while ( ( m = _vsnprintf(&tmp[0], n - 1, fmt, ap) ) < 0 )
            {
                n = n + 1028;
                tmp.resize(n, '0');
            }

            va_end(ap);
            tmp = tmp.substr(0, m);

            if ( OutputConsole( ) )
            {
                OutputDebugString(tmp.c_str( ));
            }

            if ( !DebugFile.is_open( ) )
            {
                return;
            }

            if ( Timestamping( ) )
            {
                DebugFile << Timestamp( );
            }
            DebugFile << tmp;
            DebugFile.flush( );
        }

protected:
        std::ofstream      DebugFile;
        int                _debuglevel;
        bool               bTimestamp;
        std::string        sDebugString;
        int                nOutputConsole;
        int                nDebugReset;
        std::string        filename;
        int                _nCounter;
    };
}
__declspec(selectany) Logging::CLogger GLogger;

#define logAbort(fmt, ...)                                            \
    ::GLogger.logmessage(__FILE__, __LINE__, Logging::LOG_FATAL, fmt, \
                         ## __VA_ARGS__);                             \
    ExitProcess(-1)

#define logFatal(fmt, ...)                                            \
    ::GLogger.logmessage(__FILE__, __LINE__, Logging::LOG_FATAL, fmt, \
                         ## __VA_ARGS__)

#define logStatus(fmt, ...)                                           \
    ::GLogger.logmessage(__FILE__, __LINE__, Logging::LOG_FATAL, fmt, \
                         ## __VA_ARGS__)

#define logError(fmt, ...)                                            \
    ::GLogger.logmessage(__FILE__, __LINE__, Logging::LOG_ERROR, fmt, \
                         ## __VA_ARGS__)

#define logWarn(fmt, ...)                                            \
    ::GLogger.logmessage(__FILE__, __LINE__, Logging::LOG_WARN, fmt, \
                         ## __VA_ARGS__)

#define logInform(fmt, ...)                                          \
    ::GLogger.logmessage(__FILE__, __LINE__, Logging::LOG_INFO, fmt, \
                         ## __VA_ARGS__)

#define logDebug(fmt, ...)                                            \
    ::GLogger.logmessage(__FILE__, __LINE__, Logging::LOG_DEBUG, fmt, \
                         ## __VA_ARGS__)

// logTrace is only used in debugging mode
#ifdef DEBUG
#define logTrace(fmt, ...)                                            \
    ::GLogger.logmessage(__FILE__, __LINE__, Logging::LOG_FATAL, fmt, \
                         ## __VA_ARGS__)

#else
#define logTrace(fmt, ...)
#endif
#define LOG_ONCE(X)                                   \
    {                                                 \
        static bool __log_stream_once__hit__ = false; \
        if ( !__log_stream_once__hit__ ) {            \
            __log_stream_once__hit__ = true;          \
            X;                                        \
        }                                             \
    }
