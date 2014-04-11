//
// AlarmHandler.cpp
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#include "StdAfx.h"
#include "AlarmHandler.h"
#include "SimMath.h"
#include "Registry.h"
static char * AppRegistryKey = "Software\\NIST\\MTConnectArchiver";
static char * LastTimeFault = "LastTimeFault";


//<Fault type="COMMUNICATIONS" id="cc1" sequence="26" nativeCode="IO1231"timestamp="...">Communications error</Fault>

/*
	CsvArchiver csv;
	std::vector<std::string> faultfields; 
	std::vector<DataDictionary>  faultdatum;
	csv.read("C:\\DailyLogs\\M1FaultHistory.csv", faultfields, faultdatum);
*/

boost::mutex				CAlarmHandler::_alarm_mutex;
std::vector<DataDictionary>		CAlarmHandler::_allfaultdatum;

CAlarmHandler::CAlarmHandler(void)
{
	_faultitems = "Severity,RaisedBy,Shift,Description,Program,RaisedAt,ResolvedAt,TBF, TTR";
	_tbftimer.restart();
}

CAlarmHandler::~CAlarmHandler(void)
{
	WriteRegistryTimeFailure(_tbftimer.elapsed());
}
void CAlarmHandler::Configure(std::vector<std::string> faults, 
							  std::string archivepath,
							  std::string sMachine
							  )
{
	_faults=faults;
	_archivepath=archivepath;
	_sMachine=sMachine;
	_typedatum.Mixin(_faults, "adVarWChar");
	CRegistry key(HKEY_CURRENT_USER);
	_bRejectFirstMtbf=true;
	if(!key.CreateKey(HKEY_CURRENT_USER, AppRegistryKey))
	{
		key.Open(AppRegistryKey);

		if(key.VerifyKey((_sMachine+LastTimeFault).c_str()))
		{
			double dElapsed;
			if(key.ReadType<double> ((_sMachine+LastTimeFault).c_str(),dElapsed))
			{
				_tbftimer = timer_w_set( -(dElapsed * CLOCKS_PER_SEC));
				_bRejectFirstMtbf=false;
			}
		}
	}

}
void CAlarmHandler::WriteRegistryTimeFailure(double dTBFSeconds)
{
	CRegistry key(HKEY_CURRENT_USER);
	if(!key.Open(AppRegistryKey))	
	{
			key.CreateKey(HKEY_CURRENT_USER, AppRegistryKey);
	}
	key.WriteType<double> ((_sMachine+LastTimeFault).c_str(), dTBFSeconds);
}
void CAlarmHandler::Clear()
{
	_allfaultdatum.clear(); 
}
void CAlarmHandler::LogFault(DataDictionary alarmdata)
{
	std::string filename;
	std::vector<DataDictionary>	 tempdata;
	tempdata.push_back(alarmdata);
	// Use blank types... Duration already formatted to hr:min:sec
	// Write to common fault history file for display
	{
		boost::mutex::scoped_lock  lock(_alarm_mutex);
		//filename=_archivepath+"AlarmHistory.csv";
		//_csvarchiver.append(filename.c_str(),_faultitems, DataDictionary(), tempdata);
	}
	// Use blank types... TBF,TTR  formatted to hr:min:sec change back to seconds
	filename=_archivepath+this->_sMachine+"AlarmHistory.csv";
	tempdata.back()["TBF"] = ToString<double>(GetTotalSeconds(alarmdata["TBF"]));
	tempdata.back()["TTR"] = ToString<double>(GetTotalSeconds(alarmdata["TTR"]));
	// We are skipping the first fault, if the timing is just started from when we started. Skews #s.
//	if(!_bRejectFirstMtbf)
//		_csvarchiver.append(filename.c_str(),_faultitems, DataDictionary(), tempdata);	
	_bRejectFirstMtbf=false;
}

std::string CAlarmHandler::CheckFault(DataDictionary datum)
{
	_lastfault=_currentfault;
	_currentfault.clear();
	// what if multiple faults?
	for(int i=0; i< _faults.size(); i++)
	{
		if(datum.find(_faults[i]) == datum.end())
			continue;
		int npos  =  datum[_faults[i]].find('.');
		std::string level = datum[_faults[i]];
		std::string msg = datum[_faults[i]];

		if(npos!= std::string::npos) level =  Trim(datum[_faults[i]].substr(0,npos));
		if(npos!= std::string::npos) msg =  Trim(datum[_faults[i]].substr(npos+1));

		// FIXME: just grabs the first fault and leaves.
		if( (level != "Normal") && (stricmp(level.c_str(), "Unavailable")!=0))
		{	
			_currentfault=_faults[i]+ " " + datum[_faults[i]];

			if(_currentfault != _lastfault)
			{
				// cant continually reset
				_alarmdatum.clear();
				_alarmdatum["RaisedAt"] = GetTimeStamp(LOCAL); // datum["Timestamp"];
				_alarmdatum["Shift"] = datum["Shift"];
				_alarmdatum["RaisedBy"] = datum["Machine"];
				_alarmdatum["Severity"] = level;
				_alarmdatum["Description"] =_faults[i] + "-" + msg;
				_alarmdatum["ResolvedAt"] = GetTimeStamp(LOCAL); // datum["Timestamp"];
				_alarmdatum["Program"] = datum["program"];
				double dTBF = _tbftimer.elapsed();
				_tbftimer.restart();
				WriteRegistryTimeFailure(0);
				_alarmdatum["TBF"]=DataDictionary::HrMinSecFormat((double)dTBF); 
			}

			return _currentfault;
		}
	}
	if(!_lastfault.empty() && _currentfault.empty())
	{
		_alarmdatum["ResolvedAt"] = GetTimeStamp(LOCAL); // datum["Timestamp"];
		
		ptime start = GetDateTime(_alarmdatum["RaisedAt"]);
		ptime done = GetDateTime(_alarmdatum["ResolvedAt"]);
		time_duration duration = done - start;
		_alarmdatum["TTR"] = DataDictionary::HrMinSecFormat((double)duration.total_seconds()); 


		// FIXME: there can be multiple alarms simulataneously ...
		_allfaultdatum.push_back(_alarmdatum);
		_myfaultdatum.push_back(_alarmdatum);
		LogFault(_alarmdatum);

		if(_alarmdatum["Severity"] == "Fault")
		{

		}
	}
	return _currentfault;
}

//std::string CAlarmHandler::MTBF()
//{
//	// Need to maintain this in memory better
//	CsvArchiver csv;
//	std::vector<std::string> faultfields; 
//	std::vector<DataDictionary>  faultdatum;
//	std::string filename=_archivepath+this->_sMachine+"AlarmHistory.csv";
//	csv.read(filename, faultfields, _historicalfaultdatum);
//	std::vector<std::string> tbfs = DataDictionary().Column<std::string>( _historicalfaultdatum, "RaisedAt");
//	std::vector<ptime> ptbfs;
//	std::vector<double> dtbfs;
//	for(int i=0; i< tbfs.size(); i++)
//	{
//		ptbfs.push_back(GetDateTime(tbfs[i]));
//	}
//	if(ptbfs.size() < 2)
//		return "";  // time since last fail - from start, from yesterday?
//	for(int i=1; i< ptbfs.size(); i++)
//	{
//		time_duration duration = ptbfs[i] - ptbfs[i-1];
//		dtbfs.push_back(duration.total_seconds());
//	}
//	std::vector<double> ds;
//	srand ( 1 );
//	for(int i=1; i< 100; i++)
//	{
//		int n = rand() % dtbfs.size();
//		ds.push_back(dtbfs[n] /60.0);
//	}
//
//	StatFitting statfit;
//	Distribution dist = statfit.BestFit(ds); // dtbfs);
//	return dist.ToString();
//}
//std::string CAlarmHandler::MTTR()
//{
//
//}
//bool CAlarmHandler::CalculateMeanTimes(std::string &mtbf, std::string &mttr)
//{	// Need to maintain this in memory better
////	CsvArchiver csv;
//	std::vector<std::string> faultfields; 
//	std::vector<DataDictionary>  faultdatum;
//	std::string filename=_archivepath+this->_sMachine+"AlarmHistory.csv";
//	csv.read(filename, faultfields, _historicalfaultdatum);
//
//	std::vector<double> ttrs = DataDictionary().Column<double>( _historicalfaultdatum, "TTR");
//	std::vector<double> tbfs = DataDictionary().Column<double>( _historicalfaultdatum, "TBF");
//	//std::vector<double> dtbrs, dttrs;
//	mtbf.clear(); mttr.clear();
//	if(ttrs.size() < 2)
//		return false;  // time since last fail - from start, from yesterday?
//	if(ttrs.size() > 100)
//	{
//		random_unique(tbfs.begin(), tbfs.end(), 100);
//		random_unique(ttrs.begin(), ttrs.end(), 100);
//		tbfs.erase(tbfs.begin()+100, tbfs.end());
//		ttrs.erase(ttrs.begin()+100, ttrs.end());
//	}
//	StatFitting statfit;
//	Distribution dist1 = statfit.BestFit(ttrs);
//	Distribution dist2 = statfit.BestFit(tbfs);
//	mttr=dist1.ToString();
//	mtbf=dist2.ToString();
//	return true;
//
//}