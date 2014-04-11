//
// Globals.h
//

#pragma once
#include <string>

#include "StdStringFcn.h"

class CGlobals
{
public:

	CGlobals(void)
	{
		DELAY=5000;
		Speedup=1;
		dbConnection =  "ConsolidatedData";

	}

	unsigned int DELAY;
	unsigned int  Speedup;
	std::string dbConnection;

};

__declspec(selectany)  CGlobals Globals;