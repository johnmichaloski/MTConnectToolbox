//
// ShdrEcho.h
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.


#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <ATLComTime.h>

#include "ComMTCShdrBackEnd.h"
#include "MTConnectDeviceParser.h"
#include "Logger.h"
#include "SimMath.h"


class CShdrEchoInstance
{
public:

	CShdrEchoInstance(std::string devicesxmlfilename,
		std::string shdrfilename, 
		int ipport=7878,
		double dOverride=100.0,
		std::string devicename="");
	~CShdrEchoInstance(void);
	void  FaultThread();
	void Init(std::string devicesxmlfilename,
		std::string shdrfilename, 
		int ipport=7878,
		double dOverride=100.0,
		std::string devicename="");
	int GetNextTokens();
	void OutputTokens();
	void Open() ;
	void Start();
	HRESULT Cycle();
	void Close(void);
	void EchoShdr(std::string ) ;
	void Join()								{ _thread.join(); }
	void Stop()								{ _state=Stopping; }

	std::string & DevicesXmlFilename()		{ return _devicesxmlfilename; }
	std::string & ShdrFilename()			{ return _shdrfilename; }
	std::string & IpAddress()				{ return _ipaddr; }
	int & IpPort()							{ return _ipport; }
	std::string & DeviceName()				{ return _devicename; }
	double & Override()						{ return _dOverride; }

	std::string								_devicesxmlfilename;
	std::string								_shdrfilename;
	std::string								_ipaddr; // e.g., 127.0.0.1
	std::string								_devicename; 
	int										_ipport; // e.g., 7878
	double									_dOverride;

	CComPtr<CoComMTCShdrBackEnd>			_backend;
	MTConnectDeviceParser					_deviceparser;
	DeviceModel								_devicemodel;
	std::ifstream							in;
	std::stringstream						strbuffer; 
	std::string								_buffer;
	DataDictionary							_dict;
	std::vector<std::string>				tokens;
	bool									_bRepeat;
	MapValues								localvalues;

	COleDateTime							lasttime;
	COleDateTime							intime;
	COleDateTimeSpan						diff;

	boost::thread							_thread;
	boost::thread							_faultthread;
	bool									_bRunning;
	void SetFault(	Distribution	mtbfstat, Distribution mttrstat)		
	{
		_mtbfstat=mtbfstat;
		_mttrstat=mttrstat;
		_bFaultThread=true;
	}
	bool							_bFaultThread;
	Distribution					_mtbfstat, _mttrstat;

	enum StateType {Ready=0, Faulted, Running, Stopping } ;
	StateType						_state;
	bool IsReady()					{ return _state==Ready; }
	bool IsRunning()				{ return _state==Running; }
	bool IsFaulted()				{ return _state==Faulted; }
	bool IsStopping()				{ return _state==Stopping; }
	void Fault()					{ _state=Faulted; }
};

class CShdrEchoCollection
{
	std::vector<CShdrEchoInstance *> shdrs;
public:
	CShdrEchoCollection(void){}
	~CShdrEchoCollection(void){}
	int size() { return shdrs.size(); }
	CShdrEchoInstance * AddShdrEcho(std::string devicesxmlfilename,
		std::string shdrfilename, 
		int ipport=7878,
		double dOverride=100.0,
		std::string devicename="")
	{
		CShdrEchoInstance * shdr = new CShdrEchoInstance(devicesxmlfilename, shdrfilename,ipport, dOverride, devicename);
		shdrs.push_back(shdr);
		return shdrs.back();
	}
	void StartAll()
	{
		for(int i=0; i< shdrs.size(); i++)
		{
			shdrs[i]->Start();
		}
		Join();
	}
	void StopAll()
	{
		for(int i=0; i< shdrs.size(); i++)
		{
			if(shdrs[i]->_bFaultThread)
			{
				shdrs[i]->_faultthread.interrupt();
			}
			shdrs[i]->Stop() ;
		}

	}
	void Join()
	{
		if(shdrs.size() > 0)
			shdrs[0]->Join();
	}
	void Spawn(std::string devicesxmlfilename,
		std::string shdrfilename, 
		int ipport=7878,
		std::string devicename=""){}
};
