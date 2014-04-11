/*
* Copyright (c) 2008, AMT – The Association For Manufacturing Technology (“AMT”)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the AMT nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* DISCLAIMER OF WARRANTY. ALL MTCONNECT MATERIALS AND SPECIFICATIONS PROVIDED
* BY AMT, MTCONNECT OR ANY PARTICIPANT TO YOU OR ANY PARTY ARE PROVIDED "AS IS"
* AND WITHOUT ANY WARRANTY OF ANY KIND. AMT, MTCONNECT, AND EACH OF THEIR
* RESPECTIVE MEMBERS, OFFICERS, DIRECTORS, AFFILIATES, SPONSORS, AND AGENTS
* (COLLECTIVELY, THE "AMT PARTIES") AND PARTICIPANTS MAKE NO REPRESENTATION OR
* WARRANTY OF ANY KIND WHATSOEVER RELATING TO THESE MATERIALS, INCLUDING, WITHOUT
* LIMITATION, ANY EXPRESS OR IMPLIED WARRANTY OF NONINFRINGEMENT,
* MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. 

* LIMITATION OF LIABILITY. IN NO EVENT SHALL AMT, MTCONNECT, ANY OTHER AMT
* PARTY, OR ANY PARTICIPANT BE LIABLE FOR THE COST OF PROCURING SUBSTITUTE GOODS
* OR SERVICES, LOST PROFITS, LOSS OF USE, LOSS OF Data OR ANY INCIDENTAL,
* CONSEQUENTIAL, INDIRECT, SPECIAL OR PUNITIVE DAMAGES OR OTHER DIRECT DAMAGES,
* WHETHER UNDER CONTRACT, TORT, WARRANTY OR OTHERWISE, ARISING IN ANY WAY OUT OF
* THIS AGREEMENT, USE OR INABILITY TO USE MTCONNECT MATERIALS, WHETHER OR NOT
* SUCH PARTY HAD ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES.
*/
#include "stdafx.H"
//#define BOOST_ALL_NO_LIB

#include "sys/stat.h"
#include "fcntl.h"
#include <string>
#include <vector>
#include <map>
#include "MTConnectCmdSimAgent.h"



#include "StdStringFcn.h"
#include "Config.h"
#include "AppEventLog.h"
#include "Logger.h"

#include "agent.hpp"


#include "FactoryIntegrator.h"
#include "CmdCell.h"
extern void SetupFactory();


#ifdef _WINDOWS

//Handles Win32 exceptions (C structured exceptions) as C++ typed exceptions
static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg =  StdStringFormat("COpcAdapter In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode);
	GLogger.Warning(errmsg.c_str() );
	throw std::exception(errmsg.c_str() , pExp->ExceptionRecord->ExceptionCode);
}
#endif

int _tmain(int argc, _TCHAR* argv[])
{
	// SetupFactory();
	//CFactoryIntegrator factory;
	////std::vector<Resource * > shimCells=factory.GetJobResources("shim") ;
	//Resource* resource = factory.FindResourceByName("M2");
	//Cell* cell = factory.FindCellById("c2");
	//Resource * res1= factory.FindResourceById("r1");	
	//Part * shim1= factory.FindPartById("shim");
	//ProcessPlan* pp = factory.FindProcessPlanById("pp1");
	////CostAllocation *  factory.FindCostByName(bstr_t name) ;
	//Job * workorder = factory.FindJobById("job1") ;

	int jobId=0;
	//CWorkOrders * workorders = new CWorkOrders( &factory);
	//workorders->InitAllJobs((Job *)  factory.workorders->at(0));// there may be multiple workorders - but only 1st counts in our world
	//CCellHandler(NULL, &factory).CreateFactoryResourceHandlers(); 
	//workorders->Newworkorder();
	//workorders->AddJob(&factory, jobId, "shim");

	//workorders->CreateResourcesByJobs(&factory);
	//workorders->InitJobsStats((Job *)  factory.workorders->at(0)) ;  // FIXME: verify works

	//std::string nMaxQueueSize = factory.workorders->at(0)->GetPropertyValue("MaxQueueSize");
	//CWorkOrders::MaxQueueSize=ConvertString<int>(nMaxQueueSize,2);
	//	workorders->Run(workorders);
	try {
		CoInitialize(NULL);
		// srand( (unsigned)time( NULL ) );
		GLogger.Timestamping()=true;
		GLogger.DebugString()="MTConnectCmdSimAgent";
		GLogger.OutputConsole()=true;
		GLogger.Open(File.ExeDirectory()+"Debug.txt");
		GLogger.DebugLevel()=5;
		GLogger.Fatal("Start MTConnectCmdSim Agent\n");
		//	EventLogger.Register();
#ifdef _WINDOWS
		_set_se_translator( trans_func );   //Handles Win32 exceptions (C structured exceptions) as C++ typed exceptions
		// MICHALOSKI ADDED
		TCHAR buf[1000];
		GetModuleFileName(NULL, buf, 1000);
		std::string path(buf);
		path=path.substr( 0, path.find_last_of( '\\' ) +1 );
		SetCurrentDirectory(path.c_str());
#endif
		// check for 		inifile=::ExeDirectory() + "Config.ini";	

		AgentConfigurationEx config;
		//config.setAgent(new Agent());
		Globals.Debug = 5;

		return config.main(argc, (const char **) argv);
	}
	catch(...)
	{
		GLogger.Fatal("Abnormal exception end to  MTConnectCmdSim Agent\n");
	}
	return 0;
}
