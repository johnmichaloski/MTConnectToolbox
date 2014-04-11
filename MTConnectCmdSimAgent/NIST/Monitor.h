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
public:
	bool Push(T * workorder) { if(size() < nMaxSize) { push_back(workorder); return true; } else return false; }
	T * Pop() 
	{ 
		ATLASSERT(size() >0);
		T * workorder =NULL; 
		if(size() >0) 
		{
			workorder=front(); 
			erase(begin());
		}  
		return workorder; 
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
	/////////////////////////////////////////
	int nMaxSize;
};


// Sim StateModel - off, idle, starved, blocked, running, faulted
// part name, program, # times program run, time in program, total time
typedef boost::tuple<std::string,std::string, int, double, double>  ProgramTimingTuple;

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
		SetStateMachineName(name);
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
	std::string & agentip(){ return __agentip; }
	std::string __agentip;
	std::string & statusdevice(){ return __statusdevice; }
	std::string __statusdevice;
	std::string & cmddevice(){ return __cmddevice; }
	std::string __cmddevice;
	std::string  GetMTCTagValue(std::string tag);
	void SetMTCTagValue(std::string device, std::string tag, std::string value);
	Stats GetStats();
	// State transitions
	virtual void Init() {LogMessage(Name()); LogMessage("Init\n"); }
	virtual void Load() { LogMessage(Name()); LogMessage("Load\n"); }
	virtual void Run() ;
	virtual void Stop() { LogMessage(Name()); LogMessage("Stop\n"); }
	virtual void Fail() {LogMessage(Name());  LogMessage("Fail\n"); }
	virtual void Hold() { LogMessage(Name()); LogMessage("Hold\n"); }
	virtual void Reset() ;
	virtual void Done() ;
	virtual void Next() {LogMessage(Name());  LogMessage("Next\n"); }
	virtual void Resume() {LogMessage(Name());  LogMessage("Resume\n"); }

	// State methods
	virtual void Nop()	{ LogMessage(Name());LogMessage("Nop\n");	}
	virtual void Resetting() ;
	virtual void Off();//	{ LogMessage("Off\n");	}
	virtual void Ready() ;
	virtual void Loaded() { LogMessage(Name()); LogMessage("Loaded\n");	}
	virtual void Running();// { LogMessage(name); LogMessage("Running\n");	 }
	virtual void Stopped() { LogMessage(Name()); LogMessage("Stopped\n");	 }
	virtual void Interrupted() { LogMessage(Name()); LogMessage("Interrupted\n");	 }
	virtual void Faulted(); //  { LogMessage("Faulted\n");	 }
	virtual void Exit() { LogMessage(Name()); LogMessage("Exit\n");	 }

	virtual void Blocked();// {LogMessage(name);  LogMessage("Blocked\n");	 }
	virtual void Starved();//{ LogMessage(name); LogMessage("Starved\n");	 }
	
	virtual void Postprocess();
	virtual void Preprocess();
	std::string GenerateCosts(std::string prepend="", std::string postpend="", std::string state="any");
	std::string GenerateTotalCosts(std::string prepend="", std::string postpend="");

	void GenerateStateReport(std::map<std::string,double> &states, double dDivisor=1000);
	void Setup();
	virtual std::string ToString();
	std::string GenerateReport();
	std::string GenerateCSVTiming(double divider = 1.0);
	static std::string GenerateCSVHeader(std::string units="Seconds");

	double Utilization() { return stats["busy"] /stats["TotalTime"];  }
	double Availability() { return (stats["TotalTime"]-stats["off"]-stats["down"]) /stats["TotalTime"]; }
	double Quality() { if((_nLastPartCountGood+_nLastPartCountBad)==0) return 1; return ((double) _nLastPartCountGood)  / ((double) _nLastPartCountGood+_nLastPartCountBad); }
	double Performance() { return _dPerformanceScale; }
	double OEE() { return Availability() * Quality() * Performance() ; }
	double APT() { return stats["busy"]; }  // actual production time
	double AUPT() { return stats["busy"]-stats["setup"]; } // actual unit processing time (busy + setup)
	double AUBT() { return stats["busy"]; } // actual unit busy time
	double ADOT() { return stats["down"]; }
	double ASUT() { return stats["setup"]; }
	/////////////////////////////////////////////////////////
	DataDictionary										data,lastdata;
	double												_dUpdateRateSec;
	UINT												cmdnum;
	MTConnectStreamsParser								_parser;
	UINT												programnum;
	UINT												_heartbeat;
	
	double												_nMsTimerElapsed ;
	int													_nTMsTimeStart;

	std::stack<std::string>								_statestack;

	CWorkOrder *										pWorkOrder;
	CWorkOrder *										pLastCurrent;

	unsigned int										_nLastPartCountGood;
	unsigned int										_nLastPartCountBad;

	unsigned int										_nPrePartCountGood;
	unsigned int										_nPrePartCountBad;
	double												_dPerformanceScale;

	// Stats
	std::map<std::string, Stats>						_partStats;
	Stats												 stats;
	std::string											_lastProgram;

	//std::vector<ProgramTimingTuple>                     _programTuple;
	//std::map<std::string, double>						GetPartTiming(std::string partid);
	double												GetPartTiming(std::string partid);
	
	void SetMTBF(double sec){ MTBF= sec; stats["MTBF"]=MTBF;}
	void SetMTTR(double sec){ MTTR= sec; stats["MTTR"]=MTTR;}
	void SetMTTP(double sec){  MTTP= sec; stats["MTTP"]=MTTP;}
	double MTBF, _mtbf; // counter seconds
	double MTTR, _mttr; // counter seconds
	double MTTP;  // counter seconds

	double dMtbf;
	double dRMtbf;

	double dMtp;
	double dRMtp ;

	double dMttr;
	double dRMttr;



};

