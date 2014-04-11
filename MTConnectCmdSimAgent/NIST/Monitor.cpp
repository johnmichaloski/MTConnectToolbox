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
	
	_statemachine.push_back(StateMachineTuple(finished, "done", ready,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Next, this))); 
	_statemachine.push_back(StateMachineTuple(ready, "done", ready,  boost::bind(&MyClass::NullCondition, this),	boost::bind(&MyClass::Next, this))); 

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
	_mtbf=MTBF;
	_mttr=MTTR;
	stats.nTotalParts=stats.nGoodParts=0;
	_nLastPartCountGood=_nLastPartCountBad=0;
	_nTMsTimeStart =  NIST::Millitime::GetMilliCount();
	_dPerformanceScale=1.0;
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
	ATLASSERT(0<=pWorkOrder->_currentprog && pWorkOrder->_currentprog < pWorkOrder->_estimatedTimes.size());
	SetMTCTagValue(cmddevice(),"command", "RUN");
	SetMTCTagValue(cmddevice(),"partid", pWorkOrder->_partid);
	std::string program = pWorkOrder->_programs[pWorkOrder->_currentprog];
	SetMTCTagValue(cmddevice(),"program", program);
	SetMTCTagValue(cmddevice(),"estimatedCompletion", ConvertToString<int>(pWorkOrder->_estimatedTimes[pWorkOrder->_currentprog]));
	SetMTCTagValue(cmddevice(),"lotsize", StdStringFormat("%d", pWorkOrder->_nLotSize).c_str() );
				SetMTCTagValue(cmddevice(),"lotnum", StdStringFormat("%d", pWorkOrder->_nLotNum).c_str() );

	SetMTCTagValue(cmddevice(),"cmdnum", StdStringFormat("%d", ++cmdnum));
}
void CMonitor::Ready()
{
	stats["idle"]+=_dUpdateRateSec; // assume seconds
	
}
void CMonitor::Off()
{
	stats["off"]+=_dUpdateRateSec; // assume seconds
}
void CMonitor::Running()
{ 
	//OutputDebugString(StdStringFormat("%s Running Queue Current = %X %x\n", name.c_str(), Current(), ConvertToString(_mttp).c_str()).c_str());
	if(Current()!=NULL && pWorkOrder->_currentprog>=0  && pWorkOrder->_currentprog < pWorkOrder->_programs.size() ) 
	{
		//stats.nProductionTime+= UpdateRateSec();
		Stats &partStats= _partStats[ Current()->_partid];
		partStats["busy"]+=_dUpdateRateSec; // assume seconds
		stats["busy"]+=_dUpdateRateSec; // assume seconds
		if(MakeLower(pWorkOrder->_programs[pWorkOrder->_currentprog]).find("setup")!=std::string::npos)
		{
			stats["setup"]+=_dUpdateRateSec; // assume seconds
		}
	}

	if(pWorkOrder!=NULL && pWorkOrder->_currentprog >= (int) pWorkOrder->_programs.size())
		SyncEvent("done");

}

void CMonitor::Done()
{
	// FIXME: count of parts and good/bad parts updated
	if(pWorkOrder!=NULL &&pWorkOrder->_programs.size()>=0 &&  findStringIC(pWorkOrder->_programs[pWorkOrder->_programs.size()-1].c_str(), ("inspect")))
	{
		//Stats::Update("TotalParts", 1.0, Current()->stats,partStats, stats);
		Stats &partStats= _partStats[ Current()->_partid];

		//part->goodparts=part->goodparts+nGood;  // add good bad reading from start of program, if none Zero
		//part->badparts=part->badparts+nBad;  // add good bad reading from start of program, if none Zero

		//Stats::Update("GoodParts", nPartCountGood-_nLastPartCountGood, Current()->stats,partStats, stats);
		//Stats::Update("ScrapParts", nPartCountBad-_nLastPartCountBad, Current()->stats,partStats, stats);
	}
}
void CMonitor::Faulted()
{ 
	if(pWorkOrder)
	{
		Stats &partStats= _partStats[pWorkOrder->_partid];
		partStats["down"]+=_dUpdateRateSec; // assume seconds
		partStats["repair"]+=_dUpdateRateSec; // assume seconds
	}

	stats["down"]+=_dUpdateRateSec; // assume seconds
	stats["repair"]+=_dUpdateRateSec; // assume seconds
}	
//std::map<std::string, double>	CMonitor::GetPartTiming(std::string partid)
double	CMonitor::GetPartTiming(std::string partid)
{
	//std::map<std::string, double> timings;
	double timings=0;
	std::string::iterator sit;
	std::map<std::string, Stats>::iterator it;
	for(it = _partStats.begin(); it!= _partStats.end(); it++)
	{ 
		if((*it).first.find(partid+".")!= std::string::npos)
		{

			if((*it).second.nTimes>0)
				timings += ( (*it).second["TotalTime"]/ (*it).second.nTimes);

		}
	}
	return timings;
}

void CMonitor::Preprocess()
{

	pWorkOrder=Current();

	_nMsTimerElapsed = ((double)NIST::Millitime::GetMilliSpan( _nTMsTimeStart ))/1000.0;
	_nTMsTimeStart =  NIST::Millitime::GetMilliCount();

	_dUpdateRateSec = UpdateRateSec();

	stats["TotalTime"]+= UpdateRateSec();				// update total time for resource
	//stats[StateStr(GetState())]+= UpdateRateSec();   	// update time in current state done in method?

	SetMTCTagValue(cmddevice(),"heartbeat", StdStringFormat("%d", _heartbeat++));
	SetMTCTagValue(cmddevice(),"utilization", StdStringFormat("%6.2f%%", 100.0 * Utilization()));

	// Read Resource Agentm status values
	std::vector<DataDictionary> cycledatum;
	TIME(cycledatum= _parser.ReadStream(agentip()+"/"+statusdevice()));
	if(cycledatum.size()<1)
	{
		if(GetState()!="Off")
		{
			SetMTCTagValue(cmddevice(),"power", "OFF");
			SyncEvent("off");

		}
		return;
	}
	data=cycledatum[0];
	 
	// Save Resource Agentm status values
	for(DataDictionary::iterator it = data.begin(); it!=data.end(); it++)
	{
		items.SetTag(statusdevice() + "." + (*it).first, (*it).second);
	}

	_nLastPartCountGood = ConvertString<int>(items.GetTag(statusdevice() + "." + "partcountgood", "0"),0);
	_nLastPartCountBad = ConvertString<int>(items.GetTag(statusdevice() + "." + "partcountbad", "0"),0);


	if(GetState() == "running" && items.GetTag(statusdevice() + "." + "execution","") == "READY")
	{
		ATLASSERT(Current()!=NULL);
	
		if(pWorkOrder->_currentprog > (int) pWorkOrder->_programs.size())
		{
				ATLASSERT(0);   // nothing should happen - but really idling
			
		}
		// Should only be in here once
		else if((pWorkOrder->_currentprog+1) == pWorkOrder->_programs.size())
		{

			pWorkOrder->_currentprog++;
			// If on-machine probe inspection of part, find out if good/bad part
			if(MakeLower(pWorkOrder->_programs[pWorkOrder->_currentprog-1]).find("inspect")!=std::string::npos)
			{
				Part * part = Factory.FindPartById( pWorkOrder->_partid.c_str());
				int nGood = 		_nLastPartCountGood-	_nPrePartCountGood;
				int nBad = 		_nLastPartCountBad-	_nPrePartCountBad;
				OutputDebugString(pWorkOrder->_programs[pWorkOrder->_currentprog-1].c_str()); OutputDebugString("\n");

				part->goodparts=part->goodparts+nGood;  // add good bad reading from start of program, if none Zero
				part->badparts=part->badparts+nBad;  // add good bad reading from start of program, if none Zero
			}
			// Check lot size on pallet system
			pWorkOrder->_nLotNum++;
			if(	pWorkOrder->_nLotNum < 	pWorkOrder->_nLotSize)
			{
				pWorkOrder->_currentprog=-1;
				SetMTCTagValue(cmddevice(),"lotnum", StdStringFormat("%d", pWorkOrder->_nLotNum).c_str() );
			}
			else
			{
				SyncEvent("done");
				SetMTCTagValue(cmddevice(),"command", "");
			}
		}
		// This case if for lot size > 1, to repeat program sequence
		else if(Current()!=NULL && pWorkOrder->_currentprog<0)
		{
			pWorkOrder->_currentprog=0;
			ATLASSERT(pWorkOrder->_programs.size()!=0);
			_partStats[pWorkOrder->_partid+ "." + pWorkOrder->_programs[0]].nTimes++; 
			Part * part = Factory.FindPartById(pWorkOrder->_partid.c_str());
			part->seenparts++;
			//_partStats[pWorkOrder->_partid+ "." + pWorkOrder->_programs[0]].dTotalTime=0.0;// assume zero
			if(	pWorkOrder->_nLotNum > 0 && pWorkOrder->_nLotNum < 	pWorkOrder->_nLotSize && MakeLower(pWorkOrder->_programs[0]).find("setup")!=std::string::npos)
			{
				pWorkOrder->_currentprog++;  // skip setup
			}

			ATLASSERT(pWorkOrder->_currentprog<pWorkOrder->_programs.size());

			OutputDebugString(pWorkOrder->_programs[pWorkOrder->_currentprog].c_str()); OutputDebugString("\n");
			
			Run(); // already in running state
		}
		else
		{
			pWorkOrder->_currentprog++;
			ATLASSERT(pWorkOrder->_programs.size()>pWorkOrder->_currentprog);
			//_partStats[pWorkOrder->_partid+ "." + pWorkOrder->_programs[pWorkOrder->_currentprog]].nTimes++; 
			OutputDebugString(pWorkOrder->_programs[pWorkOrder->_currentprog].c_str()); OutputDebugString("\n");

			if(MakeLower(pWorkOrder->_programs[pWorkOrder->_currentprog]).find("inspect")!=std::string::npos)
			{
				_lastProgram=pWorkOrder->_programs[pWorkOrder->_currentprog];
				OutputDebugString(pWorkOrder->_programs[pWorkOrder->_currentprog].c_str()); OutputDebugString("\n");
				// Save current number of good/bad parts before inspection
				Part * part = Factory.FindPartById( pWorkOrder->_partid.c_str());
				_nPrePartCountGood = 		_nLastPartCountGood;
				_nPrePartCountBad = 		_nLastPartCountBad;
			}

			// Send state transition for new program
			Run();  
		}

	}
	if(GetState() == "ready" && items.GetTag(statusdevice() + "." + "execution","") == "READY")
	{
		 if(Current()==NULL)
		{

		}
		else if(Current()!=NULL && pWorkOrder->_currentprog<0)
		{
			pWorkOrder=Current();
			pWorkOrder->_currentprog=0;
			ATLASSERT(pWorkOrder->_programs.size()!=0);
			_partStats[pWorkOrder->_partid+ "." + pWorkOrder->_programs[0]].nTimes++; 
			Part * part = Factory.FindPartById(pWorkOrder->_partid.c_str());
			part->seenparts++;
			//_partStats[pWorkOrder->_partid+ "." + pWorkOrder->_programs[0]].dTotalTime=0.0;// assume zero
			if(	pWorkOrder->_nLotNum > 0 && pWorkOrder->_nLotNum < 	pWorkOrder->_nLotSize && MakeLower(pWorkOrder->_programs[0]).find("setup")!=std::string::npos)
			{
				pWorkOrder->_currentprog++;  // skip setup
			}


			OutputDebugString(pWorkOrder->_programs[0].c_str()); OutputDebugString("\n");
			
			
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
			SyncEvent("next");
	}

	SetMTCTagValue(cmddevice(),"idleTime", NIST::Time.HrMinSecFormat( stats["idle"]) );
	SetMTCTagValue(cmddevice(),"faultTime", NIST::Time.HrMinSecFormat( stats["busy"]) );
	SetMTCTagValue(cmddevice(),"programTime",NIST::Time.HrMinSecFormat(stats["down"])  );

	// Updated below

	if( Current() !=NULL && pWorkOrder->_currentprog >=0  && pWorkOrder->_currentprog < pWorkOrder->_programs.size())
	{
	 	Stats &partStats= _partStats[ Current()->_partid+"."+pWorkOrder->_programs[pWorkOrder->_currentprog]];
		_partStats[pWorkOrder->_partid+ "." + pWorkOrder->_programs[pWorkOrder->_currentprog]]["TotalTime"]+=UpdateRateSec();

		//Stats::Update(StateStr(GetState()), _dUpdateRateSec, Current()->stats,Current()->GetStepStat(),partStats);
	   // Stats::Update("TotalTime", _dUpdateRateSec, Current()->stats,Current()->GetStepStat(),partStats);
	}
	if(Queue<CWorkOrder>::size() > 1)
	{
		// fix me those on queue - are idle, waiting
		 for(int i=1; i< Queue<CWorkOrder>::size(); i++)
			Stats::Update("queued", _dUpdateRateSec,  at(i)->stats);

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
	stats["blocked"]+= _dUpdateRateSec;
	if(Current() != NULL && (Current()->_mttp>0 ) )  // item to process...
		this->SyncEvent("run");
	if(Current() == NULL)
		this->SyncEvent("starved");
}
void CMonitor::Starved()
{
	//OutputDebugString(StdStringFormat("%s Starved Queue Current = %X %x\n", name.c_str(), Current(), ConvertToString(nBlockedTime).c_str()).c_str());
	stats["starved"]+= _dUpdateRateSec;

	if(Current() != NULL) 
		this->SyncEvent("run");
}
std::string CMonitor::GenerateReport()
{
	std::string tmp;
	tmp+= ToString();
	tmp+=StdStringFormat("\t Faulted Time =%8.2f\n", stats["down"]);
//	tmp+=StdStringFormat("\t Blocked Time =%8.2f\n", stats.nBlockedTime);
//	tmp+=StdStringFormat("\t Starved Time =%8.2f\n", stats.nStarvedTime);
	tmp+=StdStringFormat("\t Repair Time =%8.2f\n", stats["repair"] );
	tmp+=StdStringFormat("\t Idle Time =%8.2f\n", stats["idle"] );
	tmp+=StdStringFormat("\t Production Time =%8.2f\n", stats["busy"]);
	tmp+=StdStringFormat("\t Off Time =%8.2f\n", stats["off"] );
	return tmp;
}
Stats CMonitor::GetStats()
{
	return stats;
}
void CMonitor::GenerateStateReport(std::map<std::string,double> &states, double dDivisor )
{

	states["down"]=0.0; 
	states["blocked"]=0.0; 
	states["starved"]=0.0; 
	states["repair"]=0.0; 
	states["busy"]=0.0; 
	states["off"]=0.0; 
	states["idle"]=0.0; 

	if(dDivisor!=1.0)
		dDivisor= (stats["blocked"]+stats["starved"]+stats["down"]+stats["busy"]+stats["off"])/100.0;

	states["TotalTime"]= stats["TotalTime"];
	states["down"]+= stats["down"]/dDivisor;
	states["blocked"]+= stats["blocked"]/dDivisor;
	states["starved"]+= stats["starved"]/dDivisor;
	states["repair"]+= stats["repair"]/dDivisor;
	states["busy"]+= stats["busy"]/dDivisor;
	states["idle"]+= (stats["ready"]+stats["idle"])/dDivisor;
	states["off"]+= stats["off"]/dDivisor;
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
	tmp+=StdStringFormat("%8.2f,", stats["down"]/divider);
	tmp+=StdStringFormat("%8.2f,", stats["blocked"]/divider);
	tmp+=StdStringFormat("%8.2f,", stats["starved"]/divider);
//	tmp+=StdStringFormat("%8.2f,", nRepairTime/divider);
	tmp+=StdStringFormat("%8.2f,", stats["busy"]/divider);
	tmp+=StdStringFormat("%8.2f", stats["off"]/divider);
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