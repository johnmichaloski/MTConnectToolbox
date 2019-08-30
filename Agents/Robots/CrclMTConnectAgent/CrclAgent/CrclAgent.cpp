//
// CrclAgent.cpp : Defines the entry point for the console application.
//

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#ifdef _WINDOWS
#include "stdafx.h"
#pragma comment( user, "Compiled on " __DATE__ " at " __TIME__ ) 
#endif
#define BOOST_ALL_NO_LIB
#define MSVC

#include <string>
#include <exception>

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include "CrclAdapter.h"
#include "CrclAgent.h"
#include "Globals.h"

#include "NIST/StdStringFcn.h"
#include "NIST/File.h"
#include "DevicesXML.h"

// should not be included in header
//#include "jsmtconnect.txt"
#include "NIST/Logger.h"
#pragma comment(lib, "Ws2_32.lib")




int AgentConfigurationT::main(int aArgc, const char *aArgv[])
{
	logFatal("Main\n");
	SetCurrentDirectory(File.ExeDirectory().c_str()); // fixes Agent file lookup issue
	AgentConfiguration::main( aArgc, (const char **) aArgv);
	logFatal("Return Main\n");

	return 0;
}

// Start the server. This blocks until the server stops.
void AgentConfigurationEx::start()
{
	logFatal("Start\n");
	for(int i=0; i< mDevices.size(); i++)
	{
		CrclAdapter *  _cmdHandler = new CrclAdapter(this, mDevices[i]);
		_cncHandlers.push_back(_cmdHandler);
		mGroup.create_thread(boost::bind(&CrclAdapter::Cycle, _cncHandlers[i]));
	}

	AgentConfigurationT::start(); // does not return
}
void AgentConfigurationEx::stop()
{
	for(int i=0; i< _cncHandlers.size(); i++)
		_cncHandlers[i]->Stop();

	mGroup.join_all();

	AgentConfigurationT::stop();
}

void AgentConfigurationEx::initialize(int aArgc, const char *aArgv[])
{
	logFatal("initialize\n");
	Nist::Config config;
	std::string cfgfile = Globals.inifile; 

	if(GetFileAttributesA(cfgfile.c_str())!= INVALID_FILE_ATTRIBUTES)
	{
		config.load( cfgfile );
		Globals.mServerName=config.GetSymbolValue<std::string>("GLOBALS.ServiceName", Globals.mServerName);
		MTConnectService::setName(Globals.mServerName);
		Globals.QueryServer =config.GetSymbolValue<int>("GLOBALS.QueryServer", 10000);
		Globals.ServerRate  =config.GetSymbolValue<int>("GLOBALS.ServerRate", 2000);


		//std::string sLevel = config.GetSymbolValue("GLOBALS.logging_level", "FATAL").c_str();
		//sLevel=MakeUpper(sLevel);
		//Globals.Debug  = (sLevel=="FATAL")? 0 : (sLevel=="ERROR") ? 1 : (sLevel=="WARN") ? 2 : (sLevel=="INFO")? 3 : 5;
		Globals.Debug = config.GetSymbolValue<int>("GLOBALS.Debug", "0");
		mDevices = config.GetTokens("GLOBALS.MTConnectDevice", ",");
		Globals.mHttpPort = config.GetSymbolValue<std::string>("GLOBALS.HttpPort", "5000");
		Globals.ResetAtMidnight = config.GetSymbolValue<int>("GLOBALS.ResetAtMidnight", "0");
		GLogger.OutputConsole()= config.GetSymbolValue<int>("GLOBALS.OutputConsole", "0");

		Globals.CfgFile = File.ExeDirectory() + config.GetSymbolValue<std::string>("GLOBALS.CfgFile", "Agent.cfg");
		Globals.DevicesFile  = File.ExeDirectory() +  config.GetSymbolValue<std::string>("GLOBALS.DevicesFile", "Devices.xml");

	}
	else
	{
		AbortMsg("Could not find ini file \n");
		return;
	}

	//WriteDevicesFile(_devices,Globals.DevicesFile, "");
	//WriteAgentCfgFile(Globals.CfgFile,  File.Filename(Globals.DevicesFile) , "");
	//Globals.Dump();

	// if ( Globals.msNewConfig == "NEW" )
	{
		for ( size_t i = 0; i < mDevices.size( ); i++ )
		{
			mDevices[i] = Trim(mDevices[i]);
		}
		std::vector<std::string> keys = mDevices;
		std::string devicemodel;

		for ( size_t i = 0; i < keys.size( ); i++ )
		{
			mTagnames[keys[i]] = std::vector<std::string>( );
			std::vector<std::string> jointnames
				= config.GetTokens(mDevices[i] + ".jointnames", ",");
			std::string robotdevicemodel = CDevicesXML::ConfigureRobotDeviceXml(
				keys[i], jointnames, mTagnames[keys[i]], mConditionTags[keys[i]]);
			ReplaceAll(robotdevicemodel, "####", keys[i]);
			robotdevicemodel = ReplaceOnce(robotdevicemodel, "name=\"NNNNNN\"",
				"name=\"" + keys[i] + "\"");
			devicemodel += robotdevicemodel;
		}
		CDevicesXML::WriteDevicesFileHeader(devicemodel, File.ExeDirectory( ) + "Devices.xml");
		MSVC::WritePrivateProfileString("GLOBALS", "Config", "UPDATED",
			( File.ExeDirectory( ) + "Config.ini" ).c_str( ));
	}

	AgentConfigurationT::initialize(aArgc, aArgv);
}

