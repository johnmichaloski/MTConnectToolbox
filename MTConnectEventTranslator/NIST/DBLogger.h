//
// DBLogger.h
//


#pragma once

#include <string>
#include "StdStringFcn.h"
#include <vector>
#include "DataDictionary.h"


#import "c:\Program Files\Common Files\system\ado\msadox.dll" \
    no_namespace
#import "C:\Program Files\Common Files\System\ADO\msado15.dll" \
     rename("EOF", "EndOfFile")


inline void TESTHR(HRESULT x) {if FAILED(x) _com_issue_error(x);};


class CDBLogger 
{
public:
	CDBLogger(void);
	~CDBLogger(void);
	bool DBExists(std::string sFilepath);

	void AddRow(std::string  szLoggingDB, 
		std::string table,
		std::vector<std::string> itemlist, 
		DataDictionary typedata,
		std::vector<std::string> values);

	HRESULT  CreateLogDB(std::string  sFilepath, 
		std::vector<std::string> items,
		DataDictionary typedata);
	
	void CDBLogger::CreateTable(std::string szLoggingDB, 
		std::string szTablename,
		std::vector<std::string> itemlist, 
		DataDictionary typedata) ;

	void insert(std::string szLoggingDB, 
		std::string table, 
		std::vector<std::string> &fields, 
		DataDictionary &values,
		DataDictionary typedata);

	_CatalogPtr createdb(std::string dbname, std::string user, std::string password);
	void deleteTable(std::string schema, std::string table);
	void open(std::string dbname, std::string user, std::string password);
	void createTable(std::string schema, std::string table, std::vector<std::string> &fields, DataDictionary &typedata);
	void CDBLogger::close();
	void createSchema(std::string schema){}
	int sizeTable(std::string schema, std::string table) { return 0; }
private:
	std::string m_szLoggingDB;
		ADODB::_ConnectionPtr  _pConnection;

};
