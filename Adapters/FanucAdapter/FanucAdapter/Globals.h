////
//
//

#pragma once 

#include <vector>
#include <string>
#include "atlutil.h"


class CGlobals
{
public:
	std::string  FanucIpAddress;
	std::string FanucProtocol;
	int FanucPort;
	int FocasDelay;
	std::string FanucVersion;
	std::string szFanucExe;
	int AutoVersionDetect;
	std::string MachineToolConfig;
	//std::vector<std::string>  fanucips;
	std::vector<std::string>  configs;
	//std::vector<std::string>  fanucports;
	bool _bResetAtMidnight;
	std::string szServiceName;
	std::string szDeviceName;
	std::string ProgramLogic;
	int ShdrPort;
	int nAlarmFlag;
	int nAxesLoadFlag;
	int nToolingFlag;
	CGlobals() 
	{
#ifdef iSERIESLAN
		FanucPort=8193;
		FanucProtocol="LAN";
		FanucIpAddress="000.000.000.000";
		nAlarmFlag=nAxesLoadFlag=nToolingFlag=0;
#else
		ShdrPort=7878;
		FanucPort=0;
		FanucProtocol="HSSB";
		FanucIpAddress="000.000.000.000";
#endif
		FocasDelay=1000;
		AutoVersionDetect=0;
		ProgramLogic="FirstComment";
	}
};

__declspec(selectany)  CGlobals Globals;