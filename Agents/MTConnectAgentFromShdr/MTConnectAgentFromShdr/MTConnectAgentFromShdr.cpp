// MTConnectAgentFromShdr.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <exception>
#include <tchar.h>

#include "NIST/StdStringFcn.h"
#include "NIST/File.h"
#include "NIST/Config.h"

#include "atlutil.h"
#include "ATLComTime.h"

#include "agent.hpp"
#include "config.hpp"

#include "NIST\ResetAtMidnightThread.h"
#include "NIST\Config.h"
#include "NIST\Logger.h"
#include "NIST\WinSingleton.h"

static const char INI_FILENAME[] = "agent.cfg";

static void ErrMessage(std::string errmsg)
{
	logError(errmsg.c_str());
}

extern SERVICE_STATUS          gSvcStatus; 
extern VOID ReportSvcStatus( DWORD dwCurrentState, //    The current state (see SERVICE_STATUS)
	DWORD dwWin32ExitCode, //   The system error code
	DWORD dwWaitHint); //    Estimated time for pending operation, 


static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	logError("AgentConfigurationEx::In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode );
#ifdef DEBUG
	DebugBreak();
#endif
	throw std::exception();
}

class AgentConfigurationEx :
	public AgentConfiguration
	, public CResetAtMidnightThread<AgentConfigurationEx>
	, public MTConnectSingleton<AgentConfigurationEx>
{
public:
	AgentConfigurationEx()
	{
	}


	virtual void start()
	{
		logStatus( "MTConnect Agent Service Start %s\n" , COleDateTime::GetCurrentTime().Format() );
		try{

			if(bResetAtMidnight)
				CResetAtMidnightThread<AgentConfigurationEx>::Start();

			//Handles Win32 exceptions (C structured exceptions) as C++ typed exceptions
			_set_se_translator( trans_func ); 

			// Start the server. This blocks until the server stops.
			AgentConfiguration::start();

		}
		catch(std::exception e)
		{
			logFatal("AgentConfigurationEx::start() failed - %s\n", e.what());
			throw e;
		}
		catch(...)
		{
			ReportSvcStatus(SERVICE_STOP, 0, 0);
		}

	}

	virtual void stop()
	{
		int nWaitTime = 5000;
		ReportSvcStatus(SERVICE_STOP_PENDING, 0, 15000);
		StopAll();
		ReportSvcStatus(SERVICE_STOP, 0, 0);
	}

	void StopAll()
	{
		try {
			ErrMessage(StdStringFormat( "MTConnect Agent Service Stopped %s\n", COleDateTime::GetCurrentTime().Format() ).c_str());

			if(CResetAtMidnightThread<AgentConfigurationEx>::bResetAtMidnight)
				CResetAtMidnightThread<AgentConfigurationEx>::Stop();

			AgentConfiguration::stop();

		}
		catch(...)
		{
			ErrMessage(StdStringFormat( "MTConnect Agent Service Stop Aborted %s\n", COleDateTime::GetCurrentTime().Format() ).c_str());
		}

	}
	////////////////////////////////////////////////////////////////////
	COleDateTime now ;
	NIST::Config cfg;
};


#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
//#pragma warning(disable: 4247) //warning C4297: 'WinMain' : function assumed not to throw an exception but does

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	try 
	{	
		AgentConfigurationEx config;
		//config.bTestReset=true;  // turn this on for 2 minute resets

		// Set up local logging to debug.txt. 
		GLogger.Open(File.ExeDirectory() + "debug.txt");
		GLogger.DebugLevel()=5;
		GLogger.Timestamping()=true;
		ErrMessage(StdStringFormat( "MTConnect Agent Service Entered %s\n" , nowtimestamp().c_str()));

		/**
		* This code reads the tagname and enumeration remappings. They are substituted into static variables
		* so that all adapters use the substitutions.
		*/
		NIST::Config cfg;
		if(cfg.load(File.ExeDirectory() + "Config.ini"))
		{
			config.bResetAtMidnight = cfg.GetSymbolValue("GLOBALS.ResetAtMidnight", "0").toNumber<int>();
			config.bSingleton= cfg.GetSymbolValue("GLOBALS.Singleton", "0").toNumber<int>();
			if(config.bSingleton)
				config.KillAllOtherInstances();

			GLogger.DebugLevel() = cfg.GetSymbolValue("GLOBALS.Debug", "0").toNumber<int>();
			Adapter::rpmEntries= cfg.GetTokens("GLOBALS.RPMTAGS", ",");
			Adapter::nLogUpdates = cfg.GetSymbolValue("GLOBALS.LogUpdates", "1").toNumber<int>();
			bAbortHeartbeat = cfg.GetSymbolValue("GLOBALS.AbortHeartbeat", "0").toNumber<int>();


			std::map<std::string, std::string> tagrenames = cfg.getmap("TAGRENAMES"); //TAGRENAMES is a [section]
			std::map<std::string, std::string> enumrenames = cfg.getmap("ENUMREMAPPING");

			for(std::map<std::string, std::string>::iterator it= tagrenames.begin(); it!= tagrenames.end(); it++)
			{
				Adapter::keymapping.insert(std::make_pair<std::string, std::string>( (*it).first, (*it).second));
			}
			for(std::map<std::string, std::string>::iterator it= enumrenames.begin(); it!= enumrenames.end(); it++)
			{
				Adapter::enummapping.insert(std::make_pair<std::string, std::string>( (*it).first, (*it).second));
			}
		}


#ifdef _WINDOWS
		// MICHALOSKI ADDED
		TCHAR buf[1000];
		GetModuleFileName(NULL, buf, 1000);
		std::string path(buf);
		path=path.substr( 0, path.find_last_of( '\\' ) +1 );
		SetCurrentDirectory(path.c_str());
#endif	

		ErrMessage(StdStringFormat( "MTConnect Agent Service Started %s\n" , nowtimestamp().c_str() ).c_str());
		if(FAILED(::CoInitialize(NULL)))
			throw  std::exception("CoInitialize failed\n");

		HRESULT hr = ::CoInitializeSecurity( NULL, //Points to security descriptor 
			-1, //Count of entries in asAuthSvc 
			NULL, //Array of names to register 
			NULL, //Reserved for future use 
			RPC_C_AUTHN_LEVEL_NONE, //The default authentication //level for proxies 
			RPC_C_IMP_LEVEL_IDENTIFY, //The default impersonation //level for proxies 
			NULL, //Reserved; must be set to  NULL
			EOAC_NONE, //Additional client or //server-side capabilities 
			NULL //Reserved for future use 
			);  

		if(FAILED(hr))
			throw std::exception("CoInitializeSecurity failed\n");
		SetCurrentDirectory(File.ExeDirectory().c_str());

		//std::cout.rdbuf(&DebugLogger);
		//std::cerr.rdbuf(&DebugLogger);
		ErrMessage(StdStringFormat( "Enter config.main %s\n" , nowtimestamp().c_str() ).c_str());
		config.main( __argc,(const char **) __argv );

	}
	catch(std::exception e)
	{
		ErrMessage(e.what());
	}
	catch(...)
	{
		ErrMessage("Service terminated abnormally in main\n");
	}
	::CoUninitialize();
	ErrMessage(StdStringFormat( "MTConnect Agent Service Ended %s\n" , nowtimestamp().c_str()).c_str());
	return -1;
}
