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
#include "DBLogger.h"

class CSnapshots
{
public:
	CSnapshots();

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
	void							AddMachineToDb(std::string machine, Type type=IP);
	void							Cycle();
	void							Disconnect();
	void							Exit();
	void							Init();
	void							Logoff();
	void							Logon();
	void							Shutdown();
	void							Stop(){ _mRunning=false; }
	bool							Triggered(std::vector<std::string> events,DataDictionary& cycledatum, DataDictionary& lastcycledatum);


	CCsvParser						_csvparser;
	CDBLogger odbc;
 
	//OdbcArchiver					odbc;
	Type							_type;
	std::vector<std::string>		_faults;
	std::string						_dbConnection;
	int								bEventTriggered;

	std::vector<std::string>		_dbeventfields;  // state driven raw to event data
	std::vector<std::string>		_alarmColumns;   // alarm db recording
	std::vector<std::string>		_programColumns;   // program db recording

	std::vector<std::string>			_sCostFcns;

	DataDictionary						lastdatum;
	DataDictionary						lastcycledatum;
	DataDictionary						_kpidatum;
	DataDictionary						_typedatum;

	std::vector<std::string> 			_events;

	std::string 						_sMachine;
	std::string 						_ipaddr;
	std::string							_archivepath;
	std::string							_headers;

	bool								_mRunning;
	std::string							_ArchiveFields;

		// Configuration
	//std::string							_numberfields;
	DataDictionary						_colormapping;
	DataDictionary						_formats;
	std::vector<std::string>			_fields;
	//std::string							_dateTimefields;
	std::vector<std::string>			_KPIList;

	std::map<std::string,double>		_statetiming;
	std::string							_currentstate;
	std::string							_faultitems;

	void								UpdateCostFcns();
//	bool								CheckFault();
	bool								bTriggered;
	bool								_bClear;
};
