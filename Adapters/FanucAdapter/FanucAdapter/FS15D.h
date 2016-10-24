//
// FS15D.h
//

// If you make an application for Series 15i, you must specify the compiling option, which is "FS15D".
#pragma once
#include <string>

#pragma message("Compile Iseries with 15i  - HSSB")
#include "Fwlib/15i/fwlib32.h"

class CCmdHandler;
#ifdef F15i
class CFS15D
{
public:
	CFS15D(CCmdHandler* f);
	~CFS15D(void);
	int getPositions();
	int getStatus();
	int getSpeeds();
	int getLine();
	int getInfo();
	int getVersion();
	int getAlarmInfo();
	int LogErrorMessage(std::string msg, short errcode);
	int connect();
	void disconnect();
	CCmdHandler* _adapter;
	int nBlockNum,nLastBlockNum;
	std::string lastprognum;
};


#include "FS15D.cpp"
#endif