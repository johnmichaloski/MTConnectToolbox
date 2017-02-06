// NikonAgent.cpp : Defines the entry point for the console application.

//

#include "stdafx.h"
#define BOOST_ALL_NO_LIB
//#define RESETTEST

#include <string>
#include <exception>
#include "NikonAgent.h"
#include "NikonAdapter.h"
#include "StdStringFcn.h"
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include "Globals.h"
#include "DevicesXML.h"
#include "File.h"

// should not be included in header
// #include "jsmtconnect.txt"
#include "logger.h"
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Psapi.lib")

// #define MTCLIBPATH(X) "D:\\michalos\\My Work\\NikonMTConnectAgent\\MTConnectAgent\\win32\\libxml2-2.7.7\\lib\\"##X
#define MTCLIBPATH(X)    "C:\\Users\\michalos\\Documents\\GitHub\\Agents\\NikonMTConnectAgent\\MTConnectAgent\\win32\\libxml2-2.7.7\\lib\\" ## X

#if defined( WIN64 ) && defined( _DEBUG )
#pragma message( "DEBUG x64" )
#pragma comment(lib, MTCLIBPATH("libxml2_64d.lib"))
#pragma comment(lib, "libboost_system-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_thread-vc100-mt-sgd-1_54.lib")

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
	GLogger.Fatal("Main\n");
	SetCurrentDirectory(File.ExeDirectory( ).c_str( ) ); // fixes Agent file lookup issue
	AgentConfiguration::main(aArgc, (const char **) aArgv);
	GLogger.Fatal("Return Main\n");

	return 0;
}
// Start the server. This blocks until the server stops.
void AgentConfigurationEx::start ( )
{
	GLogger.Fatal("Start\n");

	for ( int i = 0; i < _devices.size( ); i++ )
	{
		NikonAdapter *_cmdHandler = new NikonAdapter(this, config, _devices[i]);
		_cncHandlers.push_back(_cmdHandler);
		_group.create_thread(boost::bind(&NikonAdapter::Cycle, _cncHandlers[i]) );
	}

	if ( Globals.ResetAtMidnight )
	{
		this->ResetAtMidnite();
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
	GLogger.Fatal("initialize\n");

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

		// std::string sLevel = config.GetSymbolValue("GLOBALS.logging_level", "FATAL").c_str();
		// sLevel=MakeUpper(sLevel);
		// Globals.Debug  = (sLevel=="FATAL")? 0 : (sLevel=="ERROR") ? 1 : (sLevel=="WARN") ? 2 : (sLevel=="INFO")? 3 : 5;
		Globals.Debug            = config.GetSymbolValue("GLOBALS.Debug", "0").toNumber<int>( );
		Globals.HttpPort         = config.GetSymbolValue("GLOBALS.HttpPort", "5000").c_str( );
		Globals.ResetAtMidnight  = config.GetSymbolValue("GLOBALS.ResetAtMidnight", "0").toNumber<int>( );
		GLogger.OutputConsole( ) = config.GetSymbolValue("GLOBALS.OutputConsole", "0").toNumber<int>( );

		Globals.shiftchanges =  config.GetSymbolValue("GLOBALS.SHIFTCHANGES", "06:00,15:00,23:00").c_str();


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
#if 0
std::string Nikon_device
	= "			 <Device id=\"####id\" uuid=\"####xxx\" name=\"NNNNNN\">\n"
	"			<Description>Nikon</Description>\n"
	"			<DataItems>\n"
	"				<DataItem id=\"####avail\" name=\"avail\"  type=\"AVAILABILITY\" category=\"EVENT\"/>\n"
	"			</DataItems>\n"
	"			<Components>\n"
	"				<Axes id=\"####a1\" name=\"base\">\n"
	"					<Components>\n"
	"						<Rotary id=\"####d1\" name=\"SPINDLE\">\n"
	"							<DataItems>\n"
	"								<DataItem type=\"SPINDLE_SPEED\" id=\"####cs1\" category=\"SAMPLE\" name=\"Srpm\"  units=\"REVOLUTION/MINUTE\" nativeUnits=\"REVOLUTION/MINUTE\"/>\n"
	"								<DataItem type=\"SPINDLE_SPEED\" id=\"####cso1\" category=\"SAMPLE\" name=\"Sovr\" subType=\"OVERRIDE\" units=\"PERCENT\" nativeUnits=\"PERCENT\"/>\n"
	"							</DataItems>\n"
	"						</Rotary>\n"
	"						<Linear id=\"####x1\" name=\"X\">\n"
	"							<DataItems>\n"
	"								<DataItem type=\"POSITION\" subType=\"ACTUAL\" id=\"####xp1\" category=\"SAMPLE\" name=\"Xabs\" units=\"MILLIMETER\" nativeUnits=\"MILLIMETER\" coordinateSystem=\"MACHINE\"/>\n"
	"							</DataItems>\n"
	"						</Linear>\n"
	"						<Linear id=\"####y1\" name=\"Y\">\n"
	"							<DataItems>\n"
	"								<DataItem type=\"POSITION\" subType=\"ACTUAL\" id=\"####yp1\" category=\"SAMPLE\" name=\"Yabs\" units=\"MILLIMETER\" nativeUnits=\"MILLIMETER\" coordinateSystem=\"MACHINE\"/>\n"
	"							</DataItems>\n"
	"						</Linear>\n"
	"						<Linear id=\"####z1\" name=\"Z\">\n"
	"							<DataItems>\n"
	"								<DataItem type=\"POSITION\" subType=\"ACTUAL\" id=\"####zp1\" category=\"SAMPLE\" name=\"Zabs\" units=\"MILLIMETER\" nativeUnits=\"MILLIMETER\" coordinateSystem=\"MACHINE\"/>\n"
	"							</DataItems>\n"
	"						</Linear>\n"
	"					</Components>\n"
	"				</Axes>\n"
	"				<Controller id=\"####cont\" name=\"controller\">\n"
	"					<Components>\n"
	"						<Path id=\"####path1\" name=\"path\">\n"
	"							<DataItems>\n"
	"								<DataItem type=\"CODE\" id=\"####alarm\" category=\"EVENT\" name=\"alarm\"/>\n"
	"								<DataItem type=\"PROGRAM\" id=\"####pgm\" category=\"EVENT\" name=\"program\"/>\n"
	"								<DataItem type=\"EXECUTION\" id=\"####exec\" category=\"EVENT\" name=\"execution\"/>\n"
	"								<DataItem type=\"CONTROLLER_MODE\" id=\"####mode\" category=\"EVENT\" name=\"controllermode\"/>\n"
	"								<DataItem type=\"PATH_FEEDRATE\" id=\"####pfo1\" category=\"SAMPLE\" name=\"path_feedrateovr\" units=\"PERCENT\" nativeUnits=\"PERCENT\" subType=\"OVERRIDE\" />\n"
	"                                   <DataItem category=\"EVENT\"  id=\"####alarm\"  name=\"alarm\" type=\"CODE\"></DataItem>\n"
	"                                   <DataItem category=\"EVENT\"  id=\"####operator\"  name=\"operator\" type=\"CODE\"></DataItem>\n"
	"                                   <DataItem category=\"EVENT\"  id=\"####heartbeat\"  name=\"heartbeat\" type=\"CODE\"></DataItem>\n"
	"                           </DataItems>\n"
	"						</Path>\n"
	"					</Components>\n"
	"				</Controller>\n"
	"				<Systems id=\"####systems\" name=\"systems\">\n"
	"					<Components>\n"
	"						<Electric name=\"electric\" id=\"####elec\">\n"
	"							<DataItems>\n"
	"								<DataItem type=\"POWER_STATE\" category=\"EVENT\" id=\"####pwr\" name=\"power\" />\n"
	"							</DataItems>\n"
	"						</Electric>\n"
	"					</Components>\n"
	"				</Systems>\n"
	"			</Components>\n"
	"		</Device>\n";

HRESULT AgentConfigurationEx::WriteDevicesFile (std::vector<std::string> devicenames, std::string xmlFile, std::string destFolder)
{
	std::string contents;

	contents += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	contents += "<MTConnectDevices xmlns=\"urn:mtconnect.org:MTConnectDevices:1.1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"urn:mtconnect.org:MTConnectDevices:1.1 http://www.mtconnect.org/schemas/MTConnectDevices_1.1.xsd\">\n";
	contents += StdStringFormat("<Header bufferSize=\"130000\" instanceId=\"1\" creationTime=\"%s\" sender=\"local\" version=\"1.1\"/>\n",
		GetTimeStamp( ).c_str( ) );
	contents += "<Devices>\n";

	// Check all machine names unique

	// Generate Devices.xml file with all devices in it.
	for ( UINT i = 0; i < devicenames.size( ); i++ )
	{
		std::string config = Nikon_device;
		ReplaceAll(config, "####", devicenames[i]);
		config    = ReplaceOnce(config, "name=\"NNNNNN\"", "name=\"" + devicenames[i] + "\"");
		contents += config + "\n";
	}
	contents += "</Devices>\n";
	contents += "</MTConnectDevices>\n";

	if ( !xmlFile.empty( ) )
	{
		WriteFile(destFolder + xmlFile, contents);
	}
	DWORD filesize;

	if ( FAILED(File.Size(destFolder + xmlFile, filesize) ) )
	{
		ErrMsg("Devices xml file write failed\n");
	}
	return 0;
}
LRESULT AgentConfigurationEx::WriteAgentCfgFile (std::string cfgfile, std::string xmlFile, std::string destFolder)
{
	// Generate agent.cfg file with all devices in it.
	std::string cfg;

	cfg  = "Devices = " + xmlFile + "\n";
	cfg += "ServiceName = " + Globals.ServerName + "\n"; // MTConnectAgent\n";
	cfg += "Port = " + Globals.HttpPort + "\n";

	cfg += "CheckpointFrequency=10000\n";
	cfg += "AllowPut=true\n";

	cfg += "Adapters \n{\n";

	// for(UINT i=0; i<devices.size(); i++)
	// {
	//	if(ports[i]!="0")
	//	{
	//		cfg+="\t"+names[i] + "\n";
	//		cfg+="\t{\n";
	//		cfg+="\t\tHost = " + ips[i] +  "\n";
	//		cfg+="\t\tPort = " + ports[i] + "\n";
	//		cfg+="\t\tDeviceXml = " + ExtractFiletitle(devices[i]) + "\n";
	//		cfg+="\t}\n";
	//	}
	// }
	cfg += "}\n";

	cfg += "# Logger Configuration\n";
	cfg += "logger_config\n";
	cfg += "{\n";
	cfg += "\tlogging_level = debug\n";
	cfg += "\toutput = cout\n";
	cfg += "}\n";

	if ( !cfgfile.empty( ) )
	{
		WriteFile(destFolder + cfgfile, cfg);
	}
	return 0;
}
#endif


bool AgentConfigurationEx::ResetAtMidnite ( )
{
	COleDateTime now   = COleDateTime::GetCurrentTime( );
#ifndef RESETTEST
	COleDateTime date2 = COleDateTime(now.GetYear( ), now.GetMonth( ), now.GetDay( ), 0, 0, 0) + COleDateTimeSpan(1, 0, 0, 1);
#else
	COleDateTime date2 =  now +  COleDateTimeSpan(0, 0, 2, 0); // testing reset time - 2 minutes
#endif
	COleDateTimeSpan tilmidnight = date2 - now;
	GLogger.Fatal(StdStringFormat("Agent will Reset at  %s\n", date2.Format("%A, %B %d, %Y %H:%M:%S")  ) );
	GLogger.Fatal(StdStringFormat("Agent will Reset %8.4f hours::min from now\n", ( tilmidnight.GetTotalSeconds( ) / 3600.00 ) ) );

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
#ifndef RESETTEST
		std::string cmd = StdStringFormat("cmd /c net stop \"%s\" & net start \"%s\"", Globals.ServerName.c_str( ), Globals.ServerName.c_str( ) ); // Command line
#else
		// This works when you run in from a command console as NikonAgent.exe debug, it will restart in the current DOS console.
		TCHAR buf[1000];
		GetModuleFileName(NULL, buf, 1000);
		std::string exepath = File.ExeDirectory();
		std::string exe = ExtractFilename(std::string(buf));
		//GLogger.Fatal(StdStringFormat("start Agent /d  \"%s\" /b \"%s\" debug\n", exepath.c_str(), buf  ) );
		std::string cmd = StdStringFormat("cmd /c taskkill /IM \"%s\" & \"%s\" debug", exe.c_str(),  buf ); // Command line
#endif

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
