//
// MTCFanucAgentcpp : Defines the entry point for the console application.
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.
#include "stdafx.h"
#include <windows.h>
#include "MTCFanucAdapter.h"
#include "ATLComTime.h"
#include "Shellapi.h"
#include <vector>
#include <string>

#include "sys/stat.h"
#include "string.h"
#include "fcntl.h"

#include "StdStringFcn.h"
#include "atlutil.h"
#include "logger.hpp"

#pragma comment( lib, "msxml2.lib" )
#pragma comment(lib,"psapi") 
#pragma comment(lib, "Ws2_32.lib")

#ifdef F15i
#include "FS15D.h"
#endif

#ifdef F15M	
#include "F15B.h"
#endif

#ifdef iSERIES
#include "iSeries.h"
#endif

int FanucShdrAdapter::mDebug;
FanucShdrAdapter * _mainadapter;
static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg =  StdStringFormat("In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode);
	OutputDebugString(errmsg.c_str() );
	throw std::exception(errmsg.c_str() , pExp->ExceptionRecord->ExceptionCode);
} 


static int GetShiftTime(std::string s)
{
	int Hour, Minute;
	if(sscanf(s.c_str(), "%d:%d", &Hour, &Minute)==2){}
	else throw std::exception("Bad Shift time format -  need hh:mm");
	return Hour * 60 + Minute;
}
bool FanucShdrAdapter::ResetAtMidnite()
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
	return true;

}
HRESULT FanucShdrAdapter::CResetThread::Execute(DWORD_PTR dwParam, HANDLE hObject)
{
	static char name[] = "CResetThread::Execute";

	FanucShdrAdapter * adapter = (FanucShdrAdapter*) dwParam;
	CancelWaitableTimer(hObject);
	//DebugBreak();

	try {

		PROCESS_INFORMATION pi;
		ZeroMemory( &pi, sizeof(pi) );

		STARTUPINFO si;
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;	 // set the window display to HIDE	

		// SCM reset command of this service 
		std::string cmd = StdStringFormat("cmd /c net stop \"%s\" & net start \"%s\"", adapter->name(), adapter->name());        // Command line

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
			AtlTrace("CreateProcess FAIL ") ;

		::Sleep(5000); // make sure process has spawned before killing thread
	}
	catch(...)
	{
		adapter->AbortMsg("Exception  - ResetAtMidnightThread(void *oObject");
	}
	return S_OK;
}

FanucShdrAdapter::FanucShdrAdapter(int aPort, int aSleep): Adapter(aPort, aSleep) 
,mAvail("avail")
,mPower("power")
{
#ifdef F15M	
	_F15M = new CF15B(this);
#endif
#ifdef F15i	
	_FS15D = new CFS15D(this);
#endif
#ifdef iSERIES	
	_iSeries = new CiSeries(this);
#endif
	mSleep=1000;
	heartbeat=0;
	mConnected=false;
	mFlibhndl=0;
	//mDeviceIP=Globals.FanucIpAddress;

#ifdef iSERIESLAN
	mDevicePort=8193;
	Globals.FanucProtocol="LAN";
#else
	Globals.FanucProtocol="HSSB";
	mDevicePort=0;
#endif

	addDatum(mPower);
	addDatum(mAvail);
	mAvail.available();
}

HRESULT FanucShdrAdapter::Configure()
{
	gLogger->debug(StdStringFormat("MTConnect Adapter  Configure %s" , LocalTimetamp().c_str()).c_str());

	try{
		crp::Config config;
		char result[ MAX_PATH ];
		std::string path( result, GetModuleFileName( NULL, result, MAX_PATH ) );
		std::string cfgfile = path.substr(0,path.find_last_of(".")) + ".ini";
		std::string shiftchanges;
		if( GetFileAttributesA(cfgfile.c_str())!= INVALID_FILE_ATTRIBUTES)
		{
			config.load( cfgfile );

			//_devices = config.GetTokens("CONFIG.Devices", ",");  // Device name - fanuc something

			Globals.configs=TrimmedTokenize(config.GetSymbolValue("CONFIG.Configs", "xyzabcuv").c_str(),",");
			Globals.FanucIpAddress=config.GetSymbolValue("CONFIG.FanucIpAddress", "127.0.0.2").c_str();
			Globals.FanucPort=config.GetSymbolValue("CONFIG.FanucPort", "0").toNumber<int>();

			Globals._bResetAtMidnight = config.GetSymbolValue("CONFIG.ResetAtMidnight", "0").toNumber<int>();
			Globals.FocasDelay = config.GetSymbolValue("CONFIG.FocasDelay", "5000").toNumber<int>();	
			Globals.ProgramLogic= config.GetSymbolValue("CONFIG.ProgramLogic", "").c_str();
			Globals.nAlarmFlag = config.GetSymbolValue("CONFIG.AlarmFlag", "1").toNumber<int>();	
			Globals.nAxesLoadFlag = config.GetSymbolValue("CONFIG.AxesLoadFlag", "0").toNumber<int>();	
			Globals.nToolingFlag = config.GetSymbolValue("CONFIG.ToolingFlag", "0").toNumber<int>();	

			//Globals.FanucProtocol= config.GetSymbolValue("CONFIG.FanucProtocol", "HSSB").c_str();
				
			Globals.szServiceName= config.GetSymbolValue("CONFIG.ServiceName", "MTConnectFanucAdapter").c_str();
			Globals.ShdrPort=config.GetSymbolValue("CONFIG.ShdrPort", "7878").toNumber<int>();
			Globals.szDeviceName= config.GetSymbolValue("CONFIG.DeviceName", "").c_str();

			_mainadapter->setName(Globals.szServiceName.c_str());
			_mainadapter->mScanDelay=Globals.FocasDelay;
			this->mDevicePort=Globals.ShdrPort;

			int debuglevel=config.GetSymbolValue("CONFIG.DebugLevel", "0").toNumber<int>();
			switch (debuglevel)
			{
			case 0: gLogger->setLogLevel(Logger::eERROR);
				break;
			case 1:  gLogger->setLogLevel(Logger::eWARNING);
				break;
			case 2:  gLogger->setLogLevel(Logger::eINFO);
				break;
			default:
				gLogger->setLogLevel(Logger::eDEBUG);
			}

			gLogger->info(StdStringFormat( "DeviceName=%s\n" , Globals.szDeviceName.c_str()).c_str());
			gLogger->info(StdStringFormat( "ServiceName=%s\n" , Globals.szServiceName.c_str()).c_str());
//			gLogger->info(StdStringFormat( "DebugLevel=%d\n" , GLogger.DebugLevel()).c_str());
			gLogger->info(StdStringFormat( "Protocol=%s\n" , Globals.FanucProtocol.c_str()).c_str());
			gLogger->info(StdStringFormat( "CycleSleep=%d\n" , Globals.FocasDelay).c_str());
			gLogger->info(StdStringFormat( "FanucPort=%d\n" , Globals.FanucPort).c_str());
			gLogger->info(StdStringFormat( "ResetAtMidnight=%d\n" , Globals._bResetAtMidnight).c_str());
			gLogger->info(StdStringFormat( "ShdrPort=%d\n" , Globals.ShdrPort).c_str());

			gLogger->info(StdStringFormat( "AlarmFlag=%d\n" , Globals.nAlarmFlag).c_str());
			gLogger->info(StdStringFormat( "AxesLoadFlag=%d\n" , Globals.nAxesLoadFlag).c_str());
			gLogger->info(StdStringFormat( "ToolingFlag=%d\n" , Globals.nToolingFlag).c_str());
			Globals.nAlarmFlag = config.GetSymbolValue("CONFIG.AlarmFlag", "1").toNumber<int>();	
			Globals.nAxesLoadFlag = config.GetSymbolValue("CONFIG.AxesLoadFlag", "0").toNumber<int>();	
			Globals.nToolingFlag = config.GetSymbolValue("CONFIG.Tooling", "0").toNumber<int>();	
		}
	}
	catch(...) 
	{
		gLogger->error("MTConnect Adapter Configure Failed\n");
		return -1;

	}
	return 0;
}
void FanucShdrAdapter::Clear()
{
	for(int i = 0 ; i < items.size(); i++)
	{
		if(items[i]->_type==_T("Event") || items[i]->_type==_T("Sample"))
		{
			items[i]->_event.setValue("UNAVAILABLE");
		}
	}
	//SetMTCTagValue("power", "OFF" );
	
	//mPower.setValue(PowerState::eOFF);
	mPower.unavailable();
	mAvail.unavailable();
}

void FanucShdrAdapter::CreateItems()
{
//	AddItem("avail", "UNAVAILABLE" );
	//AddItem("power", "OFF" );
	AddItem("block", "UNAVAILABLE" );
	AddItem("line", "UNAVAILABLE" );
	AddItem("program", "UNAVAILABLE" );
	AddItem("controllermode", "UNAVAILABLE" );
	AddItem("execution", "UNAVAILABLE" );
	AddItem("Xabs", "UNAVAILABLE" );
	AddItem("Yabs", "UNAVAILABLE" );
	AddItem("Zabs", "UNAVAILABLE" );
	AddItem("Aabs", "UNAVAILABLE" );
	AddItem("Babs", "UNAVAILABLE" );
	AddItem("Cabs", "UNAVAILABLE" );
	AddItem("Uabs", "UNAVAILABLE" );
	AddItem("Vabs", "UNAVAILABLE" );
	AddItem("Srpm", "UNAVAILABLE" );
	AddItem("Sovr", "UNAVAILABLE" );
	AddItem("path_feedrateovr", "UNAVAILABLE" );
	AddItem("path_feedratefrt", "UNAVAILABLE" );
	AddItem("heartbeat", "0" );
	AddItem("Sload", "UNAVAILABLE" );
	AddItem("status", "UNAVAILABLE" );
	if(!Globals.szDeviceName.empty())
	{
		AddItem("DeviceName", "UNAVAILABLE" );
		strncpy(mPower.mDeviceName,Globals.szDeviceName.c_str(),NAME_LEN);
		strncpy(mAvail.mDeviceName,Globals.szDeviceName.c_str(),NAME_LEN);
		mPower.prefixName(Globals.szDeviceName.c_str());
		mAvail.prefixName(Globals.szDeviceName.c_str());
	}

#ifdef LOADS
	AddItem("Xload", "UNAVAILABLE" );
	AddItem("Yload", "UNAVAILABLE" );
	AddItem("Zload", "UNAVAILABLE" );
	AddItem("Aload", "UNAVAILABLE" );
	AddItem("Bload", "UNAVAILABLE" );
	AddItem("Cload", "UNAVAILABLE" );
#endif
#ifdef ALARM
	AddItem("fault", "" );
#endif
#ifdef TOOLING
	AddItem("toolid", "" );

#endif
}
void FanucShdrAdapter::disconnect()
{
	Clear();
	update();
#ifdef F15i
	_FS15D->disconnect();
#endif
#ifdef F15M
	_F15M->disconnect();
#endif
#ifdef iSeries
	_iSeries->disconnect();
#endif
	mConnected=false;

}

void FanucShdrAdapter::gatherDeviceData()
{
	static char * function = "FanucShdrAdapter::gatherDeviceData()";

	gLogger->debug("FanucMTConnectAdapter::gatherDeviceData");
	_set_se_translator( trans_func );  // correct thread?
	try {
		if(!Globals.szDeviceName.empty())
		{
			RUNONCE SetMTCTagValue("DeviceName", Globals.szDeviceName );
			//SetMTCTagValue("DeviceName", Globals.szDeviceName );
		}
		mAvail.available();

#ifdef F15i
		if (!mConnected)
			_FS15D->connect();
		if (mConnected)
		{		
			//if(Globals.FanucVersion == "15i")
			{
				_FS15D->getVersion();
				_FS15D->getInfo();
				_FS15D->getLine();
				_FS15D->getPositions();
				_FS15D->getStatus();
				_FS15D->getSpeeds();
				_FS15D->getAlarmInfo();

			}
		}
#else
#ifdef F15M
		if (!mConnected)
			_F15M->connect();
		if (mConnected)
		{	
			//if(Globals.FanucVersion == "15M")
			{
				_F15M->saveStateInfo();
				_F15M->getVersion();
				_F15M->getInfo();
				_F15M->getLine();
				_F15M->getPositions();
				_F15M->getStatus();
				// hack in getSpeeds
				_F15M->getSpeeds();

			}
		}
#else
		errmsg.clear();
		sHeartbeat=StdStringFormat("%d",heartbeat++);
		SetMTCTagValue("heartbeat", sHeartbeat );

		if (!mConnected)
		{
			_iSeries->connect();
			if (!mConnected)
			{
				Clear();
				update();
				throw std::exception(StdStringFormat("Connect fail %s",errmsg.c_str()).c_str());
			}

		}
		if (mConnected)
		{	
			mPower.setValue(PowerState::EPowerState::eON);
			try {
				_iSeries->saveStateInfo();
				if(FAILED(_iSeries->getInfo()))
					gLogger->info(StdStringFormat("%s getInfo fail", function).c_str());
				if(FAILED(_iSeries->getPositions()))
					gLogger->info(StdStringFormat("%s getPositions fail", function).c_str());
				if(FAILED(_iSeries->getLine()))
					throw std::exception(("getLine fail"+errmsg).c_str());
				if(FAILED(_iSeries->getStatus()))
					throw std::exception(("getStatus fail"+errmsg).c_str());
				if(FAILED(_iSeries->getSpeeds()))
					throw std::exception(("getSpeeds fail"+errmsg).c_str());
#ifdef ALARM			
				if(Globals.nAlarmFlag)
				{
					if(FAILED(_iSeries->getAlarmInfo()))
						throw std::exception(("getAlarmInfo fail"+errmsg).c_str());
				}
#endif
#ifdef LOADS
				if(Globals.nAxesLoadFlag)
				{
					if(FAILED(_iSeries->getLoads()))
						throw std::exception(("getLoads fail"+errmsg).c_str());
				}
#endif
#ifdef TOOLING
				if(Globals.nToolingFlag)
				{
					if(FAILED(_iSeries->getToolInfo()))
						throw std::exception(("getToolInfo fail"+errmsg).c_str());
				}
#endif
				SetMTCTagValue("status", "Running" );

			}
			catch(std::exception errmsg)
			{
				std::stringstream tmp;
				tmp << "iSeries:: Failed " << errmsg.what();
				_iSeries->LogErrorMessage(tmp.str().c_str(),-1);
				disconnect();
				Clear();
				update();
				//EventLogger.LogEvent(tmp.str());
				SetMTCTagValue("status", tmp.str() );
			}
		}
#endif
#endif
		update();
	}
	catch(std::exception  errmsg)
	{
		disconnect();
		ErrMsg(errmsg.what());
		SetMTCTagValue("status", StdStringFormat("iSeries::%s Failed %s", function, errmsg.what()));
		update();
	}
	catch(...)
	{
		ErrMsg("Throw error in gatherDeviceData()\n");
		disconnect();
		SetMTCTagValue("status", StdStringFormat("iSeries::%s Failed ", function));
		update();
	}
}
void FanucShdrAdapter::update()
{
		for(int i = 0 ; i < items.size(); i++)
		{
			if(items[i]->_type==_T("Event") || items[i]->_type==_T("Sample"))
			{
				if(items[i]->_value != items[i]->_lastvalue)
					items[i]->_event.setValue(items[i]->_value.c_str());
			}
		}
}
void FanucShdrAdapter::start()
{  

	if(Globals._bResetAtMidnight)
	{
		COleDateTime now = COleDateTime::GetCurrentTime();
		COleDateTimeSpan tilnextreset = COleDateTimeSpan(0, 1, 0, 0);;
		gLogger->error( StdStringFormat("Agent will Reset from now  %8.4f", (tilnextreset.GetTotalSeconds()/3600.00)).c_str() );

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
			(long) tilnextreset.GetTotalSeconds() * 1000,
			&_ResetThread,
			(DWORD_PTR) this,
			&_ResetThread._hTimer  // stored newly created timer handle
			) ;

	}
	Adapter::startServer(); 	
}
void FanucShdrAdapter::stop()
{
	Clear();
	update();
	Adapter::stopServer();
}
std::string FanucShdrAdapter::getProgramName(char * buf) 
{
	gLogger->debug("FanucMTConnectAdapter::getProgramName Enter");

	int prognum;
	std::string progname;
	std::stringstream ss(buf); 
	std::string sLine; 
	while(std::getline(ss, sLine, '\n'))
	{ 
		//  look for O # which must be there
		if(1 != sscanf(sLine.c_str(), "O%d", &prognum))
			continue;

		progname=StdStringFormat("O%d", prognum);
#if 0
		// if find comment inside (...) return as program name
		int n = sLine.find("(");
		int m= sLine.find(")");
		if(std::string::npos !=n && std::string::npos !=m)
			return sLine.substr(n+1,m-n-1);

		// else could be standalone  on next line
		if(std::getline(ss, sLine, '\n'))
		{
			n = sLine.find("(");
			m= sLine.find(")");
			if(std::string::npos !=n && std::string::npos !=m)
				return sLine.substr(n+1,m-n-1);

		}
#endif
		return progname;
	}
	gLogger->info("FanucMTConnectAdapter::getProgramName Leave");

	return "";
}
void FanucShdrAdapter::initialize(int aArgc, const char *aArgv[])
{
	if(FAILED(this->Configure()))
	{
		throw std::exception(" FanucShdrAdapter::initialize failed");
	}
	this->CreateItems();
	MTConnectService::initialize(aArgc, aArgv);
	if (aArgc > 1) {
		mPort = atoi(aArgv[1]);
	}
}
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#pragma warning(disable: 4247) //warning C4297: 'WinMain' : function assumed not to throw an exception but does
//LPSTR* CommandLineToArgv(LPCSTR lpCmdLine, int *pNumArgs);

int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
{
	try
	{
		//EventLogger.Register();
		FILE * fd;
		fd = fopen((::ExeDirectory() + "adapter.log").c_str(), "w");
		gLogger = new Logger(fd);
		gLogger->setLogLevel(Logger::eDEBUG);
		gLogger->error("Adapter started");

		//EventLogger.LogEvent( StdStringFormat("Welcome to Fanuc HSSB world\n"));
		FanucShdrAdapter adapter(7878);
		_mainadapter=&adapter;
		adapter.setName("MTConnectFanucAdapter");

		//argv = CommandLineToArgvW(GetCommandLine(), &argc);
		return adapter.main(__argc ,(const char **) __argv );
	}
	catch(...)
	{
		gLogger->error("Service terminated abnormally in main");

	}
	return 0;
}
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


