//
// F15B.h
//

#pragma once
#include <string>
#pragma message("Compile  with 15b  - HSSB")
#include "Fwlib/150/fwlib32.h"

class FanucShdrAdapter;
#ifdef F15M
class CF15B
{
public:
	CF15B(FanucShdrAdapter* f);
	~CF15B(void);
	int getPositions();
	int getStatus();
	int getSpeeds();
	int getLine();
	int getInfo();
	int getVersion();
	int connect();
	void disconnect();
	void saveStateInfo();
	int LogErrorMessage(std::string msg, short errcode);
	FanucShdrAdapter * _adapter;
  std::string lastprognum;
  std::string lastFeed,lastX, lastY, lastZ, lastA, lastB;
  int mLag;

};


#include "F15B.cpp"
#endif