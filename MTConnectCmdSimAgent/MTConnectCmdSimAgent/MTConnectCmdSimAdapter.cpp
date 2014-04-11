//
//
//
#include "stdafx.h"
#define BOOST_ALL_NO_LIB

#include "MTConnectCmdSimAdapter.h"
#include "MTConnectCmdSimAgent.h"

//MTConnect
#include "config.hpp"
#include "Device.hpp"
#include "Agent.hpp"

// System
#include "ATLComTime.h"
#include <fstream> // ifstream
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <share.h>
#include  <math.h>

// NIST
#include "NIST/Logger.h"
#include "NIST/StdStringFcn.h"
#include "Globals.h"
#include "NIST/Config.h"

static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg =  StdStringFormat("COpcAdapter In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode);
	GLogger.Warning(errmsg.c_str() );
	throw MTConnectCmdSimAdapter::ParameterError("", errmsg);
} 
/////////////////////////////////////////////////////////////////////
HRESULT AdapterT::ErrorMessage(std::string errmsg) 
{
	AtlTrace(errmsg.c_str());
	GLogger.LogMessage(errmsg, LOWERROR);
	if(Globals.Debug>0)
	{
		//		EventLogger.LogEvent(errmsg);
	}
	return E_FAIL;
}
HRESULT AdapterT::DebugMessage(std::string errmsg) 
{
	AtlTrace(errmsg.c_str());
	GLogger.Warning(errmsg);
	if(Globals.Debug>3)
	{
		//		EventLogger.LogEvent(errmsg);
	}
	return E_FAIL;
}

AdapterT::AdapterT(AgentConfigurationEx * mtcagent, // mtconnect agent
	std::string machine,  // ip address or pc name
	std::string device) : 

_agentconfig(mtcagent), 
	_device(device)
{
	inifile=Globals.inifile.c_str();
	_nServerRate=1000;
	_nQueryServerPeriod=10000;
}


AdapterT::~AdapterT(void)
{
}


void AdapterT::SetMTCTagValue(std::string tag, std::string value)
{
#if 1
	items.SetTag(tag, value);


	Agent * agent = _agentconfig->getAgent();
	Device *pDev = agent->getDeviceByName(_device);
	DataItem *di = pDev->getDeviceDataItem(tag);
	if (di != NULL)
	{
		std::string time = getCurrentTime(GMT_UV_SEC);
		agent->addToBuffer(di, value, time);
	}
	else
	{
		LOGONCE GLogger.LogMessage(StdStringFormat(" (%s) Could not find data item: %s  \n",  _device.c_str(), tag.c_str()));
	}

#endif
}

void AdapterT::SetMTCConditionValue(std::string tag, 
	std::string alarmLevel,
	std::string sNativeCode,
	std::string alarmSeverity,
	std::string sQualifier,
	std::string sAlarmMessage)
{
	Agent * agent = _agentconfig->getAgent();
	Device *dev = agent->getDeviceByName(_device);
	DataItem *di = dev->getDeviceDataItem(tag);
	//// Conditon data: LEVEL|NATIVE_CODE|NATIVE_SEVERITY|QUALIFIER
	std::string szCondValue =  alarmLevel + /*normal/alarm*/
		"|" + sNativeCode +
		"|" + alarmSeverity+
		"|" + sQualifier+
		"|" + sAlarmMessage;
	std::string time = getCurrentTime(GMT_UV_SEC);

	items.SetTag(tag, szCondValue);

	agent->addToBuffer(di, szCondValue, time);
}
void AdapterT::Off() 
{ 	
	for (int i=0; i<items.size(); i++) 
	{
		items[i]->_value="UNAVAILABLE";
		items[i]->_lastvalue="UNAVAILABLE";
		SetMTCTagValue(items[i]->_tagname, "UNAVAILABLE");
	}
	// changed from unavailable. Causes agent to make everything unavailable.
	SetMTCTagValue("avail", "UNAVAILABLE");
	SetMTCTagValue("power", "OFF");
}


void AdapterT::On() 
{ 			 
	SetMTCTagValue("avail", "AVAILABLE");
	SetMTCTagValue("power", "ON");
	SetMTCTagValue("path_feedrateovr", "100");		
	SetMTCTagValue("Sovr", "100");
	SetMTCTagValue("controllermode", "AUTOMATIC" );

	SetMTCConditionValue("system_cond", "NORMAL", "0", "0","HIGH", "DAY TRIPPING");
	SetMTCConditionValue("comms_cond", "NORMAL", "0", "0","HIGH", "DAY TRIPPING");
	SetMTCConditionValue("electric_temp", "NORMAL", "0", "0","HIGH", "DAY TRIPPING");
	SetMTCConditionValue("comms_cond", "NORMAL", "0", "0","HIGH", "DAY TRIPPING");
	SetMTCConditionValue("system_cond", "NORMAL", "0", "0","HIGH", "DAY TRIPPING");
	

}

void MTConnectCmdSimAdapter::CreateItem(std::string tag, std::string type)
{	
	Item * item = new Item();
	item->_type=_T("Event");
	item->_tagname=tag;
	items.push_back(item);
}


void MTConnectCmdSimAdapter::Configure()
{
	//GLogger.LogMessage(StdStringFormat("COpcAdapter::Config() for IP = %s\n",_device.c_str()));
	NIST::Config config;
	std::string cfgfile = Globals.inifile; 
	std::string sLastLogDate;
	try {
		if(GetFileAttributesA(cfgfile.c_str())== INVALID_FILE_ATTRIBUTES)
			throw std::exception("Could not find ini file \n");
		config.load( cfgfile );
		
		// this is the URL and device specifying the command agent
		_ipaddr=config.GetSymbolValue(_device + ".URL", "12.0.0.1:5000/current").c_str();

		_nServerRate =config.GetSymbolValue(_device + ".ServerRate", Globals.ServerRate ).toNumber<int>();
		_nQueryServerPeriod =config.GetSymbolValue(_device + ".QueryServer", Globals.QueryServer ).toNumber<int>();
		_sFaultInfo =config.GetSymbolValue(_device + ".FAULT", "triangular,90,95,100" );
		_ttfdist.SetParameters((std::string) _sFaultInfo);
		_sFaultInfo =config.GetSymbolValue(_device + ".REPAIR", "triangular,20,30,40" );
		_ttrdist.SetParameters((std::string) _sFaultInfo);
		_sFaultInfo =config.GetSymbolValue(_device + ".RESPONSE", "uniform,10,20" );
		_ttrresponse.SetParameters((std::string) _sFaultInfo);


//		waitFault=_ttfdist.RandomVariable();

	}
	catch(std::exception errmsg)
	{
		_agentconfig->AbortMsg(StdStringFormat("Error ini file for device %s\n", _device.c_str()));
	}
	catch(...)
	{
		_agentconfig->AbortMsg(StdStringFormat("Error ini file for device %s\n", _device.c_str()));
	}

}
HRESULT MTConnectCmdSimAdapter::FailWithMsg(HRESULT hr, std::string errmsg)
{
	this->DebugMessage(errmsg);
	return hr;
}
HRESULT MTConnectCmdSimAdapter::WarnWithMsg(HRESULT hr, std::string errmsg)
{
	this->DebugMessage(errmsg);
	return S_OK;
}

int MTConnectCmdSimAdapter::ProgramDuration(std::string program)
{		
	std::map<std::string, int>::iterator it;
	if((it=Globals.programs.find(program))!=Globals.programs.end())
	{
		return (*it).second;
	}
	int n = ConvertString<int>(items.GetTag("estimatedCompletion", "1"),1);
	n= (n==0)?  1 : n;

	TCHAR  infoBuf[32767];

	// Get and display the name of the computer. 
	DWORD bufCharCount = 32767;
	GetComputerName( infoBuf, &bufCharCount ) ;

	if( stricmp(infoBuf, "mountaineer") ==0)
		return 1;
	if( stricmp(infoBuf, "grandfloria") ==0)
		return 1;
	return n;
}

HRESULT MTConnectCmdSimAdapter::FaultHandling()
{		
	*currentDecrementer=(*currentDecrementer)--;

	if(	waitRepair<0)
	{
		//if(this->_device=="CNC1_RESOURCE")
		//	DebugBreak();
		this->currentDecrementer=&waitFault;
		waitRepair=0;
		waitFault=_ttfdist.RandomVariable();
		nLastFaultNum=nFaultNum;
		command=cmdstack.top();
		cmdstack.pop();
		SetMTCConditionValue("system_cond", "NORMAL", "0", "0","HIGH", "DAY TRIPPING");
		SetMTCTagValue("execution", "ACTIVE" );
		return waitFault;
	}
	else if(waitFault<0 )
	{
		//if(this->_device=="CNC1_RESOURCE")
		//	DebugBreak();
		this->currentDecrementer=&waitRepair;
		waitRepair=_ttrdist.RandomVariable();
		waitFault=0;
		cmdstack.push(command);
		nFaultNum++;
		nLastFaultNum=nFaultNum;
		SetMTCConditionValue("system_cond", "FAULT", "0", "0","HIGH", "DAY TRIPPING");
		SetMTCTagValue("execution", "STOPPED" );

	}
	return 0;
}
void MTConnectCmdSimAdapter::Cycle()
{
	CoInitialize(NULL);

	Configure();

	_set_se_translator( trans_func );  // correct thread?
	SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);

	int nHeartbeat=0;
	nLastCmdNum=0;
	CreateItem("heartbeat");
	CreateItem("program");
	CreateItem("controllermode");
	CreateItem("execution");
	CreateItem("power");
	CreateItem("avail");
	CreateItem("alarm");
	CreateItem("path_feedrateovr");
	CreateItem("Srpm");
	CreateItem("Sovr");
	CreateItem("cmd_echo");
	CreateItem("cmdnum_echo");
	CreateItem("partid");
	CreateItem("partcount");
	CreateItem("partcountgood");
	CreateItem("partcountbad");
	CreateItem("estimatedCompletion");

	::Sleep(1000);
	//GLogger.LogMessage(StdStringFormat("COpcAdapter::Cycle() Enter Loop for IP = %s\n",_device.c_str()));
	_mRunning=true;
	Off();
	On();

	SetMTCTagValue("partcount", "0");
	SetMTCTagValue("partcountgood", "0");
	SetMTCTagValue("partcountbad", "0");

	while(_mRunning)
	{
		try {
			// PING IP?
			SetMTCTagValue("heartbeat", StdStringFormat("%d", nHeartbeat++ ));

			if(FAILED(GatherDeviceData()))
			{
				::Sleep(_nServerRate);
				//::Sleep(this->_nQueryServerPeriod);
				continue;
			}
			::Sleep(_nServerRate);
		}
		catch(std::exception e)
		{
			GLogger.Warning(StdStringFormat("MTConnectCmdSimAdapter Unhandled std::exception in %s - Cycle()\n",_device.c_str()));
			
		}
		catch(...)
		{
			GLogger.Warning(StdStringFormat("MTConnectCmdSimAdapter Unhandled ... in %s - Cycle()\n",_device.c_str()));
		}
	}
	CoUninitialize();
}

void AdapterT::SaveCmdData(DataDictionary  cycledata)
{
	for(DataDictionary::iterator it=cycledata.begin(); it!=cycledata.end(); it++)
	{
		items.SetTag((*it).first, (*it).second);
	}
}
HRESULT MTConnectCmdSimAdapter::GatherDeviceData()
{
	USES_CONVERSION;
	HRESULT hr=S_OK;
	std::vector<DataDictionary>  cycledatum ;
	try 
	{
		// Read Command Agent values
		cycledatum = _parser.ReadStream(_ipaddr);

		// Detect change event
		if(cycledatum.size() == 0)
		{
			return FailWithMsg(S_OK, StdStringFormat("No stream data for device %s\n",   _device.c_str())) ;
		}

		command = cycledatum[0]["command"];
		nCmdNum = ConvertString<int>(cycledatum[0]["cmdnum"],nLastCmdNum);

		//	SelectiveMerge(std::vector<std::string> itemlist, DataDictionary & itemstomerge)
		SetMTCTagValue("cmd_echo", cycledatum[0]["command"] );
		SetMTCTagValue("cmdnum_echo", cycledatum[0]["cmdnum"]);
		SetMTCTagValue("program", cycledatum[0]["program"]);
		SetMTCTagValue("partid", cycledatum[0]["partid"]);
		
//		FaultHandling();

		if(command=="RESET" && nLastCmdNum!=nCmdNum) //  execution== STOPPED
		{
			//AtlTrace("Device - %s Command = %s ComandNum %d:%d\n",_device.c_str(), command.c_str(), nCmdNum ,nLastCmdNum );
			SetMTCTagValue("execution", "READY" );
		}
		else if(command=="RUN" && nLastCmdNum!=nCmdNum)
		{
			//AtlTrace("Device - %s Command = %s ComandNum %d:%d\n",_device.c_str(), command.c_str(), nCmdNum ,nLastCmdNum );
			// Dont go into fault if already faulted
			if(items.GetTag("execution", "PAUSED") == "STOPPED")
				return S_OK;
			SaveCmdData(cycledatum[0]);
			_nTMsTimeStart =  NIST::Millitime::GetMilliCount();
			SetMTCTagValue("execution", "ACTIVE" );
			waitProgram= ProgramDuration(cycledatum[0]["program"]);
		}
		else
		{
			//GLogger.Warning(StdStringFormat("MTConnectCmdSimAdapter Unhandled if #1 in %s - GatherDeviceData()\n",_device.c_str()));

		}
		SaveCmdData(cycledatum[0]);

		// waitFault>0 means waiting for next fault
		if(waitFault>0 && items.GetTag("execution", "PAUSED") == "ACTIVE")
		{
			SetMTCTagValue("Srpm", "99");
		}
		
		if(waitProgram >0 && items.GetTag("execution", "PAUSED") == "ACTIVE")
		{
 			waitProgram--;
			_nMsTimerElapsed = ((double)NIST::Millitime::GetMilliSpan( _nTMsTimeStart ))/1000.0;
			SetMTCTagValue("programTime", NIST::Time.HrMinSecFormat( _nMsTimerElapsed) );
			
		}
		else if(waitProgram <= 0  && items.GetTag("execution", "PAUSED") == "ACTIVE" )
		{
			int nPartCount = ConvertString<int>(items.GetTag("partcount", "0"),0);
			SetMTCTagValue("programTime", "0");
			SetMTCTagValue( "partcount", StdStringFormat("%d",++nPartCount)  );
			if(findStringIC(items.GetTag("program", ""),"inspect"))
			{
				int ival = (rand() % 100) ;
				bool bGood = ival < 50;
				if(bGood) 
				{
					int nPartCount = ConvertString<int>(items.GetTag("partcountgood", "0"),0);
					SetMTCTagValue( "partcountgood", StdStringFormat("%d",++nPartCount)  );
				}
				else

				{
					int nPartCount = ConvertString<int>(items.GetTag("partcountbad", "0"),0);
					SetMTCTagValue( "partcountbad", StdStringFormat("%d",++nPartCount)  );
				}
			}
			SetMTCTagValue("execution", "READY" );

		}
		else
		{
			GLogger.Warning(StdStringFormat("MTConnectCmdSimAdapter Unhandled if #2 in %s - GatherDeviceData()\n",_device.c_str()));

		}

		nLastCmdNum=nCmdNum;

	}
	catch(MTConnectCmdSimAdapter::ParameterError pe)
	{
		GLogger.Warning(StdStringFormat("MTConnectCmdSimAdapter Exception in %s - COpcAdapter::GatherDeviceData()\n",_device.c_str()));
		return E_FAIL;

	}
	catch(std::exception e)
	{
		GLogger.Warning(StdStringFormat("COpcAdapter Exception in %s - COpcAdapter::GatherDeviceData() %s\n",_device.c_str(), (LPCSTR)  e.what() ));
		//Disconnect();  // Only disconnect when file says  CONNECTED=False
		return E_FAIL;
	}
	catch(...)
	{
		GLogger.Warning(StdStringFormat("COpcAdapter Exception in %s - COpcAdapter::GatherDeviceData()\n",_device.c_str() ));
		//Disconnect();  // Only disconnect when file says  CONNECTED=False
		return E_FAIL;
	}
	return hr;
}

void  MTConnectCmdSimAdapter::Disconnect()
{
	Off();  
}
