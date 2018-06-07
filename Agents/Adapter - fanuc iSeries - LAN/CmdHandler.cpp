//
// CmdHandler.cpp
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#include "StdAfx.h"
#include "CmdHandler.h"
#include "MTCFanucAdapter.h"
#include <sstream>

#include "agent.hpp"

#ifdef F15i
#include "FS15D.h"
#endif

#ifdef F15M	
#include "F15B.h"
#endif

#ifdef iSERIES
#include "iSeries.h"
#endif



#include "GLogger.h"
static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg =  StdStringFormat("COpcAdapter In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode);
	OutputDebugString(errmsg.c_str() );
	throw std::exception(errmsg.c_str() , pExp->ExceptionRecord->ExceptionCode);
} 


CCmdHandler::CCmdHandler(AgentConfigurationEx * config)
{
	_agentconfig=config;
#ifdef F15M	
	_F15M = new CF15B(this);
#endif
#ifdef F15i	
	_FS15D = new CFS15D(this);
#endif
#ifdef iSERIES	
	_iSeries = new CiSeries(this);
#endif
	mFlibhndl=0;
	mConnected=false;
}

CCmdHandler::~CCmdHandler(void)
{
}
void CCmdHandler::Configure(crp::Config	& config, 
							std::string device, 
							std::string ipaddr,
							std::string port
							)
{
	_config=config; 
	_device =device;
	_ipaddr= ipaddr;

	mDeviceIP=_ipaddr;
	mDevicePort=ConvertString<int>(port,8193);
	//mDeviceIP=Globals.FanucIpAddress;
	//mDevicePort=Globals.FanucPort;
	heartbeat=0;
	std::string sHeartbeat=StdStringFormat("%d",heartbeat++);

}
std::string CCmdHandler::GetMTCTagValue(std::string tag)
{
	if(_mtcValues.find(_device+"."+tag)!=_mtcValues.end())
		return _mtcValues[_device+"."+tag];
	return "UNAVAILABLE";
}
void CCmdHandler::SetMTCTagValue(std::string tag, std::string value)
{
	Agent * agent = _agentconfig->getAgent();
	Device *dev = agent->getDeviceByName(_device);
	DataItem *di = dev->getDeviceDataItem(tag);
	if (di != NULL)
	{
		std::string time = getCurrentTime(GMT_UV_SEC);
		agent->addToBuffer(di, value, time);
		_mtcValues[_device+"."+tag]=value;
	}
	//else
	//	sLogger << LINFO  <<  "(" << device << ") Could not find data item: " << tag << "\n";
}

void CCmdHandler::Cycle()
{
	GLogger << FATAL << "CCmdHandler::Cycle()" << std::endl; 
	boost::timer delaytimer;
	HRESULT hr;
	int minute;
	int nHeartbeat=0;
	_mRunning=true;

	hr = ::CoInitialize(NULL);
	_set_se_translator( trans_func );  // correct thread?

	::Sleep(2000);
	//boost::mutex::scoped_lock lock(_access);
	//this->_agentconfig->cond.wait(lock);


	SetMTCTagValue("avail", "AVAILABLE" );
	//if (!mConnected)
	//{
	//	_FS15D->connect();
	//	if (!mConnected)
	//	{
	//		_iSeries->getVersion();
	//	}
	//}

	while(_mRunning)
	{
		try {
			if(FAILED(gatherDeviceData()))
				::Sleep(10*1000);
			::Sleep(Globals.FocasDelay);
		}
		catch(std::exception e)
		{
			GLogger << INFO <<  "CCmdHandler::Cycle() exception " << e.what() <<std::endl;
		}
		catch(...)
		{
			GLogger << INFO <<  "CCmdHandler::Cycle() exception " <<std::endl;
		}
	}
	::CoUninitialize();
}

void CCmdHandler::AllOff()
{
	SetMTCTagValue("avail", "UNAVAILABLE" );
	SetMTCTagValue("power", "OFF" );
	SetMTCTagValue("block", "UNAVAILABLE" );
	SetMTCTagValue("program", "UNAVAILABLE" );
	SetMTCTagValue("path_feedratefrt", "UNAVAILABLE" );
	SetMTCTagValue("Xabs", "UNAVAILABLE" );
	SetMTCTagValue("Yabs", "UNAVAILABLE" );
	SetMTCTagValue("Zabs", "UNAVAILABLE" );
	SetMTCTagValue("Aabs", "UNAVAILABLE" );
	SetMTCTagValue("Babs", "UNAVAILABLE" );
	SetMTCTagValue("Cabs", "UNAVAILABLE" );
	SetMTCTagValue("Srpm", "UNAVAILABLE" );
	SetMTCTagValue("controllermode", "UNAVAILABLE" );
	SetMTCTagValue("execution", "UNAVAILABLE" );
	SetMTCTagValue("path_feedrateovr", "UNAVAILABLE" );
	SetMTCTagValue("Sovr", "UNAVAILABLE" );
}
void CCmdHandler::disconnect()
{

#ifdef F15i
	_FS15D->disconnect();
#endif
#ifdef F15M
	_F15M->disconnect();
#endif
#ifdef iSeries
	_iSeries->disconnect();
#endif

}

HRESULT CCmdHandler::gatherDeviceData()
{
	GLogger.LogMessage("FanucMTConnectAdapter::gatherDeviceData\n", HEAVYDEBUG);
	_set_se_translator( trans_func );  // correct thread?
	try {
#if 0
		if(!this->IsCNCRunning())
		{
			::Sleep(2000);
			return;

		}
#endif

#ifdef F15i
		if (!mConnected)
			_FS15D->connect();
		if (mConnected)
		{		
			//if(Globals.FanucVersion == "15i")
			{
				_FS15D->getVersion();
				_FS15D->getInfo();
				_FS15D->getLine();
				_FS15D->getPositions();
				_FS15D->getStatus();
				_FS15D->getSpeeds();
				_FS15D->getAlarmInfo();

			}
		}
#else
#ifdef F15M
		if (!mConnected)
			_F15M->connect();
		if (mConnected)
		{	
			//if(Globals.FanucVersion == "15M")
			{
				_F15M->saveStateInfo();
				_F15M->getVersion();
				_F15M->getInfo();
				_F15M->getLine();
				_F15M->getPositions();
				_F15M->getStatus();
				// hack in getSpeeds
				_F15M->getSpeeds();

			}
		}
#else
		sHeartbeat=StdStringFormat("%d",heartbeat++);
		SetMTCTagValue("heartbeat", sHeartbeat );

		if (!mConnected)
		{
			_iSeries->connect();
			if (!mConnected)
			{
				AllOff();
				return E_FAIL;
			}
			SetMTCTagValue("avail", "AVAILABLE" );
			//else
			//	_iSeries->getVersion();
		}
		if (mConnected)
		{	
			SetMTCTagValue("power", "ON" );
			try {
			_iSeries->saveStateInfo();
			if(FAILED(_iSeries->getInfo()))
				throw std::exception("getInfo fail");
			if(FAILED(_iSeries->getLine()))
				throw std::exception("getLine fail");
			if(FAILED(_iSeries->getPositions()))
				throw std::exception("getPositions fail");
			if(FAILED(_iSeries->getStatus()))
				throw std::exception("getStatus fail");
			if(FAILED(_iSeries->getSpeeds()))
				throw std::exception("getSpeeds fail");
			if(FAILED(_iSeries->getAlarmInfo()))
				throw std::exception("getAlarmInfo fail");
			}
			catch(std::exception errmsg)
			{
				std::stringstream tmp;
				tmp << "iSeries:: Failed " << errmsg.what();
				_iSeries->LogErrorMessage(tmp.str().c_str(),-1);
				disconnect();
				mConnected=false;
				AllOff();
				return E_FAIL;
			}
		}
#endif
#endif
	}
	catch(...)
	{
		GLogger.Warning("FanucMTConnectAdapter::gatherDeviceData Exception Raised\n");
		disconnect();
		return E_FAIL;

	}
	return S_OK;
}

std::string CCmdHandler::getProgramName(char * buf) 
{
	GLogger.LogMessage("FanucMTConnectAdapter::getProgramName Enter\n", 3);

	int prognum;
	std::string progname;
	std::stringstream ss(buf); 
	std::string sLine; 
	while(std::getline(ss, sLine, '\n'))
	{ 
		//  look for O # which must be there
		if(1 != sscanf(sLine.c_str(), "O%d", &prognum))
			continue;

		progname=StdStringFormat("O%d", prognum);
#if 0
		// if find comment inside (...) return as program name
		int n = sLine.find("(");
		int m= sLine.find(")");
		if(std::string::npos !=n && std::string::npos !=m)
			return sLine.substr(n+1,m-n-1);

		// else could be standalone  on next line
		if(std::getline(ss, sLine, '\n'))
		{
			n = sLine.find("(");
			m= sLine.find(")");
			if(std::string::npos !=n && std::string::npos !=m)
				return sLine.substr(n+1,m-n-1);

		}
#endif
		return progname;
	}
	GLogger.LogMessage("FanucMTConnectAdapter::getProgramName Leave\n", 3);

	return "";
}
