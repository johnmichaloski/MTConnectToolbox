// MtcOpcAgent.cpp : Defines the entry point for the console application.
//
#define BOOST_ALL_NO_LIB
#include "stdafx.h"
#include <string>
#include <exception>
//#include "MtcOpcAdapter.hpp"
#include "Logger.h"
//#include "RemoteTaskList.h"
#include "MTCSiemensAgent.h"

#pragma comment(lib, "Ws2_32.lib")

#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#pragma warning(disable: 4247) //warning C4297: 'WinMain' : function assumed not to throw an exception but does

#define BOOSTLIBPATH64(X) "C:\\Program Files\\NIST\\src\\boost_1_54_0\\vc10\\stagex64\\lib\\"##X
#define BOOSTLIBPATH32(X) "C:\\Program Files\\NIST\\src\\boost_1_54_0\\vc10\\stagex32\\lib\\"##X

#define MTCONNECTLIBXML(X) "C:\\Users\\michalos\\Documents\\GitHub\\Agents\\MtcOpcAgent\\Agent\\win32\\libxml2-2.7.7\\lib\\"##X

#if defined(WIN64) && defined( _DEBUG) 
#pragma message( "DEBUG x64" )
#pragma comment(lib, "libxml2_64d.lib")
#pragma comment(lib, "libboost_system-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_thread-vc100-mt-sgd-1_54.lib")
  
#elif !defined( _DEBUG) && defined(WIN64)
#pragma message( "RELEASE x64" )
#pragma comment(lib, "libxml2_64.lib")
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")

#elif defined(_DEBUG) && defined(WIN32)
#pragma message( "DEBUG x32" )
#pragma comment(lib, "libxml2d.lib")
#pragma comment(lib, BOOSTLIBPATH32("libboost_thread-vc100-mt-sgd-1_54.lib"))
#pragma comment(lib, BOOSTLIBPATH32("libboost_date_time-vc100-mt-sgd-1_54.lib"))
#pragma comment(lib, BOOSTLIBPATH32("libboost_regex-vc100-mt-sgd-1_54.lib"))
#pragma comment(lib, BOOSTLIBPATH32("libboost_chrono-vc100-mt-sgd-1_54.lib"))
#pragma comment(lib, BOOSTLIBPATH32("libboost_system-vc100-mt-sgd-1_54.lib"))

#elif !defined( _DEBUG) && defined(WIN32)
#pragma message( "RELEASE x32" )
#pragma comment(lib,  BOOSTLIBPATH32("libboost_thread-vc100-mt-s-1_54.lib"))
#pragma comment(lib,  BOOSTLIBPATH32("libboost_system-vc100-mt-s-1_54.lib"))
#pragma comment(lib,  BOOSTLIBPATH32("libboost_date_time-vc100-mt-s-1_54.lib"))
#pragma comment(lib,  BOOSTLIBPATH32("libboost_regex-vc100-mt-s-1_54.lib"))
#pragma comment(lib,  BOOSTLIBPATH32("libboost_chrono-vc100-mt-s-1_54.lib"))
#pragma comment(lib,  MTCONNECTLIBXML("libxml2.lib"))
#endif

int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPTSTR    lpCmdLine,
					 int       nCmdShow)
{
	HRESULT hr ;
	try 
	{
		//RedirectIOToConsole();
		//RedirectedIOProcess remote;

		//std::string output = remote.RunProcess("tasklist.exe");
		//OutputDebugString(output.c_str());
		//_CrtSetBreakAlloc(358);
		
		//_CrtSetBreakAlloc(95360);
		//CRTDBG_DELAY_FREE_MEM_DF
		//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

		GLogger.Open();
		GLogger.DebugLevel()=5;
		GLogger.Timestamping()=true;
		//std::cout.rdbuf(&GLogger._outputdebugstream);
		//std::cerr.rdbuf(&GLogger._outputdebugstream);


		if(FAILED(hr=::CoInitialize(NULL)))
			throw  std::exception("CoInitialize failed\n");

		if(FAILED(hr= ::CoInitializeSecurity( NULL, //Points to security descriptor 
			-1, //Count of entries in asAuthSvc 
			NULL, //Array of names to register 
			NULL, //Reserved for future use 
			RPC_C_AUTHN_LEVEL_NONE, //The default authentication //level for proxies 
			RPC_C_IMP_LEVEL_IDENTIFY, //The default impersonation //level for proxies 
			NULL, //Reserved; must be set to  NULL
			EOAC_NONE, //Additional client or //server-side capabilities 
			NULL //Reserved for future use 
			)))  
			throw std::exception("CoInitializeSecurity failed\n");
	
		 //std::cout.rdbuf(GLogger.DebugFile.rdbuf()); //redirect std::cout to out.txt!
		 //std::cerr.rdbuf(GLogger.DebugFile.rdbuf()); //redirect std::cout to out.txt!
#if 0
		 std::string path, contents, ipaddr, devices;
		 ipaddr="127.0.0.1";
		 devices="M33";

		 path = "C:\\Program Files\\NIST\\proj\\MTConnect\\Nist\\MTConnectGadgets\\MtcOpcAgent\\Distribution\\";
		 ReadFile(path+"MtcOpcAgent.ini", contents);

		 ReplacePattern(contents, "ServerMachineName", "\n", "ServerMachineName=" + ipaddr + "\n");
		 ReplacePattern(contents, "MTConnectDevice", "\n", "MTConnectDevice=" + devices + "\n");

		 std::vector<std::string> ips=TrimmedTokenize(ipaddr,",");
		 std::vector<std::string> devs=TrimmedTokenize(devices,",");
		 if(ips.size() != devs.size())
			 ::MessageBox(NULL, "Mismatched # ips and devices", "Error", MB_OK);
		 std::string tagsection="SIEMENS";
		 for(int i=1; i< ips.size(); i++)
		 {
			 tagsection+=",SIEMENS";
		 }
		 ReplacePattern(contents, "OpcTags", "\n", "OpcTags=" + tagsection + "\n");
		 ::MessageBox(NULL,contents.c_str(), "Error", MB_OK);


#endif

		 AgentConfigurationEx config;
#if 0
		 char *argv[4] = {
			 "agent.exe",
			 "debug",
			 "" 
		 };
		 config.main(2, (const char **) argv);
#endif
		 config.main( __argc,(const char **) __argv );
		 /*AgentConfiguration config;
		 return config.main(__argc, (const char **) __argv);*/

	}
	catch(std::exception e)
	{
		OutputDebugString(e.what());
	}
	catch(...)
	{
		OutputDebugString("Service terminated abnormally in main\n");
	}
	::CoUninitialize();

	_CrtDumpMemoryLeaks();
	return -1;
}
