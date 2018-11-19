// **************************************************************************
// OpcAdapter.cpp
//
// Description:
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.
// **************************************************************************



#include "stdafx.h"
#include <time.h>
#include <fstream>


#include "OpcAdapter.h"
#include "atlcomtime.h"
#include <atlpath.h>
//#include "DCOMUtils.h"
#include "MTCSiemensAgent.h"

#include <exception>
#include "config.hpp"
#include "Device.hpp"
#include "Agent.hpp"

//#include "MtcOpcAdapter.hpp"
#include "Logger.h"
#include "VarTypes.h"

#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")



static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg =  StdStringFormat("COpcAdapter In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode);
	OutputDebugString(errmsg.c_str() );
	throw std::exception(errmsg.c_str() , pExp->ExceptionRecord->ExceptionCode);
} 
////////////////////////////////////////////////////////////////

COpcAdapter::COpcAdapter(AgentConfigurationEx * mtcagent, 
	Config& config,
	std::string machine, 
	std::string device, 
	std::string inisection) : 

_agentconfig(mtcagent), 
	_device(device),
	_config(config),
	_sTagSectionName(inisection)
{
	_opcservermachine=machine;
	_mDelay=2000;
}

COpcAdapter::~COpcAdapter()
{
}

void COpcAdapter::SetMTCTagValue(std::string tag, std::string value)
{
#if 1
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
		LOGONCE LogMessage(StdStringFormat("(%s) Could not find data item: %s  \n",  _device.c_str(), tag.c_str()), DBUG);
	}

#endif
}
void COpcAdapter::ErrorMessage(std::string errmsg) 
{
	std::string msg = (errmsg.rfind("\n") != std::string::npos) ? errmsg.substr(0,errmsg.size()-1) : errmsg;
	LogMessage(StdStringFormat("ERROR : %s\n", msg.c_str()), LOWERROR);
#ifdef _DEBUG
	OutputDebugString(errmsg.c_str());
#endif 
}
void COpcAdapter::DebugMessage(std::string errmsg) 
{
	std::string msg = (errmsg.rfind("\n") != std::string::npos) ? errmsg.substr(0,errmsg.size()-1) : errmsg;
	LogMessage(StdStringFormat("WARNING: %s\n", msg.c_str()), WARNING);
	//OutputDebugString(errmsg.c_str());
}
std::string COpcAdapter::GetSymbolValue(std::string section, std::string tag, const char * defaultval) 
{
	return _config.GetSymbolValue(section+"."+tag, defaultval).str();
}
std::vector<std::string> COpcAdapter::GetSectionKeys(std::string section)
{
	return _config.getkeys(section);
}

void COpcAdapter::Off() 
{ 	
	for (int i =0; i<opcitems.size(); i++) 
	{
		SetMTCTagValue(opcitems[i]->_alias, "UNAVAILABLE");
	}
	// changed from unavailable. Causes agent to make everything unavailable.
	SetMTCTagValue("avail", "AVAILABLE");
	SetMTCTagValue("power", "OFF");
}
void COpcAdapter::On() 
{ 			 
	for (int i =0; i<opcitems.size(); i++) 
	{
		opcitems[i]->_value.vt=VT_NULL;
		opcitems[i]->_value=_variant_t();
	}
	SetMTCTagValue("avail", "AVAILABLE");
	SetMTCTagValue("power", "ON");
}

int COpcAdapter::ReadOPCSettings()
{
	try 
	{
		// Read OPC Server details
		COPCBaseClass::ReadOPCSettings("");
		_nFakeSpindle= _config.GetSymbolValue("OPCSERVER.FakeSpindle", "0").toNumber<long>();
		_nAlarmsEnabled= _config.GetSymbolValue("OPCSERVER.AlarmsEnabled", "0").toNumber<long>();
		_nMultiSpindle= _config.GetSymbolValue("OPCSERVER.MultiSpindle", "1").toNumber<long>();


		if(_sTagSectionName.empty())
			throw std::exception(_T("Error: OPCSERVER Tag name Undefined\n"));
		if(_opcservermachine.empty())
			throw std::exception(_T("Error: [OPCSERVER] Opc Server Machine (IP) Undefined\n"));

		_sClsid= GetSymbolValue(_sTagSectionName, "CLSID", "");
		_sClsid = "{" +_sClsid + "}";
		if(FAILED(CLSIDFromString(_bstr_t(_sClsid.c_str()) , &_gOpcServerClsid)))
			throw std::exception("Misformed or mistyped OPC Server Class ID\n");
		_gszOPCServerName= GetSymbolValue(_sTagSectionName, "SERVERNAME", "");
		_gszCncControllerProcessName= GetSymbolValue(_sTagSectionName, "CNCProcessName", "");
		_CNCTaskListCmd	= GetSymbolValue(_sTagSectionName, "CNCTaskListCmd", ""); // assume no remote task list command


		_gServerProgId= GetSymbolValue(_sTagSectionName, "ProgId", ""); // assume no remote task list command

		LogMessage(StdStringFormat("CONFIGURATION\n================================\n"), DBUG);
		LogMessage(StdStringFormat("Device                   = %s           \n", _sTagSectionName.c_str()), DBUG);
		LogMessage(StdStringFormat("Fake Spindle             = %d   \n", _nFakeSpindle), DBUG);
		LogMessage(StdStringFormat("Alarms Enabled           = %d  NO EFFECT\n", _nAlarmsEnabled), DBUG);
		LogMessage(StdStringFormat("Mutliple Spindles        = %d NO EFFECT\n", _nMultiSpindle), DBUG);
		LogMessage(StdStringFormat("CNCProcessName           = %s\n", _gszCncControllerProcessName.c_str()), DBUG);
		LogMessage(StdStringFormat("OPCServerName            = %s\n", _gszOPCServerName.c_str()), DBUG);
		LogMessage(StdStringFormat("CLSID                    = %s\n", _sClsid.c_str()), DBUG);
		LogMessage(StdStringFormat("AutoConnect              = %d\n", _bAutoConnect), DBUG);
		LogMessage(StdStringFormat("AutoReconnectMaxAttempts = %d\n", _nMaxAutoReconnects), DBUG);
		LogMessage(StdStringFormat("QueryServerPeriod        = %d ms (to connect)\n", _nQueryServerPeriod), DBUG);
		LogMessage(StdStringFormat("ProcessPriority          = %x\n", _nProcessPriority), DBUG);
		LogMessage(StdStringFormat("SynchronousUpdate        = %d\n", _bSynchronousUpdate), DBUG);
		LogMessage(StdStringFormat("ServerRate               = %d ms (when connected)\n", _nOPCServerRate), DBUG);
		LogMessage(StdStringFormat("PingTimeout              = %d ms\n", _nPingTimeout), DBUG);
		LogMessage(StdStringFormat("Agent Http Port          = %d ms\n", Globals.HttpPort), DBUG);

		LogMessage(Globals.Dump().c_str(), DBUG);
		LogMessage(StdStringFormat("================================\n"), DBUG);
				

		// Now parse the tag information into MTConnect information
		std::vector<std::string> tags = GetSectionKeys(_sTagSectionName);
		if(tags.size() < 1)
			throw std::exception(StdStringFormat("Error: No [%s] Section\n", _sTagSectionName.c_str()).c_str());
		for(int i=0; i<tags.size(); i++)
		{
			VarTypes vt;

			// make sure its is an OPC Tag
			if(tags[i].find(_T("Tag.")) == 0)
			{
				OpcItem * item = new OpcItem();
				item->_alias=tags[i];
				item->_alias=ReplaceOnce(item->_alias, _T("Tag."), _T(""));
				item->vt=VT_BSTR;
				if(item->_alias.find(_T("Enum."))==0)
				{
					item->_alias=ReplaceOnce(item->_alias, _T("Enum."), _T(""));
					item->_subtype="enumeration";
				}
				else if(item->_alias.find(_T("Const."))==0)
				{
					item->_alias=ReplaceOnce(item->_alias,_T("Const."), _T(""));
					item->vt= VT_USERDEFINED;
				}
				else if(item->_alias.find(_T("Prog."))==0)
				{
					item->_alias=ReplaceOnce(item->_alias,_T("Prog."), _T(""));
					item->vt= VT_USERDEFINED;
					item->_subtype="program";
				}

				if(item->_alias.find(_T("Sample."))==0)
				{
					item->_alias=ReplaceOnce(item->_alias,_T("Sample."), _T(""));
					item->_type=_T("Sample");
					//_mtcagent->AddSample(this->_device, item->_alias);
				}
				else if(item->_alias.find(_T("Event."))==0)
				{
					item->_alias=ReplaceOnce(item->_alias,_T("Event."), _T(""));
					item->_type=_T("Event");
					//_mtcagent->AddEvent(this->_device, item->_alias);
				}
				else if(item->_alias.find(_T("OPC."))==0)  // reads opc values does not send to MT Connect
				{
					item->_alias=ReplaceOnce(item->_alias, _T("OPC."), _T(""));  // OPC value for scripting
					item->_type=_T("OPC");
				}
				else if(item->_alias.find(_T("Condition."))==0)
				{
					item->_alias=ReplaceOnce(item->_alias,_T("Condition."), _T(""));
					item->_type="Condition";
					//_mtcagent->AddCondition(this->_device, item->_alias);
				}
				std::string tmp= GetSymbolValue(_sTagSectionName, tags[i],"");
				item->_tagname=tmp;
				opcitems.push_back(item);
			}

		}
		// Test to see if we have a power, if not add one.
		//if(opcitems.FindItem("power")==NULL)
		//	_mtcagent->AddEvent(this->_device, "power");

#ifdef ALARMS
		//Alarm Management
		_alarmconfig.load(ExeDirectory() + "English\\Alarms.cfg");
#endif
	}
	catch(std::string errmsg)
	{
		_gszStartupErrors+=errmsg;
		ErrorMessage( errmsg + "\n");
		return E_FAIL;

	}
	catch(std::exception e)
	{
		ErrorMessage( e.what());
		return E_FAIL;
	}
	catch(...)
	{
		ErrorMessage( "ReadOPCSettings() OPC Items Ini error \n");
		return E_FAIL;

	}
	LogMessage(StdStringFormat("COpcAdapter::ReadOPCSettings() Done for Device = %s \n",_device.c_str()), DBUG);

	return S_OK;
}

void COpcAdapter::Cycle()
{
	LogMessage(StdStringFormat("COpcAdapter::Cycle() for IP = %s\n",_device.c_str()), DBUG);
	HRESULT hr;
	int nHeartbeat=0;

	_set_se_translator( trans_func );  // correct thread?
	CoInitialize(NULL);

	hr = ::CoInitializeSecurity( NULL, //Points to security descriptor 
		-1, //Count of entries in asAuthSvc 
		NULL, //Array of names to register 
		NULL, //Reserved for future use 
		RPC_C_AUTHN_LEVEL_NONE, //The default authentication //level for proxies 
		RPC_C_IMP_LEVEL_IDENTIFY, //The default impersonation //level for proxies 
		NULL, //Reserved; must be set to  NULL
		EOAC_NONE, //Additional client or //server-side capabilities 
		NULL //Reserved for future use 
		);  
	_set_se_translator( trans_func );  // doesn't seem required.
	SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);

	ReadOPCSettings();
	if(_opcservermachine=="127.0.0.1"|| (stricmp(_opcservermachine.c_str(),"localhost")==0))
		_opcservermachine.clear();

	::Sleep(1000);
	//boost::mutex::scoped_lock lock(_access);
	//this->_agentconfig->cond.wait(lock);


	_mRunning=true;

	LogMessage(StdStringFormat("COpcAdapter::Cycle() Enter Loop for IP = %s\n",_device.c_str()), DBUG);
	while(_mRunning)
	{
		try {
			if(FAILED(GatherDeviceData()))
				::Sleep(10*1000);
			::Sleep(_mDelay);
			//SetMTCTagValue("heartbeat", StdStringFormat("%d",nHeartbeat++) );
			SetMTCTagValue("avail", "AVAILABLE" );
		}
		catch(std::exception e)
		{
			LogMessage(StdStringFormat("COpcAdapter::Cycle() exception - %s \n", e.what()), INFO);
		}
		catch(...)
		{
			LogMessage(StdStringFormat("COpcAdapter::Cycle() exception  \n"), INFO);
		}
	}
	::CoUninitialize();
}
HRESULT COpcAdapter::GatherDeviceData()
{
	USES_CONVERSION;
	HRESULT hr=S_OK;
	ULONG dwInterval = _nOPCServerRate;
	try 
	{
		// Check if connected - this could take too long if 
		if(!IsConnected() && _bAutoConnect)
		{
			LogMessage(StdStringFormat("COpcAdapter  %s attempt Connect at %s\n",_opcservermachine.c_str(), (LPCSTR) COleDateTime::GetCurrentTime().Format() ),INFO);
			Off();
			if(FAILED(Connect()))
			{
				throw std::exception("Attempt Connect FAILED\n");
				return E_FAIL;
			}
			LogMessage(StdStringFormat("COpcAdapter  %s Connected at %s\n",_opcservermachine.c_str(), (LPCSTR) COleDateTime::GetCurrentTime().Format() ),INFO);
		}

		if(IsConnected())
		{
			//if(FAILED(GetStatusString(_status)))
			//	throw std::exception("GetStatusString failed \n");

			// OPC Server can be working, but adding items can fail
			if(FAILED(UpdateGroup()))
			{
				throw std::exception("UpdateGroup failed \n");
				return E_FAIL;
			}

			On();
		}
	}
	catch(std::exception e)
	{
		LogMessage(StdStringFormat("COpcAdapter Exception in %s - COpcAdapter::GatherDeviceData() %s\n",_device.c_str(), (LPCSTR)  e.what() ),LOWERROR);
		Off();
		dwInterval=_nQueryServerPeriod;
		Disconnect();
		hr= E_FAIL;
	}
	catch(...)
	{
		LogMessage(StdStringFormat("COpcAdapter Exception in %s - COpcAdapter::GatherDeviceData()\n",_device.c_str() ),LOWERROR);
		Off();
		dwInterval=_nQueryServerPeriod;
		Disconnect();
		hr= E_FAIL;
	}
	return hr;
}


void COpcAdapter::OnValueChanged(ItemList & _ignore)
{

	// RPM - Update spindle based on Srpm1, Srpm2 data
	if(_nMultiSpindle)
	{
		int srpm = 0;
		int srpm1 = opcitems.GetValue<int>("Srpm1", -1);
		int srpm2 = opcitems.GetValue<int>("Srpm2", -1);

		if (srpm1 > 0)  srpm=srpm1;
		if (srpm2 >0 && srpm2 > srpm1) srpm  = srpm2;

		if(srpm> 0)
			opcitems.SetItemValue("Srpm", (long) srpm);
	}

	if(_nFakeSpindle)
	{
		std::string sMode = opcitems.GetValue("controllermode", "");
		std::string sExecution = opcitems.GetValue("execution", "");
		// If moving and in auto mode, assume spindle on
		if(	(sMode == "AUTOMATIC" ) &&
			(sExecution ==  "EXECUTING" )&&
			( 
			lastFeed!=opcitems.GetValue("path_feedratefrt", "") ||
			lastX!=opcitems.GetValue("Xabs", "") ||
			lastY!= opcitems.GetValue("Yabs", "") ||
			lastZ!=opcitems.GetValue("Zabs", "")
			)
			)
			mLag=4; // 3 cycles - 3 seconds;
		else
			mLag--;

		if(mLag<0) mLag=0;

		if(mLag>0)
		{
			opcitems.SetItemValue("Srpm","99.0");
		}
		else
		{
			opcitems.SetItemValue("Srpm","0");
		}

		lastFeed= opcitems.GetValue("path_feedratefrt", "");
		lastX= opcitems.GetValue("Xabs", "");
		lastY= opcitems.GetValue("Yabs", "");
		lastZ= opcitems.GetValue("Zabs", "");
	}
#if 0
	if( _nAlarmsEnabled)
	{
		// Simpler ALARM handling
		std::string alarmLevel;
		std::string alarmSeverity;
		std::string sAlarmMessage;
		std::string sNativeCode;
		std::string sQualifier;
		long lNativeCode = opcitems.GetValue<long>(_T("alarmNative"),-1); 

		opcitems.SetItemValue("alarm", (BSTR) _bstr_t(""));
		if(lNativeCode > 1000 )	 
		{

			//  840D Code whether alarm pending.
			//  0 = no alarm in this channel
			//	1 = alarm without stop
			//	2 = alarm with stop

			//  severity - CRITICAL, ERROR, WARNING, or INFORMATION.
			// state Either INSTANT, ACTIVE or CLEARED.
			static char * sSeverities[3] = {_T("NONE"), _T("WARNING"), _T("CRITICAL")};
			static char * sLevel[3] = {_T("NORMAL"), _T("WARNING") , _T("FAULT")};
			alarmLevel= (lNativeCode <3)? sLevel[lNativeCode] : _T("UNAVAILABLE");
			alarmSeverity= (lNativeCode <3)? sSeverities[lNativeCode] : _T("CRITICAL");	

			sNativeCode = opcitems.GetValue(_T("alarmNative") , ""); 

			sAlarmMessage = "None";
			if(lNativeCode>0)
				sAlarmMessage = _alarmconfig.GetSymbolValue("Alarms."+sNativeCode).str();

			for(int j=0; lNativeCode>0 && j<4; j++)
			{
				std::string  fillText=opcitems.GetValue(StdStringFormat("alarmField%d", j+1), "");
				if(fillText.empty())
					continue;
				if(fillText[0]==_T('S') || fillText[0]==_T('K'))
					fillText=fillText.substr(1);
				sAlarmMessage=ReplaceOnce(sAlarmMessage,StdStringFormat("%d%%",j+1), fillText);
			}
			opcitems.SetItemValue("alarm", (BSTR) _bstr_t(sAlarmMessage.c_str()));
		}
	}
#endif
	for (int i =0; i<opcitems.size(); i++) 
	{

		if(opcitems[i]->_type==_T("OPC") || opcitems[i]->_type==_T("Data") )
		{
			continue; // don't update MTConnect - opc value only
		}

		if(opcitems[i]->_value==opcitems[i]->_lastvalue)
			continue;

		std::string alias =  opcitems[i]->_alias;
		std::string value =  opcitems[i]->GetValue();

		if(opcitems[i]->_subtype=="enumeration")
		{
			std::string szEnumTag="Enum." + alias + "." + value;
			//szEnumTag.Format(_T("Enum.%s.%s"), alias, value);
			value = GetSymbolValue(_sTagSectionName, szEnumTag, (LPCSTR) "UNAVAILABLE");

			// Use default if available
			if(value == "UNAVAILABLE")
			{
				szEnumTag="Enum." + alias + ".-1";
				value = GetSymbolValue(_sTagSectionName, szEnumTag, (LPCSTR) "UNAVAILABLE");
			}
		}
		// Condition Doesn't work as advertised.

		/*
		HOW TO MAP ALARM TO CONDITION
		1.	Level – This must be normal, warning, fault, or unavailable (case is not important). If this value is not one of the previous values, unavailable will be assumed.
		2.	Native Code – This is the number or short name of the alarm or warning passed through from the device.
		3.	Native Severity – The severity of the alarm as passed through from the device.
		4.	Qualifier  – This is a modifier which indicates which end of the range the condition applies or qualifies the condition. 
		Examples are: HIGH, LOW, etc…
		5.	Text – The text describing the condition. This text is usually taken from the data source and passed through.

		2010-09-29T23:59:33.460470Z|htemp|WARNING|HTEMP|1|HIGH|Oil Temperature High

		<DataItem category="CONDITION" id="htemp" type="TEMPERATURE"/>


		<Warning dataItemId="htemp" timestamp="2010-09-29T23:59:33.460470Z" sequence="399237840" type="TEMPERATURE" nativeSeverity="1" nativeCode="HTEMP" qualifier="HIGH" >Oil Temperature High</Warning>

		If there is no message, as is usually the case when the operation is Normal, leave all the fields blank, but still provide the pipe delimiter for proper parsing as follows:
		2010-09-29T23:59:33.460470Z|htemp|NORMAL||||

		Expected data to parse in SDHR format:
		*   Time|Condition|Level|NativeCode|Native Severity|Qualifier|Description
		*   Time|Item|Value
		*   Time|Item1|Value1|Item2|Value2...

		*/
		/*
		1.	Level – This must be normal, warning, fault, or unavailable (case is not important). If this value is not one of the previous values, unavailable will be assumed.
		2.	Native Code – This is the number or short name of the alarm or warning passed through from the device.
		3.	Native Severity – The severity of the alarm as passed through from the device.
		4.	Qualifier  – This is a modifier which indicates which end of the range the condition applies or qualifies the condition. 
		Examples are: HIGH, LOW, etc…
		5.	Text – The text describing the condition. This text is usually taken from the data source and passed through.
		*/

		_CriticalSection.Lock();
#if 1
		if(opcitems[i]->_type==_T("Event"))
		{
			if(_nFakeSpindle && stricmp( opcitems[i]->_alias.c_str(),"Srpm")==0)
				continue;
			SetMTCTagValue( opcitems[i]->_alias, value);

		}
		else if( opcitems[i]->_type==_T("Sample"))
		{
			if(_nFakeSpindle && stricmp( opcitems[i]->_alias.c_str(),"Srpm")==0)
				continue;
			SetMTCTagValue( opcitems[i]->_alias, value);
		}
		else if( opcitems[i]->_type==_T("Condition"))
		{
#ifdef ALARMS
			//if(lNativeCode>0)
			//	_agentconfig->SetCondition(this->_device, opcitems[i]->_alias, 
			//	alarmLevel,
			//	sNativeCode,
			//	alarmSeverity,
			//	sQualifier,
			//	sAlarmMessage
			//	);
#endif
		}
#endif
		_CriticalSection.Unlock();

	}
}

#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
	DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
	unsigned long x;
	unsigned short s1;
	unsigned short s2;
	unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
	const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IOPCShutdown,0xF31DFDE1,0x07B6,0x11d2,0xB2,0xD8,0x00,0x60,0x08,0x3B,0xA1,0xFB);


MIDL_DEFINE_GUID(IID, IID_IOPCCommon,0xF31DFDE2,0x07B6,0x11d2,0xB2,0xD8,0x00,0x60,0x08,0x3B,0xA1,0xFB);


MIDL_DEFINE_GUID(IID, IID_IOPCServerList,0x13486D50,0x4821,0x11D2,0xA4,0x94,0x3C,0xB3,0x06,0xC1,0x00,0x00);


MIDL_DEFINE_GUID(IID, LIBID_OPCCOMN,0xB28EEDB1,0xAC6F,0x11d1,0x84,0xD5,0x00,0x60,0x8C,0xB8,0xA7,0xE9);


const IID IID_IOPCServer = {0x39c13a4d,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCServerPublicGroups = {0x39c13a4e,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCBrowseServerAddressSpace = {0x39c13a4f,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCGroupStateMgt = {0x39c13a50,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCPublicGroupStateMgt = {0x39c13a51,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCSyncIO = {0x39c13a52,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCAsyncIO = {0x39c13a53,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCItemMgt = {0x39c13a54,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IEnumOPCItemAttributes = {0x39c13a55,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCDataCallback = {0x39c13a70,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCAsyncIO2 = {0x39c13a71,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID IID_IOPCItemProperties = {0x39c13a72,0x011e,0x11d0,{0x96,0x75,0x00,0x20,0xaf,0xd8,0xad,0xb3}};


const IID LIBID_OPCDA = {0xB28EEDB2,0xAC6F,0x11d1,{0x84,0xD5,0x00,0x60,0x8C,0xB8,0xA7,0xE9}};


#undef MIDL_DEFINE_GUID

