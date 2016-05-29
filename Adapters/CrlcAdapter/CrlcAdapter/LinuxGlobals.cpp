//
// LinuxGlobals.h
//

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.
#ifdef WIN32
#include "stdafx.h"
#include <time.h>
#endif

#include "LinuxGlobals.h"
#include <iostream>
#include <istream>
#include <sstream>
#include <fstream> 
#include <sys/types.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/time.h>
#endif

#include <cstring>
#include <stdarg.h>  
//#ifdef WIN32
//#define SECURITY_WIN32
//#include "security.h"
//#pragma comment(lib, "Secur32.lib")
//#endif

#ifndef E_FAIL
#define E_FAIL -1
#endif

namespace Globals {
 std::string StrFormat(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	
	int m;
	size_t n= strlen(format) + 1028;
	std::string tmp(n,'0');	


	// Kind of a bogus way to insure that we don't
	// exceed the limit of our buffer
	while((m=vsnprintf(&tmp[0], n-1, format, ap))<0)
	{
		n=n+1028;
		tmp.resize(n,'0');
	}
	va_end(ap);
	return tmp.substr(0,m);

}
    void Sleep(int ms) {
#ifdef WIN32
		::Sleep(ms);
#else
        usleep(ms*1000);
#endif
    }

    unsigned int ErrorMessage(std::string errmsg) {
        std::cout << errmsg;
        return E_FAIL;
    }
    unsigned int DebugMessage(std::string errmsg) {
        //std::cout << errmsg;
        return E_FAIL;
    }
    std::string &LeftTrim(std::string &str) {

        size_t startpos = str.find_first_not_of(" \t\r\n");
        if (std::string::npos != startpos)
            str = str.substr(startpos);
        return str;
    }
    // trim from end

    std::string &RightTrim(std::string &str, std::string trim = " \t\r\n") {
        size_t endpos = str.find_last_not_of(trim);
        if (std::string::npos != endpos)
            str = str.substr(0, endpos + 1);
        return str;
    }
    // trim from both ends
#include <fstream> 

    std::string &Trim(std::string &s) {
        return LeftTrim(RightTrim(s));
    }

    bool ReadFile(std::string filename, std::string & contents) {
        std::ifstream in(filename.c_str());
        std::stringstream buffer;
        buffer << in.rdbuf();
        contents = buffer.str();
        return true;
    }

    void WriteFile(std::string filename, std::string & contents) {
        std::ofstream outFile(filename.c_str());
        //Write the string and its null terminator !!
        outFile << contents.c_str();
    }
#define MIN(X,Y) ((X<Y)?X:Y)

    std::string ExeDirectory() {
#ifdef WIN32
		TCHAR buf[1000];
		GetModuleFileName(NULL, buf, 1000);
		std::string pBuf(buf);
		pBuf=pBuf.substr( 0, pBuf.find_last_of( '\\' ) +1 );
#else
        int len = 32;
        char szTmp[32];
        char * pBuf;
        sprintf(szTmp, "/proc/%d/exe", getpid());
        int bytes = MIN(readlink(szTmp, pBuf, len), len - 1);
        if (bytes >= 0)
            pBuf[bytes] = '\0';
#endif
        return pBuf;
    }

    std::string GetTimeStamp(TimeFormat format) {
#ifdef WIN32
			SYSTEMTIME st;
	char timestamp[64];
	GetSystemTime(&st);
	sprintf(timestamp, "%4d-%02d-%02dT%02d:%02d:%02d", st.wYear, st.wMonth,
		st.wDay, st.wHour, st.wMinute, st.wSecond);

	if (format == GMT_UV_SEC)
	{
		sprintf(timestamp + strlen(timestamp), ".%04dZ", st.wMilliseconds);
	}
	else
	{
		strcat(timestamp, "Z");
	}

	return timestamp;
#else
        char timeBuffer[50];
        struct tm * timeinfo;
        struct timeval tv;
        struct timezone tz;

        gettimeofday(&tv, &tz);
        timeinfo = (format == LOCAL) ? localtime(&tv.tv_sec) : gmtime(&tv.tv_sec);

        switch (format) {
            case HUM_READ:
                strftime(timeBuffer, 50, "%a, %d %b %Y %H:%M:%S %Z", timeinfo);
                break;
            case GMT:
                strftime(timeBuffer, 50, "%Y-%m-%dT%H:%M:%SZ", timeinfo);
                break;
            case GMT_UV_SEC:
                strftime(timeBuffer, 50, "%Y-%m-%dT%H:%M:%S", timeinfo);
                break;
            case LOCAL:
                strftime(timeBuffer, 50, "%Y-%m-%dT%H:%M:%S%z", timeinfo);
                break;
        }


        if (format == GMT_UV_SEC) {
            sprintf(timeBuffer + strlen(timeBuffer), ".%06ldZ", tv.tv_usec);
        }

        return std::string(timeBuffer);
#endif
    }

};
