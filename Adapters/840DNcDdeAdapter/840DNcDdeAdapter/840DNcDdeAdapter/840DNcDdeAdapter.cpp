//
// 840DNcDdeAdapter.cpp : Defines the entry point for the console application.
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#include "stdafx.h"
#include "DdeAdapter.h"
#include "AppEventLog.h"
#include "StdStringFcn.h"
#include "Logger.h"
#include "Logger.hpp"


#include "OS.h"
#include "File.h"
extern VOID ReportSvcStatus( DWORD, DWORD, DWORD );

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "psapi.lib" )
#pragma comment(lib, "advapi32.lib")



/////////////////////////////////////////////////////////////////////
static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg =  StdStringFormat("In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode);
	OutputDebugString(errmsg.c_str() );
	throw std::exception(errmsg.c_str() , pExp->ExceptionRecord->ExceptionCode);
} 

/////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
	try 
	{
		_set_se_translator( trans_func );  
		HRESULT hr = CoInitialize(NULL);
		std::cerr << StdStringFormat("Welcome to 840D DDE world\n");
		DdeAdapter ddeAdapter(7878);
		ddeAdapter.setName("MTConnectDdeAdapter");
		ReportSvcStatus( SERVICE_START_PENDING, NO_ERROR, 10000 );
		//ddeAdapter.ParseOptions();
		//EventLogger.Register();


		//if(OS.GetOSMajorVersion() > 5)
		//{
		//	//std::string tmp = File.AppDirectory("MTConnect");
		//	////GLogger.Open(tmp +"debug.txt");
		//	//if(gLogger!=NULL)
		//	//{
		//	//	fclose (gLogger->mFile);
		//	//	//int fileno(const FILE *stream);
		//	//	gLogger->mFile = fopen((tmp +"agent.log").c_str(), "w+") ; 
		//	//	gLogger->error("Assign FD\n");
		//	//}
		//	//GLogger.Fatal("Hello World\n");

		//}
		//else
		//{
		//	GLogger.Open();
		//}
		ReportSvcStatus( SERVICE_START_PENDING, NO_ERROR, 10000 );

		ddeAdapter.main(argc,(const char **) argv);
		CoUninitialize();
	} 
	catch (std::exception errmsg)
	{
		GLogger.Fatal(errmsg.what());

	}
	catch (...)
	{
		GLogger.Fatal("Fatal Application Exception in DDE Adapter");

	}
	return 0;
}

