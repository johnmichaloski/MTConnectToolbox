// MTConnectAgentFromShdr.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <exception>
#include <vector>
#include <tchar.h>
#include <algorithm>

#include "NIST/StdStringFcn.h"
#include "NIST/File.h"
#include "NIST/Config.h"

#include "atlutil.h"
#include "ATLComTime.h"

#include "agent.hpp"
#include "config.hpp"
#include <boost/assign/list_of.hpp>

#include "DeviceXml.h"
#include "YamlReader.h"

#include "NIST\ResetAtMidnightThread.h"
#include "NIST\Config.h"
#include "NIST\Logger.h"
#include "NIST\WinSingleton.h"

#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")



static const char INI_FILENAME[] = "agent.cfg";

static void ErrMessage(std::string errmsg)
{
	logError(errmsg.c_str());
}
static std::string SanitizeDeviceName (std::string name)
{
	std::replace( name.begin(), name.end(), ' ', '_');
	return name;
}

extern SERVICE_STATUS          gSvcStatus; 
extern VOID ReportSvcStatus( DWORD dwCurrentState, //    The current state (see SERVICE_STATUS)
	DWORD dwWin32ExitCode, //   The system error code
	DWORD dwWaitHint); //    Estimated time for pending operation, 


static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	logError("AgentConfigurationEx::In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode );
#ifdef DEBUG
	DebugBreak();
#endif
	throw std::exception();
}

class AgentConfigurationEx :
	public AgentConfiguration
	, public CResetAtMidnightThread<AgentConfigurationEx>
	, public MTConnectSingleton<AgentConfigurationEx>
{
public:
	AgentConfigurationEx()
	{
	}


	virtual void start()
	{
		logStatus( "MTConnect Agent Service Start %s\n" , COleDateTime::GetCurrentTime().Format() );
		try{

			if(bResetAtMidnight)
				CResetAtMidnightThread<AgentConfigurationEx>::Start();

			//Handles Win32 exceptions (C structured exceptions) as C++ typed exceptions
			_set_se_translator( trans_func ); 

			// Start the server. This blocks until the server stops.
			AgentConfiguration::start();

		}
		catch(std::exception e)
		{
			logFatal("AgentConfigurationEx::start() failed - %s\n", e.what());
			throw e;
		}
		catch(...)
		{
			ReportSvcStatus(SERVICE_STOP, 0, 0);
		}

	}

	virtual void stop()
	{
		int nWaitTime = 5000;
		ReportSvcStatus(SERVICE_STOP_PENDING, 0, 15000);
		StopAll();
		ReportSvcStatus(SERVICE_STOP, 0, 0);
	}

	void StopAll()
	{
		try {
			ErrMessage(StdStringFormat( "MTConnect Agent Service Stopped %s\n", COleDateTime::GetCurrentTime().Format() ).c_str());

			if(CResetAtMidnightThread<AgentConfigurationEx>::bResetAtMidnight)
				CResetAtMidnightThread<AgentConfigurationEx>::Stop();

			AgentConfiguration::stop();

		}
		catch(...)
		{
			ErrMessage(StdStringFormat( "MTConnect Agent Service Stop Aborted %s\n", COleDateTime::GetCurrentTime().Format() ).c_str());
		}

	}
	////////////////////////////////////////////////////////////////////
	COleDateTime now ;
	Nist::Config cfg;
};


#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
//#pragma warning(disable: 4247) //warning C4297: 'WinMain' : function assumed not to throw an exception but does

//std::string Trim(std::string str)
//{
//	str.erase(str.find_last_not_of(" \n\r\t")+1);
//	str.erase(0,str.find_first_not_of(" \n\r\t"));
//	return str;
//}


int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	try 
	{	
		AgentConfigurationEx config;
		//config.bTestReset=true;  // turn this on for 2 minute resets

		// Set up local logging to debug.txt. 
		GLogger.Open(File.ExeDirectory() + "debug.txt");
		GLogger.DebugLevel()=5;
		GLogger.Timestamping()=true;
		ErrMessage(StdStringFormat( "MTConnect Agent Service Entered %s\n" , nowtimestamp().c_str()));

		/**
		* This code reads the tagname and enumeration remappings. They are substituted into static variables
		* so that all adapters use the substitutions.
		*/
		Nist::Config cfg;
		std::string sNewConfig;
		std::string HttpPort;
		std::string ServiceName;
		if(cfg.LoadFile(File.ExeDirectory() + "Config.ini"))
		{
			sNewConfig = cfg.GetSymbolValue<std::string>("GLOBALS.Config", "OLD");
			std::transform(sNewConfig.begin(), sNewConfig.end(), sNewConfig.begin(), ::toupper);
			HttpPort =  cfg.GetSymbolValue<std::string>("GLOBALS.HttpPort", "5000");
			ServiceName = cfg.GetSymbolValue<std::string>("GLOBALS.ServiceName", "MTConnectAgent");

			config.bResetAtMidnight = cfg.GetSymbolValue<int>("GLOBALS.ResetAtMidnight", "0");
			config.bSingleton= cfg.GetSymbolValue<int>("GLOBALS.Singleton", "0");
			if(config.bSingleton)
				config.KillAllOtherInstances();

			GLogger.DebugLevel() = cfg.GetSymbolValue<int>("GLOBALS.Debug", "0");
			Adapter::nLogUpdates = cfg.GetSymbolValue<int>("GLOBALS.LogUpdates", "0");
			bAbortHeartbeat = cfg.GetSymbolValue<int>("GLOBALS.AbortHeartbeat", "0");
			Adapter::rpmEntries= cfg.GetTokens<std::string>("GLOBALS.RPMTAGS", ",");
			Adapter::bAllFakeSpindle = cfg.GetSymbolValue<int>("GLOBALS.AllFakeSpindle", "0");

			if ( sNewConfig == "NEW" )
			{
				// This specifies how agent.cfg and devices.xml will be laid out if new configuration is specified
				std::vector<std::string>_devices = cfg.GetTokens<std::string>("GLOBALS.MTConnectDevice", ",");
				std::vector<std::string>_types = cfg.GetTokens<std::string>("GLOBALS.DeviceType", ",");
				std::vector<std::string>_ips = cfg.GetTokens<std::string>("GLOBALS.Ip", ",");
				std::vector<std::string>_ports = cfg.GetTokens<std::string>("GLOBALS.Port", ",");
				std::vector<std::string> fakespindle_types = cfg.GetTokens<std::string>("GLOBALS.FakeSpindleTypes", ",");

				// Handles agent.cfg and devices.xml file generation
				CDeviceXml agent_cfg;

				try
				{
					// if types.size()==1 push back same entry till size matches
					if(_devices.size() != _types.size() ||
						_ips.size() != _types.size() ||
						_ports.size() != _types.size() )
						throw StdStringFormat(" Mismatched ini parameter csv list sizes" );

					for ( size_t i = 0; i < _devices.size( ); i++ )
					{
						// std::for_each(v.begin(), v.end(),  boost::bind(&boost::trim<std::string>,_1, std::locale() ));
						_devices[i]=SanitizeDeviceName(Trim(_devices[i] ));
						_types[i]=Trim(_types[i] );
						_ips[i]=Trim(_ips[i] );
						_ports[i]=Trim(_ports[i] );

						// check if type needs fake spindle
						if(std::find(fakespindle_types.begin(), fakespindle_types.end(), _types[i])!=fakespindle_types.end())
							Adapter::bDeviceFakeSpindle[_devices[i]]=1;
						else
							Adapter::bDeviceFakeSpindle[_devices[i]]=0;

					}

					std::string devicesXML = agent_cfg.WriteDevicesFileXML(_devices,_types);
					if(devicesXML.empty())
						throw StdStringFormat(" Empty Devices.xml" );

					WriteFile(File.ExeDirectory() + "Devices.xml"  , devicesXML);
					agent_cfg.WriteAgentCfgFile("Agent.cfg", "Devices.xml", File.ExeDirectory(), HttpPort,
						_devices,  _types, _ips,  _ports, ServiceName);

					// now add tag remapping and enum mapping from device.ini file
					cfg.DeleteSection("TAGRENAMES");
					cfg.DeleteSection("ENUMREMAPPING");
					for(size_t j=0; j< _types.size(); j++)
					{
						Nist::Config ini;
						ini.LoadFile(File.ExeDirectory() + StdStringFormat("Devices/%s.ini", _types[j].c_str()));
						std::map<std::string, std::string> renames=ini.GetMap("TAGRENAMES");
						cfg.MergeKeys("TAGRENAMES", renames);
						std::map<std::string, std::string> enums=ini.GetMap("ENUMREMAPPING");
						cfg.MergeKeys("ENUMREMAPPING", enums);
					}
					// Save updated configuration ini file
					std::vector<std::string> order=boost::assign::list_of("GLOBALS")( "TAGRENAMES")("ENUMREMAPPING");
					cfg.Save(order);			

					// Reread ini file since it has been modified.
					cfg.Clear();
					cfg.LoadFile(File.ExeDirectory() + "Config.ini");

					// Everything went ok so reset ini file flag
#ifndef DEBUG
					WritePrivateProfileString("GLOBALS", "Config", "UPDATED", ( File.ExeDirectory( ) + "Config.ini" ).c_str( ) );
#endif
				}
				catch(std::string errmsg)
				{
					WritePrivateProfileString("GLOBALS", "Config", ("ERROR-"+errmsg).c_str(), ( File.ExeDirectory( ) + "Config.ini" ).c_str( ) );
				}
			}


			// Now we will read ini file for tag renames and enum mappings
			std::map<std::string, std::string> tagrenames = cfg.GetMap("TAGRENAMES"); //TAGRENAMES is a [section]
			std::map<std::string, std::string> enumrenames = cfg.GetMap("ENUMREMAPPING");

			for(std::map<std::string, std::string>::iterator it= tagrenames.begin(); it!= tagrenames.end(); it++)
			{
				Adapter::keymapping.insert(std::make_pair<std::string, std::string>( (*it).first, (*it).second));
			}
			for(std::map<std::string, std::string>::iterator it= enumrenames.begin(); it!= enumrenames.end(); it++)
			{
				Adapter::enummapping.insert(std::make_pair<std::string, std::string>( (*it).first, (*it).second));
			}




		}


#ifdef _WINDOWS
		// MICHALOSKI ADDED
		TCHAR buf[1000];
		GetModuleFileName(NULL, buf, 1000);
		std::string path(buf);
		path=path.substr( 0, path.find_last_of( '\\' ) +1 );
		SetCurrentDirectory(path.c_str());
#endif	

		ErrMessage(StdStringFormat( "MTConnect Agent Service Started %s\n" , nowtimestamp().c_str() ).c_str());
		if(FAILED(::CoInitialize(NULL)))
			throw  std::exception("CoInitialize failed\n");

		HRESULT hr = ::CoInitializeSecurity( NULL, //Points to security descriptor 
			-1, //Count of entries in asAuthSvc 
			NULL, //Array of names to register 
			NULL, //Reserved for future use 
			RPC_C_AUTHN_LEVEL_NONE, //The default authentication //level for proxies 
			RPC_C_IMP_LEVEL_IDENTIFY, //The default impersonation //level for proxies 
			NULL, //Reserved; must be set to  NULL
			EOAC_NONE, //Additional client or //server-side capabilities 
			NULL //Reserved for future use 
			);  

		if(FAILED(hr))
			throw std::exception("CoInitializeSecurity failed\n");
		SetCurrentDirectory(File.ExeDirectory().c_str());

		//std::cout.rdbuf(&DebugLogger);
		//std::cerr.rdbuf(&DebugLogger);
		ErrMessage(StdStringFormat( "Enter config.main %s\n" , nowtimestamp().c_str() ).c_str());
		config.main( __argc,(const char **) __argv );

	}
	catch(std::exception e)
	{
		ErrMessage(e.what());
	}
	catch(...)
	{
		ErrMessage("Service terminated abnormally in main\n");
	}
	::CoUninitialize();
	ErrMessage(StdStringFormat( "MTConnect Agent Service Ended %s\n" , nowtimestamp().c_str()).c_str());
	return -1;
}
