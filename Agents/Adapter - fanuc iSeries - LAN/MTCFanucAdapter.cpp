//
// MTCFanucAgentcpp : Defines the entry point for the console application.
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.
#include "stdafx.h"

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

#pragma comment( lib, "msxml2.lib" )
#pragma comment(lib,"psapi") 
#pragma comment(lib, "Ws2_32.lib")

#define FOCASLIB(X) "C:\\Users\\michalos\\Documents\\GitHub\\Fanuc\\Adapter - fanuc iSeries - LAN\\fwlib\\e1\\"##X

#pragma comment(lib, FOCASLIB("fwlib32.lib") )

#include "iSeries.h"

int FanucShdrAdapter::mDebug;
FanucShdrAdapter * _mainadapter;
static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg =  StrFormat("In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode);
	OutputDebugString(errmsg.c_str() );
	throw std::exception(errmsg.c_str() , pExp->ExceptionRecord->ExceptionCode);
} 

HRESULT FanucShdrAdapter::ErrMsg (const char * format, ...)
{
	va_list ap;
	va_start(ap, format);

	int m;
	int n= strlen(format) + 1028;
	std::string tmp(n,'0');	

	// Kind of a bogus way to insure that we don't
	// exceed the limit of our buffer
	while((m=_vsnprintf(&tmp[0], n-1, format, ap))<0)
	{
		n=n+1028;
		tmp.resize(n,'0');
	}
	va_end(ap);
	//AtlTrace(tmp.c_str( ) );
	GLogger.LogMessage(tmp.substr(0,m), LOWERROR);
	return E_FAIL;
}
HRESULT FanucShdrAdapter::DebugMsg (int level, const char * format, ...)
{
	va_list ap;
	va_start(ap, format);

	int m;
	int n= strlen(format) + 1028;
	std::string tmp(n,'0');	

	// Kind of a bogus way to insure that we don't
	// exceed the limit of our buffer
	while((m=_vsnprintf(&tmp[0], n-1, format, ap))<0)
	{
		n=n+1028;
		tmp.resize(n,'0');
	}
	va_end(ap);
	//AtlTrace(tmp.c_str( ) );
	GLogger.LogMessage(tmp.substr(0,m), level);
	if(level < 3)
		return E_FAIL;
	return S_OK;
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
		std::string cmd = StrFormat("cmd /c net stop \"%s\" & net start \"%s\"", adapter->name(), adapter->name());        // Command line

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

	_iSeries = new CiSeries(this);
	mSleep=1000;
	//heartbeat=0;
	mConnected=false;
	mFlibhndl=0;
	mProgramNum=-1;

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
	GLogger.Info(StrFormat("MTConnect Adapter  Configure %s\n" , LocalTimetamp().c_str()));

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

			//Globals.fanucips=TrimmedTokenize(config.GetSymbolValue("CONFIG.FanucIpAddress", "127.0.0.1").c_str(),",");
			Globals.configs=TrimmedTokenize(config.GetSymbolValue("CONFIG.Configs", "xyzabcuv").c_str(),",");
			Globals.FanucIpAddress=config.GetSymbolValue("CONFIG.FanucIpAddress", "127.0.0.2").c_str();
			//Globals.FanucPort=config.GetSymbolValue("CONFIG.FanucPort", "8193").toNumber<int>();
			Globals.FanucPort=8193;

			Globals._bResetAtMidnight = config.GetSymbolValue("CONFIG.ResetAtMidnight", "0").toNumber<int>();
			Globals.FocasDelay = config.GetSymbolValue("CONFIG.FocasDelay", "5000").toNumber<int>();	
			Globals.ProgramLogic= config.GetSymbolValue("CONFIG.ProgramLogic", "OrderNumber").c_str();
			Globals.nAlarmFlag = config.GetSymbolValue("CONFIG.AlarmFlag", "1").toNumber<int>();	
			Globals.nAxesLoadFlag = config.GetSymbolValue("CONFIG.AxesLoadFlag", "0").toNumber<int>();	
			Globals.nToolingFlag = config.GetSymbolValue("CONFIG.ToolingFlag", "0").toNumber<int>();	

			//Globals.FanucProtocol= config.GetSymbolValue("CONFIG.FanucProtocol", "HSSB").c_str();
			Globals.FanucProtocol="LAN";
			Globals.szServiceName= config.GetSymbolValue("CONFIG.ServiceName", "MTConnectFanucAdapter").c_str();
			Globals.szDeviceName= config.GetSymbolValue("CONFIG.DeviceName", "").c_str();
			Globals.nAlarmFlag = config.GetSymbolValue("CONFIG.AlarmFlag", "1").toNumber<int>();	
			Globals.nAxesLoadFlag = config.GetSymbolValue("CONFIG.AxesLoadFlag", "0").toNumber<int>();	
			Globals.nToolingFlag = config.GetSymbolValue("CONFIG.Tooling", "0").toNumber<int>();	

			_mainadapter->setName(Globals.szServiceName.c_str());
			_mainadapter->mScanDelay=Globals.FocasDelay;
			this->mDevicePort=Globals.FanucPort;

			GLogger.DebugLevel()=config.GetSymbolValue("CONFIG.DebugLevel", "0").toNumber<int>();
			GLogger.Timestamping()=true;

			GLogger.Fatal(StrFormat( "DeviceName=%s\n" , Globals.szDeviceName.c_str()));
			GLogger.Fatal(StrFormat( "ServiceName=%s\n" , Globals.szServiceName.c_str()));
			GLogger.Fatal(StrFormat( "DebugLevel=%d\n" , GLogger.DebugLevel()));
			GLogger.Fatal(StrFormat( "Protocol=%s\n" , Globals.FanucProtocol.c_str()));
			GLogger.Fatal(StrFormat( "CycleSleep=%d\n" , Globals.FocasDelay));
			GLogger.Fatal(StrFormat( "FanucPort=%d\n" , Globals.FanucPort));
			GLogger.Fatal(StrFormat( "ResetAtMidnight=%d\n" , Globals._bResetAtMidnight));

			GLogger.Fatal(StrFormat( "AlarmFlag=%d\n" , Globals.nAlarmFlag));
			GLogger.Fatal(StrFormat( "AxesLoadFlag=%d\n" , Globals.nAxesLoadFlag));
			GLogger.Fatal(StrFormat( "ToolingFlag=%d\n" , Globals.nToolingFlag));
		}
	}
	catch(...) 
	{
		GLogger.Fatal("MTConnect Adapter Configure Failed\n");
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
//	AddItem("heartbeat", "0" );
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

	_iSeries->disconnect();
	mConnected=false;

}
void FanucShdrAdapter::gatherDeviceData()
{
	static char * function = "FanucShdrAdapter::gatherDeviceData()";

//	GLogger.LogMessage("FanucMTConnectAdapter::gatherDeviceData\n", HEAVYDEBUG);
	try {
		if(!Globals.szDeviceName.empty())
		{
			RUNONCE SetMTCTagValue("DeviceName", Globals.szDeviceName );
			//SetMTCTagValue("DeviceName", Globals.szDeviceName );
		}
		mAvail.available();

		errmsg.clear();
//		sHeartbeat=StrFormat("%d",heartbeat++);
//		SetMTCTagValue("heartbeat", sHeartbeat );

		if (!mConnected)
		{
			_iSeries->connect();
			if (!mConnected)
			{
				Clear();
				update();
				throw std::exception(StrFormat("Connect fail %s",errmsg.c_str()).c_str());
			}

		}
		if (mConnected)
		{	
			mPower.setValue(PowerState::EPowerState::eON);
			try {
				_iSeries->saveStateInfo();
				if(FAILED(_iSeries->getInfo()))
					GLogger.LogMessage(StrFormat("%s getInfo fail", function),3);
				if(FAILED(_iSeries->getPositions()))
					GLogger.LogMessage(StrFormat("%s getPositions fail", function),3);
				if(FAILED(_iSeries->getProgram()))
					throw std::exception(("getProgram fail"+errmsg).c_str());
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
				SetMTCTagValue("status", "adapter fail" );
			}
		}
		update();
	}
	catch(std::exception  errmsg)
	{
		disconnect();
		ErrMsg(errmsg.what());
		//SetMTCTagValue("status", StrFormat("iSeries::%s Failed %s", function, errmsg.what()));
		update();
	}
	catch(...)
	{
		ErrMsg("Throw error in gatherDeviceData()\n");
		disconnect();
		//SetMTCTagValue("status", StrFormat("iSeries::%s Failed ", function));
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
		GLogger.Fatal( StrFormat("Agent will Reset from now  %8.4f\n", (tilnextreset.GetTotalSeconds()/3600.00)) ); 

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
	GLogger.LogMessage("FanucMTConnectAdapter::getProgramName Enter\n", 3);

	int prognum;
	std::string progname;
	std::stringstream ss(buf); 
	std::string sLine; 
	while(std::getline(ss, sLine, '\n'))
	{ 
		//  look for O # which must be there
		if(1 != sscanf(sLine.c_str(), "O%d", &prognum))
			continue;

		progname=StrFormat("O%d", prognum);
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
	GLogger.LogMessage("FanucMTConnectAdapter::getProgramName Leave\n", 3);

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
		GLogger.Open();
		GLogger.Timestamping() = 1;
		_set_se_translator( trans_func );  // correct thread?

		//EventLogger.LogEvent( StrFormat("Welcome to Fanuc HSSB world\n"));
		FanucShdrAdapter adapter(7878);
		_mainadapter=&adapter;
		adapter.setName("MTConnectFanucLanAdapter");

		//argv = CommandLineToArgvW(GetCommandLine(), &argc);
		return adapter.main(__argc ,(const char **) __argv );
	}
	catch(...)
	{
		GLogger.Fatal("Service terminated abnormally in main\n");

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


