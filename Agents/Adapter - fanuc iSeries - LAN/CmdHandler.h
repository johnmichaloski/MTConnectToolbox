//
// CmdHandler.h
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#pragma once
#include <boost/timer.hpp>
#include <boost/thread/mutex.hpp>

#include "config.hpp"
#include "config.h"
class AgentConfigurationEx;
class CiSeries;
class CFS15D;
class CF15B;

class CCmdHandler
{
public:
	CCmdHandler(AgentConfigurationEx * config);
	~CCmdHandler(void);
	void Configure(crp::Config	& config, 
		std::string device, 
		std::string ipaddr,
		std::string port="8193"
		);
	void Stop(){ _mRunning=false; }
	void Cycle();
	std::map<std::string, std::string> _mtcValues;
	void SetMTCTagValue( std::string tag, std::string value);
	std::string GetMTCTagValue(std::string tag);
	void disconnect();

	AgentConfigurationEx *				_agentconfig;
	//MTConnectStreamsParser				_parser;
	crp::Config							_config;
	std::string							_device, _ipaddr,_devicexmlpath;
	bool								_mRunning;
	boost::mutex _access;

	int _nAxes;
	std::map<char,int> axisnum;
	virtual HRESULT gatherDeviceData();
	static std::string getProgramName(char * buffer) ;
	unsigned short mFlibhndl;
	bool mConnected;
	int mDevicePort;
	std::string mDeviceIP;
	std::string sHeartbeat;
	int heartbeat;
#ifdef F15i
	CFS15D * _FS15D;
	friend  CFS15D;
#endif

#ifdef F15M	
	CF15B * _F15M;
	friend CF15B;
#endif

#ifdef iSERIES
	CiSeries * _iSeries;
	friend CiSeries;
#endif
	void CCmdHandler::AllOff();

};
