// ABBAgent.cpp : Defines the entry point for the console application.

//

#include "stdafx.h"
#define BOOST_ALL_NO_LIB

#include <string>
#include <exception>
#include "ABBftpAgent.h"
#include "ABBftpAdapter.h"
#include "StdStringFcn.h"
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include "Globals.h"
#include "DevicesXML.h"

// should not be included in header
// #include "jsmtconnect.txt"
#include "logger.h"
#pragma comment(lib, "Ws2_32.lib")

// #define MTCLIBPATH(X) "D:\\michalos\\My Work\\ABBMTConnectAgent\\MTConnectAgent\\win32\\libxml2-2.7.7\\lib\\"##X
#define MTCLIBPATH(X)    "C:\\Users\\michalos\\Documents\\GitHub\\Agents\\ABBftpMTConnectAgent\\MTConnectAgent\\win32\\libxml2-2.7.7\\lib\\" ## X
#define BOOSTLIBPATH(X)    "C:\\Program Files\\NIST\\src\\boost_1_54_0\\vc10\\stagex64\\lib\\" ## X

#if defined( WIN64 ) && defined( _DEBUG )
#pragma message( "DEBUG x64" )
#pragma comment(lib, MTCLIBPATH("libxml2_64d.lib"))
#pragma comment(lib, BOOSTLIBPATH("libboost_system-vc100-mt-sgd-1_54.lib"))
#pragma comment(lib, BOOSTLIBPATH("libboost_thread-vc100-mt-sgd-1_54.lib"))

#elif !defined( _DEBUG ) && defined( WIN64 )
#pragma message( "RELEASE x64" )
#pragma comment(lib,  MTCLIBPATH("libxml2_64.lib"))
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")

#elif defined( _DEBUG ) && defined( WIN32 )
#pragma message( "DEBUG x32" )

// #pragma message( MTCLIBPATH("libxml2d.lib") )
#pragma comment(lib, MTCLIBPATH("libxml2d.lib"))
#pragma comment(lib, "libboost_thread-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-sgd-1_54.lib")
#elif !defined( _DEBUG ) && defined( WIN32 )
#pragma message( "RELEASE x32" )
#pragma comment(lib, MTCLIBPATH("libxml2d.lib"))
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")
#endif
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
  sprintf(aBuffer, "%4d-%02d-%02dT%02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour,
    st.wMinute, st.wSecond);
  return aBuffer;
}
int AgentConfigurationT::main (int aArgc, const char *aArgv[])
{
  GLogger.Fatal("AgentConfigurationT::main\n");
  SetCurrentDirectory(File.ExeDirectory( ).c_str( ) ); // fixes Agent file lookup issue
  AgentConfiguration::main(aArgc, (const char **) aArgv);
  GLogger.Fatal("Return Main\n");

  return 0;
}
// Start the server. This blocks until the server stops.
void AgentConfigurationEx::start ( )
{
  GLogger.Fatal("AgentConfigurationEx::start\n");

  for ( int i = 0; i < _devices.size( ); i++ )
  {
    ABBAdapter *_cmdHandler = new ABBAdapter(this, config, _devices[i]);
    _cncHandlers.push_back(_cmdHandler);
    _group.create_thread(boost::bind(&ABBAdapter::Cycle, _cncHandlers[i]) );
  }

  if ( Globals.ResetAtMidnight )
  {
    COleDateTime     now          = COleDateTime::GetCurrentTime( );
    COleDateTimeSpan tilnextreset = COleDateTimeSpan(0, 1, 0, 0);
    GLogger.Fatal(StdStringFormat("Agent will Reset from now  %8.4f\n", ( tilnextreset.GetTotalSeconds( ) / 3600.00 ) ) );

    _resetthread.Initialize( );

    _resetthread.AddTimer(
      (long) tilnextreset.GetTotalSeconds( ) * 1000,
      &_ResetThread,
      ( DWORD_PTR ) this,
      &_ResetThread._hTimer        // stored newly created timer handle
      );
  }

  AgentConfigurationT::start( );  // does not return
}
void AgentConfigurationEx::stop ( )
{
  for ( int i = 0; i < _cncHandlers.size( ); i++ )
  {
    _cncHandlers[i]->Stop( );
  }

  _group.join_all( );

  AgentConfigurationT::stop( );
}
void AgentConfigurationEx::initialize (int aArgc, const char *aArgv[])
{
  GLogger.Fatal("AgentConfigurationEx::initialize\n");

  std::string cfgfile = Globals.inifile;

  if ( GetFileAttributesA(cfgfile.c_str( ) ) != INVALID_FILE_ATTRIBUTES )
  {
    config.load(cfgfile);
    Globals.sNewConfig = config.GetSymbolValue("GLOBALS.Config", "OLD").c_str( );
    Globals.sNewConfig = MakeUpper(Globals.sNewConfig);

    Globals.ServerName = config.GetSymbolValue("GLOBALS.ServiceName", Globals.ServerName).c_str( );
    MTConnectService::setName(Globals.ServerName);
    Globals.QueryServer = config.GetSymbolValue("GLOBALS.QueryServer", 10000).toNumber<int>( );
	Globals.ServerRate  = config.GetSymbolValue("GLOBALS.ServerRate", 2000).toNumber<int>( );
	Globals.DateFormat  =config.GetSymbolValue("GLOBALS.DateFormat", "dd/mm/yyyy").c_str();
	Globals.Delimiter=config.GetSymbolValue("GLOBALS.Delimiter", ",").c_str();
	Globals.Dither = config.GetSymbolValue("GLOBALS.Dither", 0.2).toNumber<double>( );
    // std::string sLevel = config.GetSymbolValue("GLOBALS.logging_level", "FATAL").c_str();
    // sLevel=MakeUpper(sLevel);
    // Globals.Debug  = (sLevel=="FATAL")? 0 : (sLevel=="ERROR") ? 1 : (sLevel=="WARN") ? 2 : (sLevel=="INFO")? 3 : 5;
    Globals.Debug            = config.GetSymbolValue("GLOBALS.Debug", "0").toNumber<int>( );
    Globals.HttpPort         = config.GetSymbolValue("GLOBALS.HttpPort", "5000").c_str( );
    Globals.ResetAtMidnight  = config.GetSymbolValue("GLOBALS.ResetAtMidnight", "0").toNumber<int>( );
    GLogger.OutputConsole( ) = config.GetSymbolValue("GLOBALS.OutputConsole", "0").toNumber<int>( );

    _devices = config.GetTokens("GLOBALS.MTConnectDevice", ",");

    if ( Globals.sNewConfig == "NEW" )
    {
      for ( size_t i = 0; i < _devices.size( ); i++ )
      {
        _devices[i] = Trim(_devices[i]);
      }
      std::vector<std::string> keys = _devices;       // TrimmedTokenize(_devices, ",");
      // std::for_each(v.begin(), v.end(),  boost::bind(&boost::trim<std::string>,_1, std::locale() ));
      // for_each(keys.begin(); keys.end(); Trim);

      std::vector<std::string> logs;

      for ( size_t i = 0; i < _devices.size( ); i++ )
      {
        std::string log = config.GetSymbolValue(_devices[i] + ".ProductionLog", "").c_str( );

        if ( !log.empty( ) )
        {
          logs.push_back(Trim(log) );
        }
      }

      for ( size_t i = 0; i < keys.size( ); i++ )
      {
        keys[i] = SanitizeDeviceName(keys[i]);
      }

      if ( keys.size( ) == logs.size( ) )
      {
        CDevicesXML::WriteDevicesFile(keys, CDevicesXML::ProbeDeviceXml( ), "Devices.xml", File.ExeDirectory( ) );
        WritePrivateProfileString("GLOBALS", "Config", "UPDATED", ( File.ExeDirectory( ) + "Config.ini" ).c_str( ) );
      }
      else
      {
        WritePrivateProfileString("GLOBALS", "Config", "ERROR", ( File.ExeDirectory( ) + "Config.ini" ).c_str( ) );
      }
    }
  }
  else
  {
    AbortMsg("Could not find ini file \n");
  }

  Globals.Dump( );
  AgentConfigurationT::initialize(aArgc, aArgv);
}

bool AgentConfigurationEx::ResetAtMidnite ( )
{
  COleDateTime now   = COleDateTime::GetCurrentTime( );
  COleDateTime date2 = COleDateTime(now.GetYear( ), now.GetMonth( ), now.GetDay( ), 0, 0, 0) + COleDateTimeSpan(1, 0, 0, 1);

  // COleDateTime date2 =  now +  COleDateTimeSpan(0, 0, 2, 0); // testing reset time - 2 minutes
  COleDateTimeSpan tilmidnight = date2 - now;

  _resetthread.Initialize( );
  _resetthread.AddTimer(
    (long) tilmidnight.GetTotalSeconds( ) * 1000,
    &_ResetThread,
    ( DWORD_PTR ) this,
    &_ResetThread._hTimer      // stored newly created timer handle
    );
  return true;
}
HRESULT AgentConfigurationEx::CResetThread::Execute (DWORD_PTR dwParam, HANDLE hObject)
{
  static char name[] = "CResetThread::Execute";

  AgentConfigurationEx *agent = (AgentConfigurationEx *) dwParam;

  CancelWaitableTimer(hObject);

  // DebugBreak();

  try
  {
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof( pi ) );

    STARTUPINFO si;
    ZeroMemory(&si, sizeof( si ) );
    si.cb          = sizeof( si );
    si.dwFlags     = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;        // set the window display to HIDE

    // SCM reset command of this service
    std::string cmd = StdStringFormat("cmd /c net stop \"%s\" & net start \"%s\"", Globals.ServerName.c_str( ), Globals.ServerName.c_str( ) ); // Command line

    if ( !::CreateProcess(NULL,                                                                                                                // No module name (use command line)
           const_cast<char *>( cmd.c_str( ) ),
           NULL,                                                                                                                               // Process handle not inheritable
           NULL,                                                                                                                               // Thread handle not inheritable
           FALSE,                                                                                                                              // Set handle inheritance to FALSE
           0,                                                                                                                                  // No creation flags
           NULL,                                                                                                                               // Use parent's environment block
           NULL,                                                                                                                               // Use parent's starting directory
           &si,                                                                                                                                // Pointer to STARTUPINFO structure
           &pi) )                                                                                                                              // Pointer to PROCESS_INFORMATION structure
    {
      AtlTrace("CreateProcess FAIL ");
    }

    ::Sleep(5000);     // make sure process has spawned before killing thread
  }
  catch ( ... )
  {
    agent->AbortMsg("Exception  - ResetAtMidnightThread(void *oObject");
  }
  return S_OK;
}
