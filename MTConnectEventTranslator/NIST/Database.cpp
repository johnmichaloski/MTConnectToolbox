
#include <stdio.h>
#include <iostream.h>
#include <comdef.h>
#include <conio.h>
#include "Database.h"


ADODB::_RecordsetPtr rec1=NULL;

_variant_t  vtMissing1(DISP_E_PARAMNOTFOUND, VT_ERROR); 

void ErrorHandler(_com_error &e, char* ErrStr)
{
  
	sprintf(ErrStr,"Error:\n");
	sprintf(ErrStr,"%sCode = %08lx\n",ErrStr ,e.Error());
	sprintf(ErrStr,"%sCode meaning = %s\n", ErrStr, (char*) e.ErrorMessage());
	sprintf(ErrStr,"%sSource = %s\n", ErrStr, (char*) e.Source());
	sprintf(ErrStr,"%sDescription = %s",ErrStr, (char*) e.Description());
}

Database::Database()
{
	m_Cnn=NULL;
	sprintf(m_ErrStr,"NULL POINTER");
}

void Database::GetErrorErrStr(char* ErrStr)
{
	sprintf(ErrStr,"%s",m_ErrStr);
}

void Table::GetErrorErrStr(char* ErrStr)
{
	sprintf(ErrStr,"%s",m_ErrStr);
}

bool Database::Open(char* UserName, char* Pwd,char* CnnStr)
{
	//cnn->Open(strCnn,"sa","sa",NULL);
	try
	{
		HRESULT hr;
		hr    = m_Cnn.CreateInstance( __uuidof( ADODB::Connection ) );
		m_Cnn->Open(CnnStr, UserName, Pwd, NULL);
	}
	
	CATCHERROR(m_Cnn,0)

	sprintf(m_ErrStr,"Success");
	return 1;
}

bool Database::OpenTbl(int Mode, char* CmdStr, Table &Tbl)
{
	if(m_Cnn==NULL)
	{
		Tbl.m_Rec=NULL;
		sprintf(m_ErrStr,"Invalid Connection");
		return 0;
	}
	RecPtr t_Rec=NULL;
	try
	{
		//t_Rec->putref_ActiveConnection(m_Cnn);
		//vtMissing<<-->>_variant_t((IDispatch *) m_Cnn, true)
		t_Rec.CreateInstance( __uuidof( ADODB::Recordset ) );
		t_Rec->Open(CmdStr,_variant_t((IDispatch *) m_Cnn, true),ADODB::adOpenStatic,ADODB::adLockOptimistic,Mode);
	}
	
	CATCHERROR(Tbl.m_Rec,0)

	Tbl.m_Rec=t_Rec;
	sprintf(m_ErrStr,"Success");
	return 1;
}

bool Database::Execute(char* CmdStr)
{
	try
	{
		m_Cnn->Execute(CmdStr,NULL,1);
	}
	catch(_com_error &e)
	{
		ErrorHandler(e,m_ErrStr);
		return 0;
	}
	sprintf(m_ErrStr,"Success");
	return 1;
}

bool Database::Execute(char* CmdStr, Table& Tbl)
{
	RecPtr t_Rec=NULL;
	try
	{
		t_Rec=m_Cnn->Execute(CmdStr,NULL,1);
	}

	CATCHERROR(Tbl.m_Rec,0)

	sprintf(m_ErrStr,"Success");
	Tbl.m_Rec=t_Rec;
	sprintf(m_ErrStr,"Success");
	return 1;
}

Table::Table()
{
	m_Rec=NULL;
}

int Table::ISEOF()
{
	int rs;
	if(m_Rec==NULL)
	{
		sprintf(m_ErrStr,"Invalid Record");
		return -1;
	}
	try{
		rs=m_Rec->EndOfFile;
	}
	
	CATCHERROR(m_Rec,-2)

	sprintf(m_ErrStr,"Success");
	return rs;
}

bool Table::Get(char* FieldName, char* FieldValue)
{
	try
	{
		_variant_t  vtValue;
		vtValue = m_Rec->Fields->GetItem(FieldName)->GetValue();
		sprintf(FieldValue,"%s",(LPCSTR)((_bstr_t)vtValue.bstrVal));
	}

	CATCHERRGET

	sprintf(m_ErrStr,"Success");
	return 1;
}

bool Table::Get(char* FieldName,int& FieldValue)
{
	try
	{
		_variant_t  vtValue;
		vtValue = m_Rec->Fields->GetItem(FieldName)->GetValue();
		FieldValue=vtValue.intVal;
	}

	CATCHERRGET

	sprintf(m_ErrStr,"Success");
	return 1;
}

bool Table::Get(char* FieldName,float& FieldValue)
{
	try
	{
		_variant_t  vtValue;
		vtValue = m_Rec->Fields->GetItem(FieldName)->GetValue();
		FieldValue=vtValue.fltVal;
	}

	CATCHERRGET

	sprintf(m_ErrStr,"Success");
	return 1;
}

bool Table::Get(char* FieldName,double& FieldValue)
{
	try
	{
		_variant_t  vtValue;
		vtValue = m_Rec->Fields->GetItem(FieldName)->GetValue();
		FieldValue=vtValue.dblVal;
		//GetDec(vtValue,FieldValue,3);
	}

	CATCHERRGET

	sprintf(m_ErrStr,"Success");
	return 1;
}

HRESULT Table::MoveNext()
{
	HRESULT hr;
	try
	{
		hr=m_Rec->MoveNext();
	}
	catch(_com_error &e)
	{
		ErrorHandler(e,m_ErrStr);
		//m_Rec=NULL;
		return -2;
	}
	sprintf(m_ErrStr,"Success");
	return hr;
}

HRESULT Table::MovePrevious()
{
	HRESULT hr;
	try
	{
		hr=m_Rec->MovePrevious();
	}
	catch(_com_error &e)
	{
		ErrorHandler(e,m_ErrStr);
		//m_Rec=NULL;
		return -2;
	}
	sprintf(m_ErrStr,"Success");
	return hr;
}

HRESULT Table::MoveFirst()
{
	HRESULT hr;
	try
	{
		hr=m_Rec->MoveFirst();
	}
	catch(_com_error &e)
	{
		ErrorHandler(e,m_ErrStr);
		//m_Rec=NULL;
		return -2;
	}
	sprintf(m_ErrStr,"Success");
	return hr;
}

HRESULT Table::MoveLast()
{
	HRESULT hr;
	try
	{
		hr=m_Rec->MoveLast();
	}
	catch(_com_error &e)
	{
		ErrorHandler(e,m_ErrStr);
		//m_Rec=NULL;
		return -2;
	}
	sprintf(m_ErrStr,"Success");
	return hr;
}