// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#include "StdAfx.h"
#include "Monitor.h"

//#include "NIST/Config.h"
#include "NIST/StdStringFcn.h"
#include "NIST/Logger.h"
#include "Globals.h"
#include "NIST/ElTime.h"

#include "KPI.h"
//MTConnect


std::string CMonitor::GetMTCTagValue(std::string tag)
{
	return items.GetTag(tag,"");
}

void CMonitor::SetMTCTagValue(std::string device, std::string tag, std::string value)
{
	Agent * agent = _agentconfig->getAgent();
	Device *dev = agent->getDeviceByName(device);
	DataItem *di = dev->getDeviceDataItem(tag);
	if (di != NULL)
	{
		std::string time = getCurrentTime(GMT_UV_SEC);
		agent->addToBuffer(di, value, time);
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////
// Sim StateModel - off, idle, starved, blocked, running, faulted

void CMonitor::StateMachineSetup()
{

	_statemachine.push_back(StateMachineTuple(off,init,ready,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Init, this))); 
//	_statemachine.push_back(StateMachineTuple(ready,load, loaded,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Load, this))); 
	_statemachine.push_back(StateMachineTuple(ready,run, running,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Run, this))); 
	_statemachine.push_back(StateMachineTuple(stopped,run, running,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Reset, this))); 
	_statemachine.push_back(StateMachineTuple(running,stop, stopped,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Stop, this))); 
	//_statemachine.push_back(StateMachineTuple(running,hold, interrupted,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Hold, this))); 
	_statemachine.push_back(StateMachineTuple(running, done, finished,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Done, this))); 
	_statemachine.push_back(StateMachineTuple(finished, "next", ready,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Next, this))); 
	_statemachine.push_back(StateMachineTuple(interrupted,run, running,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Resume, this))); 
	_statemachine.push_back(StateMachineTuple("resetting","next", ready,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Next, this))); 

	_statemachine.push_back(StateMachineTuple(blocked,run, running,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Running, this))); 
	_statemachine.push_back(StateMachineTuple(starved,run, running,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Running, this))); 
	_statemachine.push_back(StateMachineTuple(faulted,run, running,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Running, this))); 
	_statemachine.push_back(StateMachineTuple(faulted, "softreset", ready,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Running, this))); 
	_statemachine.push_back(StateMachineTuple(finished, "next", ready,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Next, this))); 
	_statemachine.push_back(StateMachineTuple(faulted, "next", ready,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Next, this))); 

	_statemachine.push_back(StateMachineTuple(any,quit, "myExit",  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Exit, this))); 
	_statemachine.push_back(StateMachineTuple(any,fail, faulted,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Fail, this))); 
	_statemachine.push_back(StateMachineTuple(any,estop, estopped,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Stop, this))); 
	_statemachine.push_back(StateMachineTuple(any,reset, "resetting",  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Reset, this))); 
	_statemachine.push_back(StateMachineTuple(any,off, off,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Off, this))); 
	_statemachine.push_back(StateMachineTuple(any,block, blocked,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Blocked, this))); 
	_statemachine.push_back(StateMachineTuple(any,starved, starved,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Running, this))); 
	_statemachine.push_back(StateMachineTuple(any,run, running,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Running, this))); 

	_stateMap[off]=boost::bind(&MyClass::Off, this);
	_stateMap["resetting"]=boost::bind(&MyClass::Resetting, this);
	_stateMap[ready]=boost::bind(&MyClass::Ready, this);
	_stateMap[running]=boost::bind(&MyClass::Running, this);
	_stateMap[stopped]=boost::bind(&MyClass::Stopped, this);
	_stateMap[interrupted]=boost::bind(&MyClass::Interrupted, this);
	_stateMap[faulted]=boost::bind(&MyClass::Faulted, this);
	_stateMap["myExit"]=boost::bind(&MyClass::Exit, this);
	_stateMap["blocked"]=boost::bind(&MyClass::Blocked, this);
	_stateMap["starved"]=boost::bind(&MyClass::Starved, this);

	_dUpdateRateSec=1.0;  // assume updates of 1 second
	cmdnum=0;
	Setup();
}
void CMonitor::Setup()
{
	stats.nBlockedTime=0.0;
	stats.nStarvedTime=0.0;
	stats.nDownTime=0.0;
	stats.nProductionTime=0.0;
	stats.nOffTime=0.0;
	stats.nRepairTime=0.0;
	stats.nIdleTime=0.0;
	_mtbf=MTBF;
	_mttr=MTTR;
	stats.nTotalParts=stats.nGoodParts=0;
	_nLastPartCountGood=_nLastPartCountBad=0;
	_nTMsTimeStart =  NIST::Millitime::GetMilliCount();
}

void CMonitor::Reset() 
{
	SetMTCTagValue(cmddevice(), "avail", "AVAILABLE");
	this->SetMTCTagValue(cmddevice(), "command", "RESET");
	SetMTCTagValue(cmddevice(),"cmdnum", StdStringFormat("%d", ++cmdnum));
}

void CMonitor::Resetting() 
{
	if(GetState() == "resetting" && items.GetTag(statusdevice() + "." + "execution","") == "READY")
	{
		this->SyncEvent("next");
	}

}
void CMonitor::Run()
{
	ATLASSERT(0<=pJob->_currentprog && pJob->_currentprog < pJob->_estimatedTimes.size());
	SetMTCTagValue(cmddevice(),"command", "RUN");
	SetMTCTagValue(cmddevice(),"partid", "1111");
	std::string program = pJob->_programs[pJob->_currentprog];
	SetMTCTagValue(cmddevice(),"program", program);
	SetMTCTagValue(cmddevice(),"estimatedCompletion", ConvertToString<int>(pJob->_estimatedTimes[pJob->_currentprog]));

	SetMTCTagValue(cmddevice(),"cmdnum", StdStringFormat("%d", ++cmdnum));
}
void CMonitor::Ready()
{
	//stats.nIdleTime+=this->UpdateRate();
	stats.nIdleTime+=_nMsTimerElapsed;
	
}
void CMonitor::Off()
{
	stats.nOffTime+= this->UpdateRate();
}
void CMonitor::Running()
{ 
	//OutputDebugString(StdStringFormat("%s Running Queue Current = %X %x\n", name.c_str(), Current(), ConvertToString(_mttp).c_str()).c_str());
	if(Current()!=NULL ) 
		stats.nProductionTime+= UpdateRateSec();

	if(pJob!=NULL && pJob->_currentprog >= pJob->_programs.size())
		SyncEvent("done");

}

void CMonitor::Done()
{
	// FIXME: count of parts and good/bad parts updated
	Stats &partStats= _partStats[ Current()->_partid];
	Stats::Update("TotalParts", 1.0, Current()->stats,partStats, stats);
	if(Current()!=NULL && Current()->_programs.size()>=0 &&  findStringIC(Current()->_programs[Current()->_programs.size()-1].c_str(), ("inspect")))
	{
	int nPartCountGood = ConvertString<int>(items.GetTag("partcountgood", "0"),0);
	int nPartCountBad = ConvertString<int>(items.GetTag("partcountgood", "0"),0);
	Stats::Update("GoodParts", nPartCountGood-_nLastPartCountGood, Current()->stats,partStats, stats);
	Stats::Update("ScrapParts", nPartCountBad-_nLastPartCountBad, Current()->stats,partStats, stats);
	}
}
void CMonitor::Faulted()
{ 
	stats.nDownTime+= _dUpdateRateSec; // assume seconds
	stats.nRepairTime+= _dUpdateRateSec;  // assume seconds
	// Add up utilites in faulted state? Or just add to preprocess step


}	

void CMonitor::Preprocess()
{

	_nMsTimerElapsed = ((double)NIST::Millitime::GetMilliSpan( _nTMsTimeStart ))/1000.0;
	_nTMsTimeStart =  NIST::Millitime::GetMilliCount();

	_dUpdateRateSec = UpdateRateSec();
	Stats::Update("TotalTime", UpdateRateSec(), stats);				// update total time for resource
	Stats::Update(StateStr(GetState()), _dUpdateRateSec, stats);	// update time in current state
	pJob=Current();

	SetMTCTagValue(cmddevice(),"heartbeat", StdStringFormat("%d", _heartbeat++));

	// Read Command Agent values
	//"127.0.0.1:5000"
	std::vector<DataDictionary> cycledatum = _parser.ReadStream(agentip()+"/"+statusdevice());
	if(cycledatum.size()<1)
	{
		if(GetState()!="Off")
		{
			SetMTCTagValue(cmddevice(),"power", "OFF");
			SyncEvent("off");

		}
		return;
	}
	int _nLastPartCountGood = ConvertString<int>(items.GetTag("partcountgood", "0"),0);
	int _nLastPartCountBad = ConvertString<int>(items.GetTag("partcountgood", "0"),0);
	data=cycledatum[0];

	for(DataDictionary::iterator it = data.begin(); it!=data.end(); it++)
	{
		items.SetTag(statusdevice() + "." + (*it).first, (*it).second);
	}

	if(GetState() == "running" && items.GetTag(statusdevice() + "." + "execution","") == "READY")
	{
		pJob=Current();
		if(Current()==NULL)
		{
			//DebugBreak();
		}

		pJob->_currentprog++;
		if(pJob->_currentprog >= pJob->_programs.size())
		{
//			pJob->_programs=std::vector<std::string>(); // zero out programs
			pJob->_currentprog=-1;
			SyncEvent("done");
			SetMTCTagValue(cmddevice(),"command", "");
		}
		else
		{
			Run();  // send new program
		}

	}
	if(GetState() == "ready" && items.GetTag(statusdevice() + "." + "execution","") == "READY")
	{
		 if(Current()==NULL)
		{

		}
		else if(Current()!=NULL && pJob->_currentprog<0)
		{
			pJob=Current();

			pJob->_currentprog=0;
			SyncEvent("run");
		}


	}

	bool bFault = false;
	bFault|= items.GetTag(statusdevice() + "." + "system_cond", "") == "fault";
	bFault|= items.GetTag(statusdevice() + "." + "comms_cond", "") == "fault"; 
	bFault|= items.GetTag(statusdevice() + "." + "electric_temp", "") == "fault"; 
	bFault|= items.GetTag(statusdevice() + "." + "comms_cond", "") == "fault"; 
	bFault|= items.GetTag(statusdevice() + "." + "system_cond", "") == "fault"; 
	// Detect faulted?
	if(bFault && GetState() != "faulted") 
	{
		_statestack.push(GetState());
		SyncEvent("fail");
	}
	// Done in faulted state?
	else if(!bFault && this->GetState() == "faulted")
	{
		if(_statestack.size()>0)
		{
			std::string laststate= _statestack.top();
			_statestack.pop();
			if(laststate=="running")
				SyncEvent("run");
			else if(laststate=="ready")
				SyncEvent("softreset");
			else if(laststate=="finished")
				SyncEvent("next");
			else if(laststate=="resetting")
				SyncEvent("reset");
			else
				DebugBreak();
		}
		else
		{
			OutputDebugString(StateMachineTraceDump().c_str());
			DebugBreak();
		}
	}
	
	if(GetState() == "finished")
	{
		//if(Current()!=NULL)
			SyncEvent("next");
	}

	SetMTCTagValue(cmddevice(),"idleTime", NIST::Time.HrMinSecFormat( stats.nIdleTime) );
	SetMTCTagValue(cmddevice(),"faultTime", NIST::Time.HrMinSecFormat( stats.nProductionTime) );
	SetMTCTagValue(cmddevice(),"programTime",NIST::Time.HrMinSecFormat(stats.nDownTime)  );
	
	if( Current() !=NULL)
	{
	 	Stats partStats= _partStats[ Current()->_partid];
		Stats::Update(StateStr(GetState()), _dUpdateRateSec, Current()->stats,Current()->GetStepStat(),partStats);
	    Stats::Update("TotalTime", _dUpdateRateSec, Current()->stats,Current()->GetStepStat(),partStats);
		 // fix me those on queue - are idle, waiting
		 //for(int i=1; i< Queue<CWorkOrder>::size(); i++)
			//Stats::Update("idle", _dUpdateRateSec,  at(i)->stats,Current()->GetStepStat(),partStats);

	}
}

std::string CMonitor::ToString()
{ 
	std::string tmp =  StdStringFormat("Cmd:%-16.16s  State:%-10.10s QSize:%2d Step#:%2d Part:%-8.8s Program:%-6.6s  Program#:%-4.4s\n", 
		//this->_statemachinename.c_str(), 
		this->cmddevice().c_str(), 
		GetStateName().c_str(), 
		size(),   
		Current()!=NULL? Current()->CurrentStep() : -1, 
		Current()!=NULL? Current()->_partid.c_str() : "", 
		(Current()!=NULL && Current()->_currentprog>=0 )  ?Current()->_programs[Current()->_currentprog].c_str(): "",
		(Current()!=NULL )? ConvertToString<int>(Current()->_currentprog).c_str(): "-99");
	return tmp;
}


void CMonitor::Postprocess()
{
	//OutputDebugString(StdStringFormat("%s", this->Name().c_str()).c_str());
	lastdata=data;
	pLastCurrent=Current();

	for(int i=0; i< _costfcns.size() ; i++)
	{
		if(get<0>(_costfcns[i]) == "any")
		{
			get<4>(_costfcns[i])=get<4>(_costfcns[i]) + _dUpdateRateSec * get<5>(_costfcns[i]);
		}
		if(get<0>(_costfcns[i]) == GetState()) // looking for faulted state
		{
			get<4>(_costfcns[i])=get<4>(_costfcns[i]) + _dUpdateRateSec * get<5>(_costfcns[i]);
		}

	}
}
void CMonitor::Blocked()
{
	//OutputDebugString(StdStringFormat("%s Blocked Queue Current = %X %x\n", name.c_str(), Current(), ConvertToString(nBlockedTime).c_str()).c_str());
	stats.nBlockedTime+= _dUpdateRateSec;
	if(Current() != NULL && (Current()->_mttp>0 ) )  // item to process...
		this->SyncEvent("run");
	if(Current() == NULL)
		this->SyncEvent("starved");
}
void CMonitor::Starved()
{
	//OutputDebugString(StdStringFormat("%s Starved Queue Current = %X %x\n", name.c_str(), Current(), ConvertToString(nBlockedTime).c_str()).c_str());
	stats.nStarvedTime+= _dUpdateRateSec;

	if(Current() != NULL) 
		this->SyncEvent("run");
}
std::string CMonitor::GenerateReport()
{
	std::string tmp;
	tmp+= ToString();
	tmp+=StdStringFormat("\t Faulted Time =%8.2f\n", stats.nDownTime);
//	tmp+=StdStringFormat("\t Blocked Time =%8.2f\n", stats.nBlockedTime);
//	tmp+=StdStringFormat("\t Starved Time =%8.2f\n", stats.nStarvedTime);
	tmp+=StdStringFormat("\t Repair Time =%8.2f\n", stats.nRepairTime);
	tmp+=StdStringFormat("\t Idle Time =%8.2f\n", stats.nIdleTime);
	tmp+=StdStringFormat("\t Production Time =%8.2f\n", stats.nProductionTime);
	tmp+=StdStringFormat("\t Off Time =%8.2f\n", stats.nOffTime);
	return tmp;
}

void CMonitor::GenerateStateReport(std::map<std::string,double> &states, double dDivisor )
{

	states["down"]=0.0; 
	states["blocked"]=0.0; 
	states["starved"]=0.0; 
	//states["repair"]=0.0; 
	states["production"]=0.0; 
	states["off"]=0.0; 

	if(dDivisor!=1.0)
		dDivisor= (stats.nBlockedTime+stats.nStarvedTime+stats.nDownTime+stats.nProductionTime+stats.nOffTime)/100.0;

	states["down"]+= stats.nDownTime/dDivisor;
	states["blocked"]+= stats.nBlockedTime/dDivisor;
	states["starved"]+= stats.nStarvedTime/dDivisor;
	//states["repair"]+= nRepairTime/dDivisor;
	states["production"]+= stats.nProductionTime/dDivisor;
	states["off"]+= stats.nOffTime/dDivisor;
}

std::string CMonitor::GenerateCSVHeader(std::string units)
{
	// FIXME: this will have to be smarter
	 return StdStringFormat("Down<BR>(%s)</BR>,Blocked<BR>(%s)</BR>,Starved<BR>(%s)</BR>,Production<BR>(%s)</BR>,Off<BR>(%s)</BR>",
		 units.c_str(), units.c_str(), units.c_str(),units.c_str(),units.c_str());
}
std::string CMonitor::GenerateCSVTiming(double divider)
{
	std::string tmp;
	tmp+=StdStringFormat("%8.2f,", stats.nDownTime/divider);
	tmp+=StdStringFormat("%8.2f,", stats.nBlockedTime/divider);
	tmp+=StdStringFormat("%8.2f,", stats.nStarvedTime/divider);
//	tmp+=StdStringFormat("%8.2f,", nRepairTime/divider);
	tmp+=StdStringFormat("%8.2f,", stats.nProductionTime/divider);
	tmp+=StdStringFormat("%8.2f", stats.nOffTime/divider);
	return tmp;
}


std::string CMonitor::GenerateCosts(std::string prepend, std::string postpend, std::string state)
{
	std::string tmp;
	// Units, name, state, time, cost
	for(int i=0; i< _costfcns.size(); i++)
	{
		std::string entry;
		std::string costunit = get<2>(_costfcns[i]) ;
		entry+=get<2>(_costfcns[i]) + "," ; // units
		entry+=get<1>(_costfcns[i]) + ",";  // name
		entry+=get<0>(_costfcns[i])+ ",";  // state
		entry+=ConvertToString(get<4>(_costfcns[i]))+ ",";  // time
		entry+="$";
		if(globalCosts.find(costunit) != globalCosts.end())
			entry+=ConvertToString(get<4>(_costfcns[i] ) * globalCosts[costunit]);
		else
			entry+=ConvertToString(get<4>(_costfcns[i]));

		if(get<0>(_costfcns[i]) != state)
			continue;
		tmp+=prepend;
		tmp+=entry;
		tmp+=postpend;
	}
	return tmp;
}
std::string CMonitor::GenerateTotalCosts(std::string prepend, std::string postpend)
{
	std::string tmp;
	double cost=0.0;
	// Units, name, state, time, cost
	for(int i=0; i< _costfcns.size(); i++)
	{
		std::string costunit = get<2>(_costfcns[i]) ;
		if(globalCosts.find(costunit) != globalCosts.end())
			cost+=get<4>(_costfcns[i] ) * globalCosts[costunit];
		else
			return ",";
	}
	tmp=prepend;
	tmp+=StdStringFormat("$%8.2f,",cost);
	tmp+=postpend;
	return tmp;
}
/**

	Electrical Voltage
	Electrical  HP
	Electrical  Motor kW
	Electrical  Load kW
	Electrical  kVA
	Electrical Demand Factor
	Electrical  Demand kVA
	Electrical  Circuit Size

	Natual Gas PSI
	Natual Gas CFG
	Natual Gas  Size

	Water City GPM
	Water Plant GPM
	Water Chilled GPM
	Water Size

	Hydraulics Unit
	Hydraulics PSI	
	Hydraulics GPM	
	Hydraulics Size

	Air Dry CFM	
	Air Plant CFM
	Air PSI	
	Air	Size

	Tea Piping

	Resin Piping

	Steam

	Pattern Spray



	#1 LINE
Cast Line 1 Entry Elevator Upper Conveyor
Cast Line 1 Entry Elevator Lower Conveyor
Cast Line 1 Entry Elevator Hydraulic Unit Pump
Cast Line 1 Entry Elevator Hydraulic Unit Recirc Pump

Cast Line 1 Double Deck Entry Upper Conveyor #1
Cast Line 1 Double Deck Entry Upper Conveyor #2
Cast Line 1 Double Deck Entry Lower Conveyor #1
Cast Line 1 Double Deck Entry Lower Conveyor #2

Cast Line 1 Lift-Locate Unit W/Stop

Cast Line 1 Chill Insert Station Upper Elevator Conveyor
Cast Line 1 Chill Insert Station Lower Elevator Conveyor
Cast Line 1 Chill Insert Station Exit Conveyor W/Blow Off
Cast Line 1 Chill Insert Station Hydraulic Unit Pump
Cast Line 1 Chill Insert Station Hydraulic Unit Recirc Pump

Cast Line 1 Cover Insertion Infeed Conveyor
Cast Line 1 Cover Insertion Insertion Conveyor
Cast Line 1 Cover Insertion Exit Conveyor

Cast Line 1 Load Grantry X-Axis Travel
Cast Line 1 Load Grantry Y-Axis Travel
Cast Line 1 Unload Grantry X-Axis Travel
Cast Line 1 Unload Grantry Y-Axis Travel
Cast Line 1 Load/Unload Grantry Hydraulic Unit Pump
Cast Line 1 Load/Unload Grantry Hydraulic Unit Recirc Pump

Cast Line 1 Outfeed Conveyor #1
Cast Line 1 Outfeed Conveyor #2
Cast Line 1 Outfeed Conveyor #3

Cast Line 1 Chill Extraction Station Inlet Conveyor
Cast Line 1 Chill Extraction Station Upper Elevator Conveyor
Cast Line 1 Chill Extraction Station Lower Elevator Conveyor
Cast Line 1 Chill Extraction Station Hydraulic Unit Pump
Cast Line 1 Chill Extraction Station Hyd. Unit Recirc Pump

Cast Line 1 Double Deck Outfeed Upper Conveyor #1
Cast Line 1 Double Deck Outfeed Upper Conveyor #2
Cast Line 1 Double Deck Outfeed Lower Conveyor #1
Cast Line 1 Double Deck Outfeed Lower Conveyor #2

Cast Line 1 Stuck Chill Reject Station
Cast Line 1 Double Deck Exit Upper Conveyor #1
Cast Line 1 Double Deck Exit Upper Conveyor #2
Cast Line 1 Double Deck Exit Lower Conveyor #1
Cast Line 1 Double Deck Exit Lower Conveyor #2
Cast Line 1 Exit Elevator Hydraulic Unit Pump
Cast Line 1 Exit Elevator Hydraulic Unit Recirc Pump
Cast Line 1 Rollover Main Panel
Cast Line 1 Rollover Index Table
Cast Line 1 Rollover Hydraulic Motor #1
Cast Line 1 Rollover Hydraulic Motor #2
Cast Line 1 Rollover Recirc Pump
Cast Line 1 EM Pump Preheat Oven
Cast Line 1 EM Pump Cooling Fan Control Panel
Cast Line 1 EM Pump Cooling Motor #1
Cast Line 1 EM Pump Cooling Motor #2
Cast Line 1 EM Pump Main Control Panel

*/

//double CMonitor::GetUpdateFactor()
//{
//
//	std::string  state = GetState();
//	double maxWait=10000.0;
//	if(state == "starved")
//	{
//		maxWait=1.0;
//	}
//	else if(state == "blocked")
//	{
//		maxWait=1.0;
//	}
//	else if(state == "running")
//	{
//		if(MTTP>0 && currentJob!=NULL) 
//			maxWait=MIN(maxWait,_mttp);
//		if(MTBF>0) 
//			maxWait=MIN(maxWait,_mtbf);
//		if(currentJob!=NULL && _mttp<=0)
//			maxWait=MIN(maxWait,_mttp);
//		if(currentJob==NULL && inqueue.size() > 0)
//			maxWait=1.0;
//
//	}
//	else if(state == "faulted")
//	{
//		maxWait=MIN(maxWait,_mttr); 
//	}
//	else
//	{
//		maxWait=1.0;
//	}
//	return maxWait;
//}
