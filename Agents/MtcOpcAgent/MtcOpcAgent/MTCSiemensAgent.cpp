//
// MTCFanucAgentcpp : Defines the entry point for the console application.
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.
#include "stdafx.h"


#define MTCLIBPATH(X) "C:\\Users\\michalos\\Documents\\GitHub\\Agents\\MtcOpcAgent\\Agent\\win32\\libxml2-2.7.7\\lib\\"##X

#if defined(WIN64) && defined( _DEBUG) 
#pragma message( "DEBUG x64" )
#pragma comment(lib, MTCLIBPATH("libxml2_64d.lib"))
#pragma comment(lib, "libboost_system-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_thread-vc100-mt-sgd-1_54.lib")

#elif !defined( _DEBUG) && defined(WIN64)
#pragma message( "RELEASE x64" )
#pragma comment(lib,  MTCLIBPATH("libxml2_64.lib"))
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")

#elif defined(_DEBUG) && defined(WIN32)
#pragma message( "DEBUG x32" )
//#pragma message( MTCLIBPATH("libxml2d.lib") )
#pragma comment(lib, MTCLIBPATH("libxml2d.lib"))
#pragma comment(lib, "libboost_thread-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-sgd-1_54.lib")
#elif !defined( _DEBUG) && defined(WIN32)
#pragma message( "RELEASE x32" )
#pragma comment(lib, MTCLIBPATH("libxml2.lib"))
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")
#endif

#pragma comment( lib, "msxml2.lib" )
#pragma comment(lib,"psapi") 
#pragma comment(lib, "Ws2_32.lib")

#include "MTCSiemensAgent.h"
#include "ATLComTime.h"
#include "Shellapi.h"
#include <vector>
#include <string>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include "sys/stat.h"
#include "string.h"
#include "fcntl.h"

#include "StdStringFcn.h"
#include "File.h"
#include "Globals.h"

#include "agent.hpp"
#include "config.hpp"
//#include "dlib/config_reader.h"
//#include <dlib/logger.h>





using namespace std;
using namespace dlib;


//static logger sLogger("main");
//static   const log_level LENTRY(400,"ENTRY");

static const char INI_FILENAME[] = "agent.cfg";
static const char LOG_FILENAME[] = "errors.log";


std::string opc_device = 
"			 <Device id=\"####id\" uuid=\"####xxx\" name=\"NNNNNN\">\n"
"			<Description>MTConnect Siemens 840D Device Using OPC Connect as Backend</Description>\n"
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
"								<DataItem category=\"SAMPLE\" id=\"####cl3\" name=\"Slod_percent\" nativeUnits=\"PERCENT\" type=\"LOAD\" units=\"PERCENT\"/>\n"
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
"								<DataItem type=\"PART_COUNT\" id=\"####pc1\" category=\"EVENT\" name=\"PartCountAct\" />\n"
"								<DataItem type=\"PATH_FEEDRATE\" id=\"####pf1\" category=\"SAMPLE\" name=\"path_feedratefrt\" units=\"MILLIMETER/SECOND\" subType=\"ACTUAL\" coordinateSystem=\"WORK\"/>\n"
"								<DataItem type=\"TOOL_ID\" id=\"####tid1\" category=\"EVENT\" name=\"Tool_number\"/>\n"
"								<DataItem type=\"CODE\" id=\"####alarm\" category=\"EVENT\" name=\"alarm\"/>\n"
"								<DataItem type=\"PROGRAM\" id=\"####pgm\" category=\"EVENT\" name=\"program\"/>\n"
"								<DataItem type=\"EXECUTION\" id=\"####exec\" category=\"EVENT\" name=\"execution\"/>\n"
"								<DataItem type=\"CONTROLLER_MODE\" id=\"####mode\" category=\"EVENT\" name=\"controllermode\"/>\n"
"								<DataItem type=\"PATH_FEEDRATE\" id=\"####pfo1\" category=\"SAMPLE\" name=\"path_feedrateovr\" units=\"PERCENT\" nativeUnits=\"PERCENT\" subType=\"OVERRIDE\" />\n"
"        							<DataItem category=\"EVENT\"  id=\"####alarm\"  name=\"alarm\" type=\"CODE\"></DataItem>\n"
"        							<DataItem category=\"EVENT\"  id=\"####heartbeat\"  name=\"heartbeat\" type=\"CODE\"></DataItem>\n"
" 							</DataItems>\n"
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

//#include "jsmtconnect.txt"

//class AgentEx : public Agent
//{
//public:
//	std::string contents; 
//	AgentEx() 
//	{
//	}
//	virtual std::string handleExtensionCall(const std::string& call, const std::string& device)
//	{ 
//		if(call=="dump")
//			_CrtDumpMemoryLeaks();
//		return monitorHtml ; 
//	}
//};

AgentConfigurationEx::AgentConfigurationEx()
{		
	//setAgent(new AgentEx());
} 

void AgentConfigurationEx::initialize(int aArgc, const char *aArgv[])
{
	GLogger.LogMessage(StdStringFormat("AgentConfigurationEx::initialize() \n"),DBUG);
	try{
		std::string path(File.ExeDirectory()+"MtcOpcAgent.ini");
		std::string cfgfile = File.ExeDirectory()+"MtcOpcAgent.ini"; // path.substr(0,path.find_last_of(".")) + ".ini";
		GLogger.LogMessage(StdStringFormat("TConnect Agent Service Read Configuration - %s \n",cfgfile.c_str()), DBUG);
		if(GetFileAttributesA(cfgfile.c_str())!= INVALID_FILE_ATTRIBUTES)
		{
			config.load( cfgfile );
			Globals.ServerName=config.GetSymbolValue("Agent.ServiceName", "MtcOpcAgent").c_str();
			MTConnectService::setName(Globals.ServerName);
			Globals.HttpPort=config.GetSymbolValue("Agent.HttpPort", 5000).toNumber<int>() ;
			getAgent()->set_listening_port(Globals.HttpPort);

			Globals.Delay=config.GetSymbolValue("Agent.ScanDelay", 1000).toNumber<int>() ;

			std::string sLevel = config.GetSymbolValue("logger_config.logging_level", "FATAL").c_str();
			sLevel=MakeUpper(sLevel);
			Globals.Debug = (sLevel=="FATAL")? 0 : (sLevel=="ERROR") ? 1 : (sLevel=="WARN") ? 2 : (sLevel=="INFO")? 3 : 5;
			Globals.DbgConsole= config.GetSymbolValue("OPCSERVER.DbgConsole", 0).toNumber<int>() ;
			Globals.ResetAtMidnight	= config.GetSymbolValue("OPCSERVER.ResetAtMidnight", 0).toNumber<int>() ;


			GLogger.LogMessage(StdStringFormat("MTConnect Agent Service Read OPCSERVER.logging_level %s\n",sLevel.c_str()),DBUG);

			_ipaddrs = config.GetTokens("OPCSERVER.ServerMachineName", ",");
			_devices = config.GetTokens("OPCSERVER.MTConnectDevice", ",");
			_tags = config.GetTokens("OPCSERVER.OpcTags", ",");

			// Unused - TODO: Modify agent.cfg based on ini settings
			//_devicefile =  config.GetSymbolValue("OPCSERVER.DevicesFile", "Devices.xml").c_str();
			//_cfgfile =  config.GetSymbolValue("OPCSERVER.CfgFile","Agent.cfg").c_str();

			if(_ipaddrs.size() != _devices.size() && _ipaddrs.size() != _tags.size())
				throw std::exception("OPC Number of Server IPs does not match number of MTConnect devices or Section Tags" );

			Globals.Dump();
			GLogger.LogMessage(StdStringFormat("MTConnect Agent Service Read Configuration DONE! \n"),DBUG);

		}
#ifdef WRITENEWDEVICESFILE
		WriteDevicesFile(_devices, opc_device,  "Devices.xml", File.ExeDirectory());
#endif
	}
	catch(std::exception err) 
	{
		GLogger.LogMessage(StdStringFormat("MTConnect Agent Service Configuration Failed %s\n", err.what()));

	}
	catch(...) 
	{
		GLogger.LogMessage(StdStringFormat("MTConnect Agent Service Configuration Failed \n"));

	}
	GLogger.LogMessage(StdStringFormat("Call MTConnect Agent AgentConfiguration::initialize DONE\n"), DBUG);
	AgentConfiguration::initialize( aArgc,  aArgv);
}

int AgentConfigurationEx::main(int aArgc, const char *aArgv[])
{
	GLogger.LogMessage(StdStringFormat("MTConnect Agent Service Started\n"), DBUG);
	SetCurrentDirectory(File.ExeDirectory().c_str()); // fixes Agent file lookup issue

	MTConnectService::main( aArgc, (const char **) aArgv);

	return 0;
}
// Start the server. This blocks until the server stops.
void AgentConfigurationEx::start()
{
	GLogger.LogMessage(StdStringFormat("AgentConfigurationEx::start()\n"));
	getAgent()->set_listening_port(Globals.HttpPort);
	MTConnectService::setName(Globals.ServerName);

	for(int i=0; i< _ipaddrs.size(); i++)
	{
		COpcAdapter *  _cmdHandler = new COpcAdapter(this, config, _ipaddrs[i], _devices[i], _tags[i]);
		_cncHandlers.push_back(_cmdHandler);
		GLogger.LogMessage(StdStringFormat("AgentConfigurationEx::start COpcAdapter::Cycle() %x\n",_ipaddrs[i]), DBUG);
		_group.create_thread(boost::bind(&COpcAdapter::Cycle, _cncHandlers[i]));
	}
	GLogger.LogMessage(StdStringFormat("Call AgentConfiguration::start() ed \n"));

	if(Globals.ResetAtMidnight)
	{
		COleDateTime now = COleDateTime::GetCurrentTime();
		COleDateTime date2 =  COleDateTime(now.GetYear(), now.GetMonth(), now.GetDay(), 0, 0, 0) +  COleDateTimeSpan(1, 0, 0, 1);
		//COleDateTime date2 =  now +  COleDateTimeSpan(0, 0, 2, 0); // testing reset time - 2 minutes
		COleDateTimeSpan tilmidnight = date2-now;
		_resetthread.Initialize();
		_resetthread.AddTimer(
			(long) tilmidnight.GetTotalSeconds() * 1000,
			&_ResetThread,
			(DWORD_PTR) this,
			&_ResetThread._hTimer  // stored newly created timer handle
			) ;

		GLogger.LogMessage(StdStringFormat("Agent will Reset At Midnight %s \n", (LPCSTR) date2.Format()), DBUG);
	}

	AgentConfiguration::start(); // does not return
}
void AgentConfigurationEx::stop()
{
	for(int i=0; i< _cncHandlers.size(); i++)
		_cncHandlers[i]->Stop();

	// Could take 10 seconds if waitingn for OPC connections.
	_group.join_all();

	AgentConfiguration::stop();
}

static std::string GetTimeStamp()
{
	char aBuffer[256];
	SYSTEMTIME st;
	GetSystemTime(&st);
	sprintf(aBuffer, "%4d-%02d-%02dT%02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, 
		st.wMinute, st.wSecond);
	return aBuffer;
}

HRESULT	AgentConfigurationEx::WriteDevicesFile(std::vector<std::string> devicenames, std::string xml,  std::string xmlFile, std::string destFolder)
{
	std::string contents;
	contents+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	contents+="<MTConnectDevices xmlns=\"urn:mtconnect.org:MTConnectDevices:1.1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"urn:mtconnect.org:MTConnectDevices:1.1 http://www.mtconnect.org/schemas/MTConnectDevices_1.1.xsd\">\n";
	contents+=StdStringFormat("<Header bufferSize=\"130000\" instanceId=\"1\" creationTime=\"%s\" sender=\"local\" version=\"1.1\"/>\n",
		GetTimeStamp().c_str());
	contents+="<Devices>\n";

	// Check all machine names unique

	// Generate Devices.xml file with all devices in it.
	for(UINT i=0; i<devicenames.size(); i++)
	{
		std::string config = xml; 
		ReplaceAll(config,"####", devicenames[i]); 
		config=ReplaceOnce(config,"name=\"NNNNNN\"","name=\""+devicenames[i]+"\"");
		contents+=config+"\n";
	}
	contents+="</Devices>\n";
	contents+="</MTConnectDevices>\n";

	if(!xmlFile.empty())
		WriteFile(destFolder + "\\" + xmlFile  , contents);
	return 0;
}
#include <strsafe.h>
static void ReportError(LPTSTR lpszFunction) 
{ 
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError(); 

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
	StringCchPrintf((LPTSTR)lpDisplayBuf, 
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"), 
		lpszFunction, dw, lpMsgBuf); 

	GLogger.LogMessage(StdStringFormat("%s\n",(LPCSTR) lpDisplayBuf));

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw); 
}


HRESULT AgentConfigurationEx::CResetThread::Execute(DWORD_PTR dwParam, HANDLE hObject)
{
	static char name[] = "CResetThread::Execute";

	AgentConfigurationEx * agent = (AgentConfigurationEx*) dwParam;
	CancelWaitableTimer(hObject);
	//DebugBreak();

	try {
		GLogger.LogMessage(StdStringFormat("MTConnect adapter Service Start Resetting \n"));

		PROCESS_INFORMATION pi;
		ZeroMemory( &pi, sizeof(pi) );

		STARTUPINFO si;
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;	 // set the window display to HIDE	

		// SCM reset command of this service 
		std::string cmd = StdStringFormat("cmd /c net stop \"%s\" & net start \"%s\"", agent->name().c_str(), agent->name().c_str());        // Command line

		if(!::CreateProcess( NULL,   // No module name (use command line)
			const_cast<char *>(cmd.c_str()),
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi ))           // Pointer to PROCESS_INFORMATION structure
			ReportError("CreateProcess ") ;

		::Sleep(5000); // make sure process has spawned before killing thread
	}
	catch(...)
	{
		GLogger.LogMessage(StdStringFormat("Exception  - ResetAtMidnightThread(void *oObject) \n"));
	}
	return S_OK;
}