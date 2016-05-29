// **************************************************************************
// OpcAdapter.h : Declaration of the CMachineState
//
// Description:
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.
// **************************************************************************
#pragma once

#include "OPCBackEnd.h"
#include "wininet.h"
#include <algorithm>
#include <fstream>
#include "Config.h"
#include "atlutil.h"

using namespace crp;

class AgentConfigurationEx;
class Device;

class COpcAdapter : public  COPCBackEnd<COPCItem>
{
	typedef  COPCBackEnd<COPCItem> COPCBaseClass;
public:
	COpcAdapter(AgentConfigurationEx * mtcagent, // mtconnect agent
		Config& config,
		std::string machine,  // ip address or pc name
		std::string device,  // mtconnect device name
		std::string tagsection); // tag name for opc mapping
	~COpcAdapter();
	void SetMTCTagValue(std::string tag, std::string value);
	virtual int ReadOPCSettings();
	virtual std::string GetSymbolValue(std::string section, std::string tag, const char * defaultval) ;
	virtual std::vector<std::string>  GetSectionKeys(std::string section);
	
	bool _mRunning;
	void Stop(){ _mRunning=false; }
	void Cycle();
	HRESULT GatherDeviceData();

	virtual void Off() ;
	virtual void On() ;

	virtual void ErrorMessage(std::string errmsg) ;
	virtual void DebugMessage(std::string errmsg) ;

	virtual	void COpcAdapter::OnValueChanged(ItemList & _opcitems);
	/////////////////////////////////////////////////////////////////////////
protected:
	Config						&_config;
	std::string					_device;
	AgentConfigurationEx *		_agentconfig;
	std::string					_status;
	std::string					_sTagSectionName;
	// std::string					_opcservermachine; inherited
	long						_mDelay;
	std::string					lastFeed,lastX, lastY, lastZ, lastA, lastB;
	long		_nFakeSpindle, _nAlarmsEnabled, _nMultiSpindle,mLag;

#ifdef ALARMS
	Config						_alarmconfig;
#endif
};