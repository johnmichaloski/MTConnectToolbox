//
// MTConnectCmdSimAdapter.h
//
#pragma once
#include <string>
#include <stack>

#include "Config.h"
#include "MTConnectCmdSimAgent.h"
#include "MTConnectStreamsParser.h"

#include "NIST/StdStringFcn.h"
#include "NIST/Config.h"
#include "NIST/AppEventLog.h"
#include "SimMath.h"
#include "NIST/ElTime.h"

class AgentConfigurationEx;
class Device;

class AdapterT
{
public:
	AdapterT(AgentConfigurationEx * mtcagent, // mtconnect agent
		std::string machine,  // ip address or pc name
		std::string device);
	~AdapterT(void);
	virtual void			Cycle(){}
	virtual void			Configure(){};
	virtual HRESULT			DebugMessage(std::string errmsg) ;
	virtual  void			Disconnect(){  }
	virtual HRESULT			ErrorMessage(std::string errmsg) ;
	virtual  HRESULT		GatherDeviceData(){ return E_NOTIMPL; }
	virtual void			Off() ;
	virtual void			On() ;

	void					SetMTCTagValue(std::string tag, std::string value);
	virtual void			SetMTCConditionValue(std::string tag, 
		std::string alarmLevel,
		std::string sNativeCode,
		std::string alarmSeverity,
		std::string sQualifier,
		std::string sAlarmMessage);
	void					Stop(){ _mRunning=false; }


	struct Item
	{
		std::string _alias;
		std::string  _tagname;
		std::string  _type;
		std::string _subtype;
		std::string _value;
		std::string _lastvalue;
		VARENUM vt;
		//Event * _event;
	};

	struct ItemsType : public std::vector<Item * > 
	{
	/*	Item * FindAlias(std::string aliasname)
		{
			std::string name = MakeLower(aliasname);
			for(int i =0; i< this->size(); i++)
			{
				if(at(i)->_alias == name)
					return at(i);
			}
			return NULL;
		}*/
		Item * FindTag(std::string tagname)
		{
			std::string name = MakeLower(tagname);
			for(int i =0; i< this->size(); i++)
			{
				Item * item = at(i);
				if(at(i)->_tagname == name)
					return item;
			}
			return NULL;
		}
		void SetTag(std::string tagname, std::string value)
		{
			std::string name = MakeLower(tagname);
			AddIfNone(name);

			for(int i =0; i< this->size(); i++)
			{
				if(at(i)->_tagname == name)
				{
					at(i)->_value=value;
					return ;
				}
			}
		}
		std::string  GetTag(std::string tagname, std::string defaultVal)
		{
			std::string name = MakeLower(tagname);
			for(int i =0; i< this->size(); i++)
			{
				if(at(i)->_tagname == name)
				{
					return at(i)->_value;
				}
			}
			return defaultVal;
		}
		std::string GetSymbolEnumValue(std::string szEnumTag, std::string defaultVal)
		{
			for(int i =0; i< this->size(); i++)
			{
				if(at(i)->_type != "Enum")
					continue;
				if(at(i)->_alias != szEnumTag)
					return at(i)->_value;
			}
			return defaultVal;
		}
		bool AddIfNone(std::string tagname)
		{
			std::string name = MakeLower(tagname);
			Item * item = FindTag(name);
			if(item==NULL)
			{
				Item * item = new Item();
				item->_type=_T("Event");
				item->_tagname=name;
				push_back(item);
				return true;
			}
			return false;
		}

	} items;
	void SaveCmdData(DataDictionary  cycledata);
	/////////////////////////////////////////////
	static int					mDebug;
	int							_nServerRate;
	bool						_mRunning;
	int							_nQueryServerPeriod;
	std::string					_device;
	AgentConfigurationEx *		_agentconfig;
	std::string					_status;
	EventLogging				eventlog;
	std::string					inifile;

};

class MTConnectCmdSimAdapter :public AdapterT
{
public:
	MTConnectCmdSimAdapter(AgentConfigurationEx * mtcagent, // mtconnect agent
		//std::string machine,  // ip address or pc name
		std::string device) : AdapterT(mtcagent,  "127.0.0.1", device) 
	{
		
		waitProgram=0;
		waitRepair=0;
		waitFault=5;
		nFaultNum=nLastFaultNum=0;
		_ttfdist.SetParameters(_T("uniform"), 9, 10);
		_ttrdist.SetParameters(_T("uniform"), 9.5, 10);
		_ttrresponse.SetParameters(_T("uniform"), 9.9, 10);
		currentDecrementer=&waitFault;
		_nTMsTimeStart =  NIST::Millitime::GetMilliCount();
	}
	struct ParameterError
	{
		ParameterError(const std::string &aCode, const std::string &aMessage) 
		{
			GLogger.Warning(aMessage.c_str() );
		}
	};
	void				CreateItem(std::string tag, std::string type="Event");
	virtual void		Configure();
	virtual void		Cycle();
	virtual  void		Disconnect();
	HRESULT				FailWithMsg(HRESULT hr, std::string errmsg);
	HRESULT				FaultHandling();
	virtual  HRESULT	GatherDeviceData();
	int					ProgramDuration(std::string program);
	HRESULT				WarnWithMsg(HRESULT hr, std::string errmsg);
	////////////////////////////////////////////////////////////////////////////

	MTConnectStreamsParser			_parser;
	UINT							nLastCmdNum;
	int								waitProgram;
	std::string						_ipaddr;

	UINT							nFaultNum,nLastFaultNum;
	int								waitFault,waitRepair;
	int *							currentDecrementer;
	std::string						_sFaultInfo;
	std::stack<std::string>			cmdstack;
	MATH::Distribution				_ttfdist,_ttrdist,_ttrresponse;
	std::string						command;
	int nCmdNum;
	double							_nMsTimerElapsed ;
	int								_nTMsTimeStart;
};
