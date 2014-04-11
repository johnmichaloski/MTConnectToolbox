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
	std::string HttpPort;
	std::string ServerName;
	std::map<std::string, int> programs;
	std::vector<std::string> machineprograms;
	std::vector<std::string> setupprograms;
	std::vector<std::string> inspectionprograms;
	std::map<std::string, int> yield;
	std::map<std::string, int> workorder;
	std::map<std::string,std::string> devices;
	int ResetAtMidnight;
	int & DbgConsole;
	CGlobals() : Debug(GLogger.DebugLevel()), 
		DbgConsole(GLogger.OutputConsole())
	{
		ServerRate=1000;

		inifile=File.ExeDirectory() + "Config.ini";	
		CfgFile=File.ExeDirectory() + "Afgent.cfg";	
		DevicesFile=File.ExeDirectory() + "Devices.xml";	

		GLogger.Timestamp()=true;
		Debug=0;
		QueryServer=10000;
		HttpPort="5001";
		ServerName="MTConnectCmdSimAgent";
		ResetAtMidnight=0;
	
	}
	void Dump()
	{
		std::stringstream str;
		str << "Globals ServerName " << ServerName << std::endl;
		str << "Globals HttpPort " << HttpPort << std::endl;
		str << "Globals Debug " << Debug << std::endl;
		str << "Globals QueryServer " << QueryServer << std::endl;
		str << "Globals ServerRate " << ServerRate << std::endl;
		str << "Globals ResetAtMidnight " << ResetAtMidnight << std::endl;
		str << "Globals Debug View " << DbgConsole << std::endl;
		str << "Globals Inifile " << inifile << std::endl;
		str << "Globals Cfgfile " << CfgFile << std::endl;
		str << "Globals Devicesfile " << DevicesFile << std::endl;
		OutputDebugString(str.str().c_str());
	}


};

__declspec(selectany)  CGlobals Globals;

