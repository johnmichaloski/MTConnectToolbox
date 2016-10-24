//
// iSeries.h
//

#pragma once
#include <string>

#ifdef iSERIESLAN
#pragma message("Compile Iseries with Fwlib/e1  - Lan")
#include "Fwlib/e1/fwlib32.h"

#elif defined(iSERIESHSSB)
#ifdef i160
#pragma message("Compile Iseries with 160i  - HSSB")
#include "Fwlib/160/fwlib32.h"
#else
#pragma message("Compile Iseries with 30i  - HSSB")
#include "Fwlib/30i/fwlib32.h"
#endif
#else
#pragma message("No Iseries to compile")
#error  No Iseries to compile
#endif

class CCmdHandler;
#ifdef iSERIES
class CiSeries
{
public:
	CiSeries(FanucShdrAdapter*);
	~CiSeries(void);
	int getStatus();
	int getSpeeds();
	int getPositions();
	int getLine();
	int getInfo();
	int getVersion();
	int getAlarmInfo();
	int getToolInfo();
	int getLoads();
	int LogErrorMessage(std::string msg, short errcode);
	int connect();
	void disconnect();
	FanucShdrAdapter* _adapter;
	int nBlockNum;
	std::string lastprognum;
	std::string lastFeed,lastX, lastY, lastZ, lastA, lastB;
	int mLag;
	void saveStateInfo();
};


#include "iSeries.cpp"
#endif