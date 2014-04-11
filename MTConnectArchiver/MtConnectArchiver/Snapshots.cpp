//
// Snapshots.cpp
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#include "StdAfx.h"
#include "Snapshots.h"
#include "StdStringFcn.h"
#include "MtConnectArchiver.h"
#include "Logger.h"
#include "File.h"


#define TESTHR(X) \
	if(FAILED(hr=X))\
{ std::string errmsg=StdStringFormat("%s:%s%s",__FILE__ ,__LINE__ ,#X) ;\
	errmsg+=ErrorFormatMessage(hr);\
	throw std::exception(errmsg.c_str());\
}


static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg = StdStringFormat("Fanuc Shdr trans_func - Code = 0x%x\n",  
		pExp->ExceptionRecord->ExceptionCode) ;

	GLogger << FATAL << "FanucMTConnectAdapter::gatherDeviceData Exception Raised\n" << errmsg << std::endl;
	throw std::exception(errmsg.c_str());
}

static int GetShiftTime(std::string s)
{
	int Hour, Minute;
	if(sscanf(s.c_str(), "%d:%d", &Hour, &Minute)==2){}
	else return 0;//throw std::exception("Bad Shift time format -  hh:mm\n");
	return Hour * 60 + Minute;

}

void __stdcall CSnapshots::OnReset(BSTR items)
{
	::MessageBox(NULL, "Hello World","", MB_OK);
}
	  
	  ///////////////////////////////////////////////////////////////////////////////

CSnapshots::CSnapshots() 
{
	_nSnapshot=-1;
	_sShift="<BR><H3>Shift 1</H3>";
	_loggedon=true;
	shiftbusytime=0;
}
void CSnapshots::Logoff()
{
	GLogger.Fatal( "CSnapshots::Logoff()\n" ); 
	_loggedon=false;
	Disconnect();
}

void CSnapshots::Clear()
{
	datum.clear();
	_alarmHandler.Clear(); 
	shifttimer.restart();
	shiftbusytime=0;

	try {

	}
	catch(...){}
}

void CSnapshots::Logon()
{
	GLogger.Fatal( "CSnapshots::Logon()\n" ); 
	_loggedon=true;
}
void CSnapshots::Shutdown()
{
	GLogger.Fatal( "CSnapshots::Shutdown()\n" ); 
	_mRunning=false;
	_loggedon=false;
}

void CSnapshots::Disconnect()
{
	GLogger.Fatal( "CSnapshots::Disconnect()\n" ); 
	_lasttooltip.clear();
	_sLastShift.clear();

}

void CSnapshots::Update()
{
	static int cnt=0;
	HRESULT hr=S_OK;
	try{

		while(_alarmHandler._allfaultdatum.size() > 0)
		{
			std::string values = _alarmHandler._allfaultdatum.back().ValueString(_alarmHandler._faultitems);
			_alarmHandler._allfaultdatum.pop_back();
//			pTrayIcon->AddFaultRow(bstr_t(ToString(_alarmHandler._faultitems).c_str()),
//				bstr_t(values.c_str()));
			
			//std::string dbvalues = _alarmHandler._allfaultdatum.back().ValueString(_alarmHandler._faultitems);

			odbc.insert("Factory","Alarms",
				TrimmedTokenize(_alarmHandler._faultitems,","),
				TrimmedTokenize(values,","), 
				_typedatum);
		}

		static char * Events [] ={ "program","power","PartCountAct","mode","execution", "Alarm"};
		static std::vector<std::string> events(Events, Events + sizeof Events / sizeof Events[ 0 ]);
		bTriggered = Triggered(events,datum[0], lastdatum);

		if(bTriggered)
		{
			odbc.insert("Factory",MakeLower(datum[0]["Machine"]+"Events"), _dbeventfields, datum[0],_typedatum);
		}

		bEventTriggered= IsEndOfProgram(datum[0],lastcycledatum, _events);
		// Save program data before data is cleared.
		// Conflict here - either PartCountAct or program name changes, don't count both. Likewise line
		if(datum.size()!=0 && IsEndOfProgram(datum[0],lastcycledatum, TrimmedTokenize("PartCountAct,program",",")))
		{
			odbc.insert("Factory","Programs", _programColumns, datum[0], _typedatum);
		}


	}
	catch(std::exception errmsg)
	{
		Disconnect();
		GLogger.Warning(StdStringFormat("CSnapshots::Update() %s\n" , errmsg.what() ));

	}	
	catch(...)
	{
		Disconnect();
	}
}



void  CSnapshots::AddMachineToDb(std::string machine, Type type)
{
	std::string _machinefields = "Name,IP";
	std::vector<std::string> _machineitems=TrimmedTokenize(_machinefields,",");

	std::vector<std::vector<std::string> > rows = odbc.selectTableWhere("Factory", "Machines", StdStringFormat("Name = '%s'", machine.c_str()) , _machineitems, _typedatum);
	if( rows.size() <1)
	{
		DataDictionary  values;
		values["Name"]=machine;
		if(type==IP)
			values["IP"]=_ipaddr;
		odbc.insert("Factory", "Machines", _machineitems, values, _typedatum);

		odbc.deleteTable("Factory", machine+"events");
		odbc.createTable("Factory", machine+"events",_dbeventfields, _typedatum);

	}


}
void CSnapshots::Configure(crp::Config &config,
		std::string sMachine, // machine name
		std::string ipaddr, // ip address of agent
		std::string configname, // section name to lookup configuration info
		Type type
		)
{
	_ipaddr= ipaddr;
	_delay =  config.GetSymbolValue("CONFIG.DELAY", L"2500").toNumber<int>();
	_type=type;
	_sMachine=sMachine;

	_dbConnection = config.GetSymbolValue("CONFIG.DbConnection", "CMSD").c_str();


	_colormapping =  config.getmap("COLORCODING");
	_formats =  config.getmap("FORMATS");

	//_numberfields =  config.GetSymbolValue("CONFIG.Numbers", L"").c_str();
	//_numberfields+=",Shift";
	//_dateTimefields =  config.GetSymbolValue("CONFIG.DateTime", L"").c_str();
	_KPIList = config.GetTokens("CONFIG.KPI", ",");
	_fields = config.GetTokens("CONFIG.Fields", ","); 
	_dbeventfields = config.GetTokens("CONFIG.DbEventFields", ","); 
	_faults = config.GetTokens("CONFIG.Fault", ","); 


	//???
	_headers= config.GetSymbolValue("CONFIG.SummaryHeader", L"").c_str();
	//_sHeader =  config.GetSymbolValue("CONFIG.Header", L"MTConnect Readings").c_str();

	_typedatum["Name"]="adVarWChar";
	_typedatum["IP"]="adVarWChar";

	_typedatum.Mixin(MTConnectStreamsParser::_saveddata, "adVarWChar");
	_typedatum.Mixin(_fields, "adVarWChar");
	_typedatum.Mixin(config.GetTokens("CONFIG.Numbers", ",") , "adDouble"); 
	_typedatum.Mixin(config.GetTokens("CONFIG.DateTime", ",") , "adDBTime"); 
	_typedatum["Timestamp"]="adDate"; 
	_typedatum["Alarm"]="adVarWChar"; // should be in _fields
	_typedatum["Shift"]="adInteger"; //


	_alarmColumns = TrimmedTokenize("Severity,RaisedBy,Description,Program,RaisedAt,ResolvedAt,TBF,TTR",",");
	_typedatum.Mixin(_alarmColumns, "adVarWChar");
	_typedatum["ResolvedAt"]=_typedatum["RaisedAt"]="adDate";
	_typedatum["TBF"]=_typedatum["TTR"]="adTime";// 00:00:00


	_programColumns= TrimmedTokenize("Timestamp,Machine,Shift,program,Duration,Machining,Faulted,RpmTotal",",");
	_typedatum["Faulted"]="adDBTime"; // seconds
	_typedatum["RpmTotal"]="adInteger"; // revolutions


	_sCostFcns =  config.getsection("COSTFCNS");
	_events = config.GetTokens("CONFIG.Events", ","); 
	_bClear = config.GetSymbolValue("CONFIG.CLEAR", "0").toNumber<int>();

	// Archive information
	_archivepath =  config.GetSymbolValue("CONFIG.ARCHIVEPATH", File.ExeDirectory()).c_str();
	_ArchiveFields = config.GetSymbolValue("CONFIG.Archive", File.ExeDirectory()).c_str();

	// Shift information
	std::string shiftchanges =  config.GetSymbolValue("CONFIG.SHIFTCHANGES", L"00:00,08:00,16:00").c_str();
	std::vector<std::string> shifttimes =TrimmedTokenize(shiftchanges, ",");
	for(int i=0; i< shifttimes.size(); i++)
	{
		_shiftchanges.push_back(GetShiftTime(shifttimes[i]));
		// shift i+1 must be > shift i
	}

	_delay = _delay / 1000;

	if(_delay == 0) 
		_delay=1;

	odbc.open(_dbConnection,"root","bingo");
	odbc.createSchema("Factory");


	if(type==IP)
	{

		AddMachineToDb(sMachine);

		if(odbc.sizeTable("Factory", sMachine+"Events")==0 ||  _bClear ) // no rows table?
		{
			odbc.deleteTable("Factory", sMachine+"Events");
			odbc.createTable("Factory", sMachine+"Events",_dbeventfields, _typedatum);
		}

	}
	else if(type==CSV)
	{
		 _csvparser.Initialize(_ipaddr);
	}

	_alarmHandler.Configure(_faults,_archivepath, sMachine); // each machine supports different alarm archive
	odbc.deleteTable("Factory", "Alarms");
	odbc.createTable("Factory", "Alarms", TrimmedTokenize(_alarmHandler._faultitems,","),_typedatum);

	odbc.deleteTable("Factory", "Programs");
	odbc.createTable("Factory", "Programs",_programColumns,_typedatum);
}
void CSnapshots::Exit()
{
	GLogger.Fatal("CSnapshots::Exit()\n" ); 
	_parser.Release();
	::CoUninitialize();
}
void CSnapshots::Init()
{
	GLogger.Fatal("CSnapshots::Init()\n" ); 
	HRESULT hr;
	COleDateTime now;

	hr = ::CoInitialize(NULL);
	_set_se_translator( trans_func );  // correct thread?
	shifttimer.restart();
}
//void CSnapshots::ExcelArchive(int saveshift)
//{
//	COleDateTime now = COleDateTime::GetCurrentTime();
//	// FIXME: Check if any parts were actually made
//	std::string filename=_archivepath;
//	filename+=this->_sMachine+"Log";
//	filename+=(LPCSTR) now.Format("%y_%m_%d");
//	filename+=StdStringFormat("Shift%d", saveshift+1);
//	filename+=".csv"; 
//	_csvarchiver.write(filename.c_str(),_headers, _typedatum, datum);
//}


bool CSnapshots::CheckFault()
{
	int n = _alarmHandler.GetNumberAlarms();
	datum[0]["Alarm"] = _alarmHandler.CheckFault(datum[0]);

	// Check for detection of new alarm 
	if(n != _alarmHandler.GetNumberAlarms())
		return true; // new alarm found
	return false;
}
bool CSnapshots::IsMachining(DataDictionary cycledatum)
{
	return (ConvertString<int>(cycledatum["Srpm"], 0) >0);
}
bool CSnapshots::IsFaulted(DataDictionary cycledatum)
{
	return !cycledatum["Alarm"].empty();
}
bool CSnapshots::Triggered(std::vector<std::string> events,DataDictionary& cycledatum, DataDictionary& lastcycledatum)
{
	for(int i=0; i< events.size(); i++)
	{
		if( lastcycledatum[events[i]] != cycledatum[events[i]])
			return true;
	}
	if(_type==CSV)
		return  true;
	return false;

}
int CSnapshots::GetCurrentShift()
{
	COleDateTime now = COleDateTime::GetCurrentTime();
	int minute = now.GetHour() * 60 + now.GetMinute();
	if(minute < _shiftchanges[0])
		return _shiftchanges.size();
	for(int i=1; i<= _shiftchanges.size(); i++)
	{
		int high = (i<_shiftchanges.size())? (_shiftchanges[i]-1) : _shiftchanges[i-1]+ _shiftchanges[0];
		if(minute >= _shiftchanges[i-1]  && minute <= high )
		{
			return i;
		}
	}
	return 1;
}

bool CSnapshots::IsEndOfProgram(DataDictionary cycledatum,DataDictionary lastcycledatum, std::vector<std::string> 	events)
{
	// Detect change programming event - program,power,PartCountAct, line<lastline
	if(cycledatum.size() > 0)
	{
		for(int i=0; i< events.size(); i++)
		{
			if( lastcycledatum[events[i]] != cycledatum[events[i]])
				return true;
		}
		if(_type==CSV && stricmp(cycledatum["Event"].c_str() , "CYCLETIME")==0)
			return  true;
		if(ConvertString<long>(cycledatum["line"],0) <  ConvertString<long>(lastcycledatum["line"],0))
			return true;
	}
	return false;
}

void CSnapshots::Cycle()
{
	GLogger.Fatal( "CSnapshots::Cycle()\n" ); 
	double machineduration, totalduration, faultduration;
	std::string html;
	bool bNoData;
	std::vector<DataDictionary>  cycledatum ;
	boost::timer machiningtimer;
	_mRunning=true;

	Init();
	_currentshift=GetCurrentShift();
	while(_mRunning)
	{
		try {
			// Only 1 datum returned, sequences found to inexact
			bEventTriggered=0;
			if(_type==IP)
				cycledatum = _parser.ReadStream(_ipaddr);
			else if(_type==CSV)
				cycledatum = _csvparser.ReadStream();
	
			bNoData=false;
			if(cycledatum.size() == 0)
				bNoData=true;
			// Detect change programming event - program,power,PartCountAct, line<lastline
			
			if(datum.size()==0 )
			{	
				totalduration=machineduration=faultduration=0;
			}

			if(bNoData)
			{
				cycledatum.push_back(DataDictionary());
			}
			else
			{
				AddMachineToDb(cycledatum[0]["Machine"], CSV);
			}

			cycledatum[0]["Shift"]= GetCurrentShift();

			if(bNoData)
			{
				// MTConnect agent communication error - agent down or computer down			
				cycledatum[0]["power"]= "OFF";
				machineduration=faultduration=0;
				if(lastcycledatum["power"]!=cycledatum[0]["power"] )
				{
					totalduration=0;
				}
				else
					totalduration+= machiningtimer.elapsed();
			}

			else if(IsEndOfProgram(cycledatum[0],lastcycledatum, _events)) //  lastcycledatum["program"] != cycledatum[0]["program"])
			{
				totalduration=machineduration=faultduration=0;
			}
			else 
			{
				totalduration+= machiningtimer.elapsed(); 
				if(IsMachining(cycledatum[0])) //  ConvertString<int>(cycledatum[0]["Srpm"], 0) >0)
				{
					machineduration+=machiningtimer.elapsed();
					shiftbusytime+=machiningtimer.elapsed();
				}

				// No change in datum length - just update KPI or other numbers
				cycledatum[0].Update(machiningtimer.elapsed());
			}

			datum.insert(datum.begin(),cycledatum.begin(), cycledatum.end());
			lastcycledatum=cycledatum[0];

			bEventTriggered=IsEndOfProgram(cycledatum[0],lastcycledatum,_dbeventfields);
			
			// Update non MTConnect information
			//datum[0]["Duration"] = DataDictionary::HrMinSecFormat(totalduration); 
			//datum[0]["Machining"]= DataDictionary::HrMinSecFormat(machineduration); 
			datum[0]["NewEvent"]= ToString(bEventTriggered); 
			datum[0]["Shift"]= ToString(_currentshift); 


			if(_type==IP)
			{
				datum[0]["Timestamp"]= GetTimeStamp(LOCAL);  // reset to now?
				datum[0]["Machine"]= _sMachine;
				datum[0]["Duration"] = ToString(totalduration); 
				datum[0]["Machining"]= ToString(machineduration); 
				datum[0]["Elapsed"]= ToString(machiningtimer.elapsed()); 
				datum[0]["OEE"] = ToString(shiftbusytime/shifttimer.elapsed() * 100.0 ) ;
			}
			else 
			{
				datum[0]["Timestamp"]= GetTimeStamp(LOCAL);  // reset to now?
				datum[0]["Machining"]= datum[0]["Cycletime"]; 
				datum[0]["Elapsed"]= datum[0]["Cycletime"]; 
				datum[0]["OEE"]="100"; 
				//datum[0]["Timestamp"]= GetTimeStamp(LOCAL);  // reset to now?
			}


			// Check for detection of new alarm - will trigger some recalculations
			std::string sMTBF, sMTTR;
			//if(CheckFault())
			//{
			//	_alarmHandler.CalculateMeanTimes(sMTBF, sMTTR);
			//	datum[0]["MTBF"]= sMTBF;
			//	datum[0]["MTTR"]= sMTTR;
			//}
			//else
			//{
			//	datum[0]["MTBF"]= lastdatum["MTBF"];
			//	datum[0]["MTTR"]= lastdatum["MTTR"];
			//}

			if(IsFaulted(datum[0]))
			{
				faultduration+=machiningtimer.elapsed();
				datum[0]["Faulted"]= ToString(faultduration); 
			}


	
			// Calculate current state and then add to timing of this state
			if(stricmp(datum[0]["power"].c_str(),"OFF")==0) _currentstate= "OFF";
			else if(stricmp(datum[0]["mode"].c_str(),"AUTO")!=0) _currentstate= "MANUAL";
			else if(!datum[0]["Alarm"].empty()) _currentstate= "FAULTED";
			else if(IsMachining(datum[0]))  _currentstate= "BUSY"; 
			else if(stricmp(datum[0]["execution"].c_str(),"IDLE")==0) _currentstate= "IDLE";
			else if(stricmp(datum[0]["execution"].c_str(),"RUNNING")!=0) _currentstate= "PAUSED";

			_statetiming[_currentstate]+= machiningtimer.elapsed();

			UpdateCostFcns();

			// Update 
			Update();

			// Save current state for next interval
			lastdatum=datum[0];

			int saveshift=_currentshift;
			_currentshift=GetCurrentShift();
			if(saveshift!=_currentshift)
			{
				//ExcelArchive(saveshift);
				//DbArchive();			
				Clear(); // clear datum
			}

			machiningtimer.restart();
			// Sleep a second
			boost::xtime xt;
			boost::xtime_get(&xt, boost::TIME_UTC_);
			xt.sec += _delay; 
			this->m_thread.sleep(xt);
		}
		catch(std::exception e)
		{
			AtlTrace("%s%s", "CEchoService::serviceThread() exception " , e.what());
		}
		catch(...)
		{
			AtlTrace("CEchoService::serviceThread() exception ");
		}
	}
	Exit();
}


//extern void Interpret(DataDictionary &vars, std::vector<std::string> script);

void CSnapshots::UpdateCostFcns()
{
	return;

	// Note there will no usable data if the mtconnect agent is down ... 
	if(datum.size() ==0)
		return;

	if(_sCostFcns.size() ==0)
		return;
	
	_kpidatum.insert(datum[0].begin(), datum[0].end());
	//Interpret(_kpidatum, _sCostFcns);
	datum[0].insert(_kpidatum.begin(), _kpidatum.end());
	int n=0;
}
