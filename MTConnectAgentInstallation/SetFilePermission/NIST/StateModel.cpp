//
// C:\Program Files\NIST\proj\MTConnect\Nist\MTConnectGadgets\CMSDCommandCell\CMSDCommandCell\StateModel.h
//


#include "Stdafx.h"
#include "StateModel.h"
#include <sys/timeb.h>

// To do: 
// RCS StateModel - use enum strings, add mutex cmd/status queue or shared mem use CONNECTOR to write
// Sim StateModel - off, idle, starved, blocked, running, faulted
// boost thread synchronization: http://www.boost.org/doc/libs/1_45_0/doc/html/thread/synchronization.html
// ref: http://en.highscore.de/cpp/boost/functionobjects.html 
namespace StateModel
{

	bool AsyncMethod::IsRunning()
	{
		if(_thread==NULL)
			return false;

		if(_thread->timed_join(boost::posix_time::microseconds(10)))
		{
			delete _thread;
			_thread=NULL;
			return false;
		}

		return true;
	} 

	void AsyncMethod::Join() 
	{ 
		if(_thread != NULL) 
		{ 
			_thread->join(); 
		} 
	} 

	ControlThread::ControlThread() : _thread(NULL)
	{
		_cycle=0;
		_laststate=_state=off;
		_nDebugLevel=0;
		_updaterate=1000; // 1 second for now, eventually speed up
		_dSpeedup=1.0;
		_dLoopTime=1.0; // loop time 1 second 
		_dTotalTime=0.0;
		_nTMsTimeStart = GetMilliCount();
	}
	ControlThread::~ControlThread() 
	{ 
		Join(); 
		delete _thread; 
	} 

	// Error handling
	std::string ControlThread::GetErrorMessage() { return _errmsg; }
	void ControlThread::SetErrorMessage(std::string err) {  _errmsg=err; }
	unsigned int & ControlThread::UpdateRate() { return _updaterate; }
	double ControlThread::UpdateRateSec() { return _updaterate / 1000.0 * _dSpeedup; }
	void ControlThread::SetUpdateRate(unsigned int milliseconds) {if(milliseconds>0) UpdateRate()=milliseconds; }


	void ControlThread::AddCostFunction(CostFcnTuple s) 
	{
		_costfcns.push_back(s);
	}

	void ControlThread::Delay(double amt)
	{
		_dTotalTime += _dLoopTime;  // again in secondes (double)

		if(amt<0)
			return; // no delay...
		// Sleep a second
		boost::xtime xt;
		boost::xtime_get(&xt, boost::TIME_UTC);
		xt.sec += amt; 
		boost::thread::sleep(xt);
	}
	// Event handling
	void ControlThread::Trigger(StateType e)
	{ 
		_eventQueue.push_back(e); 
	}
	// Event handling
	void ControlThread::SyncEvent(StateType e)
	{ 
		_eventQueue.push_back(e); 
		//Process(e); // make events synchronously happen  - PROBLEM - endless recursion
	}
	// Conditions
	bool ControlThread::NullCondition() 
	{
		return true; 
	}
	void ControlThread::SetStateMachineName(std::string name) { _statemachinename=name; }
	void ControlThread::SetStateUpdate(StateType state1, StateModel::ControlThreadFnc fcn)
	{
		_stateMap[state1]=fcn;
	}
	// ISSUE here is that state transition has to exist first before eadding
	void ControlThread::SetStateTransition(StateType s1,  EventType e, StateType s2, StateModel::ControlThreadFnc fcn)
	{
		for(int i=0; i< _statemachine.size(); i++)
		{
			if(s1==get<0>(_statemachine[i]) && e==get<1>(_statemachine[i])&& s2==get<2>(_statemachine[i]) )
			{
				get<4>(_statemachine[i])=fcn;
			}
		}
	}
	void ControlThread::SetCondition(StateType s1, EventType e, StateModel::ControlThreadCondFnc fcn)
	{
		for(int i=0; i< _statemachine.size(); i++)
		{
			if(s1==get<0>(_statemachine[i]) && e==get<1>(_statemachine[i]))
			{
				get<3>(_statemachine[i])=fcn;
			}
		}
	}
	void ControlThread::Nop()	{ LogMessage("Nop\n");	}
	void ControlThread::Break()	{ DebugBreak();	}
	void ControlThread::Postprocess(){}
	void ControlThread::Preprocess(){}
	void ControlThread::Breakpoint()	
	{ 
		LogMessage("Breakpoint\n");	
	}
	int ControlThread::IsActive() { return !_nloop; }
	void ControlThread::Loop()
	{
		boost::timer t; // start timing 
		while(IsActive())
		{
			t.restart();
			this->Cycle();
			int wait_time = UpdateRate() - (1000* t.elapsed()); 
			::Sleep(UpdateRate());
			//if(wait_time > 0)
			//	::Sleep(wait_time);
			//else
			//	::Sleep(1);

		}
	}
	bool ControlThread::FindStateTransitionMatch(StateType _state, EventType _e, std::vector<StateMachineTuple> statemachine, StateMachineTuple &statematch)
	{
		for(int i=0; i< statemachine.size(); i++)
		{
			if((_state==get<0>(statemachine[i]) && _e==get<1>(statemachine[i]))
				|| (any==get<0>(statemachine[i]) && _e==get<1>(statemachine[i]))
				)
			{
				statematch=statemachine[i];
				return true;
			}
		}
		return false;
	}
	std::string ControlThread::StateMachineTraceDump()
	{
		std::string tmp;
		for(int i= statetrace.size()-1; i>=0; i--)
			tmp+=StdStringFormat("%s,", statetrace[i].c_str());
		return tmp;
	}

	void ControlThread::TraceSave(std::string e, double dSeconds)
	{
		if(statetrace.size() ==0 ||  e !=statetrace.back())
		{
			statetrace.push_back(e);
			timetrace.push_back(dSeconds);
		}
		else
			timetrace.back()+=dSeconds;

		if(statetrace.size() > 100)			
			statetrace.erase(statetrace.begin());
	};

	void ControlThread::Cycle()
	{
		_cycle++;

		// Save current trace for reporting
		for(std::list<EventType>::iterator it=_eventQueue.begin(); it!= _eventQueue.end(); it++)
			TraceSave(*it);
		TraceSave(GetState());

		_laststate=GetState();
		_errmsg.clear();
		Preprocess();
		//if(_eventQueue.size() > 0)
		while(_eventQueue.size() > 0)
		{
			EventType e = _eventQueue.front();
			_eventQueue.pop_front();
			StateType state = GetState();
			bool bFail=true;
			StateMachineTuple statematch;
			if(FindStateTransitionMatch(state, e, _statemachine, statematch))
			{
				bFail=false;
				// Test guard condition
				if(get<3>(statematch)())
				{
					ControlThreadFnc fcn = get<4>(statematch);
					fcn();
					SetState(get<2>(statematch));
				}
				ListenerIterator it;
				if((it=_eventlisteners.find(e))!=_eventlisteners.end())
				{
					std::vector<ControlThreadCallbackFnc > listeners=(*it).second;
					for(int i=0; i< listeners.size(); i++) 
						listeners[i](this);

				}
			}
			if(bFail)
			{
				std::stringstream buffer;
				buffer << _statemachinename << " ERROR: Cannot process Event " << EventStr(e) << " in state " << StateStr(state) <<std::endl;
				LogMessage(buffer.str(), -1);
				if(_nDebugLevel>10) 
					DebugBreak();
			}

		}

		StateType state = GetState();
		//std::string msg = StdStringFormat("%s State update %s \n", this->_statemachinename.c_str(), StateStr(state));
		//StateSave(state, UpdateRateSec()) ; 

		// Execute current state update method - if no update method, do nothing...
		if(_stateMap.find(state)!=_stateMap.end())
		{
			boost::timer t; // start timing 
			_stateMap[state]();
			_stateMapTiming[state]= MAX(_stateMapTiming[state], t.elapsed());
		}
		Postprocess();

		_nMsTimerElapsed = ((double)GetMilliSpan( _nTMsTimeStart ))/1000.0;
		stateTimes[GetState()]+=_nMsTimerElapsed;
		_nTMsTimeStart =  GetMilliCount();

	}

	void ControlThread::Process(EventType e)
	{
		StateType state = GetState();
		bool bFail=true;
		StateMachineTuple statematch;
		if(FindStateTransitionMatch(state, e, _statemachine, statematch))
		{
			// Test guard condition
			if(get<3>(statematch)())
			{
				SetState(get<2>(statematch));
			}
		}
		//StateSave(state, UpdateRateSec()) ; 
		// Execute current state update method - if no update method, do nothing...
		if(_stateMap.find(state)!=_stateMap.end())
		{
			boost::timer t; // start timing 
			_stateMap[state]();
			_stateMapTiming[state]= MAX(_stateMapTiming[state], t.elapsed());
		}
	}

	void ControlThread::Join() 
	{ 
		if(_thread != NULL) 
			_thread->join(); 
	} 
	// boost::bind(&BThread::do_work, boost::ref(*this)
	void ControlThread::StartThread() 
	{ 
		_nloop=0;
		//  if(_thread ==NULL)
		if(_thread!=NULL)
			DebugBreak();
		_thread = new boost::thread(boost::bind(&ControlThread::Loop, this)); 
	} 
	void ControlThread::StopThread()
	{
		InterlockedIncrement( &_nloop ); 
		Join();
		SetState(off);
		if(_thread!=NULL)
			_thread = NULL;
	}
	void ControlThread::AddStateTransition(StateMachineTuple s) 
	{
		_statemachine.push_back(s);
	}


	void ControlThread::AddEventListener(EventType e, ControlThreadCallbackFnc fcn)
	{
		_eventlisteners[e].push_back(fcn);
	}
	void ControlThread::RemoveEventListener(EventType e, ControlThreadCallbackFnc fcn)
	{
		//_eventlisteners::iterator it;
		//if((it=_eventlisteners.find(e)) !=  _eventlisteners.end())
		//	for(int i=0; i< (*it).second.size(); i++)
		//	{
		//		if((*it).second[i] == fcn)
		//			(*it).second.erase(i);
		//	}
	}		
	std::string& ControlThread::Name() { return this->_statemachinename; }

	static bool statecompare (const StateMachineTuple &lhs, const StateMachineTuple &rhs){
		bool b= stricmp(get<0>(lhs).c_str(), get<0>(rhs).c_str())<0;
		return b;
	}

	std::string ControlThread::ToString()
	{ 
		std::vector<StateMachineTuple> _sortstatemachine=_statemachine;
		sort(_sortstatemachine.begin(),_sortstatemachine.end(),statecompare);
		std::string tmp;
		tmp+"==============================================================================\n";
		tmp+StdStringFormat("%16.16s, %16.16s, %16.16s\n", "state", "event", "newstate");
		//for(int i=0; i< _sortstatemachine.size(); i++)
		for(std::vector<StateMachineTuple>::iterator it = _sortstatemachine.begin(); it != _sortstatemachine.end(); it++)
		{
			// cant easily name condition or action or update action
			tmp+=StdStringFormat("%16.16s, %16.16s, %16.16s\n", 
				StateStr(get<0>(*it)).c_str(), 
				EventStr(get<1>(*it)).c_str(),
				StateStr(get<2>(*it)).c_str());
		}
		//tmp+="===========================================================================\n";
		//tmp+StdStringFormat("%16.16s%8s\n", "state updates", "max timing to exec state");
		//for(std::map<StateType, ControlNullThreadCondFnc>::iterator it = _stateMap.begin(); it!= _stateMap.end(); it++)
		//{
		//		tmp+=StdStringFormat("%16.16s%8.2f\n", (*it).first.c_str(),_stateMapTiming[(*it).first.c_str()]) ;
		//}

		return tmp;
	}

	StateType ControlThread::GetState() 
	{
		StateType state;
		{ boost::mutex::scoped_lock(_mx);state=_state; }
		return state;
	}
	std::string ControlThread::GetStateName() 	{ return StateStr(GetState()); }
	StateType ControlThread::GetLastState() 
	{
		StateType state;
		{ boost::mutex::scoped_lock(_mx);state=_laststate; }
		return state;
	}
	StateType ControlThread::SetState(StateType state) 
	{
		boost::mutex::scoped_lock(_mx); 
		StateType laststate=_state;
		_state=state; 
		return laststate;
	}	

	void ControlThread::PushState(StateType state)
	{
		boost::mutex::scoped_lock(_mx); 
		_statestack.push_back(_state);
		StateType laststate=_state;
		_state=state; 
	}
	void ControlThread::PopState()
	{
		boost::mutex::scoped_lock(_mx); 
		_state = _statestack.front();
		_statestack.pop_front();
	}

	void ControlThread::ResetStateTiming()
	{
		for(std::map<StateType, double>::iterator it=_stateMapTiming.begin(); it!=_stateMapTiming.end(); it++)
			(*it).second=0.0;
	}
	double ControlThread::GetStateTiming(StateType state) { return _stateMapTiming[state]; }
	void ControlThread::RestartTimer() { threadtimer.restart(); }


	void ControlThread::LogMessage(std::string str, int n)
	{
		if(n<_nDebugLevel)
			OutputDebugString(str.c_str());
	}

	int ControlThread::GetMilliCount()
	{
		// Something like GetTickCount but portable
		// It rolls over every ~ 12.1 days (0x100000/24/60/60)
		// Use GetMilliSpan to correct for rollover
		timeb tb;
		ftime( &tb );
		int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
		return nCount;
	}

	int ControlThread::GetMilliSpan( int nTimeStart )
	{
		int nSpan = GetMilliCount() - nTimeStart;
		if ( nSpan < 0 )
			nSpan += 0x100000 * 1000;
		return nSpan;
	}


};

