//
// OdbcArchiver.cpp
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#pragma once
#include "stdafx.h"
#include "OdbcArchiver.h"
#include "StdStringFcn.h"
std::string SqlErrorMessage(SQLSMALLINT handleType, SQLHANDLE theHandle);


static void CHECK(SQLRETURN X, DWORD Type,SQLHANDLE Handle,std::string Y)
{
	try{ 
		SQLRETURN rc=X; 
		if(!SQL_SUCCEEDED(rc)) 
			DebugBreak(); 
	} 
	catch(...)  
	{ 
		std::string errmsg = SqlErrorMessage(Type,Handle) + Y;  
		throw std::exception(errmsg.c_str());
	}
}

std::string SqlErrorMessage(SQLSMALLINT handleType, SQLHANDLE theHandle)
{
	SQLCHAR       SqlState[6], Msg[SQL_MAX_MESSAGE_LENGTH];
	std::stringstream errmsg;
	SQLINTEGER    NativeError;
	SQLSMALLINT   i=1, MsgLen;
	SQLRETURN     rc;
	while ((rc = SQLGetDiagRec(handleType,theHandle,//SQL_HANDLE_STMT, hstmt,
		i, SqlState, &NativeError,
		Msg, sizeof(Msg), &MsgLen)) != SQL_NO_DATA) 
	{
		Msg[MsgLen+1]=0;
		errmsg<< SqlState << " " << NativeError << " " <<  Msg ;
		i++;
	}
	errmsg << std::endl;
	return errmsg.str().c_str();
}


OdbcArchiver::OdbcArchiver()
{
	_hStmt=NULL;
	_hConn=NULL;
	_hEnv=NULL;
}
OdbcArchiver::~OdbcArchiver()
{
	close();
}

SQLRETURN OdbcArchiver::GetColData(int colnum, std::string& str)
{
	SQLRETURN rc;
	SQLCHAR buf[255] = {0};
	if( (rc = SQLGetData(_hStmt, colnum, SQL_CHAR, buf, sizeof(buf), NULL)) == SQL_SUCCESS)
		str = reinterpret_cast<char*>(buf);
	else
		OutputDebugString(SqlErrorMessage(SQL_HANDLE_STMT,_hStmt).c_str());
	return rc;
}
SQLRETURN OdbcArchiver::GetResultset(std::vector<std::vector<std::string> > & colData)
{
    // Get all column description
    DescribeColumns();
    // erase anything that's in the colData vector
    colData.clear();
    // fetch a row from the resultset
    while( SQLFetch(_hStmt) == SQL_SUCCESS)
    {
        // vector of strings to hold the column data
		std::vector<std::string> col;
        std::string data;
        // column counter
        int i = 1;
        // get the data for each column and add it to
        // the col vector
		for(int i=0; i< cols.size(); i++)
		{
			if(GetColData(i+1, data) == SQL_SUCCESS)
        //while( GetColData(i, data) == SQL_SUCCESS)
        // {
            col.push_back(data);
			else 
				col.push_back("");

        }
        // add column data to the colData vector
        colData.push_back(col);
    }
    return SQL_SUCCESS;
}
 
// Get the description for one column in the resultset.
// This was made a seprate function to simplify the coding
SQLRETURN  OdbcArchiver::Describe(ColDescription& c)
{
    return SQLDescribeCol(_hStmt,c.colNumber,
        c.colName, sizeof(c.colName), &c.nameLen,
        &c.dataType, &c.colSize, &c.decimalDigits, &c.nullable);
}
// Get the description for all the columns in the resultset.
void OdbcArchiver::DescribeColumns()
{
    ColDescription c;
    c.colNumber = 1;
    cols.clear();
    while( Describe(c) == SQL_SUCCESS)
    {
        cols.push_back(c);
        ++c.colNumber;
    }   
}
 

void OdbcArchiver::status( SQLSMALLINT handleType, SQLHANDLE theHandle, int line )
{
	SQLCHAR sqlState[6];
	SQLINTEGER nativeError;
	SQLCHAR msgStr[256];
	SQLSMALLINT overBy ; // the number of characters that msgStr buffer was TOO SHORT..

	// http://msdn.microsoft.com/en-us/library/ms716256(VS.85).aspx
	// This must be the WEIRDEST ERROR REPORTING FUNCTION I've EVER seen.
	// It requires 8 parameters, and its actually pretty .. silly
	// about the amount of state information it expects YOU to keep track of.

	// It isn't so much a "GetLastError()" function
	// as it is a "GetMeStatus( something very, very specific )" function.

	SQLRETURN retCode ;

	for( int i = 1 ; i < 20 ; i++ )
	{
		retCode = SQLGetDiagRecA(

			handleType,  // the type of object you're checking the status of
			theHandle,   // handle to the actual object you want the status of

			i, // WHICH status message you want.  The "Comments" section at the 
			// bottom of http://msdn.microsoft.com/en-us/library/ms716256(VS.85).aspx
			// seems to explain this part well.

			sqlState,    // OUT:  gives back 5 characters (the HY*** style error code)
			&nativeError,// numerical error number
			msgStr,      // buffer to store the DESCRIPTION OF THE ERROR.
			// This is the MOST important one, I suppose

			255,         // the number of characters in msgStr, so that
			// the function doesn't do a buffer overrun in case it
			// has A LOT to tell you
			&overBy      // again in case the function has A LOT to tell you,
			// the 255 character size buffer we passed might not be large
			// enough to hold the entire error message.  If that happens
			// the error message will truncate and the 'overBy' variable
			// will have a value > 0 (it will measure number of characters
			// that you 'missed seeing' in the error message).

			) ;

		if( SQL_SUCCEEDED( retCode) )
		{
			AtlTrace( "LINE %d:  [%s][%d] %s\n", line, sqlState, nativeError, msgStr ) ;
		}
		else
		{
			// Stop looping when retCode comes back
			// as a failure, because it means there are
			// no more messages to tell you
			break ;
		}
	}
}
HRESULT OdbcArchiver::Check( SQLRETURN rc, char * msg, bool throwFailedExpection, bool printSucceededMsg )
{
	if( SQL_SUCCEEDED( rc ) )
	{
		if( printSucceededMsg )  AtlTrace( "%s succeeded\n", msg ) ; 
		return S_OK ;
	}
	else
	{
		std::string  errmsg = StdStringFormat( "%s has FAILED!! Error Code=%d\n", msg , rc) ;
		status( SQL_HANDLE_DBC, _hConn, __LINE__ ) ;
		if(throwFailedExpection)
			throw std::exception((std::string(errmsg) + " exception").c_str());
		return E_FAIL ;
	}
}
static std::string GetDDLType(std::string type )
{

	if(type == "adChar") 
		return "CHAR";
	if(type == "adDouble") 
		return "DOUBLE";
	if((type == "adTinyInt") 
		||(type == "adSmallInt") 
		||(type == "adInteger") 
		||(type == "adBigInt") 
		)
		return "INT(11)";
	if((type == "adBSTR")
		||(type == "adChar")
		||(type == "adVarWChar") 
		||(type == "adVarChar")
		)
		return "VARCHAR(255)";
	if((type == "adDBTime")  || (type == "adTime") ) 
		return "TIME";

	if(type == "adDate") 
		return "TIMESTAMP";

	return "VARCHAR(255)";
}

static std::string GetDDLDefault(std::string type )
{
	if((type == "adTinyInt") 
		||(type == "adSmallInt") 
		||(type == "adInteger") 
		||(type == "adBigInt") 
		)		return "DEFAULT 0";

	if(type == "adDouble") 
		return "DEFAULT 0.0";

	if(type == "adChar") 
		return "DEFAULT ''";

	if((type == "adBSTR")
		||(type == "adChar")
		||(type == "adVarWChar") 
		||(type == "adVarChar")
		)
		return "DEFAULT ''";

	if(type == "adDBTime" ||(type == "adTime")) 
		return "";
	if(type == "adDate") 
		return "";
	return "";
}

static std::string GetValue(std::string type, std::string value )
{
	if((type == "adTinyInt") 
		||(type == "adSmallInt") 
		||(type == "adInteger") 
		||(type == "adBigInt") 
		)		
	{
		if(value.empty()) value="0";
		return value;
	}

	if(type == "adDouble") 
	{
		if(value.empty()) value="0.0";
		return value;
	}

	if(type == "adChar") 
		return "'" + value +"'";

	if((type == "adBSTR")
		||(type == "adChar")
		||(type == "adVarWChar") 
		||(type == "adVarChar")
		)
		return "'" + value +"'";

	// adTime - 00:00:00
	if(type == "adTime")
	{
		value = "'"+value+"'";
		return value;
	}

	// adDBTime - convert seconds to 00:00:00
	if(type == "adDBTime") 
	{
		// Duration - most likely
		double seconds = ConvertString<double>(value,0.0);
		return "'"+DataDictionary::HrMinSecFormat((double)seconds)+"'"; 
	}
	// adDate -  yyyy-mm-dd 00:00:00
	if(type == "adDate") 
	{
		ReplaceAll(value,"Z","");
		ReplaceAll(value,"T"," ");
		value = value+".000";
		value = "'"+value+"'";
		return value;
	}
	return "'"+value+"'";
}

void OdbcArchiver::open(std::string dbname, std::string suser, std::string spassword)
{

	SQLRETURN retCode;
	try {
		CHECK( SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_hEnv ), SQL_HANDLE_ENV, SQL_NULL_HANDLE, "allocate environment handle" ) ;
		CHECK( SQLSetEnvAttr( _hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0 ), SQL_HANDLE_ENV, _hEnv, "setting the environment attribute setting to ODBC version 3" ) ;

		CHECK( SQLAllocHandle( SQL_HANDLE_DBC, _hEnv, &_hConn ), SQL_HANDLE_DBC, _hEnv, "allocate handle" ) ;
		SQLCHAR* dsnName = (SQLCHAR*)dbname.c_str() ;
		SQLCHAR* userid = (SQLCHAR*)suser.c_str();
		SQLCHAR* password = (SQLCHAR*)spassword.c_str(); 

		CHECK( SQLConnectA(
			_hConn,
			dsnName,  
			SQL_NTS,  // the DSN name is a NULL TERMINATED STRING, so "count it yourself"
			userid,
			SQL_NTS,  // userid is a null-terminated string 
			password,
			SQL_NTS   // password is a null terminated string  
			),
			SQL_HANDLE_DBC,
			_hConn,
			"SqlConnectA");
		CHECK(SQLAllocHandle(SQL_HANDLE_STMT, _hConn, &_hStmt),SQL_HANDLE_DBC, _hConn, "open SQLAllocHandle Statement Handle"); 
	}
	catch(...)
	{

	}

	//return S_OK;
}

void OdbcArchiver::close()
{
	if (_hStmt != NULL)
		SQLFreeHandle(SQL_HANDLE_STMT, _hStmt);
	// Free The Connection Handle
	if (_hConn != NULL)
		SQLFreeHandle(SQL_HANDLE_DBC, _hConn);
	// Free The Environment Handle
	if (_hEnv != NULL)
		SQLFreeHandle(SQL_HANDLE_ENV, _hEnv);
	_hStmt=NULL ;
	_hConn=NULL ;
	_hEnv =NULL;

}
std::string OdbcArchiver::GetData(SQLHSTMT  hStmt, int j)
{
	SQLRETURN retCode;
	char buf[256];
	SQLLEN numBytes;

	CHECK( SQLGetData(
		hStmt,
		j,           // COLUMN NUMBER of the data to get
		SQL_C_CHAR,  // the data type that you expect to receive
		buf,         // the place to put the data that you expect to receive
		255,         // the size in bytes of buf (-1 for null terminator)
		(SQLLEN *) &numBytes    // size in bytes of data returned
		), SQL_HANDLE_DBC, _hConn,"SQLGetData") ;


	buf[numBytes+1]=0;
	return buf;
}

void OdbcArchiver::createSchema(std::string schema)
{
	std::stringstream buffer;
	if(_hConn==NULL  || _hStmt == NULL)
		throw "createSchema no DB Connection";

	buffer <<  StdStringFormat("CREATE SCHEMA IF NOT EXISTS `%s` DEFAULT CHARACTER SET latin1\n", schema.c_str());
	CHECK(SQLExecDirect(_hStmt, (SQLCHAR *) buffer.str().c_str(), SQL_NTS),SQL_HANDLE_STMT, _hStmt,"SQLExecDirect CREATE SCHEMA ");
	CHECK(SQLExecDirect(_hStmt, (SQLCHAR *) StdStringFormat("USE `%s`\n", schema.c_str()).c_str(), SQL_NTS), SQL_HANDLE_STMT, _hStmt, "SQLExecDirect USE SCHEMA");
}

void OdbcArchiver::useSchema(std::string schema)
{
	if(_hConn==NULL || _hStmt==NULL)
		throw "useSchema no DB Connection";

	CHECK(SQLExecDirect(_hStmt, (SQLCHAR *) StdStringFormat("USE `%s`;", schema.c_str()).c_str(), SQL_NTS), SQL_HANDLE_STMT, _hStmt, "SQLExecDirect USE SCHEMA");
}
void OdbcArchiver::createTable(std::string schema, std::string table, std::vector<std::string> &fields, DataDictionary &typedata)
{
	std::stringstream buffer;
	if(_hConn==NULL || _hStmt==NULL)
		throw "createTable no DB Connection";


	buffer <<  StdStringFormat("CREATE TABLE IF NOT EXISTS `%s`.`%s`(", schema.c_str(), table.c_str());

	buffer <<  std::endl << "Id INTEGER PRIMARY KEY AUTO_INCREMENT," << std::endl;
	for(int i=0; i< fields.size(); i++)
	{
		std::string type = typedata[fields[i]];
		std::string field = fields[i];
		ReplaceAll(field,".", "_");
		ReplaceAll(field," ", "_");
		buffer <<   "`" << field << "` " << GetDDLType(type ) << " " ;
		buffer <<   "NULL ";
		buffer << GetDDLDefault(type );
		if((i+1)!=fields.size()) buffer <<  ", " << std::endl;
	}
	buffer << ") ENGINE=InnoDB DEFAULT CHARSET=utf8;" << std::endl;
	//CHECK(SQLAllocHandle(SQL_HANDLE_STMT, _hConn, &_hStmt), SQL_HANDLE_DBC, _hConn, "SQLAllocHandle Statement Handle"); 
	OutputDebugString( buffer.str().c_str());
	CHECK(SQLExecDirect(_hStmt, (SQLCHAR *) buffer.str().c_str(), SQL_NTS), SQL_HANDLE_STMT, _hStmt, "SQLExecDirect OdbcArchiver::create");
}

void OdbcArchiver::deleteTable(std::string schema, std::string table)
{
	if(_hConn==NULL || _hStmt==NULL)
		throw "deleteTable no DB Connection";
	CHECK(SQLExecDirect(_hStmt, (SQLCHAR *) StdStringFormat("DROP TABLE IF EXISTS `%s`.`%s`;", schema.c_str(), table.c_str()).c_str(), SQL_NTS), 
		SQL_HANDLE_STMT, _hStmt, "DROP TABLE");
}

void OdbcArchiver::insert(std::string schema, std::string table, std::vector<std::string> &fields, DataDictionary &values,DataDictionary typedata)
{
	std::vector<std::string> orderedvalues;
	for(int i=0; i< fields.size(); i++)
		orderedvalues.push_back(values[fields[i]]);
	insert(schema,  table, fields, orderedvalues,  typedata);
}

void OdbcArchiver::insert(std::string schema, std::string table, std::vector<std::string> &fields, std::vector<std::string> &values, DataDictionary typedata)
{
	std::stringstream buffer;

	if(_hConn==NULL || _hStmt==NULL)
		throw "insert no DB Connection";

	CHECK(SQLExecDirect(_hStmt, (SQLCHAR *) StdStringFormat("USE `%s`\n", schema.c_str()).c_str(), SQL_NTS),
		SQL_HANDLE_STMT, _hStmt, "SQLExecDirect USE SCHEMA");
	buffer <<  StdStringFormat("INSERT INTO `%s`.`%s`", schema.c_str(), table.c_str());
	buffer <<  StdStringFormat("(");
	for(int i=0; i< fields.size() ; i++)
	{
		buffer << fields[i] << (((i+1) < fields.size()) ? "," : "");
	}
	buffer <<  StdStringFormat(")");
	buffer <<  StdStringFormat(" VALUES(");
	for(int i=0; i< fields.size() ; i++)
	{
		buffer << GetValue(typedata[fields[i]] , values[i] ) << (((i+1) < fields.size()) ? "," : "");
	}

	buffer <<  StdStringFormat(")");
	OutputDebugString(buffer.str().c_str());
	CHECK(SQLExecDirect(_hStmt, (SQLCHAR *) buffer.str().c_str(), SQL_NTS), 
		SQL_HANDLE_STMT, _hStmt,"SQLExecDirect INSERT INTO");
}
std::string  OdbcArchiver::SanitizeColumnName(std::string field)
{
	ReplaceAll(field,".", "_");
	ReplaceAll(field," ", "_");
	return field;
}
static void MkDate(TIMESTAMP_STRUCT& ts, TCHAR* szBuffer)
{
	TCHAR szMonth[4],szDay[4],szYear[8];
	memset(szBuffer,0,128);

 _stprintf(szMonth,_T("%u"),ts.month);
 _stprintf(szDay,_T("%u"),ts.day);
 _stprintf(szYear,_T("%u"),ts.year);
 _tcscpy(szBuffer,szMonth);
 _tcscat(szBuffer,_T("/"));
 _tcscat(szBuffer,szDay);
 _tcscat(szBuffer,_T("/"));
 _tcscat(szBuffer,szYear);

 return;
}
int OdbcArchiver::sizeTable(std::string schema, std::string table)
{
	std::string data;
	std::string buffer;
	SQLHANDLE hStmt ;

	CHECK(SQLAllocHandle(SQL_HANDLE_STMT, _hConn, &hStmt),SQL_HANDLE_DBC, _hConn, "open SQLAllocHandle Statement Handle"); 
	buffer+="SELECT COUNT( Id ) AS row_count  ";
	buffer+=StdStringFormat(" FROM `%s`.`%s`", schema.c_str(),table.c_str());
	SQLRETURN rc=SQLExecDirect(hStmt, (SQLCHAR *) buffer.c_str(), SQL_NTS); // ), SQL_HANDLE_STMT, _hStmt, "SQLExecDirect OdbcArchiver::updateTimeTable");
	if(rc != SQL_SUCCESS)
	{
		status(SQL_HANDLE_STMT,hStmt, 0);
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return 0;
	}
	if((rc=SQLFetch(hStmt))==SQL_NO_DATA)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return 0;
	}
	data=GetData( hStmt,1);
	if(data.empty())
	//if(this->GetColData(1,data)!= SQL_SUCCESS)
	{
		status(SQL_HANDLE_STMT,hStmt, 0);
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return 0;
	}
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return ConvertString<int>(data,0);
}

 bool OdbcArchiver::selectTable(std::vector<std::vector<std::string> > & result , std::string schema, std::string table, std::vector<std::string> &fields, DataDictionary &typedata)
{
	return selectTableWhere(result, schema, table, "", fields, typedata);
}

//SELECT identifier, name, type, resourceIds, properties, description FROM `factory`.`cell`;
bool
OdbcArchiver::selectTableWhere(std::vector<std::vector<std::string> > & colData, std::string schema, std::string table, std::string where, std::vector<std::string> &fields, DataDictionary &typedata)
{
	std::stringstream buffer;
	if(_hConn==NULL || _hStmt==NULL)
		throw "selectTable no DB Connection";

	buffer <<  "SELECT " ;

	for(int i=0; i< fields.size(); i++)
	{
		std::string field = SanitizeColumnName(fields[i]);
		//buffer <<   "'" << field << "' " ;
		buffer <<   field  ;
		if((i+1)!=fields.size()) buffer <<  ", " << std::endl;
	}
	if(fields.size() == 0)
		buffer << "* ";

	buffer <<  StdStringFormat(" FROM `%s`.`%s`", schema.c_str(), table.c_str());
	if(!where.empty())
		buffer << " WHERE " << where;
	buffer << ";";

	OutputDebugString( buffer.str().c_str());
	SQLRETURN rc=SQLExecDirect(_hStmt, (SQLCHAR *) buffer.str().c_str(), SQL_NTS);
	if(rc != SQL_SUCCESS)
		status(SQL_HANDLE_STMT,_hStmt, 0);
	if( GetResultset(colData)== SQL_SUCCESS)
		return true;
	return false;

}
