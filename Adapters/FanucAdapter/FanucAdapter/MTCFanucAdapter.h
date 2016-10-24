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

#include "Config.h"

#include <windows.h>
#include <atlbase.h>
#include "atlutil.h"


#include "StdStringFcn.h"
#include "Config.h"
#include "StdStringFcn.h"
//#include "AppEventLog.h"
#include "Globals.h"

#include "adapter.hpp"
#include "device_datum.hpp"
#include "service.hpp"
#include "condition.hpp"

class CiSeries;
class CFS15D;
class CF15B;

class FanucShdrAdapter : public Adapter, public MTConnectService
{
public:
	PowerState mPower;
	Availability mAvail;
	std::string errmsg;
	struct Item
	{
		std::string _alias;
		std::string  _tagname;
		std::string  _type;
		std::string _subtype;
		std::string _value;
		std::string _lastvalue;
		VARENUM vt;
		Event  _event;
		Item(std::string name="", std::string devicename="") : _event(name.c_str(),devicename.c_str() ){}
	};

	struct ItemsType : public std::vector<Item * > 
	{
		Item * FindAlias(std::string name)
		{
			for(int i =0; i< this->size(); i++)
			{
				if(at(i)->_alias == name)
					return at(i);
			}
			return NULL;
		}
		Item * FindTag(std::string name)
		{
			for(int i =0; i< this->size(); i++)
			{
				Item * item = at(i);
				if(at(i)->_tagname == name)
					return item;
			}
			return NULL;
		}

		std::string GetSymbolValue(std::string szEnumTag, std::string defaultVal)
		{
			for(int i =0; i< this->size(); i++)
			{
				if(at(i)->_type != "Enum")
					continue;
				if(at(i)->_alias == szEnumTag)
					return at(i)->_value;
			}
			return defaultVal;
		}

	} items;

	void WarningMsg( std::string err)
	{
		AtlTrace(err.c_str());
		if(mDebug>3)
		{
			gLogger->warning(err.c_str());
			//EventLogger.LogEvent(err);
		}
	}
	void ErrMsg( std::string err)
	{
		AtlTrace(err.c_str());
		if(mDebug>0)
		{
			gLogger->error(err.c_str());
			//EventLogger.LogEvent(err);
		}
	}
	void AbortMsg( std::string err)
	{
		AtlTrace(err.c_str());
		errmsg+=err;
		//EventLogger.LogEvent(err);
		gLogger->error(err.c_str());
		// 	std::string cmd = StdStringFormat("cmd /c net stop \"%s\" ",  adapter->name().c_str());        
		// taskmgr.Launch(cmd, 000);
	}
	void AddItem(std::string tag, std::string value)
	{
		Item * item;
		item = new Item(tag, Globals.szDeviceName);
		//if(!Globals.szDeviceName.empty())
		//{
		//	strncpy(item->_event.mDeviceName,Globals.szDeviceName.c_str(),NAME_LEN);
		//	item->_event.prefixName(item->_event.mDeviceName);
		//}
	
		item->_tagname=tag;
		item->_type="Event";
		item->_lastvalue=item->_value=value;
		item->_event.setValue(value.c_str());
		item->_event.mHasValue=true;
		addDatum(item->_event);
//		strncpy(item->_event.mName,tag.c_str(),NAME_LEN);
		items.push_back(item);
	}
	void SetMTCTagValue( std::string tag, std::string value)
	{
		if(tag=="avail") return;
		if(tag=="avail")
		{
			if(value=="UNAVAILABLE")
				mAvail.unavailable();
			else
				mAvail.available();
				return;
		}
		else if(tag=="power")
		{
			if(value=="ON")
				mPower.setValue(PowerState::eON);
			else
				mPower.setValue(PowerState::eOFF);
			return;
		}

		Item* item = items.FindTag(tag);
		if(item==NULL)
		{
			//EventLogger.LogEvent(StdStringFormat("SetMTCTagValue tag %s not found\n", tag.c_str()));
			gLogger->warning(StdStringFormat("SetMTCTagValue tag %s not found", tag.c_str()).c_str());
			return;
		}
		
		item->_lastvalue=item->_value;
		item->_value=value;
	}
	std::string GetMTCTagValue(std::string tag)
	{	

		if(tag=="avail")
		{
			if(mConnected)
				return "AVAILABLE";
			return "UNAVAILABLE";

		}
		else if(tag=="power")
		{
			if(mPower.getValue() == PowerState::eON)
				return "ON";

			return "OFF";
		}

		Item * item=items.FindTag(tag);
		if(item!=NULL)
		{
			gLogger->warning(StdStringFormat("SetMTCTagValue tag %s not found", tag.c_str()).c_str());
			//EventLogger.LogEvent(StdStringFormat("SetMTCTagValue tag %s not found\n", tag.c_str()));
			return item->_value;
		}
		return "";
	}
	int mPort;
	int mSleep;
	static int mDebug;
	unsigned short mFlibhndl;
	int _nAxes;
	std::map<char,int> axisnum;
	//std::string mDeviceIP;
	std::string sHeartbeat;
	int heartbeat;
	int mDevicePort;
	bool mConnected;

	//////////////////////////////////////////////////////////////////

	FanucShdrAdapter(int aPort, int aSleep=1000);
	virtual HRESULT Configure();
	virtual void Clear();
	virtual void CreateItems();
	std::string getProgramName(char * buf) ;

	virtual void initialize(int aArgc, const char *aArgv[]);
	virtual void start();
	virtual void stop();
	virtual  void gatherDeviceData();
	virtual void disconnect();
	virtual void update();
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
	////////////////////////////////////////////////////////////////////
	bool ResetAtMidnite();
	CWorkerThread<> _resetthread;
	struct CResetThread : public IWorkerThreadClient
	{
		HRESULT Execute(DWORD_PTR dwParam, HANDLE hObject);
		HRESULT CloseHandle(HANDLE){ ::CloseHandle(_hTimer); return S_OK; }
		HANDLE _hTimer;
	} _ResetThread;
};

