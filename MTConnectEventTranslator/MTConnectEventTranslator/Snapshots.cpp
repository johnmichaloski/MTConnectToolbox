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
#include "MTConnectEventTranslator.h"
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

static int GetSecondsTime(std::string s)
{
	int Hour, Minute,Second;
	if(sscanf(s.c_str(), "%d:%d:%d", &Hour, &Minute,&Second)==3){}
	else return 0;//throw std::exception("Bad Shift time format -  hh:mm\n");
	return 3600* Hour + 60 * Minute+Second;

}

//2010-03-18T11:32:33.8457100
static std::string SanitizeMTConnectUTCTime(std::string s)
{
	s=ReplaceOnce(s,"T", " ");
	size_t delimPos = s.find_last_of(".");
	if(delimPos != std::string::npos)
		s = s.substr(0,delimPos);
	return s;
}


///////////////////////////////////////////////////////////////////////////////

CSnapshots::CSnapshots() 
{
	_faultitems = "Timestamp,Machine,Shift,Program,MTBF, MTTR";
}
void CSnapshots::Logoff()
{
	GLogger.Fatal( "CSnapshots::Logoff()\n" ); 
	Disconnect();
}

void CSnapshots::Logon()
{
	GLogger.Fatal( "CSnapshots::Logon()\n" ); 
}
void CSnapshots::Shutdown()
{
	GLogger.Fatal( "CSnapshots::Shutdown()\n" ); 
	_mRunning=false;
}

void CSnapshots::Disconnect()
{
	GLogger.Fatal( "CSnapshots::Disconnect()\n" ); 
}

void  CSnapshots::AddMachineToDb(std::string machine, Type type)
{
	std::string _machinefields = "Name,IP";
	std::vector<std::string> _machineitems=TrimmedTokenize(_machinefields,",");

	//std::vector<std::vector<std::string> > rows = odbc.selectTableWhere("Factory", "Machines", StdStringFormat("Name = '%s'", machine.c_str()) , _machineitems, _typedatum);
	//if( rows.size() <1)
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

	odbc.deleteTable("Factory", "Alarms");
	odbc.createTable("Factory", "Alarms", TrimmedTokenize(_faultitems,","),_typedatum);

	odbc.deleteTable("Factory", "Programs");
	odbc.createTable("Factory", "Programs",_programColumns,_typedatum);
}
void CSnapshots::Exit()
{
	GLogger.Fatal("CSnapshots::Exit()\n" ); 
	::CoUninitialize();
}
void CSnapshots::Init()
{
	GLogger.Fatal("CSnapshots::Init()\n" ); 
	HRESULT hr;
	COleDateTime now;

	hr = ::CoInitialize(NULL);
	_set_se_translator( trans_func );  // correct thread?
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


//bool CSnapshots::CheckFault()
//{
//	int n = _alarmHandler.GetNumberAlarms();
//	cycledatum["Alarm"] = _alarmHandler.CheckFault(cycledatum);
//
//	// Check for detection of new alarm 
//	if(n != _alarmHandler.GetNumberAlarms())
//		return true; // new alarm found
//	return false;
//}
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
	DataDictionary  cycledatum ;
	_mRunning=true;

	Init();
	try {
		while(_mRunning)
		{

			// Only 1 datum returned, sequences found to inexact
			bEventTriggered=0;
			cycledatum.clear();
			cycledatum = _csvparser.ReadStream();

			bNoData=false;

			if(cycledatum.size() == 0)
				bNoData=true;

			// Detect change programming event - program,power,PartCountAct, line<lastline
			if(bNoData)
			{
			}
			else
			{
				AddMachineToDb(cycledatum["Machine"], CSV);
			}

			bEventTriggered=IsEndOfProgram(cycledatum,lastcycledatum,_dbeventfields);

			// Update non MTConnect information
			cycledatum["NewEvent"]= ToString(bEventTriggered); 
			//		cycledatum["Shift"]= ToString(_currentshift); // Given by csv file
			cycledatum["Timestamp"]= SanitizeMTConnectUTCTime(cycledatum["Timestamp"]); //  GetTimeStamp(LOCAL);  // reset to now?
			cycledatum["Machining"]= ConvertToString(GetSecondsTime(cycledatum["Cycletime"])); 
			cycledatum["Elapsed"]= GetSecondsTime(cycledatum["Cycletime"]); 
			cycledatum["OEE"]="100"; 



			// Check for detection of new alarm - will trigger some recalculations
			std::string sMTBF, sMTTR;
			if(stricmp(cycledatum["Event"].c_str(),"MTBF")==0)
			{
				cycledatum["MTTR"]= "0";
				odbc.insert("Factory","Alarms", TrimmedTokenize(_faultitems,","), cycledatum,_typedatum);
			}
			if(stricmp(cycledatum["Event"].c_str(),"MTTR")==0)
			{
				cycledatum["MTBF"]= "0";
				odbc.insert("Factory","Alarms", TrimmedTokenize(_faultitems,","), cycledatum,_typedatum);
			}
			if(stricmp(cycledatum["Event"].c_str(),"CYCLETIME")==0)
			{
				odbc.insert("Factory", MakeLower( cycledatum["Machine"]+"Events"), _dbeventfields, cycledatum,_typedatum);
				odbc.insert("Factory","Programs", _programColumns, cycledatum, _typedatum);
			}

			UpdateCostFcns();

			// Save current state for next interval
			lastdatum=cycledatum;

			//if(saveshift!=_currentshift)
			//{
			//	//ExcelArchive(saveshift);
			//	//DbArchive();			
			//	Clear(); // clear datum
			//}


		}

	}
	catch(std::exception e)
	{
		AtlTrace("%s%s", "CEchoService::serviceThread() exception " , e.what());
	}
	catch(...)
	{
		AtlTrace("CEchoService::serviceThread() exception ");
	}
	Exit();
}


//extern void Interpret(DataDictionary &vars, std::vector<std::string> script);

void CSnapshots::UpdateCostFcns()
{
	return;

	// Note there will no usable data if the mtconnect agent is down ... 
	//if(datum.size() ==0)
	//	return;

	if(_sCostFcns.size() ==0)
		return;

	//_kpidatum.insert(cycledatum.begin(), cycledatum.end());
	////Interpret(_kpidatum, _sCostFcns);
	//cycledatum.insert(_kpidatum.begin(), _kpidatum.end());
	int n=0;
}
