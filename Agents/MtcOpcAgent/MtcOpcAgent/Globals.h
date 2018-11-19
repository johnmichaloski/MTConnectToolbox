//
// Globals.h
//

#pragma once
#include <vector>
#include <map>
#include "StdStringFcn.h"
#include "Logger.h"
#include <stdio.h>
#include "File.h"

class CGlobals
{
public:
	int &Debug;
	int QueryServer;
	int ServerRate;
	std::string inifile;
	std::string CfgFile;
	std::string DevicesFile;
	int HttpPort;
	std::string ServerName;
	int ResetAtMidnight;
	int & DbgConsole;
	int WriteNewDevicesFile;
		
	int Delay;
	std::string Version;
	std::string szExe;
	int AutoVersionDetect;
	std::string MachineToolConfig;
	CGlobals() : Debug(GLogger.DebugLevel()), 
		DbgConsole(GLogger.OutputConsole())
	{
		ServerRate=2000;

		inifile=File.ExeDirectory() + "Config.ini";	
		CfgFile=File.ExeDirectory() + "Afgent.cfg";	
		DevicesFile=File.ExeDirectory() + "Devices.xml";	

		GLogger.Timestamp()=true;
		Debug=0;
		QueryServer=10000;
		HttpPort=5000;
		ServerName="MTCOPCAgent";
		ResetAtMidnight=0;
		Delay=1000;
		AutoVersionDetect=0;	
	}
	std::string Dump()
	{
		std::stringstream str;
		str << std::endl;
		str << "Globals ServiceName     = " << ServerName << std::endl;
		str << "Globals HttpPort        = " << HttpPort << std::endl;
		str << "Globals Debug           = " << Debug << std::endl;
		str << "Globals QueryServer     = " << QueryServer << std::endl;
		str << "Globals ServerRate      = " << ServerRate << std::endl;
		str << "Globals ResetAtMidnight = " << ResetAtMidnight << std::endl;
		str << "Globals Debug View      = " << DbgConsole << std::endl;
		str << "Globals Inifile         = " << inifile << std::endl;
		str << "Globals Cfgfile         = " << CfgFile << std::endl;
		str << "Globals Devicesfile     = " << DevicesFile << std::endl;
		str << "Globals ResetAtMidnight = " << ResetAtMidnight << std::endl;
		return str.str();
	}


};

__declspec(selectany)  CGlobals Globals;

