//
// AgentCfg.h
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.
#pragma once

#include <string>
#include <thread>
#include <vector>
#include <string>

class AgentConfiguration;

class CAgentCfg
{
public:
	CAgentCfg(void);
	~CAgentCfg(void);
	void SetHttpPort(int portnum) { _agenthttpport=portnum; }
	void SetCfgfolder(std::string cfgfolder) { _cfgFolder=cfgfolder; }
	void SetCfgFile(std::string cfgfile) { _cfgFile=cfgfile; }
	void SetXmlFile(std::string xmlfile) { _xmlFile=xmlfile; }
	void AddDevice(std::string cfgname,  // M1, M2
		std::string devicefilename, // lookup name in devices.xml directory
		std::string ip, 
		std::string port);
	void UpdateDevice(std::string cfgname, 
		std::string ip="", 
		std::string port="");
	void AddFanucDevice(std::string devicename, std::string config="xyzabcuv");
	void Run();
	void SpawnProcess();
//void							StopHeartbeat() { _agentthread.interrupt(); }
	void Start(AgentConfiguration *config= NULL) 
	{
		_config=config;
		_agentthread= std::thread(std::bind(&CAgentCfg::Run, this));
	}
	long	WriteCfgFile(std::string cfgfile, std::string xmlfile,  std::string _destFolder);
	long	WriteAgentCfgFile(std::string cfgfile, std::string xmlfile, std::string _destFolder);
	long	WriteDevicesFile(std::string xmlfile,  std::string _destFolder);
	std::string ReadXmlDevicesFile(std::string name);
	void SetDevicesXmlFolder(std::string folder) { _cfgFolder= folder; }
	void SetDestinationFolder(std::string folder) { _destFolder= folder; }
	void Clear() ;
	std::string FindXmlDevicesFilePath(std::string device);

	std::vector<std::string>		_fanucconfigs;
	std::vector<std::string>		ips, ports, rates, debugs, devices,names, alldevices;
	std::string 					_destFolder,_cfgFolder,_cfgFile,_xmlFile;
	std::string						_serviceName;
	std::thread					_agentthread;
	int								_agenthttpport;
	AgentConfiguration * _config;

};
