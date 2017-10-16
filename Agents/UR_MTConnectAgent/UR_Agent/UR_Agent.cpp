
// UR_Agent.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#define BOOST_ALL_NO_LIB

// #define RESETTEST

#include "DevicesXML.h"
#include "Globals.h"
#include "NIST/File.h"
#include "NIST/StdStringFcn.h"
#include "NIST/logger.h"
#include "UR_Adapter.h"
#include "UR_Agent.h"
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <exception>
#include <string>

// should not be included in header
// #include "jsmtconnect.txt"

static std::string SanitizeDeviceName (std::string name)
{
    ReplaceAll(name, " ", "_");
    return name;
}
static std::string GetTimeStamp ( )
{
    char       aBuffer[256];
    SYSTEMTIME st;

    GetSystemTime(&st);
    sprintf(aBuffer, "%4d-%02d-%02dT%02d:%02d:%02d", st.wYear, st.wMonth, st.wDay,
            st.wHour, st.wMinute, st.wSecond);
    return aBuffer;
}
int AgentConfigurationT::main (int aArgc, const char *aArgv[])
{
    MSVC::SetCurrentDirectory(
        File.ExeDirectory( ).c_str( ));                    // fixes Agent file lookup issue
    AgentConfiguration::main(aArgc, (const char **) aArgv);
    return 0;
}
// Start the server. This blocks until the server stops.
void AgentConfigurationEx::start ( )
{
    logDebug("Start\n");

    for ( int i = 0; i < mDevices.size( ); i++ )
    {
        UR_Adapter *_cmdHandler = new UR_Adapter(this, mConfig, mDevices[i]);
        mAdapterHandlers.push_back(_cmdHandler);
        mGroup.create_thread(
            boost::bind(&UR_Adapter::doCycle, mAdapterHandlers[i]));
    }

    if ( Globals.mbResetAtMidnight )
    {
        this->ResetAtMidnite( );
    }

    AgentConfigurationT::start( );                         // does not return
}
void AgentConfigurationEx::stop ( )
{
    for ( int i = 0; i < mAdapterHandlers.size( ); i++ )
    {
        mAdapterHandlers[i]->doStop( );
    }

    mGroup.join_all( );

    AgentConfigurationT::stop( );
}
void AgentConfigurationEx::initialize (int aArgc, const char *aArgv[])
{
    logDebug("initialize\n");

    std::string cfgfile = Globals.mInifile;

    if ( GetFileAttributesA(cfgfile.c_str( )) != INVALID_FILE_ATTRIBUTES )
    {
        mConfig.load(cfgfile);
        Globals.msNewConfig
            = mConfig.GetSymbolValue("GLOBALS.Config", "OLD").c_str( );
        Globals.msNewConfig = MakeUpper(Globals.msNewConfig);

        Globals.mServerName
            = mConfig.GetSymbolValue("GLOBALS.ServiceName", Globals.mServerName)
                  .c_str( );
        MTConnectService::setName(Globals.mServerName);
        Globals.mQueryServer
            = mConfig.GetSymbolValue("GLOBALS.QueryServer", 10000).toNumber<int>( );
        Globals.mServerRate
            = mConfig.GetSymbolValue("GLOBALS.ServerRate", 2000).toNumber<int>( );

        // std::string sLevel = config.GetSymbolValue("GLOBALS.logging_level",
        // "FATAL").c_str();
        // sLevel=MakeUpper(sLevel);
        // Globals.Debug  = (sLevel=="FATAL")? 0 : (sLevel=="ERROR") ? 1 :
        // (sLevel=="WARN") ? 2 : (sLevel=="INFO")? 3 : 5;
        Globals.mDebug
            = mConfig.GetSymbolValue("GLOBALS.Debug", "0").toNumber<int>( );
        Globals.mHttpPort
            = mConfig.GetSymbolValue("GLOBALS.HttpPort", "5000").c_str( );
        Globals.mbResetAtMidnight
            = mConfig.GetSymbolValue("GLOBALS.ResetAtMidnight", "0").toNumber<int>( );
        GLogger.OutputConsole( )
            = mConfig.GetSymbolValue("GLOBALS.OutputConsole", "0").toNumber<int>( );
        Globals.mVersion
            = mConfig.GetSymbolValue("GLOBALS.Version", "0").toNumber<double>( );

        mDevices = mConfig.GetTokens("GLOBALS.MTConnectDevice", ",");

        // if ( Globals.msNewConfig == "NEW" )
        {
            for ( size_t i = 0; i < mDevices.size( ); i++ )
            {
                mDevices[i] = Trim(mDevices[i]);
            }
            std::vector<std::string> keys = mDevices;
#if 0
            for ( size_t i = 0; i < keys.size( ); i++ )
            {
                keys[i] = SanitizeDeviceName(keys[i]);
            }
#endif
            std::string devicemodel;

            for ( size_t i = 0; i < keys.size( ); i++ )
            {
                mTagnames[keys[i]] = std::vector<std::string>( );
                std::vector<std::string> jointnames
                    = mConfig.GetTokens(mDevices[i] + ".jointnames", ",");
                std::string robotdevicemodel = CDevicesXML::ConfigureRobotDeviceXml(
                    keys[i], jointnames, mTagnames[keys[i]], mConditionTags[keys[i]]);
                ReplaceAll(robotdevicemodel, "####", keys[i]);
                robotdevicemodel = ReplaceOnce(robotdevicemodel, "name=\"NNNNNN\"",
                                               "name=\"" + keys[i] + "\"");
                devicemodel += robotdevicemodel;
            }
            CDevicesXML::WriteDevicesFileHeader(devicemodel, File.ExeDirectory( ) + "Devices.xml");
            MSVC::WritePrivateProfileString("GLOBALS", "Config", "UPDATED",
                                            ( File.ExeDirectory( ) + "Config.ini" ).c_str( ));
        }
    }
    else
    {
        logAbort("Could not find ini file \n");
    }
    AgentConfigurationT::initialize(aArgc, aArgv);
}
bool AgentConfigurationEx::ResetAtMidnite ( )
{
    COleDateTime now = COleDateTime::GetCurrentTime( );

#ifndef RESETTEST
    COleDateTime date2
        = COleDateTime(now.GetYear( ), now.GetMonth( ), now.GetDay( ), 0, 0, 0)
          + COleDateTimeSpan(1, 0, 0, 1);
#else
    COleDateTime date2
        = now + COleDateTimeSpan(0, 0, 2, 0);              // testing reset time - 2 minutes
#endif
    COleDateTimeSpan tilmidnight = date2 - now;
    logStatus("Agent will Reset at  %s\n",
              date2.Format("%A, %B %d, %Y %H:%M:%S"));
    logStatus("Agent will Reset %8.4f hours::min from now\n",
              ( tilmidnight.GetTotalSeconds( ) / 3600.00 ));

    _resetthread.Initialize( );
    _resetthread.AddTimer(
        (long) tilmidnight.GetTotalSeconds( ) * 1000, &_ResetThread,
        (DWORD_PTR) this,
        &_ResetThread._hTimer                              // stored newly created timer handle
        );
    return true;
}
HRESULT AgentConfigurationEx::CResetThread::Execute (DWORD_PTR dwParam,
                                                     HANDLE    hObject)
{
    static char name[] = "CResetThread::Execute";

    AgentConfigurationEx *agent = (AgentConfigurationEx *) dwParam;

    MSVC::CancelWaitableTimer(hObject);

    // DebugBreak();

    try
    {
        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof( pi ));

        STARTUPINFO si;
        ZeroMemory(&si, sizeof( si ));
        si.cb          = sizeof( si );
        si.dwFlags     = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;                          // set the window display to HIDE

// SCM reset command of this service
#ifndef RESETTEST
        std::string cmd
            = StdStringFormat("cmd /c net stop \"%s\" & net start \"%s\"",
                              Globals.mServerName.c_str( ),
                              Globals.mServerName.c_str( )); // Command line
#else

        // This works when you run in from a command console as UR_Agent.exe debug,
        // it will restart in the current DOS console.
        TCHAR buf[1000];
        GetModuleFileName(NULL, buf, 1000);
        std::string exepath = File.ExeDirectory( );
        std::string exe     = File.ExtractFilename(std::string(buf));

        // GLogger.Fatal(StdStringFormat("start Agent /d  \"%s\" /b \"%s\" debug\n",
        // exepath.c_str(), buf  ) );
        std::string cmd
            = StdStringFormat("cmd /c taskkill /IM \"%s\" & \"%s\" debug",
                              exe.c_str( ), buf);          // Command line
#endif

        if ( !::CreateProcess(NULL,                        // No module name (use command line)
                              const_cast<char *>( cmd.c_str( ) ),
                              NULL,                        // Process handle not inheritable
                              NULL,                        // Thread handle not inheritable
                              FALSE,                       // Set handle inheritance to FALSE
                              0,                           // No creation flags
                              NULL,                        // Use parent's environment block
                              NULL,                        // Use parent's starting directory
                              &si,                         // Pointer to STARTUPINFO structure
                              &pi) )                       // Pointer to PROCESS_INFORMATION structure
        {
            MSVC::AtlTrace("CreateProcess FAIL ");
        }

        MSVC::Sleep(5000);                                 // make sure process has spawned before killing thread
    }
    catch ( ... )
    {
        logAbort("Exception  - ResetAtMidnightThread(void *oObject");
    }
    return S_OK;
}
