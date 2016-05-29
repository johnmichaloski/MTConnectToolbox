//
// CmdHandler.cpp
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#include "StdAfx.h"
#include "CmdHandler.h"
#include "agent.hpp"
#include "MTCFanucAgent.h"
#include "iSeries.h"

#include "NIST/Logger.h"
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
							std::string ipaddr)
{
	_config=config; 
	_device =device;
	_ipaddr= ipaddr;

	mDeviceIP=Globals.FanucIpAddress;
	mDevicePort=Globals.FanucPort;

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

	while(_mRunning)
	{
		try {
			//SetMTCTagValue("heartbeat", StdStringFormat("%d",nHeartbeat++) );
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
		if (!mConnected)
		{
			_iSeries->connect();
			if (!mConnected)
			{
				SetMTCTagValue("power", "OFF" );
				return E_FAIL;
			}
		}
		if (mConnected)
		{	
			SetMTCTagValue("power", "ON" );
			_iSeries->getVersion();
			_iSeries->getInfo();
			_iSeries->getLine();
			_iSeries->getPositions();
			_iSeries->getStatus();
			_iSeries->getSpeeds();
			_iSeries->getAlarmInfo();
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
