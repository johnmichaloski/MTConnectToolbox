//
// MTCFanucAgentcpp : Defines the entry point for the console application.
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.
#define INTMAX_MAX   9223372036854775807i64 

#include "stdafx.h"
#include <windows.h>

#pragma comment( lib, "msxml2.lib" )
#pragma comment(lib,"psapi") 
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")

#define MTCLIBPATH(X)    "C:\\Users\\michalos\\Documents\\Visual Studio 2015\\Projects\\FanucAgentx64\\Agent\\win32\\libxml2-2.7.7\\lib\\" ## X
#define FANUCLIBPATH(X) "C:\\Users\\michalos\\Documents\\Visual Studio 2015\\Projects\\FanucAgentx64\\Fwlib64\\" ## X

#if defined( WIN64 ) && defined( _DEBUG )
#pragma message( "DEBUG x64" )
#pragma comment(lib, MTCLIBPATH("libxml2_64d.lib"))
#pragma comment(lib, FANUCLIBPATH("fwlib64.lib"))

#elif !defined( _DEBUG ) && defined( WIN64 )
#pragma message( "RELEASE x64" )
#pragma comment(lib,  MTCLIBPATH("libxml2_64.lib"))
#pragma comment(lib, FANUCLIBPATH("fwlib64.lib"))
#endif


#include "MTCFanucAgent.h"

#include "Shellapi.h"
#include <vector>
#include <string>
#include <thread>
#include "sys/stat.h"
#include "string.h"
#include "fcntl.h"

#include "agent.hpp"
#include "config.hpp"



#include "StdStringFcn.h"
#include "atlutil.h"


using namespace std;
using namespace dlib;


std::string ExeDirectory()
{
	TCHAR buf[1000];
	GetModuleFileName(NULL, buf, 1000);
	std::string path(buf);
	path = path.substr(0, path.find_last_of('\\') + 1);
	return path;
}
std::string nowtimestamp()
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	return  StrFormat("%4d-%02d-%02dT%02d:%02d:%02d.%04d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
}

static const char INI_FILENAME[] = "agent.cfg";
static const char LOG_FILENAME[] = "errors.log";

static int GetShiftTime(std::string s)
{
	int Hour, Minute;
	if (sscanf(s.c_str(), "%d:%d", &Hour, &Minute) == 2) {}
	else throw std::exception("Bad Shift time format -  need hh:mm\n");
	return Hour * 60 + Minute;
}

int AgentConfigurationEx::thread(int aArgc, const char *aArgv[])
{
	GLogger.Info(StrFormat("MTConnect Agent Service Started %s\n", LocalTimetamp().c_str()));

	try {
		char result[MAX_PATH];
		std::string path(result, GetModuleFileName(NULL, result, MAX_PATH));
		std::string cfgfile = path.substr(0, path.find_last_of(".")) + ".ini";
		std::string shiftchanges;
		if (GetFileAttributesA(cfgfile.c_str()) != INVALID_FILE_ATTRIBUTES)
		{
			config.load(cfgfile);

			//_devices = config.GetTokens("CONFIG.Devices", ",");  // Device name - fanuc something

			Globals.fanucips = TrimmedTokenize(config.GetSymbolValue("CONFIG.FanucIpAddress", "127.0.0.1").c_str(), ",");
			Globals.configs = TrimmedTokenize(config.GetSymbolValue("CONFIG.Configs", "xyzabcuv").c_str(), ",");
			Globals.FanucPort = config.GetSymbolValue("CONFIG.FanucPort", "0").toNumber<int>();
			//Globals.fanucports=TrimmedTokenize(config.GetSymbolValue("CONFIG.ipaddrs", "127.0.0.1").c_str(),",");
			Globals._bResetAtMidnight = config.GetSymbolValue("CONFIG.ResetAtMidnight", "1").toNumber<int>();

			//Globals.FanucIpAddress=config.GetSymbolValue("CONFIG.FanucIpAddress", "127.0.0.1").c_str();

			_devicefile = config.GetSymbolValue("CONFIG.DevicesFile", "Devices.xml").c_str();
			_cfgfile = config.GetSymbolValue("CONFIG.CfgFile", "Agent.cfg").c_str();
			_httpPort = config.GetSymbolValue("CONFIG.HttpPort", "80").toNumber<int>();
			_agentcfg._serviceName = config.GetSymbolValue("CONFIG.ServiceName", "MTCFanucAgent").c_str();

			_agentcfg.SetHttpPort(_httpPort);
			_agentcfg.SetXmlFile(_devicefile);
			_agentcfg.SetCfgFile(_cfgfile);
			_agentcfg.SetDestinationFolder(::ExeDirectory());

			SetCurrentDirectory(::ExeDirectory().c_str()); // fixes Agent file lookup issue

			GLogger.DebugLevel() = config.GetSymbolValue("CONFIG.DebugLevel", "0").toNumber<int>();
			GLogger.Timestamping() = true;

			Globals.MachineToolConfig = config.GetSymbolValue("CONFIG.MachineToolConfig", "xyz").c_str();
			Globals.FanucProtocol = config.GetSymbolValue("CONFIG.Protocol", "HSSB").c_str();
			Globals.FanucVersion = config.GetSymbolValue("CONFIG.FanucVersion", "iSeries").c_str();

			Globals.FocasDelay = config.GetSymbolValue("CONFIG.FocasDelay", "2000").toNumber<int>();
			Globals.AutoVersionDetect = config.GetSymbolValue("CONFIG.AutoVersionDetect", "1000").toNumber<int>();
			Globals.szFanucExe = config.GetSymbolValue("CONFIG.FanucExe", "CNCScr.exe").c_str();

			// New shift changes - Mon 04/22/13 03:23:06 PM
			shiftchanges = config.GetSymbolValue("CONFIG.SHIFTCHANGES", "06:00,15:00,23:00").c_str();

		}

		std::vector<std::string> shifttimes = TrimmedTokenize(shiftchanges, ",", true);
		for (int i = 0; i < shifttimes.size(); i++)
		{
			_shiftchanges.push_back(GetShiftTime(shifttimes[i]));
		}
		//if(_shiftchanges.size() < 1)
		//	throw std::exception("Need at least 1 shift break");

		// shift error checking: i>=1 for all i, shift i+1 must be > shift i

		GLogger.Fatal(StrFormat("DebugLevel=%d\n", GLogger.DebugLevel()));
		GLogger.Fatal(StrFormat("Protocol=%s\n", Globals.FanucProtocol.c_str()));
		GLogger.Fatal(StrFormat("ScanDelay=%d\n", Globals.FocasDelay));
		GLogger.Fatal(StrFormat("FanucPort=%d\n", Globals.FanucPort));
		//GLogger.Fatal(StrFormat("FanucIpAddress=" , Globals.FanucIpAddress ))";
		GLogger.Fatal(StrFormat("FocasDelay=%d\n", Globals.FocasDelay));
		GLogger.Fatal(StrFormat("ResetAtMidnight=%d", Globals._bResetAtMidnight));


		//std::string cfgfile, std::string xmlFile, std::string destFolder
		//_agentcfg.WriteAgentCfgFile(_cfgfile,_devicefile,::ExeDirectory());

		//mAgent = new AgentEx();

		for (int i = 0; i < Globals.fanucips.size(); i++)
		{
			std::string config;
			if (Globals.configs.size() > i)
				config = Globals.configs[i];
			else
				config = "xyzabcuv";
			_agentcfg.AddFanucDevice("FanucCNC" + ConvertToString(i + 1), config); // changed 3/29/2013
		}

		_agentcfg.WriteDevicesFile(_devicefile, ::ExeDirectory());
		_agentcfg.WriteCfgFile(_cfgfile, _devicefile, ::ExeDirectory());

	}
	catch (...)
	{
		GLogger.Fatal(StrFormat("MTConnect Agent Service Configuration Failed %s\n", nowtimestamp().c_str()));
		return -1;

	}
	MTConnectService::main(aArgc, (const char **)aArgv);

	return 0;
}

void AgentConfigurationEx::join_all() {
	for (auto& thread : _group)
		if (thread.joinable())
			thread.join();
}

// Start the server. This blocks until the server stops.
void AgentConfigurationEx::start()
{


	if (Globals._bResetAtMidnight)
	{
		COleDateTime now = COleDateTime::GetCurrentTime();
		COleDateTimeSpan tilnextreset = COleDateTimeSpan(0, 1, 0, 0);;
#if 0
		COleDateTime today = COleDateTime(now.GetYear(), now.GetMonth(), now.GetDay(), 0, 0, 0);
		COleDateTime tomorrow = COleDateTime(now.GetYear(), now.GetMonth(), now.GetDay() + 1, 0, 0, 0);
		//COleDateTime date2 =  COleDateTime(now.GetYear(), now.GetMonth(), now.GetDay(), 0, 0, 0)
		//	+  COleDateTimeSpan(1, 0, 0, 1);
		//COleDateTimeSpan tilmidnight = date2-now;
		//	COleDateTimeSpan tilmidnight = date2-now;
		//COleDateTime date2 =  now +  COleDateTimeSpan(0, 0, 2, 0); // testing reset time - 2 minutes
		COleDateTimeSpan dt3 = COleDateTimeSpan(1, 0, 0, 1);
		COleDateTimeSpan dt4 = dt3; //(0, 0, _shiftchanges[1], 1);
		COleDateTimeSpan dt5 = dt3; // (0, 0, _shiftchanges[2], 1);
		COleDateTimeSpan dt6 = dt3; // (0, 0, _shiftchanges[2], 1);


		if (_shiftchanges.size() < 1)
			dt3 = COleDateTimeSpan(1, 0, 0, 1);
		if (_shiftchanges.size() >= 1)
			dt3 = COleDateTimeSpan(0, 0, _shiftchanges[0], 0);
		if (_shiftchanges.size() >= 2)
			dt4 = COleDateTimeSpan(0, 0, _shiftchanges[1], 0);
		if (_shiftchanges.size() >= 3)
			dt5 = COleDateTimeSpan(0, 0, _shiftchanges[2], 0);

		COleDateTime date3 = today + dt3;
		COleDateTime date4 = today + dt4;
		COleDateTime date5 = today + dt5;
		COleDateTime date6 = tomorrow + dt3;

		COleDateTimeSpan tilnextreset = date6;

		if (date6 > now)
			tilnextreset = date6 - now;
		if (date5 > now)
			tilnextreset = date5 - now;
		if (date4 > now)
			tilnextreset = date4 - now;
		if (date3 > now)
			tilnextreset = date3 - now;
#endif
		GLogger.Fatal(StrFormat("Agent will Reset from now  %8.4f\n", (tilnextreset.GetTotalSeconds() / 3600.00)));

		_resetthread.Initialize();

		// Call this method to add a periodic waitable timer to the list maintained by the worker thread.
		// Parameters
		/*	dwInterval	Specifies the period of the timer in milliseconds.
		pClient	The pointer to the IWorkerThreadClient interface on the object to be called when the handle is signaled.
		dwParam	The parameter to be passed to IWorkerThreadClient::Execute when the handle is signaled.
		phTimer	[out] Address of the HANDLE variable that, on success, receives the handle to the newly created timer.
		Return Value Returns S_OK on success, or an error HRESULT on failure.
		*/

		_resetthread.AddTimer(
			(long)tilnextreset.GetTotalSeconds() * 1000,
			&_ResetThread,
			(DWORD_PTR) this,
			&_ResetThread._hTimer  // stored newly created timer handle
		);

	}
	CCmdHandler * _cmdHandler;

	for (int i = 0; i < Globals.fanucips.size(); i++)
	{
		_cmdHandler = new CCmdHandler(this);
		_cmdHandlers.push_back(_cmdHandler);
		_cmdHandler->Configure(config, "FanucCNC" + ConvertToString(i + 1), Globals.fanucips[i], ConvertToString(Globals.FanucPort));
		//		_group.create_thread(std::bind(&CCmdHandler::Cycle, _cmdHandler));
		_group.emplace_back(std::thread(std::bind(&CCmdHandler::Cycle, _cmdHandler)));
	}
	AgentConfiguration::start();
}
void AgentConfigurationEx::stop()
{
	for (int i = 0; i < _cmdHandlers.size(); i++)
		_cmdHandlers[i]->Stop();

	join_all();
	//_group.join_all();
	AgentConfiguration::stop();
}


#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#pragma warning(disable: 4247) //warning C4297: 'WinMain' : function assumed not to throw an exception but does

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{

	AgentConfigurationEx config;

	//READY, ACTIVE, INTERRUPTED, or STOPPED 
	// Version 0.9 EXECUTION IDLE, EXECUTING, or PAUSED
	//Adapter::keymapping["execution.READY"]="IDLE";
	//Adapter::keymapping["execution.ACTIVE"]="EXECUTING";
	//Adapter::keymapping["execution.INTERRUPTED"]="PAUSED";
	//Adapter::keymapping["execution.STOPPED"]="PAUSED";
	//Adapter::keymapping["mode"]="controllermode";
	//Adapter::keymapping["Fovr"]="path_feedrateovr";
	//Adapter::keymapping["Fact"]="path_feedratefrt";

	//_CrtSetBreakAlloc(310); // if no  break, then must be "static" allocated beforehand
	try
	{
		GLogger.Open();
		config.configfile() = ExeDirectory() + INI_FILENAME;

		//config.configureLogger();
		GLogger.Info(StrFormat("MTConnect Agent Service Started %s\n", LocalTimetamp().c_str()));
		if (FAILED(::CoInitialize(NULL)))
			throw  std::exception("CoInitialize failed\n");

		HRESULT hr = ::CoInitializeSecurity(NULL, //Points to security descriptor 
			-1, //Count of entries in asAuthSvc 
			NULL, //Array of names to register 
			NULL, //Reserved for future use 
			RPC_C_AUTHN_LEVEL_NONE, //The default authentication //level for proxies 
			RPC_C_IMP_LEVEL_IDENTIFY, //The default impersonation //level for proxies 
			NULL, //Reserved; must be set to  NULL
			EOAC_NONE, //Additional client or //server-side capabilities 
			NULL //Reserved for future use 
		);

		if (FAILED(hr))
			throw std::exception("CoInitializeSecurity failed\n");
		SetCurrentDirectory(ExeDirectory().c_str());

		//std::cout.rdbuf(&DebugLogger);
		//std::cerr.rdbuf(&DebugLogger);
		//DebugBreak();

		config.thread(__argc, (const char **)__argv);

		GLogger.Fatal(StrFormat("MTConnect Agent Service Aborted %s\n", nowtimestamp().c_str()));
		//::ExitProcess(0);
		//SetErrorMode(GetErrorMode () | SEM_NOGPFAULTERRORBOX);  // vista
		_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
		//abort();
		::TerminateProcess(::GetCurrentProcess(), -1); // Zero is the exit code
		throw std::exception("Done");

		//// Main message loop:
		//MSG msg;
		//while (GetMessage(&msg, NULL, 0, 0))
		//{
		//	if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
		//	{
		//		TranslateMessage(&msg);
		//		DispatchMessage(&msg);
		//	}
		//}
	}
	catch (std::exception e)
	{
		OutputDebugString(e.what());
		GLogger.Fatal(StrFormat("%s\n", e.what()));
	}
	catch (...)
	{
		OutputDebugString("Service terminated abnormally in main\n");
		GLogger.Fatal(StrFormat("Service terminated abnormally in main\n"));
	}
	GLogger.Info(StrFormat("MTConnect Agent Service Stopped%s\n", LocalTimetamp().c_str()));
	return -1;
}

HRESULT AgentConfigurationEx::CResetThread::Execute(DWORD_PTR dwParam, HANDLE hObject)
{
	static char name[] = "CResetThread::Execute";

	AgentConfigurationEx * agent = (AgentConfigurationEx*)dwParam;
	CancelWaitableTimer(hObject);
	//DebugBreak();

	try {
		GLogger.Fatal(StrFormat("MTConnect adapter Service Start Resetting %s\n", COleDateTime::GetCurrentTime().Format("%H:%M:%S")));
		//::CopyFile((::ExeDirectory () + "debug.txt").c_str() , (::ExeDirectory () + "debug1.txt").c_str(), FALSE);

		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));

		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;	 // set the window display to HIDE	

		// SCM reset command of this service 
		std::string cmd = StrFormat("cmd /c net stop \"%s\" & net start \"%s\"", agent->name().c_str(), agent->name().c_str());        // Command line

		if (!::CreateProcess(NULL,   // No module name (use command line)
			const_cast<char *>(cmd.c_str()),
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi))           // Pointer to PROCESS_INFORMATION structure
			ReportError("CreateProcess ");

		::Sleep(5000); // make sure process has spawned before killing thread
	}
	catch (...)
	{
		GLogger.Fatal(StrFormat("Exception  - ResetAtMidnightThread(void *oObject)\n"));
	}
	return S_OK;
}

#include <strsafe.h>
void ReportError(char * lpszFunction)
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
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);

	GLogger.Fatal(StrFormat("%s\n", (char *)lpDisplayBuf));

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}