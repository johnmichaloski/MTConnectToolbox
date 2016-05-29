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
#include "ResetAtMidniteThread.h"
#include "NIST\Config.h"

#pragma comment(lib, "Ws2_32.lib")
static const char INI_FILENAME[] = "agent.cfg";

#define RESETATMIDNITE
#define NISTADDITIONS


#define MTCLIBPATH(X)    "C:\\Users\\michalos\\Documents\\GitHub\\Agents\\MTConnectAgentFromShdr\\MTConnectAgent\\win32\\libxml2-2.7.7\\lib\\" ## X


#if defined(WIN64) && defined( _DEBUG) 
#pragma message( "DEBUG x64" )
//#pragma comment(lib, "C:\\Users\\michalos\\Documents\\GitHub\\MTConnectSolutions\\MTConnectAgentFromShdr\\MTConnectAgentFromShdr\\x64\\Debug\\libiconv_a_debug.lib")
#pragma comment(lib, MTCLIBPATH("libxml2_64d.lib"))
#pragma comment(lib, "libboost_system-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_thread-vc100-mt-sgd-1_54.lib")

#elif !defined( _DEBUG) && defined(WIN64)
#pragma message( "RELEASE x64" )
#pragma comment(lib,  MTCLIBPATH("libxml2_64.lib"))
//#pragma comment(lib, "C:\\Users\\michalos\\Documents\\GitHub\\MTConnectSolutions\\MTConnectAgentFromShdr\\MTConnectAgentFromShdr\\x64\\Release\\libiconv_a.lib")
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")

#elif defined(_DEBUG) && defined(WIN32)
#pragma message( "DEBUG x32" )
#pragma comment(lib, "C:\\Users\\michalos\\Documents\\GitHub\\MTConnectSolutions\\MTConnectAgentFromShdr\\MTConnectAgentFromShdr\\Win32\\Debug\\libiconv_a_debug.lib")
#pragma comment(lib, "libboost_thread-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-sgd-1_54.lib")
#elif !defined( _DEBUG) && defined(WIN32)
#pragma message( "RELEASE x32" )
#pragma comment(lib, "C:\\Users\\michalos\\Documents\\GitHub\\MTConnectSolutions\\MTConnectAgentFromShdr\\MTConnectAgentFromShdr\\Win32\\Release\\libiconv_a.lib")
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")
#endif
static void ErrMessage(std::string errmsg)
{
	ATLTRACE2(errmsg.c_str());
}
#include <strsafe.h>
void ReportError(LPTSTR lpszFunction) 
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

	ErrMessage(StdStringFormat("%s\n", lpDisplayBuf ));

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw); 
}

extern SERVICE_STATUS          gSvcStatus; 
extern VOID ReportSvcStatus( DWORD dwCurrentState, //    The current state (see SERVICE_STATUS)
							DWORD dwWin32ExitCode, //   The system error code
							DWORD dwWaitHint); //    Estimated time for pending operation, 


static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	ErrMessage( StdStringFormat("AgentConfigurationEx::In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode).c_str() );
#ifdef DEBUG
	DebugBreak();
#endif
	throw std::exception();
}
#ifdef NISTADDITIONS
class AgentConfigurationEx :
	public AgentConfiguration
#ifdef RESETATMIDNITE
	, public CResetAtMidniteThread<AgentConfigurationEx>
#endif
{
public:
	AgentConfigurationEx()
	{
	}

	  //CWorkerThread<> _resetthread;
	  //struct CResetThread : public IWorkerThreadClient
	  //{
		 // HRESULT Execute(DWORD_PTR dwParam, HANDLE hObject);
		 // HRESULT CloseHandle(HANDLE){ ::CloseHandle(_hTimer); return S_OK; }
		 // HANDLE _hTimer;
	  //} _ResetThread;


	  virtual void start()
	  {
		  ErrMessage(StdStringFormat( "MTConnect Agent Service Start %s\n" , COleDateTime::GetCurrentTime().Format() ).c_str());
		  try{

			  std::string cfgfile = File.ExeDirectory()+"Config.ini"; 


			  //_bResetAtMidnight = ConvertString<bool>(reader.block("OPCSERVER")["ResetAtMidnight"], true);
#ifdef RESETATMIDNITE
			  CResetAtMidniteThread<AgentConfigurationEx>::Start();
#endif

			  //Handles Win32 exceptions (C structured exceptions) as C++ typed exceptions
			  _set_se_translator( trans_func ); 

			  // Start the server. This blocks until the server stops.
			  AgentConfiguration::start();

		  }
		  catch(std::exception e)
		  {
			   ErrMessage(StdStringFormat("AgentConfigurationEx::start() failed - %s\n", e.what()).c_str());
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
#ifdef RESETATMIDNITE
			   CResetAtMidniteThread<AgentConfigurationEx>::Stop();
#endif

			  AgentConfiguration::stop();

		  }
		  catch(...)
		  {
			   ErrMessage(StdStringFormat( "MTConnect Agent Service Stop Aborted %s\n", COleDateTime::GetCurrentTime().Format() ).c_str());
		  }

	  }
		  ////////////////////////////////////////////////////////////////////
	  //bool _bResetAtMidnight;
	  CHandle _hThread;
	  unsigned int _threadID;
	  COleDateTime now ;
	  NIST::Config cfg;
};

#else
typedef AgentConfiguration AgentConfigurationEx;
#endif

#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
//#pragma warning(disable: 4247) //warning C4297: 'WinMain' : function assumed not to throw an exception but does

int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
 {
	try 
	{	
		ErrMessage(StdStringFormat( "MTConnect Agent Service Entered %s\n" , nowtimestamp().c_str() ).c_str());
		AgentConfigurationEx config;

#ifdef NISTADDITIONS
 
		GLogger.Open(File.ExeDirectory() + "debug.txt");
		GLogger.DebugLevel()=5;
		GLogger.Timestamping()=true;
		NIST::Config cfg;


		if(cfg.load(File.ExeDirectory() + "Config.ini"))
		{
#ifdef RESETATMIDNITE
				  config.CResetAtMidniteThread<AgentConfigurationEx>::_bResetAtMidnight = cfg.GetSymbolValue("GLOBALS.ResetAtMidnight", "0").toNumber<int>();
#endif
				   GLogger.DebugLevel() = cfg.GetSymbolValue("GLOBALS.Debug", "0").toNumber<int>();
				   
				   std::map<std::string, std::string> tagrenames = cfg.getmap("TAGRENAMES");
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
#endif


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
