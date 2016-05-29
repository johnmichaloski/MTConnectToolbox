//
// MTCSiemensAgent.h : Defines the entry point for the console application.
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.
#pragma once
#include <vector>
#include <string>
#include <boost\thread.hpp>
#include "atlutil.h"
#include "config.hpp"
#include "Config.h"
#include "Logger.h"
#include "Globals.h"

#include "OpcAdapter.h"


class AgentConfigurationEx : public AgentConfiguration
{
	crp::Config										config;
	//std::vector<std::string>						_devices, ipaddrs;
	boost::thread_group								_group;
	std::vector<COpcAdapter * >						 _cncHandlers;
	std::string										_devicefile ;
	std::string										_cfgfile ;
	std::string _probexml;

	std::vector<std::string> _ipaddrs ;
	std::vector<std::string> _devices ;
	std::vector<std::string> _tags;

public:
	AgentConfigurationEx(); 
	virtual int main(int aArgc, const char *aArgv[]);
	virtual void initialize(int aArgc, const char *aArgv[]);
	virtual void start();
	virtual void stop();
	
	HRESULT	WriteDevicesFile(std::vector<std::string> devicenames, std::string probexml,  std::string xmlFile, std::string destFolder);
	int Configure();

	boost::condition_variable cond;
	CWorkerThread<> _resetthread;
	struct CResetThread : public IWorkerThreadClient
	{
		HRESULT Execute(DWORD_PTR dwParam, HANDLE hObject);
		HRESULT CloseHandle(HANDLE){ ::CloseHandle(_hTimer); return S_OK; }
		HANDLE _hTimer;
	} _ResetThread;

};