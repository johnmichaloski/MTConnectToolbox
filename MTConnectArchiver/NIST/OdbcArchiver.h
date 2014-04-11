//
// OdbcArchiver.h
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#pragma once

#include <string>
#include <map>
#include <vector>

#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>

#include "DataDictionary.h"
struct ColDescription
{
	SQLSMALLINT colNumber;
	SQLCHAR colName[80];
	SQLSMALLINT nameLen;
	SQLSMALLINT dataType;
	SQLULEN colSize;
	SQLSMALLINT decimalDigits;
	SQLSMALLINT nullable;
};

class OdbcArchiver
{
public:
	OdbcArchiver();
	~OdbcArchiver();
	void open(std::string dbname, std::string user, std::string password);
	void close();

	void createTable(std::string schema, std::string table, std::vector<std::string> &fields, DataDictionary &typedata);
	void useSchema(std::string schema);
	void createSchema(std::string schema);
	void deleteTable(std::string schema, std::string table);
	void insert(std::string schema, std::string table, std::vector<std::string> &fields, DataDictionary& values, DataDictionary typedata);
	void insert(std::string schema, std::string table, std::vector<std::string> &fields, std::vector<std::string> &values, DataDictionary typedata);
	int sizeTable(std::string schema, std::string table);
	std::vector<std::vector<std::string> > selectTable(std::string schema, std::string table, std::vector<std::string> &fields, DataDictionary &typedata);
	std::vector<std::vector<std::string> > selectTableWhere(std::string schema, std::string table,std::string where, std::vector<std::string> &fields, DataDictionary &typedata);

	//void insert(std::string table, std::string fields, DataDictionary typedata, std::vector<DataDictionary>  datum);
	//void select(std::string table, std::vector<std::string> &fields, std::vector<DataDictionary> & datum);
	//std::vector<std::string>  header();
	//std::string  header(std::string separator=",");
protected:
	//std::string GenerateSchema(std::string schema) ;
	//std::string  GenerateTableData(std::string schema) ;
	SQLHANDLE _hConn ;
	SQLHANDLE _hEnv ;
	SQLHANDLE _hStmt ;
	std::string SanitizeColumnName(std::string);
	HRESULT OpenSqlDatabase(std::string dbname="CMSD");
	void LoadCMSDFromSql();
	std::string GetData(SQLHSTMT  hStmt, int j);
	HRESULT Check( SQLRETURN rc, char * msg, bool throwFailedExpection=true, bool printSucceededMsg=false  );
	void status( SQLSMALLINT handleType, SQLHANDLE theHandle, int line );
	std::vector<ColDescription> cols;
	std::vector<std::vector<std::string> > colData;
	void DescribeColumns();
	SQLRETURN Describe(ColDescription& c);
	SQLRETURN GetResultset();
	SQLRETURN GetColData(int colnum, std::string& str);

};
