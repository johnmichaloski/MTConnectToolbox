//
// CmdCell.cpp
//
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.
#define NOMINMAX
#include "StdAfx.h"
#include "CmdCell.h"
#include <boost/foreach.hpp>
#include <algorithm>
#include <functional>
#include <numeric>

#include "NIST/Config.h"
#include "NIST/StdStringFcn.h"
//#include "NIST/CLogger.h"
#include "NIST/Logger.h"
#include "NIST/ElTime.h"

#include "KPI.h"
//#include "Reporting.h"

#include "Monitor.h"


using std::size_t;
using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::ios;

/*
workorders = new CWorkOrders( _cmsd);
Factory.CreateResourcesByPart(_cmsd);

workorders->InitAllJobs((Job *)  _cmsd->workorders->at(0).get());// there may be multiple workorders - but only 1st counts in our world
workorders->InitJobsStats((Job *)  _cmsd->workorders->at(0).get()) ;  // FIXME: verify works

std::string nMaxQueueSize = _cmsd->workorders->at(0)->GetPropertyValue("MaxQueueSize");
CWorkOrders::MaxQueueSize=ConvertString<int>(nMaxQueueSize,2);
workorders->Run(workorders);

*/

int CWorkOrders::MaxQueueSize=2;
//std::vector<CCellHandler*> CCellHandler::_CellHandlers;

template<typename T>
T DOCHECK(T x, std::string y) 
{
	if (x!=NULL) 
		return x ;

	throw std::exception(y.c_str());
	return NULL;
}


static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg =  StdStringFormat("COpcAdapter In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode);
	OutputDebugString(errmsg.c_str() );
	throw std::exception(errmsg.c_str() , pExp->ExceptionRecord->ExceptionCode);
} 
static std::string GetTimeEstimate(std::string etime)
{
	if(etime.find("TRIA") != std::string::npos)
	{
		std::vector<std::string> decimals = TrimmedTokenize(etime, ",");
		int sec = ConvertString<int>(decimals[decimals.size()-1],0) * 60;
		if( decimals.size() > 0) 
			return NIST::Time.HrMinSecFormat(sec);
	}
	return "00:00:00";

}
//////////////////////////////////////////////////////////////////////////////////////////////
CWorkOrder::CWorkOrder(CWorkOrders * workorders)
{
	_parent=workorders; 	
	orderTime.SimStart(workorders->serviceTime.SimElapsed());
	_currentprog=-1;
	_nLotNum=0;
}

int CWorkOrder::IsDone() 
{ 
	if(processplan ==NULL) 
	{
		OutputDebugString(StdStringFormat("CWorkOrder::NextStep Error no process plan \n").c_str());
		DebugBreak();
		return -1;

	}		
	int step = processplan->currentStep;
	return (step >= processplan->steps.size()); 
}

CWorkOrder *  CWorkOrder::AddJob(int &workorder, std::string partid)
{

	_partid=partid;
	_jobId = ConvertToString(workorder++);

	bActive=false;
	CurrentStep()=0;
	CurrentStep()=-1;  // signal not started

	Part* part = Factory.FindPartById( _partid.c_str());
	if(part==NULL)
	{
		OutputDebugString(StdStringFormat("CWorkOrder::AddPlan Error no part \n").c_str());
		DebugBreak();
		return this;
	}
	processplan = new ProcessPlan();
	// copy master process plan to this new process plan copy
	*processplan = *Factory.FindProcessPlanById(part->processplanidentifier);

	processplan->_pParentPart=part;
	processplan->currentStep =0;
	processplan->jobId=(LPCSTR)_jobId.c_str();;
	processplan->partId= (LPCSTR) _partid.c_str();


	//int step = plan->currentStep;

	sequence.clear(); oplist.clear();cells.clear(); operations.clear();
	for(int i=0;i< 	processplan->processes.size(); i++)
	{
		Process *process = processplan->processes[i];
		//processes.push_back(process);
		statcoll.push_back(Stats());
		steps.push_back(process->identifier);

		processplan->steps.push_back( process->identifier); 
		if(i>0) sequence+=","; sequence+=StdStringFormat("%d", i*10); 
		if(i>0 && processplan!=NULL) oplist+=","; oplist+=(LPCSTR)processplan->steps[i]; 
		if(i>0&& process!=NULL)
		{
			if(i>0) cells+=","; 
			cells+=process->cellsRequired[0];
			for(int j=1; j< process->cellsRequired.size(); j++)
			{
				cells+=","; cells+=process->cellsRequired[j];
			}
		}
		if(i>0&& process!=NULL) operations+=","; operations+=(LPCSTR) process->description;

	}

	
//	this->_nLotSize=1;
	this->_nLotSize=5;

	this->LoadPrograms();
	return this;
}
std::string	CWorkOrder::Dump()
{
	std::string tmp;
	for(int i=0; i< _ProgramMapping.size(); i++)
	{
		tmp+= get<1>(_ProgramMapping[i])->identifier + ", ";  // process plan
		tmp+= get<2>(_ProgramMapping[i])->identifier + ", "; // process
		tmp+= get<3>(_ProgramMapping[i])->identifier + ", "; // cell
		tmp+= get<4>(_ProgramMapping[i])->_resource->identifier + ", "; // resource
		std::vector<std::string> programs = get<5>(_ProgramMapping[i]);
		std::vector<int> times =  get<6>(_ProgramMapping[i]);
		std::vector<double> actualtimes =  get<7>(_ProgramMapping[i]);
		tmp+="(";
		for(int j=0; j< programs.size(); j++)
			tmp+=programs[j]+",";

		tmp+=")\n";
	}
	return tmp;
}

void CWorkOrder::LoadPrograms()
{
	if(processplan==NULL)
	{
		DebugBreak();
	}
	ProcessPlan * pp = processplan;
	std::vector<Process* > processes = pp->processes;
	//_CellResourcesRequired.clear(); //  should delete pointers.
	//_CellResourcesRequired=CCellHandler::CreateCellHandlers(pp);
	for(int i=0; i< pp->processes.size(); i++)
	{
		std::vector<bstr_t>	 cellids(pp->processes[i]->cellsRequired); 
		for(int j=0; j< cellids.size(); j++)  // assume only 1 cell at a time per process...
		{
			ATLASSERT(Factory.FindCellById(cellids[j])!=NULL);
			Cell * cell = Factory.FindCellById(cellids[j]);
			for(int k=0; k< cell->resourceIds.size(); k++)
			{
				//std::vector<int> programindexes = pp->processes[i]->GetPropertyMatches<0>(cell->resourceIds[k]);
				std::vector<IObject::PropIterator>  programindexes = pp->processes[i]->GetPropertyMatches<0>(cell->resourceIds[k]);
				CResourceHandler * rh= CResourceHandler::FindHandler((LPCSTR) cell->resourceIds[k]);
				std::vector<std::string> programs;
				std::vector<int> estDuration;
				for(int n=0; n< programindexes.size(); n++)
				{
					CResourceHandler *rh= CResourceHandler::FindHandler((LPCSTR) cell->resourceIds[k]);
					ATLASSERT(rh!=NULL);
					std::string program = pp->processes[i]->GetProperty(programindexes[n], 1);
					programs.push_back(program);

					int duration;
					std::string str = pp->processes[i]->GetProperty(programindexes[n], 3);
					if(MATH::Distribution::IsDistribution(str))
					{
						MATH::Distribution dist(str);
						duration=dist.RandomVariable();
					}
					else
					{
						duration = ConvertString(pp->processes[i]->GetProperty(programindexes[n], 3),20);
					}
					
					estDuration.push_back(duration);
				}
				std::vector<double> actualTimes;
				ResourcesProgramTuple rpt= ResourcesProgramTuple(i, pp, pp->processes[i],cell,rh, programs, estDuration,actualTimes);
				_ProgramMapping.push_back(rpt);
			}
		}
	}
}
std::vector<std::string>	CWorkOrder::GetPrograms(int currentstep,std::string resource)
{
	int n = currentstep; //CurrentStep();
	int i;
	for(i=0; i< _ProgramMapping.size(); i++)
	{
		if(n==get<0>(_ProgramMapping[i]) && get<4>(_ProgramMapping[i])->_resource->identifier == bstr_t(resource.c_str()) )
			break;
	}
	
	ATLASSERT(0<=CurrentStep() && CurrentStep() < _ProgramMapping.size());
	ATLASSERT(i!=_ProgramMapping.size());
	//ATLASSERT(get<3>(_ProgramMapping[CurrentStep ()])->_resource->identifier == (bstr_t(resource.c_str())));

	// what if multiple reosurces/per cell?
	return  get<5>(_ProgramMapping[i]);
	
}

std::vector<int> CWorkOrder::GetEstimatedTimes(int currentstep, std::string resource)
{
	int n = currentstep;
	if(n<0)
		n=0;
	int i;
	for(i=0; i< _ProgramMapping.size(); i++)
	{
		if(n==get<0>(_ProgramMapping[i]) && get<4>(_ProgramMapping[i])->_resource->identifier == bstr_t(resource.c_str()) )
			break;
	}
	return  get<6>(_ProgramMapping[i]);
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CWorkOrders::CWorkOrders() 
{
	_dDeadline=300000;
	_dUpdateRateSec=0;
	_jobid=0;
}

CWorkOrder *  CWorkOrders::AddJob(int &jobId, std::string partid)
{
	if( this->size() >= MaxQueueSize)
		return NULL; 

	CWorkOrder * workorder = new CWorkOrder(this);
	workorder->AddJob(jobId, partid);
	push_back(workorder);
	return workorder;
}

//
void CWorkOrders::IncFinishedPart(CWorkOrder * wo) // std::string partid)
{
	std::string partid = wo->CurrentPartId();
	finishedparts[partid]=finishedparts[partid] + wo->_nLotSize;
	random_finished.push_back(partid);
}
void CWorkOrders::IncStartedPart(CWorkOrder * wo, int multipler) // std::string partid)
{
	std::string partid = wo->CurrentPartId();
	numactiveparts[partid]= numactiveparts[partid] + (wo->_nLotSize*multipler);
}

int CWorkOrders::TotalPartsInProcess()
{
	int parts=0;
	for(std::map<std::string,int >::iterator it = finishedparts.begin(); it!=finishedparts.end(); it++)
		parts+=(*it).second;
	return parts;
}

int CWorkOrders::AllFinished() 
{			
	for(std::map<std::string,int >::iterator it=finishedparts.begin(); it!=finishedparts.end(); it++)
	{
		if((*it).second < totnumparts[(*it).first])
			return false;
	}
	return true;
}
// This is the root of the workorders tree
void CWorkOrders::InitAllJobs(Job *	workorder) 
{
	//jobids = workorder->partIds;
	for(int i=0 ; i< workorder->partIds.size(); i++)
	{
		parts.push_back( (LPCSTR) workorder->partIds[i]);
		numactiveparts[ (LPCSTR) workorder->partIds[i]]= 0;
		finishedparts[ (LPCSTR) workorder->partIds[i]]=0;
		totnumparts[ (LPCSTR) workorder->partIds[i]]=ConvertString<int>((LPCSTR)workorder->partQuantity[i],1);
	}
	std::map<std::string, int> partcounts;
	for(int i=0 ; i< workorder->partIds.size(); i++)
	{
		partcounts[(LPCSTR) workorder->partIds[i]]=ConvertString<int>((LPCSTR) workorder->partQuantity[i],99);
	}
	for(std::map<std::string, int>::iterator it=partcounts.begin(); it!= partcounts.end(); it++)
	{		
		for(int i=0; i< (*it).second; i++)
			random_part.push_back((*it).first);
	}

	// Now randomize part making
	std::random_shuffle ( random_part.begin(), random_part.end() );
	// Get Properties that may change from workorder to workorder - These are GLOBALS
	// Get KWH Cost
	std::string kwh= workorder->GetPropertyValue("KWH"); // we will assume cost but should add description tag
	ControlThread::globalCosts["KWH"] = ConvertString<double>(kwh,0.0);   // no cost if not found :(
	GLogger.Fatal(StdStringFormat("KWH=%f\n", ControlThread::globalCosts["KWH"] ));

}
std::string CWorkOrders::JobsToString() 
{
	std::string tmp;
	tmp="=========================================\n";
	tmp += StdStringFormat("%-16.16s",	"Parts");
	for(int i=0; i< parts.size(); i++)
	{
		tmp += StdStringFormat("%-16.16s",	parts[i].c_str());
	}
	tmp += StdStringFormat("\n%-16.16s",	"Numbers to Make");
	for(int i=0; i< parts.size(); i++)
	{
		tmp += StdStringFormat("%16d",	totnumparts[parts[i]]);
	}
	tmp += StdStringFormat("\n%-16.16s",	"Numbers Started");
	for(int i=0; i< parts.size(); i++)
	{
		tmp += StdStringFormat("%16d",	numactiveparts[parts[i]]);
	}
	tmp += StdStringFormat("\n%-16.16s",	"Numbers Finished");
	for(int i=0; i< parts.size(); i++)
	{
		tmp += StdStringFormat("%16d",	finishedparts[parts[i]]);
	}
	tmp+="\n";
	return tmp;

}
void CWorkOrders::InitJobsStats(Job *	workorder) 
{
	static std::string fcnname="CWorkOrders::InitJobsStats(Job *	workorder)  ";

	// For each part, dissect to each Ex resource used, time on each resource
	//std::vector<double> dOperationTimeForPart(workorder->partIds.size(),0.0); // operation time on this line to finish this workorder

	for(int k=0 ; k< Factory.workorders->size(); k++)
	{
		ATLASSERT (Factory.workorders->at(k)!=NULL);
		Job * workorder = (Job *) Factory.workorders->at(k);

		// For each part, dissect to each Ex resource used, time on each resource
		std::vector<double> dOperationTimeForPart(workorder->partIds.size(),0.0); // operation time on this line to finish this workorder

		for(int i=0 ; i< workorder->partIds.size(); i++)
		{
			// This sums up how long a part is to take given a set of resources (using the resource MTTP)
			std::string partid = workorder->partIds[i];
			std::vector<CResourceHandler * > ResourceHandlers = GetJobResources(partid);
			for(int k=0 ; k< ResourceHandlers.size(); k++)
			{
				dOperationTimeForPart[i]+= ResourceHandlers[k]->_statemachine->MTTP;
				// PTU Production time per unit (PEZ)
				Stats::Update("PTU" , dOperationTimeForPart[i] , ResourceHandlers[k]->_partStats[partid]);
			}
		}

		double dTotalOperationTime=0.0;
		for(int i=0 ; i< workorder->partIds.size(); i++)
		{
			std::string partid = workorder->partIds[i];
			dTotalOperationTime+=dOperationTimeForPart[i] * totnumparts[partid];  // total seconds for all parts

			// each resource will take  # parts to make in workorder * time/part
			std::vector<CResourceHandler * > ResourceHandlers = GetJobResources(partid);
			for(int k=0 ; k< ResourceHandlers.size(); k++)
			{
				ResourceHandlers[k]->_statemachine->_partStats[partid].Property("name")= (LPCSTR) ResourceHandlers[k]->_resource->name;
				ResourceHandlers[k]->_statemachine->_partStats[partid]["OT"]=ResourceHandlers[k]->_statemachine->MTTP * totnumparts[partid];
				ResourceHandlers[k]->_statemachine->_partStats[partid]["PSUT"]= 0.0;
				ResourceHandlers[k]->_statemachine->_partStats[partid]["PlannedStandstill"]= 0.0;
				ResourceHandlers[k]->_statemachine->_partStats[partid].Get("PBT")= ResourceHandlers[k]->_partStats[partid]["OT"];

				// Planned order time (POET) 
				ResourceHandlers[k]->_statemachine->stats["POET"]=ResourceHandlers[k]->_statemachine->stats["POET"] + ResourceHandlers[k]->_statemachine->MTTP * totnumparts[partid];
				// POQ Order quantity
				ResourceHandlers[k]->_statemachine->stats["OQ"]=ResourceHandlers[k]->_statemachine->stats["OQ"] +  totnumparts[partid];

			}
		}

		stats.Property("name")="Factory Planned"; 
		stats.vals["OT"]=dTotalOperationTime;		// Operation Time (OT)
		stats.vals["PSUT"]=0.0;						// Planned set-up time
		stats.vals["PlannedStandstill"]=0.0;
		stats.vals["PBT"]=dTotalOperationTime;		//Planned allocation time (PBT)

	}
}

int CWorkOrders::PartCount() 
{			

	int finishedParts=0,partialParts=0;						
	//BOOST_FOREACH (PartMap::value_type it, finishedparts)  
	//	finishedParts +=  it.second;
	finishedParts=random_finished.size();
	partialParts=this->size();  // 1 part, may have lot of 5
	//for(std::vector< std::string >::iterator it = parts.begin(); it!= parts.end(); it++)
	//	partialParts+=numactiveparts[*it];
	return  finishedParts +  partialParts;

}


int CWorkOrders::AllDoneQueuing() 
{
	for(std::vector< std::string >::iterator it = parts.begin(); it!= parts.end(); it++)
		if(finishedparts[*it]<totnumparts[*it]) return false;
	return true;

	//int n = PartCount();
	//int totalParts=0;		
	//BOOST_FOREACH (PartMap::value_type it, totnumparts)  
	//	totalParts += it.second;


	//if(n>=totalParts)
	//	return true;

	/*for(std::map<std::string,int >::iterator it=numactiveparts.begin(); it!=numactiveparts.end(); it++)
	{
		if((*it).second < totnumparts[(*it).first])
			return false;
	}*/
	return false;
}
// All ready added 1 part, now need to add lotsize-1
void CWorkOrders::ReducePartQuantity(int index, int nLotSize, std::string partid)
{
	int n=0;
	if(nLotSize==0)
		return;

	for(int i=index; i< random_part.size(); i++)
	{
		 if(random_part[i]==partid)
		 {
			random_part.erase(random_part.begin() + i);
			n++;
			i--;
		 }
		if(n>=nLotSize)
			break;
	}


}


void  CWorkOrders::Newworkorder()
{	
	static std::string fcnname="JobCommands::Newworkorder() ";
	while(IsNewWorkorder()) //this->size() < MaxQueueSize 
	{
		try {
			if(AllDoneQueuing()) // AllFinished())
				break;


			//if(this->size() >0  )
			//{
			//	CWorkOrder * workorder = at(this->size()-1);
			//	if(workorder == NULL || workorder->GetNumCells() <=0)
			//		break;

			//	//CCellHandler * cellHandler = workorder->_CellResourcesRequired[0];
			//	if(CResourceHandlers::AllResourcesInState("blocked"))
			//		break;
			//}
			int nextpart =  PartCount(); // ((int) (this->size()-1) >0) ? this->size()-1 : 0;
			if(nextpart < random_part.size())
			{
				std::string partid = GetPartIdFromAllJobsList(nextpart);
				CWorkOrder * workorder = DOCHECK(AddJob(_jobid ,partid), fcnname + "Add workorder failed");
				IncStartedPart(workorder);
				ReducePartQuantity(nextpart+1, workorder->_nLotSize-1, workorder->CurrentPartId());
			}
			else
			{
				break;
			}
			//OutputDebugString(workorder->Dump().c_str());
		}
		catch(std::exception err) 
		{ 
			OutputDebugString(err.what()); 
		}
	}
}
// shouldnt this need a part type to determine length of stay on queue
int CWorkOrders::EstimateTimeOnQueue(Cell * cell, CResourceHandler * rh)
{
	int nTimeEstimate=0;
	int n;
	for(int i=0; i< rh->_statemachine->Queue<CWorkOrder>::size(); i++)
	{
		CWorkOrder * workorder = rh->_statemachine->Queue<CWorkOrder>::at(i);
		n=workorder->CurrentStep();
		if(n<0)
			n=0;
		std::vector<int> estimatedTimes=workorder->GetEstimatedTimes(n, (LPCSTR) workorder->CurrentResourceAssigned()->_resource->identifier);
		nTimeEstimate+=std::accumulate(estimatedTimes.begin(),estimatedTimes.end(),0, std::plus<int>());
		CResourceHandler * rh2 = CResourceHandler::FindHandler((LPCSTR) workorder->CurrentResourceAssigned()->_resource->identifier); 
		if(rh2->_statemachine->GetStateName() == "faulted")
			nTimeEstimate+=40;                   // add time since faulted
	}
	return nTimeEstimate;
}
static bool myFunction(std::pair<CResourceHandler * , int> &p, std::pair<CResourceHandler * , int> &q)
{
	return p.second < q.second;
}
 /** 
	match best resource available.
 */
CResourceHandler  * CWorkOrders::FindBestUtilization(int n, CWorkOrder * workorder, Cell * cell, std::string partid)
{
	std::map<CResourceHandler * , int> _handlerstime;
	// First find resources that are free.
	for(int k=0; k< cell->resourceIds.size(); k++)
	{
		CResourceHandler * rh = CResourceHandler::FindHandler((LPCSTR) cell->resourceIds[k]);// workorder->_CellResourcesRequired[0]->_ResourceHandlers[i]; 
//		int n=workorder->CurrentStep();
		if(n<0)
			n=0;
		std::vector<int> estimatedTimes=workorder->GetEstimatedTimes(n, (LPCSTR) rh->_resource->identifier);
		int total=std::accumulate(estimatedTimes.begin(),estimatedTimes.end(),0, std::plus<int>());
		_handlerstime[rh]=total ;
		if(rh->_statemachine->GetStateName() == "faulted")
			_handlerstime[rh]+=40;                   // add time since faulted
		_handlerstime[rh]+= EstimateTimeOnQueue(cell, rh) ;
	}

	// Now sort by times and choose fastest throughput (always 0, no checking if multiple best)
	std::vector<std::pair<CResourceHandler * , int> > myVec(_handlerstime.begin(), _handlerstime.end());
	std::sort(myVec.begin(),myVec.end(),&myFunction);
	if(myVec.size()>0)
		return myVec[0].first;
	return NULL;
}

void CWorkOrder::UpdateResourceWorkOrder(CResourceHandler * rh) 
{
	int n=this->CurrentStep();
	if(n<0)
		n=0;

	this->factoryTime.Start();
	this->AssignCurrentResource(rh);
	this->bActive=true;
	this->_programs=this->GetPrograms(n, (LPCSTR) rh->_resource->identifier);  // resource name
	this->_estimatedTimes=this->GetEstimatedTimes(n, (LPCSTR) rh->_resource->identifier);
	this->_currentprog=-1; // redundant - done in constructor - but making explicitly clear
//	this->_nLotSize=nLotSize;
	rh->_statemachine->Push(this);  // start workorder

}

/**
* THe workorders commands list updates its list by checking each items on the list. 
* If a workorder is done (current step >= maximum step) then it is removed from the workorder list.
* FIXME: should check if workorder is active, if not, then skip rest of workorders, otherwise, as list gets shorter
* the update method will get faster.
* Haver to check all resource within workorder, as some may not be active and can have another workorder step assigned to it.
*/
void CWorkOrders::Update() // CResourceHandlers * resourceHandlers)
{

	for(int i=0; i < size() ; i++)
	{
		CWorkOrder * workorder = at(i);

		// only check this - if done
		// check next - if done and free - else nothing...
		if(workorder->CurrentStep()>= workorder->MaxStep() )
		{
			workorder->orderTime.Stop();
			workorder->factoryTime.Stop();
			IncFinishedPart(workorder); // ->CurrentPartId());
#if 0
			OutputDebugString(JobsToString().c_str());
			OutputDebugString(CResourceHandlers::GenerateReport().c_str());
			std::cout<< JobsToString()<< std::endl;
			std::cout<< CResourceHandlers::GenerateReport()<< std::endl;
#endif
			// Done with workorder for now erase
			// Eliminate this part
			IncStartedPart(workorder,-1);
			//numactiveparts[workorder->CurrentPartId()]= numactiveparts[workorder->CurrentPartId()]-1;
			this->archive.push_back(workorder);
			this->erase(begin() + i);
			i--; 
			continue;
		}

		// Check to see if workorder has not started and a  1st resource is avaible.
		// Might be easier to check if can't push all resource then break from loop.
		if(workorder->CurrentStep() < 0 ) 
		{
			// starting at pp process 0!
			std::vector<bstr_t>	 cellids(workorder->GetProcessPlan()->processes[0]->cellsRequired); 

			ATLASSERT(Factory.FindCellById(cellids[0])!=NULL); // must be at least one
			Cell * cell = Factory.FindCellById(cellids[0]);
			CResourceHandler  * rh = CWorkOrders::FindBestUtilization(0, workorder, cell, workorder->CurrentPartId());
			workorder->CurrentStep()=0;
			workorder->UpdateResourceWorkOrder(rh);
			continue; // maybe reset to zero - no more workorders?
		}

		// Job started
		//CCellHandler * cellHandler = workorde
		//->_CellResourcesRequired[workorder->CurrentStep()];
		if(workorder->CurrentStep() >= 0 ) 
		{
			CResourceHandler *  resource = workorder-> CurrentResourceAssigned(); // cellHandler->_ResourceHandlers[workorder->CurrentStep()] ; 
			if(resource == NULL) 
				continue;

			// Done with processing at current step - ARE WE SKIPPING STEPS?
			if(resource->_statemachine->Current() != NULL && workorder->_currentprog>= (int) workorder->_programs.size()) // (at(i)-> _mttp <= 0.0) )
			{
				// Done no more steps
				if((workorder->CurrentStep()+1)>= workorder->MaxStep())
				{
					resource->_statemachine->Pop();  // finish/pop current step
					// which is at front of queue
					workorder->CurrentStep()++; // increment current step
					i=i-1; // redo this to remove !?
					continue; // skip all processing
				}
				// Move to next step
				std::vector<bstr_t>	 cellids( workorder->GetProcessPlan()->processes[workorder->CurrentStep()+1]->cellsRequired); //  workorder->_CellResourcesRequired[workorder->CurrentStep()+1];
				Cell * cell1 = Factory.FindCellById(cellids[0]);

				CResourceHandler  * resource1 = CWorkOrders::FindBestUtilization(workorder->CurrentStep()+1, workorder, cell1, workorder->CurrentPartId());
				ATLASSERT(resource1!=NULL);

				resource->_statemachine->Pop();    // finished current step in resource	
				workorder->CurrentStep()++;
				workorder->UpdateResourceWorkOrder(resource1);
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** 
* Each resource in the factory definition is assigned a resource handler, which , adds a state machine, 
* a factory pointer  and a  MTConnect agent cmd/status connections. This is creation of resource handlers is
* done manually before the start of workorder.
*/
CResourceHandler * CResourceHandler::FindHandler(std::string id)
{
	for(int i=0 ; i<_rhandlers.size(); i++)
		if( std::string((LPCSTR) _rhandlers.at(i)->_resource->identifier) == id) 
			return _rhandlers.at(i); 
	return NULL; 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string CResourceHandlers::GenerateReport() 
{
	std::string tmp;
	for(int i=0 ; i<CResourceHandler::_rhandlers.size(); i++)
		tmp+= CResourceHandler::_rhandlers.at(i)->_statemachine->GenerateReport();
	return tmp;
}
std::string CResourceHandlers::ToStateTable() 
{
	std::string tmp;
	for(int i=0 ; i<CResourceHandler::_rhandlers.size(); i++)
		tmp+= CResourceHandler::_rhandlers.at(i)->_statemachine->ControlThread::ToString();
	return tmp;
}


//void CResourceHandlers::Send()
//{
//	SendVarToHandlers<Queue<CWorkOrder>, &Queue<CWorkOrder>::nMaxSize> (2);
//}

	void CResourceHandlers::UpdateResourceHandlers()
{	
	for(int i=0 ; i<CResourceHandler::_rhandlers.size(); i++)
	{
		CResourceHandler *  _resourceHandler = CResourceHandler::_rhandlers.at(i);
		if( _resourceHandler == NULL)
			continue;
		TIME(_resourceHandler->_statemachine->ControlThread::Cycle());
	}
}

CResourceHandler * CResourceHandlers::Find(std::string id)
{
	return CResourceHandler::FindHandler(id);
}
void CResourceHandler::SendAllResourceHandlers(std::string event)
{
	std::vector<IObjectPtr> *resources= Factory.resources;
	for(int i=0; i< resources->size(); i++)
	{
		CResourceHandler *  rh = CResourceHandler::FindHandler((LPCSTR)((Resource *) resources->at(i))->identifier);
		if(rh==NULL)
		{	
			DebugBreak();
			continue;
		}
		rh->_statemachine->SyncEvent(event);
	}
}
#include "MTConnectCmdSimAdapter.h"
void  CResourceHandler::CreateFactoryResourceHandlers(AgentConfigurationEx * mtcagent) // mtconnect agent)
{
	std::vector<IObjectPtr> *resources= Factory.resources;
	for(int i=0; i< resources->size(); i++)
	{
		std::string id = ((Resource *) resources->at(i))->identifier;
		CResourceHandler *  rh = CResourceHandler::FindHandler( id);
		if(rh==NULL)
		{	

			rh = new CResourceHandler(((Resource *) resources->at(i)), mtcagent,"127.0.0.1:5000", Globals.devices[id]+"_CMD",Globals.devices[id]);
		}
	}
}
void CResourceHandler::AddAgent(AgentConfigurationEx * mtcagent,
	std::string _agentip,
	std::string _cmddevice, 
	std::string _statusdevice)
{
	_statemachine->_agentconfig=mtcagent;
	_statemachine->statusdevice()=_statusdevice;
	_statemachine->cmddevice()=_cmddevice;
	_statemachine->agentip()=_agentip;
}
CResourceHandler::CResourceHandler(Resource * r, 
	AgentConfigurationEx * mtcagent, // mtconnect agent
	std::string _agentip,
	std::string _cmddevice, 
	std::string device)
{
	_rhandlers.push_back(this);
	workorder=NULL;
	_resource=r;
	_statemachine = new CMonitor((LPCSTR) r->identifier, 
		mtcagent,
		_agentip, // "127.0.0.1:5000"
		device,
		_cmddevice); // device+"_CMD");  
	identifier = (LPCSTR) r->identifier;
}
/** 
This method checks to see that all resource state machines are in the state "blocked".
*/
bool CResourceHandlers::AllResourcesInState(std::string state)
{
	bool bFlag=true;
	for(int i=0; i< CResourceHandler::_rhandlers.size(); i++)
	{
		bFlag= bFlag & ((CResourceHandler::_rhandlers).at(i)->_statemachine->GetState() == state);
	}
	return bFlag;
}
////////////////////////////////////////////////////////////
std::string CWorkOrder::ToString()
{
	std::stringstream str;
	str<<	"Resource          " << ((CurrentResourceAssigned()==NULL)?"": (LPCSTR) CurrentResourceAssigned()->_resource->identifier ) << std::endl;
	str<<	"Current Job Step  " << CurrentStep() << std::endl;
	str<<	"Current JobId     " << processplan->jobId << std::endl;
	str<<	"Active            " << bActive << std::endl;
	str<<	"PartID            " << _partid  << std::endl;
	str<<	"Program           " << ((_currentprog<0 || _currentprog>=_programs.size())? "" : _programs[_currentprog]) << std::endl;
	str<<	"Program#          " << _currentprog << std::endl;

	////	for(int i=0; i < currentcell.size(); i++);
	//for(int i=0; i < processplan->processes.size(); i++)
	//{

	//	//for(int i=0;i< 	plan->steps.size(); i++)
	//	//{
	//	//	Process *process = plan->processes[i];
	//	str<<	"     ==================================" << std::endl;
	//	str<<	"     ProcessPlan      " << i <<  "  " <<  processplan->identifier  << std::endl;
	//	str<<	"     Process         " << i <<  "  " <<  processplan->processes[i]->identifier  << std::endl;
	//	str<<	"     JobId           " << i <<  "  " <<  _jobId  << std::endl;
	//	str<<	"     PartID          " << i <<  "  " <<  _partid  << std::endl;
	//	str<<	"     Cell            " << i <<  "  " <<  processplan->processes[i]->cellsRequired[0]  << std::endl;
	//	Cell * cell= _factory->FindCellById((LPCSTR)  processplan->processes[i]->cellsRequired[0]);
	//	//str<<	"     Resource        " << i <<  "  " <<  currentcell[i]  << std::endl;
	//	if(cell!= NULL)
	//		str<<	"     Resource        " << i <<  "  " <<  cell->resourceIds[0] << std::endl;
	//}
	return str.str();
}
std::string CWorkOrders::ToString()
{
	std::string tmp;
	for(int i=0; i< size(); i++)
	{
		if(!at(i)->bActive)
			break;
		tmp+=at(i)->ToString();
	}
	return tmp;
}
std::string CResourceHandlers::ToString()
{
	std::string tmp;
	tmp+="============================================================\n";
	for(int i=0; i< CResourceHandler::_rhandlers.size(); i++)
	{
		tmp+=((CResourceHandler::_rhandlers).at(i))->_statemachine->ToString();
	}
	return tmp;
}
std::map< std::string, std::vector<Cell *>> CWorkOrder::GetCells()
{ 
	std::map< std::string, std::vector<Cell *>	> cellmap;
	ProcessPlan * pp = GetProcessPlan(); 
	ATLASSERT(pp!=NULL); 
	for(int i=0; i< pp->processes.size(); i++) 
	{
		std::vector<bstr_t>	 cellids(pp->processes[i]->cellsRequired); 
		std::vector<Cell *> cells;
		for(int j=0; j< cellids.size(); j++)
		{
			ATLASSERT(Factory.FindCellById(cellids[j])!=NULL);
			cells.push_back(Factory.FindCellById(cellids[j]));
		}
		cellmap[(LPCSTR) pp->processes[i]->identifier]= cells;
	}
	return cellmap; 
}
std::vector<CResourceHandler *>	CResourceHandlers::GetResourceHandlers(Cell * c)
{
	std::vector<CResourceHandler *> rhs;
	std::vector<bstr_t> rids = c->resourceIds;
	for(int i=0; i< rids.size(); i++)
	{
		ATLASSERT(CResourceHandlers().Find((LPCSTR) rids[i])!=NULL);
		rhs.push_back(CResourceHandlers().Find((LPCSTR) rids[i]));
	}
	return rhs;
}
std::vector<CResourceHandler *>	CWorkOrder::GetResourceHandlers(Cell * cell){ return CResourceHandlers::GetResourceHandlers(cell); }
std::vector<CResourceHandler *>	CWorkOrder::GetAllResourceHandlers()
{
	std::vector<CResourceHandler *>	 rhs;
	for(int i=0; i< GetProcessPlan()->processes.size(); i++)
	{
		std::vector<bstr_t>	 cellids(GetProcessPlan()->processes[i]->cellsRequired); 
		for(int j=0; j< cellids.size(); j++)  // assume only 1 cell at a time per process...
		{
			ATLASSERT(Factory.FindCellById(cellids[j])!=NULL);
			Cell * cell = Factory.FindCellById(cellids[j]);
			std::vector<CResourceHandler *> rs = CResourceHandlers::GetResourceHandlers(cell);
			rhs.insert(rhs.end(), rs.begin(), rs.end());
		}

	}
	return rhs;

}
// std::vector<CResourceHandler *>	CWorkOrder::GetResourceHandlers(Cell * cell)
//{
//	std::map< std::string, std::vector<CResourceHandler *>	> resourcemap;
//	std::vector<CResourceHandler *>	 rhs;
//	for(int k=0; k< cell->resourceIds.size(); k++)
//	{
//		CResourceHandler * rh= CResourceHandler::FindHandler((LPCSTR) cell->resourceIds[k]);
//		rhs.push_back(rhs);
//	}
//	resourcemap[cell->identifier]= rhs; // .insert(rhs.begin(), rhs.end());// FIXME this should be append
//
//	return resourcemap;
//}



#if 0
/** 
* For each resource in the factory definition, this method creates augments the resource definition
* with a resoruce handler that adds a state machine, a factory pointer BUT omits the  MTConnect
* agent cmd/status connections.
*/
void CCellHandler::CreateFactoryResourceHandlers()
{
	std::vector<IObjectPtr> *resources= Factory.resources;
	for(int i=0; i< resources->size(); i++)
	{
		CResourceHandler *  rh = CResourceHandler::FindHandler((LPCSTR)((Resource *) resources->at(i))->identifier);
		if(rh==NULL)
		{	
			DebugBreak();
			//rh = new CResourceHandler(((Resource *) resources->at(i)), _factory);
		}
	}
#if 0
	std::vector<IObjectPtr> *cells= Factory.cells;
	for(int i=0; i< cells->size(); i++)
	{
		Cell * cell = (Cell *) cells->at(i);
		for(int k=0; k< cell->resourceIds.size(); k++)
		{
			CResourceHandler *  rh = CResourceHandler::FindHandler((LPCSTR)cell->resourceIds[k]);
			if(rh==NULL)
			{	
				Resource * r = Factory.FindResourceById((LPCSTR) cell->resourceIds[k]);
				rh = new CResourceHandler(r);
			}
		}
	}
#endif
}
#endif