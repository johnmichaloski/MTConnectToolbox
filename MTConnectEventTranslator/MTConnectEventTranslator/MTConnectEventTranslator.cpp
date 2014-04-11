//
// MtConnectDboard.cpp : main source file for MtConnectDboard.exe
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#include "stdafx.h"

#include "resource.h"
#include "MTConnectEventTranslator.h"
#include <functional>
#include <numeric>
#include <algorithm>
#include "Logger.h"

#include <boost/config/user.hpp>
#include <boost/timer.hpp>
#include "service.hpp"


CAppModule _Module;

#pragma comment(lib, "Ws2_32.lib")


#if defined(WIN64) && defined( _DEBUG) 
#pragma message( "DEBUG x64" )
#pragma comment(lib, "libboost_system-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_thread-vc100-mt-sgd-1_54.lib")

#elif !defined( _DEBUG) && defined(WIN64)
#pragma message( "RELEASE x64" )
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")

#elif defined(_DEBUG) && defined(WIN32)
#pragma message( "DEBUG x32" )
#pragma comment(lib, "libboost_thread-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-sgd-1_54.lib")
#elif !defined( _DEBUG) && defined(WIN32)
#pragma message( "RELEASE x32" )
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")
#endif


static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg = StdStringFormat("CMtConnectDboard trans_func - Code = 0x%x\n",  
		pExp->ExceptionRecord->ExceptionCode) ;

	GLogger.Fatal("CMtConnectDboard Exception Raised\n" + errmsg ); 
	throw std::exception(errmsg.c_str());
}


	////////////////////////////////////////
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	AtlTrace("_tWinMain Enter");
	_set_se_translator( trans_func );  // correct thread?
	HRESULT hRes = ::CoInitialize(NULL);
	hRes = _Module.Init(NULL, hInstance);

#if 0
	CAlarmHandler alarm;
	std::vector<std::string> faults = TrimmedTokenize("HydraulicLevel,HydraulicPressure,HydraulicTemp,CoolantLow,LogicProgram,Cloadc,Xloadc,Yloadc,Zloadc,condition", ",");
	alarm.Configure(faults, 
		::ExeDirectory(),
		"M001"
		);
	OutputDebugString(alarm.MTBF().c_str());
	OutputDebugString(alarm.MTTR().c_str());
#endif
	GLogger.Open(0);
	CMtConnectDboard myservice;
	myservice.setName("MtConnectDboard");
	myservice.main(__argc, (const char **) __argv);
	AtlTrace("_tWinMain Exit1");
	_Module.Term();
	AtlTrace("_tWinMain Exit2");
	::CoUninitialize();
	AtlTrace("_tWinMain Exit3");

}

CMtConnectDboard::CMtConnectDboard()// : dlgMain(this)
{
 
	m_ThreadEndEvent=CreateEvent(NULL,		// default security attributes
		FALSE,								// manual-reset event
		TRUE,								// initial state is nonsignaled
		"CMtConnectDboard Done");
}
CMtConnectDboard::~CMtConnectDboard()
{
	//delete _snapshot;

}

void CMtConnectDboard::logoff()
{
	GLogger.Warning("CMtConnectDboard::logoff()\n" ); 
	for(int i=0; i< _snapshots.size(); i++)
		_snapshots[i]->Logoff();
	File.KillProgram("MtConnectDboardGui.exe");
}
void CMtConnectDboard::logon()
{
	GLogger.Warning("CMtConnectDboard::logon()\n" ); 
	File.KillProgram("MtConnectDboardGui.exe");
	for(int i=0; i< _snapshots.size(); i++)
		_snapshots[i]->Logon();

}
void CMtConnectDboard::initialize(int aArgc, const char *aArgv[])
{
	GLogger.Warning("CEchoService::initialize(int aArgc, const char *aArgv[])\n" ); 
	char result[ MAX_PATH ];
	std::string path( result, GetModuleFileName( NULL, result, MAX_PATH ) );
	std::string cfgfile = path.substr(0,path.find_last_of(".")) + ".ini";
	if( GetFileAttributesA(cfgfile.c_str())!= INVALID_FILE_ATTRIBUTES)
	{
		config.load( cfgfile );

		// Service name
		std::string servicename =  config.GetSymbolValue("CONFIG.ServiceName", L"MtConnectDboard").c_str();
		setName(servicename.c_str());
			
		std::string stype=  config.GetSymbolValue("CONFIG.Type", L"IP").c_str();
		_type = stricmp(stype.c_str(), "IP") == 0 ? CSnapshots::IP : stricmp(stype.c_str(), "CSV")==0 ? CSnapshots::CSV : CSnapshots::SHDR;

		// Globals
		GLogger.DebugLevel()=  config.GetSymbolValue("CONFIG.Debug", L"0").toNumber<int>();
		GLogger.Timestamping()= (bool) config.GetSymbolValue("CONFIG.Timestamp", L"0").toNumber<int>();
		
		MTConnectStreamsParser::_TagRenames =  config.getmap("ALIASES");
		MTConnectStreamsParser::_valuerenames =  config.getmap("VALUEREMAPPING");
		MTConnectStreamsParser::_saveddata = config.GetTokens("CONFIG.Data", ","); // only this data is saved!

		ipaddrs=TrimmedTokenize(config.GetSymbolValue("DEVICES.IP", L"agent.mtconnet.org").c_str(),",");
		sMachines=TrimmedTokenize(config.GetSymbolValue("DEVICES.Machine", L"VMC-3Axis").c_str(),",");

		if(ipaddrs.size() != sMachines.size())
		{
			GLogger.Fatal("CMtConnectDboard::initialize # ip addresses != # machines\n" ); 
		}

		// Archive information
		_archivepath =  config.GetSymbolValue("CONFIG.ARCHIVEPATH", File.ExeDirectory()).c_str();
		_ArchiveFields = config.GetSymbolValue("CONFIG.Archive", File.ExeDirectory()).c_str();


		_fields = config.GetTokens("CONFIG.Fields", ","); 
		_dbeventfields = config.GetTokens("CONFIG.DbEventFields", ","); 

		_dbConnection = config.GetSymbolValue("CONFIG.DbConnection", "CMSD").c_str();
		_bClear =  config.GetSymbolValue("CONFIG.CLEAR", "0").toNumber<int>();

		//_alarmColumns = TrimmedTokenize("Severity,RaisedBy,Description,Program,RaisedAt,ResolvedAt,TBF,TTR",",");
		//_typedatum.Mixin(_alarmColumns, "adVarWChar");
		//_typedatum["ResolvedAt"]=_typedatum["RaisedAt"]="adDate";
		//_typedatum["TBF"]=_typedatum["TTR"]="adTime";// 00:00:00

		//_programColumns= TrimmedTokenize("Timestamp,Machine,Shift,program,Duration,Machining,Faulted,RpmTotal",",");
		//_typedatum["Faulted"]="adDBTime"; // seconds
		//_typedatum["RpmTotal"]="adInteger"; // revolutions


		//_typedatum.Mixin(MTConnectStreamsParser::_saveddata, "adVarWChar");
		//_typedatum.Mixin(_fields, "adVarWChar");
		//_typedatum.Mixin(config.GetTokens("CONFIG.Numbers", ",") , "adDouble"); 
		//_typedatum.Mixin(config.GetTokens("CONFIG.DateTime", ",") , "adDBTime"); 
		//_typedatum["Timestamp"]="adDate"; 
		//_typedatum["Alarm"]="adVarWChar"; // should be in _fields
		//_typedatum["Shift"]="adInteger"; //
		//_alarmColumns = TrimmedTokenize("Severity,RaisedBy,Description,Program,RaisedAt,ResolvedAt,TBF,TTR",",");
		//_typedatum.Mixin(_alarmColumns, "adVarWChar");
		//_typedatum["ResolvedAt"]=_typedatum["RaisedAt"]="adDate";
		//_typedatum["TBF"]=_typedatum["TTR"]="adTime";// 00:00:00
		//_programColumns= TrimmedTokenize("Timestamp,Machine,Shift,program,Duration,Machining,Faulted,RpmTotal",",");
		//_typedatum["Faulted"]="adDBTime"; // seconds
		//_typedatum["RpmTotal"]="adInteger"; // revolutions
	}
	else
	{
		GLogger.Fatal(StdStringFormat("Could not find ini configuration file %s \n" , cfgfile.c_str())); 
	}

	MTConnectService::initialize(aArgc, aArgv);
}
void CMtConnectDboard::start()
{
	HRESULT hRes = ::CoInitialize(NULL);
	AtlTrace("CEchoService::start())");
	serviceThread(); // not really thread anymore, never returns
}
void CMtConnectDboard::stop()
{
	GLogger.Warning("CEchoService::stop())\n" ); 
	//CServiceEndThread * enditThread = new CServiceEndThread(this, dlgThread);
	//enditThread->Resume();
	AtlTrace("CMtConnectDboard::stop() thread done");
	for(int i=0; i< _snapshots.size(); i++)
		_snapshots[i]->Shutdown();

	::WaitForSingleObject(m_ThreadEndEvent, 5000 );
	File.KillProgram("MtConnectDboardGui.exe");
}
void CMtConnectDboard::serviceThread()
{
	GLogger.Warning("CEchoService::serviceThread()\n" ); 
	_set_se_translator( trans_func );  // correct thread?

	DataDictionary	_machinetypedatum;
	_machinetypedatum["Name"]="adVarWChar";
	_machinetypedatum["IP"]="adVarWChar";
	std::string _machinefields = "Name,IP";
	std::vector<std::string> _machineitems=TrimmedTokenize(_machinefields,",");

	//DbConnection=ConsolidatedData
	_dbConnection = config.GetSymbolValue("CONFIG.DbConnection", "CMSD").c_str();
	agentodbc.open(_dbConnection,"root","bingo");
	agentodbc.createSchema("Factory");
	if(agentodbc.sizeTable("Factory",  "Machines")!=0 || _bClear) // no rows table?
	{
		agentodbc.deleteTable("Factory", "Machines");  // in case 0 rows size
		agentodbc.createTable("Factory", "Machines",_machineitems, _machinetypedatum);
	}
			


	try {
		::Sleep(1000*5);
		for(int i=0; i< ipaddrs.size(); i++)
		{
			CSnapshots *  snapshot = new CSnapshots();
			std::string machine;
			machine=sMachines[i];

			_snapshots.push_back(snapshot);
			snapshot->Configure( config,
				sMachines[i], // machine name
				ipaddrs[i],
				"",
				_type); // ip address of agent
			group.create_thread(boost::bind(&CSnapshots::Cycle, snapshot));
			::Sleep(1000);
		}
		group.join_all();

	}
	catch(std::exception e)
	{
		AtlTrace("%s%s", "CEchoService::serviceThread() exception " , e.what());
	}
	catch(...)
	{
		AtlTrace("CEchoService::serviceThread() exception ");
	}
//	parser.Release();
	if(m_ThreadEndEvent != NULL)
		SetEvent(m_ThreadEndEvent);	
	//::CoUninitialize();
	AtlTrace("CEchoService::serviceThread() Exit");
}
