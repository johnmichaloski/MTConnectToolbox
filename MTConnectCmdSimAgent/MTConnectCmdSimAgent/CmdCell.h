//
// CmdCell.h
//
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.
// C:\Program Files\NIST\proj\MTConnect\Nist\MTConnectGadgets\CMSDCommandCell\CMSDCommandCell\CmdCell.h

#pragma once
#include <vector>
#include <string>
#include <map>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/timer.hpp>
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"

#include "NIST/config.h"
#include "NIST/ElTime.h"

#include "FactoryIntegrator.h"

#include "StateModel.h"
#include "Monitor.h"

using namespace StateModel;


// Only put class references with no header includes
class AgentMgr;
//class CCommand;
//class CCellHandler;
class CWorkOrder;
class CWorkOrders;
class CResourceHandler;
class CResourceHandlers;
class CMonitor ;
class FactoryIntegrator ;

//  process plan, process, cell, resoruce, vector<string>
typedef boost::tuple<int, ProcessPlan * , Process*, Cell * , CResourceHandler*, std::vector<std::string>, std::vector<int>, std::vector<double> > ResourcesProgramTuple;


class CWorkOrder
{
	// workorder/part/process plan/process (per machine)
public:

	CWorkOrder(CWorkOrders * workorders);

	CWorkOrder *				AddJob(int &workorder, std::string partid);
	int							IsDone() ;
	std::string					CurrentJobId(){ return _jobId ; }
	std::string					CurrentPartId(){ return _partid ;}
	int &						CurrentStep () { return _currentstep;}
	int							MaxStep() { ATLASSERT(this->processplan!=NULL ); return this->processplan->steps.size() ; }
	Part *						GetPart() { return  Factory.FindPartById( _partid.c_str()); }
	ProcessPlan *				GetProcessPlan() {  return  processplan; }
	Process *					GetProcessItem(int i) {ATLASSERT(i>=0 && i< processplan->processes.size());return processplan->processes[i]; }
	Cell *						GetCellItem(int i) {Process * p = GetProcessItem(i); ATLASSERT(p!=NULL); return Factory.FindCellById(p->cellsRequired[0]); }
	std::vector<Process *>		GetProcesses() { return GetProcessPlan()->processes ; }

	std::string					ToString();
	int							GetNumCells(){return  GetProcessPlan()->processes.size(); }
	std::map< std::string, std::vector<Cell *>	>	GetCells(); // process plan -> proces -> cell, 1st is process plan id 
	std::vector<CResourceHandler *>	GetResourceHandlers(Cell * cell);
	std::vector<CResourceHandler *>		GetAllResourceHandlers();
	std::vector<ResourcesProgramTuple> _ProgramMapping;
	std::string					Dump();
	std::vector<std::string>	GetPrograms(int currentstep, std::string resource);
	std::vector<int>			GetEstimatedTimes(int currentstep, std::string  resource);
	void						AssignCurrentResource(CResourceHandler * r) { _CurrentResourceHandler=r; }
	CResourceHandler *			CurrentResourceAssigned() { return _CurrentResourceHandler; }

	void						UpdateResourceWorkOrder(CResourceHandler * r) ;

	///////////////////////////////////////////////////
	// Back pointers
	ProcessPlan * processplan;


	//std::vector<NIST::CTimer> _TimePerStep;

	std::string _partid,_jobId;
	CWorkOrders * _parent;
	int currentJobNum;
	bool bActive;
	unsigned int _nLotSize;
	unsigned int _nLotNum;

	//int nStartGood;
	//int nStartBad;

	//std::vector <Process *> processes;
	//std::vector <CCellHandler *> currentcell;
	std::vector<bstr_t> steps ;
	std::string sequence, oplist,cells,operations;

	//std::string _currentprogram;  
	std::vector<std::string> _programs;  
	int _currentprog;
	std::vector<int> _estimatedTimes;  

	double _mttp;  // this workorder has spent # time at resource, counter in seconds
	Stats & GetStepStat() { return statcoll[_currentstep]; }
	NIST::CTimer orderTime, factoryTime;
	Stats stats;
	std::vector<Stats> statcoll;
	int _currentstep;

private:
	CResourceHandler * _CurrentResourceHandler;
	void LoadPrograms();
};


class CWorkOrders :	public std::vector<CWorkOrder * > 
{
public:
	typedef std::map<std::string, int > PartMap;
	//	CWorkOrders(AgentMgr * _agentconfig, CCMSDIntegrator * _factory);
	CWorkOrders();


	int						AllFinished() ;
	int						AllDoneQueuing();
	CWorkOrder *			AddJob(int &jobId, std::string partid);

	double &				Deadline() { return _dDeadline; }
	int						DoneParts(std::string partid) { return finishedparts[ partid]; }
	int						GetCurrentNuber() { return this->size();}
	std::string				GetPartIdFromAllJobsList(int i) { ATLASSERT(0<=i && i<random_part.size()); return random_part[i]; }
	void					InitAllJobs(Job *	masterjob) ;
	void					IncFinishedPart(CWorkOrder * wo); // std::string partid);
	void					IncStartedPart(CWorkOrder * wo, int multipler=1); // std::string partid);
	void					InitJobsStats(Job *	workorder) ;
	int						IsNewWorkorder() { return this->size() < MaxQueueSize  ; }
	std::string				JobsToString() ;
	int						MaxSize() { return MaxQueueSize;}
	void					Newworkorder();
	int						TotalPartsInProcess();
	int						PartCount();
	double &				TimeElapsed() { return _dUpdateRateSec; }
	std::vector<CResourceHandler * > & GetJobResources(std::string partid) { return _resourceByPartStats[partid]; }
	std::string				ToString();
	void					Update(); 

	CResourceHandler	*	FindBestUtilization(int n, CWorkOrder * workorder, Cell * cell, std::string partid);
	int						EstimateTimeOnQueue(Cell * cell, CResourceHandler * rh);
	std::string				Dump()
	{
		std::string	 tmp;
		tmp+="===================================================\n";
		for(int i=0;i<size(); i++)
		{
			tmp+=at(i)->Dump();
			tmp+="===================================================\n";
		}
		return tmp;
	}

		void						ReducePartQuantity(int index, int nLotSize, std::string);

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	std::string JobStatus; 
	std::string Jobs; 
	std::string DeviceStatus;

	// This are lists for various states of all the workorders.
	static int MaxQueueSize;  // Max size of active workorders
	std::vector< std::string > parts;  // list of all parts that can be made
	std::map<std::string, int > numactiveparts;  // current number for each workorder parts made, starts at zero
	std::map<std::string, int > finishedparts;  // current number of made # for each part
	std::map<std::string, int > totnumparts;  
	std::vector<CWorkOrder  * > archive;  // list of workorders that have completed

	std::vector<std::string> random_part;  // randomize list of  workorders to make parts
	std::vector<std::string> random_finished;  // randomize list of  workorders to make parts

	//std::vector<bstr_t> jobids ;
	//std::vector<std::string> partids ; // list of parts within workorder as they happen

	int _jobid;
	Stats stats;
	double _dUpdateRateSec;
	//CTimestamp orderTime;
	NIST::CTimer serviceTime;
private:
	boost::thread    m_Thread;  
	bool m_bRunning;
	double _dDeadline;
	//std::vector<CResourceHandler * > _resources;
	//std::vector<CCellHandler * > _cells;
	std::map<std::string, std::vector<CResourceHandler * > > _resourceByPartStats;
	std::map<std::string, Stats> partStats;
};



class CWorkOrder;
class FactoryIntegrator ;
class CMonitor;
#include "Globals.h"

class AgentConfigurationEx;
class CResourceHandler 
{
	std::string identifier;
public:
	CResourceHandler(Resource * r, 
		AgentConfigurationEx * mtcagent, // mtconnect agent
		std::string _agentip,
		std::string _cmddevice, 
		std::string device);
	~CResourceHandler(void){}
	CWorkOrder *				CurrentJob() { return workorder; }
	void						AddAgent(AgentConfigurationEx * mtcagent,std::string _agentip,std::string _cmddevice,std::string _statusdevice);

	static void					CreateFactoryResourceHandlers(AgentConfigurationEx * mtcagent); // mtconnect agent)
	static void					SendAllResourceHandlers(std::string event="reset");
	static CResourceHandler *	FindHandler(std::string id);
	static std::vector<CResourceHandler*> & ResourceHandlers() { return _rhandlers; }
	static CResourceHandler* & ResourceHandler(int i) { return _rhandlers[i]; }
	/////////////////////////////////////////////////////////////////////////////////////
	CWorkOrder	*						workorder;  // my current workorder
	CMonitor *							_statemachine;
	Resource *							_resource;
	std::string							_identifier;
	bool								_mRunning;
	std::string							_execution;
	int									_nCmdnum;
	std::vector<std::string>			_programs;
	std::vector<int>					_estDuration;
	std::map<std::string, Stats>		_partStats;
	static std::vector<CResourceHandler*> _rhandlers;

};

__declspec(selectany)  std::vector<CResourceHandler*> CResourceHandler::_rhandlers;

class CResourceHandlers : public std::vector<CResourceHandler * >
{
public:
	CResourceHandlers() {  }

	CResourceHandler *						Find(std::string id);
	static bool								AllResourcesInState(std::string state="blocked");
	static std::vector<CResourceHandler *>	GetResourceHandlers(Cell * c);
	static std::string						GenerateReport() ;
	static std::string						ToString();
	static void								UpdateResourceHandlers();
	static std::string						ToStateTable() ;
	//static void								SendAllResourceHandlers(std::string e);

	//	SendVarToHandlers<Queue<CWorkOrder>, &Queue<CWorkOrder>::nMaxSize> (2);

	template<typename T, int T::*Var2>
	static void SendVarToHandlers(int n)
	{
		for(int i=0 ; i<CResourceHandler::_rhandlers.size(); i++)
		{
			CResourceHandler * t ( (CResourceHandler *) CResourceHandler::_rhandlers.at(i));
			(t->_statemachine)->*Var2=n;

		}
	}
	//static void Send();

};

__declspec(selectany)  CResourceHandlers FactoryHandlers;


#if 0

class CCellHandler 
{
public:
	CCellHandler(Cell * cell)
	{
		_cell=cell;
		_CellHandlers.push_back(this);
	}
	~CCellHandler(void)	{}

	//bool AllResourcesInState(std::string state);
	static std::vector<CResourceHandler *>		GetResourceHandlers(Cell * c);
	//static	CResourceHandler * FindResourceHandler(std::string id);
	static	CCellHandler * FindCellHandler(std::string id)
	{
		for(int i=0; i< _CellHandlers.size(); i++)
			if(_CellHandlers[i]->_cell->identifier==bstr_t(id.c_str()))
				return _CellHandlers[i];
		return NULL;
	}
	static std::vector<CCellHandler * > CreateCellHandlers(ProcessPlan * pp) 
	{
		std::vector<CCellHandler * > _CellResourcesRequired;

		for(int i=0; i< pp->processes.size(); i++)
		{
			std::vector<bstr_t>	 cellids(pp->processes[i]->cellsRequired); 
			std::vector<Cell *> cells;
			for(int j=0; j< cellids.size(); j++)
			{
				ATLASSERT(Factory.FindCellById(cellids[j])!=NULL);
				Cell * cell = Factory.FindCellById(cellids[j]);
				CCellHandler * ch =	FindCellHandler((LPCSTR) cellids[j]); //  new CCellHandler(cell, &Factory);
				if(ch==NULL) ch =  new CCellHandler(cell);
				ch->_ResourceHandlers=GetResourceHandlers(cell);
				_CellResourcesRequired.push_back(ch);
				//// Now assign programs (but can be different for the resource because of part?*/
				//std::vector<int> programindexes = pp->processes[i]->GetPropertyMatches<0>(cell->resourceIds[i]);
				//for(int k=0; k< programindexes.size(); k++)
				//{
				//	CResourceHandler *rh= CResourceHandler::FindHandler((LPCSTR) cell->resourceIds[i]);
				//	ATLASSERT(rh!=NULL);
				//	std::string program = pp->processes[i]->GetProperty(programindexes[k], 1);
				//	rh->_programs.push_back(program);

				//	int duration = ConvertString(pp->processes[i]->GetProperty(programindexes[k], 3),20);
				//	rh->_estDuration.push_back(duration);
				//}
			}

		}
		return _CellResourcesRequired;
	}
	void CreateFactoryResourceHandlers(); 
	//////////////////////////////////////////////////////
	static std::vector<CCellHandler *>			_CellHandlers;
	std::vector<CResourceHandler *>				_ResourceHandlers;
	Cell *										_cell;

};

#endif