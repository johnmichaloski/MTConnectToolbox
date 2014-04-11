//
// DBLogger.h
//


#pragma once

#include <string>
#include "StdStringFcn.h"
#include <vector>
#include "DataDictionary.h"

inline void TESTHR(HRESULT x) {if FAILED(x) _com_issue_error(x);};


class CDBLogger 
{
public:
	CDBLogger(void);
	~CDBLogger(void);

	void AddRow(std::string  szLoggingDB, 
		std::vector<std::string> itemlist, 
		DataDictionary typedata,
		std::vector<std::string> values);
	
	HRESULT  CreateLogDB(std::string  sFilepath, 
		std::vector<std::string> items,
		DataDictionary typedata);
private:
	std::string m_szLoggingDB;
};
