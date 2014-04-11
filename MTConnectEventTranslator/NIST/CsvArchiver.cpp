//
// CsvArchiver.cpp
//
#include "stdafx.h"
#include "CsvArchiver.h"
#include "StdStringFcn.h"


// Utility function that gets error information from _com_error and
// displays an error message box.
std::string ADODataBase::DumpError(_com_error &e)
{
	std::string ErrStr; 
	ErrStr+= "Error:\n";
	ErrStr+=StdStringFormat("ErrorCode = %08lx\n",e.Error());
	ErrStr+=StdStringFormat("ErrorsCode meaning = %s\n",  (TCHAR*) e.ErrorMessage());
	ErrStr+=StdStringFormat("ErrorSource = %s\n",  (TCHAR*) e.Source());
	ErrStr+=StdStringFormat("ErrorsDescription = %s", (TCHAR*) e.Description());
	return ErrStr;
}

std::string ADODataBase::DumpRecordset(ADODB::_RecordsetPtr pRecordset, bool bValueOnly)
{
	if(pRecordset==NULL)
		_com_issue_error(FWP_E_NULL_POINTER);
	
	CString tmp;
	pRecordset->MoveFirst();
	while (!pRecordset->adoEOF)
	{
		for (short i = 0; i < pRecordset->Fields->Count; i++)
		{
			try{
				_bstr_t bstrName = pRecordset->Fields->Item[i]->Name;
				_variant_t varValue = pRecordset->Fields->Item[i]->Value;
				if(bValueOnly)
					tmp =  (LPCSTR) _bstr_t(varValue+"\n");
				else
					tmp = StdStringFormat(_T("%s: %s\n"), (LPCSTR) bstrName, (LPCTSTR) _bstr_t(varValue)).c_str();
				OutputDebugString(tmp);
			}
			catch(...)
			{
				// most likely here for VT_NULL variant
			}
		}
		pRecordset->MoveNext();
	}
}

void DumpRecordSet(ADODB::_RecordsetPtr recordset)
{

	while(!recordset->adoEOF)
	{
		_variant_t var;
//		var = recordset->Fields->Item[1]->Value; // GetItem(_variant_t((long) 1))->GetValue();
		var = recordset->Fields->Item[1]->Value; // GetItem(_variant_t((long) 1))->GetValue();
		if(var.vt==VT_NULL)
			continue;
//		var = recordset->Fields->GetItem(_variant_t(L"MachineName"))->GetValue();
		std::cout << static_cast<char *>(_bstr_t(var.bstrVal)) << std::endl;
		recordset->MoveNext();
	};
}
void ADODataBase::CreateJetDb(LPCTSTR filename,LPCTSTR UserName, LPCTSTR Pwd,LPCTSTR tablename, LPCTSTR values)
{
	ADODB::_ConnectionPtr connection;
	hr = connection.CreateInstance(__uuidof(ADODB::Connection));

	connection->CursorLocation = ADODB::adUseClient;
	connection->Open(L"Provider=sqloledb;Data Source=fifa;"
		L"Initial Catalog=test;User Id=testsa;Password=testsa;", L"", 
		L"", ADODB::adConnectUnspecified);

	ADODB::_RecordsetPtr recordset;
	hr = recordset.CreateInstance(__uuidof(ADODB::Recordset));

	std::string tabledescr = "CREATE TABLE" + tablename + "(" + values + ")";
	recordset->Open(tabledescr.c_str(),
		connection.GetInterfacePtr(), ADODB::adOpenForwardOnly,
		ADODB::adLockReadOnly, ADODB::adCmdText);

}
void ADODataBase::OpenJetDb(LPCTSTR filename, LPCTSTR UserName, LPCTSTR Pwd)
{

	_bstr_t bstrConnect = "Provider=Microsoft.Jet.OLEDB.4.0;";
	bstrConnect+="Data Source=";
	bstrConnect+=filename;
	bstrConnect+=";";
	::CoInitialize(NULL);
	const char* DAM = "ADO";
	Conn1 = ADODB::_ConnectionPtr("ADODB.Connection");
	hr = Conn1->Open(bstrConnect, "", "", ADODB::adConnectUnspecified);

}


void ADODataBase::OpenExcel(LPCTSTR excelfile, LPCTSTR UserName, LPCTSTR Pwd)
{

	CString conn = _T("Data Source=%s;Extended Properties=Excel 8.0;");

	// Trap any error/exception.
	try
	{
		// Create and Open Connection Object.
		Conn1.CreateInstance( __uuidof( ADODB::Connection ) );
		Conn1->ConnectionString =_bstr_t(conn);
		Conn1->Provider =_bstr_t( L"Microsoft.Jet.OLEDB.4.0");
		Conn1->Open( _bstr_t(L""), _bstr_t(L""), _bstr_t(L""), -1 );
	}
	catch(_com_error &e)            
	{ 
		// Exception handling.
		DumpError(e);
		throw e;

	}   
}
/**
	Example:
		"SELECT AverageCycleTime FROM [Finish Line 1$] WHERE MachineName='-Core Dump 1'"
*/
ADODB::_RecordsetPtr ADODataBase::Execute(LPCTSTR sqlquery)
{		
	_variant_t  vtEmpty (DISP_E_PARAMNOTFOUND, VT_ERROR);
	_variant_t  vtEmpty2(DISP_E_PARAMNOTFOUND, VT_ERROR);
	
	if(Conn1==NULL)
		_com_issue_error(FWP_E_NULL_POINTER);

	// Create Command Object.
	try
	{		
		ADODB::_CommandPtr     Cmd1( __uuidof( ADODB::Command ) );
		Cmd1->ActiveConnection = Conn1;

		Cmd1->CommandText = _bstr_t(sqlquery);

		// Create Parameter Object.
		//Param1 = Cmd1->CreateParameter( _bstr_t(L""),
		//	ADODB::adInteger,
		//	ADODB::adParamInput,
		//	-1,
		//	_variant_t( (long) 5) );
		//Param1->Value = _variant_t( (long) 5 );
		//Cmd1->Parameters->Append( Param1 );

		// Execute Recordset Object.
		Rs1 = Cmd1->Execute( &vtEmpty, &vtEmpty2, ADODB::adCmdText );
		return Rs1;
	}
	catch(_com_error &e)            
	{ 
		// Exception handling.
		DumpError(e);
		throw e;

	}  
	return NULL;
}

// untested
void ADODataBase::AddRecord(ADODB::_RecordsetPtr pRecordset, _variant_t FieldValues[], short iFieldCount )
{
	if(pRecordset==NULL)
		_com_issue_error(FWP_E_NULL_POINTER);

	pRecordset->AddNew();
	for (short i = 0; i < iFieldCount; i++)
	{
		pRecordset->Fields->Item[i]->Value = FieldValues[i];
	}
}
