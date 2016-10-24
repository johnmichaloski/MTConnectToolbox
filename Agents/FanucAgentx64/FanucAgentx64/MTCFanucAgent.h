//
// MTCFanucAgenth : Defines the entry point for the console application.
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.
#pragma once
#include <vector>
#include <string>
#include "atlutil.h"

#include "config.hpp"
#include "Config.h"
#include "GLogger.h"
#include "AgentCfg.h"

#include "CmdHandler.h"


class CGlobals
{
public:
	//std::string  FanucIpAddress;
	std::string FanucProtocol;
	int FanucPort;
	int FocasDelay;
	std::string FanucVersion;
	std::string szFanucExe;
	int AutoVersionDetect;
	std::string MachineToolConfig;
	std::vector<std::string>  fanucips;
	std::vector<std::string>  fanucdevicess;
	std::vector<std::string>  configs;
	//std::vector<std::string>  fanucports;
	bool _bResetAtMidnight;

	CGlobals() 
	{
		FanucPort=8193;
		FocasDelay=1000;
		AutoVersionDetect=0;
	}
};

__declspec(selectany)  CGlobals Globals;

class AgentConfigurationEx : public AgentConfiguration
{
	crp::Config										config;
	//std::vector<std::string>						_devices, ipaddrs;
	//std::thread_group								_group;
	std::vector<std::thread> _group;
	std::vector<CCmdHandler * >						 _cmdHandlers;
	CAgentCfg										_agentcfg;
	std::string										_devicefile ;
	std::string										_cfgfile ;
	int												_httpPort;
	void join_all();
public:
	AgentConfigurationEx(){} 
	virtual int thread(int aArgc, const char *aArgv[]);
	virtual void start();
	virtual void stop();
	std::vector<int> _shiftchanges;

	CWorkerThread<> _resetthread;
	struct CResetThread : public IWorkerThreadClient
	{
		HRESULT Execute(DWORD_PTR dwParam, HANDLE hObject);
		HRESULT CloseHandle(HANDLE){ ::CloseHandle(_hTimer); return S_OK; }
		HANDLE _hTimer;
	} _ResetThread;
};