//
// AlarmHandler.h
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#pragma once
#include "DataDictionary.h"
#include <boost/thread/thread.hpp>
#include "timer_w_set.hpp"

class CAlarmHandler
{
public:
	CAlarmHandler(void);
	~CAlarmHandler(void);
	void Configure(std::vector<std::string> faults, 
		std::string archivepath,
		std::string sMachine
		);
	std::string							CheckFault(DataDictionary datum);
	void								Clear();
	void								LogFault(DataDictionary alarmdata);
//	bool								CalculateMeanTimes(std::string &mtbf, std::string &mttr);
	int									GetNumberAlarms() { return _myfaultdatum.size(); }
	void								WriteRegistryTimeFailure(double dTBFSeconds);

	DataDictionary						_alarmdatum;
	std::string 						_sMachine;
	std::string							_archivepath;
	std::string							_lastfault;
	std::string							_currentfault;
	static	boost::mutex				_alarm_mutex;
//	CsvArchiver							_csvarchiver;
	std::vector<std::string>			_faults;  // list of MTConnect fault tags
	static std::vector<DataDictionary>	_allfaultdatum;   // collection of all faults
	std::vector<DataDictionary>			_myfaultdatum;   // collection of my faults 
	std::string							_faultitems; // csv list of faults
	DataDictionary						_typedatum;
	std::vector<DataDictionary>			_historicalfaultdatum;
	timer_w_set							_tbftimer;
	bool								_bRejectFirstMtbf;
};
