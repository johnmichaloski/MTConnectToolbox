//
// CrclAdapter.cpp
//

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

//#include "winsock2.h"
#include "stdafx.h"
#define BOOST_ALL_NO_LIB
#include <stdio.h>
#include <stdlib.h>
#include <share.h>
#include <fstream> 
#include <algorithm>

#include "CrclAdapter.h"
#include "CrclAgent.h"

#include "config.hpp"
#include "Device.hpp"
#include "Agent.hpp"
#include "ATLComTime.h"

#include "Globals.h"
#include "NIST/StdStringFcn.h"
#include "NIST/WinTricks.h"
#include "NIST/Logger.h"
#include "CrclInterface.h"
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
using namespace Nist;

static 		boost::asio::io_service  io_service;

HRESULT AdapterT::ErrorMessage(std::string errmsg) 
{
	AtlTrace(errmsg.c_str());
	logWarn(errmsg.c_str());
	if(Globals.Debug>0)
	{
//		EventLogger.LogEvent(errmsg);
	}
	return E_FAIL;
}
HRESULT AdapterT::DebugMessage(std::string errmsg) 
{
	AtlTrace(errmsg.c_str());
	logWarn(errmsg.c_str());
	if(Globals.Debug>3)
	{
//		EventLogger.LogEvent(errmsg);
	}
	return E_FAIL;
}

/////////////////////////////////////////////////////////////////////
static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg =  StrFormat("CrclAdapter In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode);
	OutputDebugString(errmsg.c_str() );
	throw std::exception(errmsg.c_str() , pExp->ExceptionRecord->ExceptionCode);
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
		LOG_ONCE( logWarn(" (%s) Could not find data item: %s  \n",  _device.c_str(), tag.c_str()));
	}

#endif
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
}
//============================================================
CrclAdapter::CrclAdapter(AgentConfigurationEx * mtcagent, // mtconnect agent
	//std::string machine,  // ip address or pc name
	std::string device) : 
AdapterT(mtcagent, "127.0.0.1", device), 
	xmlClient(io_service)
{
	_lenthconversion= 1.0;
	_jointconversion= 1.0;
	_orientconversion= 1.0;
}

void CrclAdapter::CreateItem(std::string tag, std::string type)
{	
	Item * item = new Item();
	item->_type=_T("Event");
	item->_tagname=tag;
	items.push_back(item);
}

void CrclAdapter::Dump()
{
	std::stringstream str;
	str << _device << ":QueryServer " << _nQueryServerPeriod << std::endl;
	str << _device << ":ServerRate " << _nServerRate << std::endl;
	OutputDebugString(str.str().c_str());
}
void CrclAdapter::Config()
{
	//GLogger.LogMessage(StrFormat("COpcAdapter::Config() for IP = %s\n",_device.c_str()));
	std::string cfgfile = Globals.inifile; 
	std::string sLastLogDate;
	Nist::Config config;
	try {
		if(GetFileAttributesA(cfgfile.c_str())== INVALID_FILE_ATTRIBUTES)
			throw std::exception("Could not find ini file \n");
		config.load( cfgfile );

		_nServerRate =config.GetSymbolValue<int>(_device + ".ServerRate", Globals.ServerRate );
		_nQueryServerPeriod =config.GetSymbolValue<int>(_device + ".QueryServer", Globals.QueryServer );
		_sIp =config.GetSymbolValue<std::string>(_device + ".Ip", "127.0.0.1" );
		_sPort =config.GetSymbolValue<std::string>(_device + ".Port", "64444" );

		_lenthconversion=config.GetSymbolValue<double>(_device + ".lengthconversion", 1.0);
		_jointconversion=config.GetSymbolValue<double>(_device + ".jointconversion", 1.0);
		_orientconversion=config.GetSymbolValue<double>(_device + ".orientconversion", 1.0);

		xmlClient.Init(_sIp, _sPort);
		jointnames	= config.GetTokens(_device + ".jointnames", ",");
	}
	catch(std::exception errmsg)
	{
		_agentconfig->AbortMsg(StrFormat("Could not find ini file for device %s\n", _device.c_str()));
	}
	catch(...)
	{
		_agentconfig->AbortMsg(StrFormat("Could not find ini file for device %s\n", _device.c_str()));
	}

	Dump();
}

void CrclAdapter::Cycle()
{

	Config();
	xercesc::XMLPlatformUtils::Initialize();

	_set_se_translator( trans_func );  // correct thread?
	SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);

	int nHeartbeat=0;
	CreateItem("heartbeat");
	CreateItem("last_update");
	CreateItem("power");
	CreateItem("avail");

	CreateItem("controllermode");
	CreateItem("execution");

	CreateItem("alarm");
	CreateItem("pose");
	for(size_t i=0; i<  jointnames.size(); i++)
	{
		CreateItem( StrFormat("%s_actpos", jointnames[i].c_str()));
	}

	Off();

	//GLogger.LogMessage(StrFormat("COpcAdapter::Cycle() Enter Loop for IP = %s\n",_device.c_str()));
	_mRunning=true;
	Off();

	while(_mRunning)
	{

		try {
			// Every cyccle - try to connect dont proceed till connected
			// No reason to http server is in another thread
			if(!xmlClient.IsConnected() )
			{
				xmlClient.Connect();
				while(!xmlClient.IsConnected() && _mRunning )
				{
					if(!xmlClient.IsWaitingToConnect())
						xmlClient.Connect();
					io_service.run_one();
					::Sleep(100);
				}
			}

			SetMTCTagValue("heartbeat", StrFormat("%d", nHeartbeat++ ));
			if(FAILED(GatherDeviceData()))
			{
				::Sleep(this->_nQueryServerPeriod);
				continue;
			}
			On();
			::Sleep(_nServerRate);
		}
		//catch (const xsd::cxx::tree::bits::xml_schema::exception& e)
		//{
		//	GLogger.LogMessage(StrFormat("COpcAdapter::Cycle() Exception %s\n",e.what()));
		//	Off();
		//}
		catch(std::exception e)
		{
			Off();
			Disconnect();
		}
		catch(...)
		{
			Off();
			Disconnect();
		}
	}
		xercesc::XMLPlatformUtils::Terminate ();
}


HRESULT CrclAdapter::FailWithMsg(HRESULT hr, std::string errmsg)
{
	this->DebugMessage(errmsg);
	return hr;
}

HRESULT CrclAdapter::WarnWithMsg(HRESULT hr, std::string errmsg)
{
	this->DebugMessage(errmsg);
	return S_OK;
}


HRESULT CrclAdapter::GatherDeviceData()
{
	USES_CONVERSION;
	HRESULT hr=S_OK;
	COleDateTime modtime;
	try 
	{
		COleDateTime now = COleDateTime::GetCurrentTime();//-COleDateTimeSpan(/* days, hours, mins, secs*/  0,0,0,0);

		// Check if connected - this could take too long if 
		this->SetMTCTagValue("last_update",StrFormat("%4d/%02d/%02d %02d:%02d:%02d",
			now.GetYear(), now.GetMonth(), now.GetDay(),
			now.GetHour(), now.GetMinute(), now.GetSecond())); // this call updates agent

		std::string statcmd = CrclInterface().CRCLGetStatusCmd();
		xmlClient.SyncWrite(statcmd);
		::Sleep(100);
		for(int i=0; i< 10 && (xmlClient.SizeMsgQueue() < 1); i++)
		{
			io_service.run_one(); //http://antonym.org/2009/05/threading-with-boost---part-i-creating-threads.html
			::Sleep(100);
		}
		// If nothing yet, give up
		if(xmlClient.SizeMsgQueue() < 1)
			return WarnWithMsg(E_FAIL, StrFormat("No status device %s\n",  _device.c_str())) ;

		std::string msg=xmlClient.LatestMsgQueue(); // This clears the queue and gives the latest entry
		//OutputDebugString(msg.c_str()); OutputDebugString("\n");
		if(xmlClient.FindLeadingElement(msg) =="</CRCLStatus>")
		{
			CrclInterface crcl;
			if(crcl.ParseCRCLStatusString(msg)<0)
				return  WarnWithMsg(E_FAIL, StrFormat("Failed to parse CRCL XML for device %s\n",  _device.c_str())) ;

			if(crcl._status.CommandState == "CRCL_Ready")
				items.SetTag("execution","idle");
			else if(crcl._status.CommandState == "CRCL_Working")
				items.SetTag("execution","running");
			else if(crcl._status.CommandState == "CRCL_Error")
				items.SetTag("execution","error");
			else 
				items.SetTag("execution","idle");
			// no paused

			items.SetTag("alarm",crcl._status.Alarm);

			items.SetTag("controllermode","manual");
			//items.SetTag("program","");

			

			items.SetTag("pose",StrFormat("%8.4f %8.4f %8.4f, %8.4f %8.4f %8.4f", 
				crcl._status.Point[0]*_lenthconversion,
				crcl._status.Point[1]*_lenthconversion,
				crcl._status.Point[2]*_lenthconversion,
				crcl._status.roll*_orientconversion,
				crcl._status.pitch*_orientconversion,
				crcl._status.yaw*_orientconversion));

			for(size_t i=0; i<  jointnames.size(); i++)
			{
				if(crcl._status.joint_pos.size() < i)
					continue;
				std::string tag = StrFormat("%s_actpos", jointnames[i].c_str());
				std::string val = StrFormat("%8.4f", crcl._status.joint_pos[i]*_jointconversion);
				items.SetTag(tag,val);
		}


			//items.SetTag("xorient",StrFormat("%8.4f %8.4f %8.4f", crcl._status.XAxis[0],crcl._status.XAxis[1],crcl._status.XAxis[2]));
			//items.SetTag("zorient",StrFormat("%8.4f %8.4f %8.4f", crcl._status.ZAxis[0],crcl._status.ZAxis[1],crcl._status.ZAxis[2]));
		}
		else
		{
			logWarn("Unknown XML status message from robot\n");
		}
		xmlClient.ClearMsgQueue(); // just get latest information
	
		for(int i = 0 ; i < items.size(); i++)
		{
			if(items[i]->_type==_T("Event") || items[i]->_type==_T("Sample"))
			{
				if(items[i]->_value != items[i]->_lastvalue)
				{
					this->SetMTCTagValue(items[i]->_tagname,items[i]->_value);
					items[i]->_lastvalue=items[i]->_value;
				}
			}
		}
	}
	catch(std::exception e)
	{
		logWarn("COpcAdapter Exception in %s - COpcAdapter::GatherDeviceData() %s\n",_device.c_str(), (LPCSTR)  e.what() );
		Off();
		Disconnect();
		hr= E_FAIL;
	}
	catch(...)
	{
		logWarn("COpcAdapter Exception in %s - COpcAdapter::GatherDeviceData()\n",_device.c_str() );
		Off();
		Disconnect();
		hr= E_FAIL;
	}
	return hr;
}