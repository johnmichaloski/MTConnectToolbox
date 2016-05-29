//
//
//

#pragma once
#include <vector>
#include <string>
#include <map>
#include <stack>

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/timer.hpp>

#include "StateModel.h"
using namespace StateModel;

#include "MTConnectStreamsParser.h"

#include "CmdCell.h"
#include "MTConnectCmdSimAdapter.h"

class AgentMgr;
class CCommand;
class CCellHandler;
class CWorkOrder;
class CWorkOrders;
class CResourceHandler;
class CResourceHandlers;


#include "Device.hpp"
#include "Agent.hpp"
//class AgentConfigurationEx;
//class Device;

template <typename T>
class Queue : public std::vector<T *>
{
	int nMaxSize;
public:
	bool Push(T * workorder) { if(size() < nMaxSize) { push_back(workorder); return true; } else return false; }
	T * Pop() 
	{ 
		T * workorder =NULL; 
		if(size() >0) {workorder=front(); 
		erase(begin());
		}  return workorder; 
	}
	bool IsWaiting() { return ((size()-1) >0 )  && ((nMaxSize-1) > 0); }
	int & MaxSize() { return nMaxSize; }
	bool CanPush() 
	{ 
		if(size() < nMaxSize) 
			return true;  
		else
			return false; 
	}
	bool CanPop()
	{
		if(size() > 0 ) 
			return true;  
		else
			return false; 
	}
	T* Current() 
	{ 
		if(size() ==0)
			return NULL;

		if(CanPop()) 
			return front(); 
		else 
			return NULL; 
	}
};


// Sim StateModel - off, idle, starved, blocked, running, faulted

class CMonitor : public ControlThread, public Queue<CWorkOrder>, public AdapterT
{
public:

	typedef CMonitor MyClass;

	CMonitor(
		std::string name,
		AgentConfigurationEx * mtcagent, // mtconnect agent
		std::string _agentip,
		std::string _statusdevice,
		std::string _cmddevice
		) : AdapterT(mtcagent, _agentip, _cmddevice)  // adapterT is added for items handling
	{
		_name=name;
		programnum=0;
		StateMachineSetup();
		agentip()=_agentip;
		statusdevice()=_statusdevice;
		cmddevice()=_cmddevice;
		Init();
		_heartbeat=0;
	}
	void StateMachineSetup();
	//ControlThread & StateMachine() { return _thread; }
	std::string _name;
	std::string & agentip(){ return __agentip; }
	std::string __agentip;
	std::string & statusdevice(){ return __statusdevice; }
	std::string __statusdevice;
	std::string & cmddevice(){ return __cmddevice; }
	std::string __cmddevice;
	std::string  GetMTCTagValue(std::string tag);
	void SetMTCTagValue(std::string device, std::string tag, std::string value);
	UINT cmdnum;
	MTConnectStreamsParser		_parser;
	UINT programnum;
	UINT _heartbeat;
	// State transitions
	virtual void Init() {LogMessage(_name); LogMessage("Init\n"); }
	virtual void Load() { LogMessage(_name); LogMessage("Load\n"); }
	virtual void Run() ;
	virtual void Stop() { LogMessage(_name); LogMessage("Stop\n"); }
	virtual void Fail() {LogMessage(_name);  LogMessage("Fail\n"); }
	virtual void Hold() { LogMessage(_name); LogMessage("Hold\n"); }
	virtual void Reset() ;
	virtual void Done() ;
	virtual void Next() {LogMessage(_name);  LogMessage("Next\n"); }
	virtual void Resume() {LogMessage(_name);  LogMessage("Resume\n"); }

	// State methods
	virtual void Nop()	{ LogMessage(_name);LogMessage("Nop\n");	}
	virtual void Resetting() ;
	virtual void Off();//	{ LogMessage("Off\n");	}
	virtual void Ready() ;
	virtual void Loaded() { LogMessage(_name); LogMessage("Loaded\n");	}
	virtual void Running();// { LogMessage(name); LogMessage("Running\n");	 }
	virtual void Stopped() { LogMessage(_name); LogMessage("Stopped\n");	 }
	virtual void Interrupted() { LogMessage(_name); LogMessage("Interrupted\n");	 }
	virtual void Faulted(); //  { LogMessage("Faulted\n");	 }
	virtual void Exit() { LogMessage(_name); LogMessage("Exit\n");	 }

	virtual void Blocked();// {LogMessage(name);  LogMessage("Blocked\n");	 }
	virtual void Starved();//{ LogMessage(name); LogMessage("Starved\n");	 }
	
	virtual void Postprocess();
	virtual void Preprocess();
	std::string GenerateCosts(std::string prepend="", std::string postpend="", std::string state="any");
	std::string GenerateTotalCosts(std::string prepend="", std::string postpend="");

	void GenerateStateReport(std::map<std::string,double> &states, double dDivisor=1000);
	void SetMTBF(double sec){ MTBF= sec; stats["MTBF"]=MTBF;}
	void SetMTTR(double sec){ MTTR= sec; stats["MTTR"]=MTTR;}
	void SetMTTP(double sec){  MTTP= sec; stats["MTTP"]=MTTP;}
	void Setup();
	virtual std::string ToString();

	DataDictionary data,lastdata;
	double _dUpdateRateSec;

	double _nMsTimerElapsed ;
	int _nTMsTimeStart;

	std::string GenerateReport();
	std::string GenerateCSVTiming(double divider = 1.0);
	std::string GenerateCSVHeader(std::string units="Seconds");
	std::stack<std::string> _statestack;
	Stats stats;
	CWorkOrder *  pJob;
	CWorkOrder *  pLastCurrent;
	UINT _nLastPartCountGood, _nLastPartCountBad;
		double MTBF, _mtbf; // counter seconds
	double MTTR, _mttr; // counter seconds
	double MTTP;  // counter seconds

	double dMtbf;
	double dRMtbf;

	double dMtp;
	double dRMtp ;

	double dMttr;
	double dRMttr;


	std::map<std::string, Stats> _partStats;
	std::string _partid;

};

