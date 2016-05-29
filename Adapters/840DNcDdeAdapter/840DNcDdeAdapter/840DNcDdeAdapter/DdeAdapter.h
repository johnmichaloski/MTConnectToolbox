//
// DdeAdapter.h
//


#pragma once
#include "conio.h"
#include <windows.h>
#include <atlbase.h>
#include "atlutil.h"

#include "adapter.hpp"
#include "device_datum.hpp"
#include "service.hpp"
#include "condition.hpp"
#include "DDEapi.h"

#include "StdStringFcn.h"
#include "Config.h"
#include "StdStringFcn.h"
#include "ListExeProcesses.h"
#include "AppEventLog.h"
#include "RegieIni.h"
#include "Logger.h"

#define REALTHING

class DdeAdapter : public Adapter, public MTConnectService
{
public:
	Availability mAvailability; 
	PowerState mPower;


	struct Item
	{
		std::string _alias; // mtconnect tag name!
		std::string  _tagname;  // opc name
		std::string  _type;
		std::string _subtype;
		std::string _value;
		std::string _lastvalue;
		VARENUM vt;
		Event  _event;
		Item() : _event(""){}
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
				if(at(i)->_tagname == name)
					return at(i);
			}
			return NULL;
		}
		bool FindEnum(std::string name) // match partial e.g., Enum.controllermode 
		{
			for(int i =0; i< this->size(); i++)
			{
				if(at(i)->_alias.find(name) != std::string::npos)
					return true;
			}
			return false;
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
		GLogger.LogMessage(err);
		//if(mDebug>3)
		//{
		//	EventLogger.LogEvent(err);
		//}
	}
	void ErrMsg( std::string err);
	void AbortMsg( std::string err)
	{
		AtlTrace(err.c_str());
		GLogger.Fatal(err);
		//EventLogger.LogEvent(err);
		ExitProcess(-1); 
		// 	std::string cmd = StdStringFormat("cmd /c net stop \"%s\" ",  adapter->name().c_str());        
		// taskmgr.Launch(cmd, 000);
	}


	DdeAdapter(int aPort, int aSleep=1000): Adapter(aPort, aSleep), 
		mAvailability("avail"), mPower("power")
	{
		mSleep=1000;
		tagdefs="TAGS";
		addDatum(mPower);
		addDatum(mAvailability);
		bConnected=false;
		mCyclesUntilUp=10;
#ifdef SIEMENS
		_p840DShutdownSink= new CComMcEventCreationSink();
#endif
	}
	// Cannot throw from here - can be called from within catch
	virtual void clear()
	{
#ifdef SIEMENS
		_p840DShutdownSink->Unregister();
#endif
		ddeAdapter.Disconnect();
		mAvailability.unavailable();
		for(int i = 0 ; i < items.size(); i++)
		{
			if(items[i]->_type==_T("Event") || items[i]->_type==_T("Sample"))
			{
				items[i]->_event.setValue("UNAVAILABLE");
			}
		}
		mPower.unavailable();
	}

	virtual void initialize(int aArgc, const char *aArgv[]);

	virtual HRESULT init()
	{
		try{			
			ddeAdapter.Init();
			if(DMLERR_NO_ERROR!=ddeAdapter.Connect((LPSTR) app.c_str(), (LPSTR) topic.c_str()))
				throw std::exception(StdStringFormat("Error: ddeAdapter Connect App=%s Topic %s FAILED \n").c_str()); 

		}
		catch(std::exception e)
		{
			AtlTrace( e.what());
			return E_FAIL;
		}
		return S_OK;
	}
	virtual void start()
	{  
		startServer(); 	
	}
	virtual void stop()
	{
		clear();
		// Should wait till all unknowns posted to listeners
		stopServer();
		ddeAdapter.Quit();
	}

	virtual void gatherDeviceData();
	HRESULT ParseOptions();
	//////////////////////////////////////////////////////////////////
	std::string mSpawnDDE;
	std::string app;
	std::string topic ;
	std::string cnc;
	DDEAdapter ddeAdapter;
	EventLogging eventlog;
#ifdef SIEMENS
	CComMcEventCreationSink * _p840DShutdownSink;
#endif
	//	Condition mSystem; 
	std::string tagdefs;
	bool bConnected;
//	std::map<std::string, Sample> samples;
//	std::map<std::string, Event> events;

	int mPort;
	int mSleep;
	static int mDebug;
	static int mCyclesUntilUp;
	//////////////////////////////////////////////////////////////////
	bool ResetAtMidnite();
	CWorkerThread<> _resetthread;
	struct CResetThread : public IWorkerThreadClient
	{
		HRESULT Execute(DWORD_PTR dwParam, HANDLE hObject);
		HRESULT CloseHandle(HANDLE){ ::CloseHandle(_hTimer); return S_OK; }
		HANDLE _hTimer;
	} _ResetThread;
};


