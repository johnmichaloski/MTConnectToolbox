
//
//  cppagent.cpp
//

/*
* Copyright (c) 2008, AMT  The Association For Manufacturing Technology (AMT)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the AMT nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* DISCLAIMER OF WARRANTY. ALL MTCONNECT MATERIALS AND SPECIFICATIONS PROVIDED
* BY AMT, MTCONNECT OR ANY PARTICIPANT TO YOU OR ANY PARTY ARE PROVIDED "AS IS"
* AND WITHOUT ANY WARRANTY OF ANY KIND. AMT, MTCONNECT, AND EACH OF THEIR
* RESPECTIVE MEMBERS, OFFICERS, DIRECTORS, AFFILIATES, SPONSORS, AND AGENTS
* (COLLECTIVELY, THE "AMT PARTIES") AND PARTICIPANTS MAKE NO REPRESENTATION OR
* WARRANTY OF ANY KIND WHATSOEVER RELATING TO THESE MATERIALS, INCLUDING,
WITHOUT
* LIMITATION, ANY EXPRESS OR IMPLIED WARRANTY OF NONINFRINGEMENT,
* MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.

* LIMITATION OF LIABILITY. IN NO EVENT SHALL AMT, MTCONNECT, ANY OTHER AMT
* PARTY, OR ANY PARTICIPANT BE LIABLE FOR THE COST OF PROCURING SUBSTITUTE GOODS
* OR SERVICES, LOST PROFITS, LOSS OF USE, LOSS OF DATA OR ANY INCIDENTAL,
* CONSEQUENTIAL, INDIRECT, SPECIAL OR PUNITIVE DAMAGES OR OTHER DIRECT DAMAGES,
* WHETHER UNDER CONTRACT, TORT, WARRANTY OR OTHERWISE, ARISING IN ANY WAY OUT OF
* THIS AGREEMENT, USE OR INABILITY TO USE MTCONNECT MATERIALS, WHETHER OR NOT
* SUCH PARTY HAD ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES.
*/
#include "stdafx.H"

// #define BOOST_ALL_NO_LIB

#include "fcntl.h"
#include "sys/stat.h"
#include <algorithm>
#include <map>
#include <stdio.h>
#include <string>
#include <vector>

#include "Moto_Agent.h"

// #include "agent.hpp"
// #include "config.hpp"
//
// #include "dlib/config_reader.h"
// #include <dlib/logger.h>
// #include "dlib/timeout/timeout_kernel_1.h"
//
// using namespace std;
// using namespace dlib;
//
//
// static logger sLogger("main");
// static   const log_level LENTRY(400,"ENTRY");
//
// static const char INI_FILENAME[] = "agent.cfg";
// static const char LOG_FILENAME[] = "debug.log";
//
//
// extern SERVICE_STATUS          gSvcStatus;
// extern VOID ReportSvcStatus( DWORD dwCurrentState, //    The current state
// (see SERVICE_STATUS)
//					 DWORD dwWin32ExitCode, //   The system error
// code
//					 DWORD dwWaitHint); //    Estimated time for
// pending operation,
//

#include "agent.hpp"
#include "config.hpp"
#include "dlib/revision.h"
#include "libxml/XmlVersion.h"
#include "version.h"
#include "versionno.h"

#include "NIST/AppEventLog.h"
#include "NIST/Config.h"
#include "NIST/Logger.h"
#include "NIST/StdStringFcn.h"
#include "NIST/WinSingleton.h"
#include "ProductVersion.h"

#include "Moto_Interface.h"



#ifdef _WINDOWS

using namespace std;
class AgentEx : public Agent
{
public:
    AgentEx( ) { }
protected:
    CProductVersion version;
#ifdef HANDLECALL
    virtual std::string handleCall (ostream & out, const string & path,
                                    const key_value_map & queries,
                                    const string & call, const string & device)
#else
    virtual std::string handleExtensionCall (const std::string & call,
                                             const std::string & device)
#endif
    {
        if ( call == "version" )
        {
            std::string html;
            std::string v = "<versions><version>" + getVersionString( );
            ReplaceAll(v, "\n", "</version>\n<version>");
            v = v.substr(0, v.size( ) - std::string("<version>").size( ));
            v = v + "</versions>";
            logTrace(v.c_str( ));
            html = version.GenerateVersionTable(v);

            return html;
        }
        else if ( call == "documentation" )
        {
            std::string html = version.GenerateXSLTVersion(
                "<versions><version>" + version.GenerateVersionDocument( )
                + "</version></versions>");
            return html;
        }
#ifdef HANDLECALL
        return Agent::handleCall(out, path, queries, call, device);
#else
        return "";
#endif
    }

    std::string getVersionString ( )
    {
        std::string str;

        str += StdStringFormat(
            "MTConnect Core Agent Version %d.%d.%d.%d - built on " __TIMESTAMP__
            "\n",
            AGENT_VERSION_MAJOR, AGENT_VERSION_MINOR, AGENT_VERSION_PATCH,
            AGENT_VERSION_BUILD);
#ifdef WIN64
        std::string compilearch = "64 bit";
#else
        std::string compilearch = "32 bit";
#endif
        str += StdStringFormat("moto_ Agent Extensions  %s Platform Version %s - "
                               "built on  " __TIMESTAMP__ "\n",
                               compilearch.c_str( ), STRPRODUCTVER);
        str += StdStringFormat(
            "moto_ Agent MSI Install Version %s  \n",
            version
                .Getmoto_InstallVersion(
                "C:\\Users\\michalos\\Documents\\GitHub\\Agents\\moto_"
                "MTConnectAgent\\moto_SetupX64\\moto_SetupX64.vdproj")
                .c_str( ));
        str += StdStringFormat("Windows Version : %s\n",
                               version.GetOSDisplayString( ).c_str( ));
        str += StdStringFormat("Microsoft Visual C++ Version %s\n",
                               version.GetMSVCVersion(_MSC_VER));

        //		str+=StdStringFormat("Dlib version %d.%d dated %s \n",
        // DLIB_MAJOR_VERSION , DLIB_MINOR_VERSION,DLIB_DATE);
        // str+=StdStringFormat("Dlib version %d.%d\n", 17,47);
        str += StdStringFormat("XML Lib version %s \n", LIBXML_DOTTED_VERSION);
		str += StdStringFormat("Boost libraries %d.%d.%d \n",
			BOOST_VERSION / 100000    ,
			BOOST_VERSION / 100 % 1000 ,  // min. version
			BOOST_VERSION % 100   );        // patch version
        str += StdStringFormat("moto_ Devices = %s\n", getAllDevices( ).c_str( ));
        return str;
    }

    // This is here becuase the mDeviceMap variable member of Agent is protected,
    // and its easier to just access here than in CProductVersion
	std::string getAllDevices ( )
	{
		std::string d;
		for(size_t i=0; i < mDevices.size() ; i++)
		{
			if (i>0)
			{
				d += ",";
			}
			d += mDevices[i]->getName();
		}
		return d;
	}
};

// Handles Win32 exceptions (C structured exceptions) as C++ typed exceptions
// Converts into C++ std exception
static void trans_func (unsigned int u, EXCEPTION_POINTERS *pExp)
{
    std::stringstream outbuf;

    logStatus("Runtime win32 C structured exception=%s\n", outbuf.str( ).c_str( ));
    throw std::exception( );
}
#endif

int main (int argc, char *argv[])
{
    try
    {
        GLogger.Timestamping( )  = true;
        GLogger.DebugString( )   = "moto_Agent";
        GLogger.OutputConsole( ) = true;
        GLogger.Open(File.ExeDirectory( ) + "Debug.txt");
        GLogger.DebugLevel( ) = 0;
        logStatus("Start moto_ Agent\n");
		//moto::moto_robot_mode_data::mVersion = 3.4;
		//Globals.bHexdump=true;
		//Globals.bDebugMask |= DEBUG_FEEDBACK;
#ifdef _WINDOWS

        // Handles Win32 exceptions (C structured exceptions) as C++ typed
        // exceptions
        _set_se_translator(trans_func);

        // Handle initialization of winsock, although dlib does also
        //WSADATA wsaData;

        //if ( MSVC::WSAStartup(MAKEWORD(2, 0), &wsaData) != 0 )
        //{
        //    logAbort("WSAStartup moto_ Agent failed\n");
        //}

        // MICHALOSKI ADDED
        // This sets up the proper folder for the agent to find agent.cfg, etc.
        TCHAR buf[1000];
        GetModuleFileName(NULL, buf, 1000);
        std::string path(buf);
        path = path.substr(0, path.find_last_of('\\') + 1);
        MSVC::SetCurrentDirectory(path.c_str( ));
#endif

// This is uncessary, as two exe with same image, must collide.
// We cannot check if two agents collide on same http port, also bad.
#if 0
        if ( filepaths.size( ) > 1 )
        {
            YamlReader  yaml;
            std::string exefolder = singleton.ExtractDirectory(modulepath);
            std::string filename  = exefolder + "\\Agent.cfg";
            yaml.LoadFromFile(filename);
            std::string portnum = yaml.Find("GLOBAL.Port");

            for ( size_t i = 0; i < filepaths.size( ); i++ )
            {
                if ( pids[i] == pid )
                {
                    continue;
                }
                std::string exefolder = singleton.ExtractDirectory(filepaths[i]);
                filename = exefolder + "\\Agent.cfg";
                yaml.LoadFromFile(filename);
                std::string portnum2 = yaml.Find("GLOBAL.Port");

                if ( portnum == portnum2 )
                {
                    CLogger Mayday;
                    Mayday.Timestamping( )  = true;
                    Mayday.DebugString( )   = "moto_Agent";
                    Mayday.OutputConsole( ) = false;
                    Mayday.Open(File.ExeDirectory( ) + "MaydayMayday.txt");
                    Mayday.DebugLevel( ) = 5;
                    Mayday.Fatal("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                    Mayday.Fatal("Conflict with another MTConnect agent with same exe name and http port number\n");
                    Mayday.Fatal("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                    Mayday.Close( );
                }
            }
        }
#endif

        AgentConfigurationEx config;
        config.setAgent(new AgentEx( ));
        Globals.mDebug = 0;
        return config.main(argc, (const char **) argv);
    }
    catch ( ... )
    {
        logFatal("Abnormal exception end to  moto_ Agent\n");
    }
    return 0;
}
