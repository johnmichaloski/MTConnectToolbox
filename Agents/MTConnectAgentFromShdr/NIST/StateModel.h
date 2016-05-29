//
// C:\Program Files\NIST\proj\MTConnect\Nist\MTConnectGadgets\CMSDCommandCell\CMSDCommandCell\StateModel.h
//
#pragma once
#include <boost/thread/thread.hpp>
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/lambda/bind.hpp"
#include <boost/function.hpp> 
#include <boost/timer.hpp> 
#include <boost/date_time/posix_time/posix_time.hpp> 
#include <boost/timer.hpp>

#include "StdStringFcn.h"

#define BOOST_CB_DISABLE_DEBUG // The Debug Support has to be disabled, otherwise the code produces a runtime error.
#include <boost/circular_buffer.hpp>
#include <map>

using namespace boost::tuples;

#include <boost/version.hpp>
#ifndef TIME_UTC
#define TIME_UTC TIME_UTC_ 
#endif

struct Stats
{
	Stats()
	{	 
		nTotalParts= nGoodParts=0;
		dTotalTime=nDownTime=nProductionTime=nBlockedTime=nStarvedTime=nOffTime=nRepairTime=nIdleTime=0.0;
	}
	int  nTotalParts;
	int  nGoodParts;
	double dTotalTime;
	double nDownTime;
	double nProductionTime;
	double nBlockedTime;
	double nStarvedTime;
	double nOffTime;
	double nRepairTime;
	double nIdleTime;
	std::map<std::string, double> vals;
	std::map<std::string, std::string> props;

	static  void Update(std::string state, double time, Stats &stats1)
	{
		stats1.vals[state]=stats1.vals[state]+time;
	}
	static  void Update(std::string state, double time, Stats &stats1, Stats &stats2)
	{
		Update( state, time, stats1);
		Update( state, time, stats2);
	}
	static  void Update(std::string state, double time, Stats &stats1, Stats &stats2, Stats &stats3)
	{
		Update(state,  time, stats1, stats2);
		Update(state,  time, stats3);
	}
	static void Update(std::string state, double time, Stats &stats1, Stats &stats2, Stats &stats3,Stats &stats4)
	{
		Update(state,  time, stats1, stats2,stats3);
		Update(state,  time, stats4);
	}
	static double & Get(std::string state, Stats &stats1) { return stats1.vals[state]; }
	double & Get(std::string state) { return vals[state]; }
	std::string & Property(std::string state) { return props[state]; }
	double & operator[] (const std::string state){ return vals[state]; }
};


// To do: 
// RCS StateModel - use enum strings, add mutex cmd/status queue or shared mem use CONNECTOR to write
// Sim StateModel - off, idle, starved, blocked, running, faulted
// boost thread synchronization: http://www.boost.org/doc/libs/1_45_0/doc/html/thread/synchronization.html
// ref: http://en.highscore.de/cpp/boost/functionobjects.html 
namespace StateModel
{

	enum ConditionType {Clear=0, Fault, Warning, Message } ;
	class IModule
	{
	public:
		virtual void Configure(std::string inifilename) =0;
		virtual void Init() = 0;
		virtual void Run() = 0;
		virtual void Stop() = 0;
		virtual void Reset() = 0;
		virtual void Quit()=0;
		virtual void Condition(ConditionType type, const char * id, const char *s)=0;

	};

	class AsyncMethod
	{
	public:
		AsyncMethod() : _thread(NULL) {}
		bool IsRunning();
		template<typename T>
		bool Run(T t) 
		{ 
			if(IsRunning())
				return false;
			//  if(_thread ==NULL)
			if(_thread!=NULL)
			{
				delete _thread;
			}
			_thread = new boost::thread(t); 
			return true;
		} 
		void Join() ;
	private:
		boost::thread *_thread; 
	};




__declspec(selectany)  std::string off = std::string("off");
_declspec(selectany)  std::string ready = std::string("ready");
_declspec(selectany)  std::string loaded =std::string("loaded");
_declspec(selectany)  std::string running= std::string("running");
_declspec(selectany)  std::string stopped =std::string("stopped");
_declspec(selectany)  std::string interrupted =std::string("interrupted");
_declspec(selectany)  std::string faulted = std::string("faulted");
_declspec(selectany)  std::string  finished =std::string("finished");
_declspec(selectany)  std::string  exit =std::string("exit");
_declspec(selectany)  std::string  estopped =std::string("estopped");
_declspec(selectany)  std::string  any= std::string("any");
_declspec(selectany)  std::string  stepping =std::string("stepping");
_declspec(selectany)  std::string  starved =std::string("starved");
_declspec(selectany)  std::string  blocked= std::string("blocked");

_declspec(selectany)  std::string  ok =std::string("ok");
_declspec(selectany)  std::string  init =std::string("init");
_declspec(selectany)  std::string  load =std::string("load");
_declspec(selectany)  std::string  run =std::string("run");
_declspec(selectany)  std::string  stop =std::string("stop");
_declspec(selectany)  std::string  hold =std::string("hold");
_declspec(selectany)  std::string  fail =std::string("fail");
_declspec(selectany)  std::string  reset =std::string("reset");
_declspec(selectany)  std::string  resume =std::string("resume");
_declspec(selectany)  std::string  done =std::string("done");
_declspec(selectany)  std::string  quit= std::string("quit");
_declspec(selectany)  std::string  rewind= std::string("rewind");
_declspec(selectany)  std::string  estop= std::string("estop");
_declspec(selectany)  std::string  block= std::string("block");
_declspec(selectany)  std::string  starve =std::string("starve");
_declspec(selectany)  std::string  step =std::string("step");
_declspec(selectany)  std::string  singlestep =std::string("singlestep");
_declspec(selectany)  std::string  update =std::string("update");
_declspec(selectany)  std::string  empty =std::string("empty");


class ControlThread;

typedef std::string StateType;
typedef std::string EventType;

inline std::string StateStr(StateType s) { return s; }
inline std::string EventStr(EventType s) { return s; }



typedef boost::function< bool () > ControlThreadCondFnc;
typedef boost::function< void () > ControlThreadFnc;
typedef boost::function< void () > ControlNullThreadCondFnc;

typedef boost::function< void (ControlThread *) > ControlThreadCallbackFnc;
typedef boost::tuple<std::string, std::string, std::string, ControlThreadCondFnc ,ControlNullThreadCondFnc  > StateMachineTuple;
// State, Name, Unit, Description, init value, cost multiplier
// 0    , 1   ,  2  , 3           , 4        , 5
typedef boost::tuple<std::string, std::string, std::string, std::string, double , double  > CostFcnTuple;

	class ControlThread : public Stats
	{ 
		//////
		///////////////////////////////////////////////////////////////////////
	public: 

		ControlThread() ;
		virtual ~ControlThread() ;
		
		std::string					GetErrorMessage() ;
		void						SetErrorMessage(std::string err) ;
		unsigned int &				UpdateRate() ;
		double						UpdateRateSec();
		void						SetUpdateRate(unsigned int milliseconds) ;
		std::string&				Name() ;
		void						SetStateMachineName(std::string name) ;


		void						AddCostFunction(CostFcnTuple s) ;
		static void					Delay(double amt =0);
		// Event handling
		void						Trigger(StateType e);
		// Event handling
		void						SyncEvent(StateType e);
		
		// Conditions
		bool						NullCondition() ;
		void						SetStateUpdate(StateType state1, StateModel::ControlThreadFnc fcn);
		// ISSUE here is that state transition has to exist first before eadding
		void						SetStateTransition(StateType s1,  EventType e, StateType s2, StateModel::ControlThreadFnc fcn);
		void						SetCondition(StateType s1, EventType e, StateModel::ControlThreadCondFnc fcn);

		virtual void				Nop()	;
		virtual void				Break()	;
		virtual void 				Postprocess();
		virtual void				Preprocess();
		virtual void				Breakpoint();	

		int							IsActive() ;
		void						Loop();
		bool						FindStateTransitionMatch(StateType _state, 
			EventType _e, 
			std::vector<StateMachineTuple> statemachine, 
			StateMachineTuple &statematch);

		void						Cycle();
		void						Process(EventType e);
		void						Join() ;
		void						StartThread() ;
		void						StopThread();
		
		void						AddEventListener(EventType e, ControlThreadCallbackFnc fcn);
		void						RemoveEventListener(EventType e, ControlThreadCallbackFnc fcn);		
		virtual std::string			ToString();

		void						AddStateTransition(StateMachineTuple s);
		template<typename U>
		void						AddStateUpdate(StateType state, U t){_stateMap[state]=t;}		
		StateType					GetLastState() ;
		StateType					GetState() ;
		std::string					GetStateName();
		double						GetStateTiming(StateType state) ;
		void						PopState();
		void						PushState(StateType state);
		static void					RestartTimer() ;
		void						ResetStateTiming();
		StateType					SetState(StateType state) ;
		
		void						TraceSave(std::string e, double dSeconds=0);
		std::string					StateMachineTraceDump();
		std::string					StateDump(std::string e);

		int							GetMilliSpan( int nTimeStart );
		int							GetMilliCount();
		/////////////////////////////////////////////////////////////////
		//boost::circular_buffer<EventType> eventtrace;
		std::vector<std::string>	statetrace;
		std::vector<double>			timetrace;
		std::list<StateType>		_statestack;
		
		unsigned int				_updaterate;

		static double				_dSpeedup;  // good for all the state machines
		static boost::timer			threadtimer;
		static double				_dTotalTime;
		static double				_dLoopTime;
		static std::map<std::string, double> globalCosts;
		std::map<std::string, double> stateTimes;
		double _nMsTimerElapsed ;
		int _nTMsTimeStart;


	protected:
		typedef std::map<StateType, std::vector<ControlThreadCallbackFnc > >::iterator ListenerIterator;

		boost::thread *_thread; 
		StateType _state,_laststate;
		std::vector<StateMachineTuple> _statemachine;
		std::map<EventType, std::vector<ControlThreadCallbackFnc > > _eventlisteners;
		std::list<EventType> _eventQueue;
		std::map<StateType, ControlNullThreadCondFnc> _stateMap;
		std::map<StateType, double> _stateMapTiming;
		std::vector<CostFcnTuple> _costfcns;
		unsigned int _cycle;
		long _nloop;
		std::string _errmsg;
		int _nDebugLevel;
		std::string _statemachinename;
		virtual void LogMessage(std::string str, int n=0);
	}; 

	__declspec(selectany)   double ControlThread::_dSpeedup;  // good for all the state machines
	__declspec(selectany)   boost::timer ControlThread::threadtimer;
	__declspec(selectany)   double ControlThread::_dTotalTime;
	__declspec(selectany)   double ControlThread::_dLoopTime;
	__declspec(selectany)   std::map<std::string, double>  ControlThread::globalCosts;


};
