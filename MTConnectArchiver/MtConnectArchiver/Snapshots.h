//
// Snapshots.h
//
#pragma once
#if defined( DEBUG) && ( _MSC_VER >= 1600)   // 2010
#import  "c:\\Program Files\\NIST\\proj\\MTConnect\\Nist\\MTConnectGadgets\\MtConnectDboard\\MtConnectDboardGui\\Debug10\\MtConnectDboardGui.exe"
#elif  ( _MSC_VER >= 1600 ) 
#import  "c:\\Program Files\\NIST\\proj\\MTConnect\\Nist\\MTConnectGadgets\\MtConnectDboard\\MtConnectDboardGui\\Debug10\\MtConnectDboardGui.exe"
#elif  defined( DEBUG) && (_MSC_VER >= 1500 && _MSC_VER < 1600)
#import  "c:\\Program Files\\NIST\\proj\\MTConnect\\Nist\\MTConnectGadgets\\MtConnectDboard\\MtConnectDboardGui\\Debug9\\MtConnectDboardGui.exe"
#elif (_MSC_VER >= 1500 && _MSC_VER < 1600)
#import  "c:\\Program Files\\NIST\\proj\\MTConnect\\Nist\\MTConnectGadgets\\MtConnectDboard\\MtConnectDboardGui\\Debug9\\MtConnectDboardGui.exe"
#else
#pragma message("_MSC_VER????")
#endif

namespace MTC=MtConnectDboardGuiLib;

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/timer.hpp>

#include "DataDictionary.h"
//#include "CsvArchiver.h"
#include "MTConnectStreamsParser.h"
#include "Config.h"
#include "AlarmHandler.h"
#include "OdbcArchiver.h"

#include "CsvParser.h"

__declspec(selectany)_ATL_FUNC_INFO OnResetInfo = {  CC_STDCALL, VT_EMPTY, 1, {VT_BSTR}};

class CSnapshots:
	public IDispEventSimpleImpl<1, CSnapshots, &__uuidof(MtConnectDboardGuiLib::_IDboardNotificationEvents)>
{
public:
	CSnapshots();
	void __stdcall OnReset(BSTR items);

BEGIN_SINK_MAP(CSnapshots)
	SINK_ENTRY_INFO(1, __uuidof(MtConnectDboardGuiLib::_IDboardNotificationEvents), 0x1, OnReset, &OnResetInfo)
END_SINK_MAP()

	enum StateDef { AUTO_IDLE, AUTO_RUNNING, MANUAL, FAULT,STOPPED };
	enum Type { IP, CSV, SHDR };
	bool							IsMachining(DataDictionary cycledatum);
	bool							IsFaulted(DataDictionary cycledatum);
	bool							IsEndOfProgram(DataDictionary cycledatum,DataDictionary lastcycledatum,std::vector<std::string> events);
	void							Clear();

	void							Configure(crp::Config &config,
		std::string sMachine, // machine name
		std::string ipaddr, // ip address of agent
		std::string configname, // section name to lookup configuration info
		Type type=IP
		); 
	void  AddMachineToDb(std::string machine, Type type=IP);
	void							Cycle();
	void							Disconnect();
	void							ExcelArchive(int nShift);
	void							Exit();
	int								GetCurrentShift();
	void							Init();
	void							Logoff();
	void							Logon();
	void							SetTooltip(std::string tooltip){ _tooltip= tooltip; }
	void							Shutdown();
	void							Stop(){ _mRunning=false; }
	bool							Triggered(std::vector<std::string> events,DataDictionary& cycledatum, DataDictionary& lastcycledatum);
	void							Update();


	CCsvParser _csvparser;
	OdbcArchiver					odbc;
	Type							_type;
	std::vector<std::string>		_faults;
	std::string						_dbConnection;
	int								bEventTriggered;
	std::vector<std::string>		_dbeventfields;  // state driven raw to event data
	std::vector<std::string>		_alarmColumns;   // alarm db recording
	std::vector<std::string>		_programColumns;   // program db recording

	std::vector<std::string>			_sCostFcns;
	std::string							_snapshot, _lastsnapshot;
	std::vector<std::string>			_snapshots ;
	unsigned int						_nSnapshot;
	std::string							_sShift,_sLastShift;
	StateDef							_state;
	bool								_loggedon;
	std::string							_tooltip, _lasttooltip;
	std::vector<DataDictionary>			datum;
	DataDictionary						lastdatum;
	DataDictionary						lastcycledatum;
	DataDictionary						_kpidatum;
	DataDictionary						_typedatum;
	boost::timer						shifttimer;
	double								shiftbusytime;

	int 								_currentshift;
	std::vector<std::string> 			_events;
	std::vector<int> 					_shiftchanges; // in minutes
	std::string 						_sMachine;
	std::string 						_ipaddr;
	std::string							_archivepath;
	std::string							_headers;
	MTConnectStreamsParser				_parser;
	//CsvArchiver							_csvarchiver;
	boost::thread						m_thread;
	bool								_mRunning;
	int									_delay; 
	std::string							_ArchiveFields;

		// Configuration
	//std::string							_numberfields;
	DataDictionary						_colormapping;
	DataDictionary						_formats;
	std::vector<std::string>			_fields;
	//std::string							_dateTimefields;
	std::vector<std::string>			_KPIList;

	CAlarmHandler						_alarmHandler;
	std::map<std::string,double>		_statetiming;
	std::string							_currentstate;

	void								UpdateCostFcns();
	bool								CheckFault();
	bool								bTriggered;
	bool								_bClear;
};