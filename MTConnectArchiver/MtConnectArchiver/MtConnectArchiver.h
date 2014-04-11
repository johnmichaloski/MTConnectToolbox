//
// MtConnectDboard.h
//

#pragma once
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include "service.hpp"
#include "Config.h"
#include "DataDictionary.h"
#include "MTConnectStreamsParser.h"
#include "ExcelArchiver.h"
#include "CsvArchiver.h"
#include "Snapshots.h"

class CMtConnectDboard;
class CServiceEndThread;
extern SERVICE_STATUS          gSvcStatus; 
extern VOID ReportSvcStatus( DWORD dwCurrentState, //    The current state (see SERVICE_STATUS)
							DWORD dwWin32ExitCode, //   The system error code
							DWORD dwWaitHint); //    Estimated time for pending operation, 




class CMtConnectDboard : public MTConnectService
{
public:
	CMtConnectDboard();
	~CMtConnectDboard();
	virtual void initialize(int aArgc, const char *aArgv[]);
	virtual void start();
	virtual void stop();
	virtual void logoff();
	virtual void logon();
	void serviceThread();

	std::string										html;
	crp::Config										config;
	std::vector<CSnapshots *>						_snapshots;
	std::vector<std::string>						sMachines;
	std::vector<std::string>						ipaddrs;
	HANDLE m_ThreadEndEvent;
	boost::thread_group group;

	//OdbcArchiver					odbc;
	std::string						_dbConnection;
	DataDictionary					_typedatum;
	std::vector<std::string>		_fields;
	std::vector<std::string>		_alarmColumns;   // alarm db recording
	std::vector<std::string>		_programColumns;   // program db recording
	std::vector<std::string>		_dbeventfields;  // state driven raw to event data
	std::string						_archivepath;
	std::string						_ArchiveFields;
	OdbcArchiver					agentodbc;
	CSnapshots::Type				_type;
	bool								_bClear;

};


