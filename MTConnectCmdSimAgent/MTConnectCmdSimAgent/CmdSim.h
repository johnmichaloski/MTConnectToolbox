//
// CmdSim.h
//

#pragma once


#include <string>
#include "MTConnectCmdSimAgent.h"
#include "MTConnectCmdSimAdapter.h"
#include "Config.h"
#include "MTConnectStreamsParser.h"
#include "FactoryIntegrator.h"
#include "CmdCell.h"

#include "agent.hpp"
#include "config.hpp"

class AgentConfigurationEx;


class CCmdSim :public AdapterT
{
public:
	CCmdSim(AgentConfigurationEx * mtcagent, // mtconnect agent
		std::vector<std::string> devices);
	~CCmdSim(void);
	virtual void		Configure();
	virtual void		Cycle();
	void				Stop(){ _mRunning=false; }
	void				SetMTCTagValue(std::string device, std::string tag, std::string value);
	void				MTConnectKpiReport();
	////////////////
	bool						_mRunning;
	std::vector<std::string>	_devices;
	int							cmdnum;
	int							nHeartbeat;
	MTConnectStreamsParser		_parser;
	CFactoryIntegrator			factory;
	int							jobId;
	CWorkOrders *				workorders;
	NIST::CTimer				_factoryElapsed;

};

