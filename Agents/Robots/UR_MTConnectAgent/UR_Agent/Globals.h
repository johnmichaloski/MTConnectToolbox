
// Globals.h

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#pragma once
#include <map>
#include <stdio.h>
#include <vector>

#include "NIST/File.h"
#include "NIST/Logger.h"
#include "NIST/StdStringFcn.h"

/**
 * @brief The CGlobals class contains the application global
 * variables.
 */
class CGlobals
{
public:
    int & mDebug;                                          //!<  reference to logging debug level
    int mQueryServer;                                      //!<  global query rate of client device in milliseconds,
                                                           // !can be overriden
    int mServerRate;                                       //!<  global rate of each adapter in milliseconds, can be
                                                           // !overriden
    std::string
        mInifile;                                          //!< path of ini file to read for agent/adapter extensions
    std::string
        mCfgFile;                                          //!<  name of agent configuration file (typically agent.cfg)
    std::string mDevicesFile;                              //!<  name of devices file (typically devices.xml)
    std::string mHttpPort;                                 //!<  port that agent listens to for http gets
    double mVersion;                                       //!<  version of the ur controller e.g., 1.8 3.1 3.4
    std::string mServerName;                               //!<  name of service for Windows SCM
    std::string msNewConfig;                               //!< flag read from config.ini to determine if new
                                                           // !device.xml file to ge generated.
    bool mbResetAtMidnight;                                //!<  flag to determine if agent will reset at midnight

    /**
     * @brief CGlobals initializes most variables.
     */
    CGlobals( ) : mDebug(GLogger.DebugLevel( ))
    {
        mServerRate          = 2000;
        GLogger.Timestamp( ) = true;
        mDebug            = 0;
        mQueryServer      = 10000;
        mHttpPort         = "5000";
        mServerName       = "UR_Agent";
        mbResetAtMidnight = 0;

        // This works becuase File does not use argc/argv for path
        mInifile     = File.ExeDirectory( ) + "Config.ini";
        mCfgFile     = File.ExeDirectory( ) + "Agent.cfg";
        mDevicesFile = File.ExeDirectory( ) + "Devices.xml";
    }
};

__declspec(selectany) CGlobals Globals;
