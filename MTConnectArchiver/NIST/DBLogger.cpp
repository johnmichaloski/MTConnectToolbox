//
// MachineStateDBLogger.cpp
//

#include "StdAfx.h"
#include "DBLogger.h"
#include "atlcomtime.h"

#import "c:\Program Files\Common Files\system\ado\msadox.dll" \
    no_namespace
#import "C:\Program Files\Common Files\System\ADO\msado15.dll" \
     rename("EOF", "EndOfFile")

#ifndef ERRORCHECK
#define ERRORCHECK(X) { std::string str; HRESULT hr; if(FAILED(hr=X)) throw StdStringFormat("%s %x", #X, hr); }
#endif


/**
	{
		CDBLogger db;
		std::string path= GetModulePath();
		std::string cfgfile = path.substr(0,path.find_last_of(".")) + ".ini";
		std::string dbfile = path.substr(0,path.find_last_of(".")) + ".mdb";
		crp::Config config;
		config.load( cfgfile );
		std::string items =  config.GetSymbolValue("CONFIG.Fields", L"").c_str();
		std::vector<std::string> fields =Tokenize(items,",");
		std::vector<VARENUM> itemtypes(fields.size(), VT_BSTR);
		db.CreateLogDB(dbfile, fields, itemtypes);
		std::vector<std::string> values(fields.size(), "0");
		COleDateTime dateTest = COleDateTime::GetCurrentTime();
		values[0]= dateTest.Format("%m/%d/%Y %H:%M:%S");
		db.AddRow(dbfile,fields,values);

	}
*/

static std::string PrintProviderError(ADODB::_ConnectionPtr pConnection)
{
	// Print Provider Errors from Connection object.
	// pErr is a record object in the Connection's Error collection.
	std::stringstream str;
	ADODB::ErrorPtr    pErr  = NULL;
	long      nCount  = 0;    
	long      i     = 0;

	if( (pConnection->Errors->Count) > 0)
	{
		nCount = pConnection->Errors->Count;
		// Collection ranges from 0 to nCount -1.
		for(i = 0; i < nCount; i++)
		{
			pErr = pConnection->Errors->GetItem(i);
			str<<"Error number: %x\n Error Description: " << pErr->Number << (LPCSTR) pErr->Description << std::endl;
		}
	}
	return str.str();
}

static COleDateTime GetOleDateTime(std::string s)
{
	// parse 2012-02-03T17:31:51.0968Z 
	int Year, Month, Day, Hour, Minute, Second, Millisecond;
	if(sscanf(s.c_str(), "%d-%d-%dT%d:%d:%d.%d", &Year, &Month, &Day, &Hour, &Minute,&Second, &Millisecond)==7){}
		//12/2/2009 2:42:25 PM
	else if(sscanf(s.c_str(), "%d/%d/%4d%d:%d:%d", &Month, &Day, &Year,  &Hour, &Minute,&Second)==6){}
	else return COleDateTime::GetCurrentTime(); // throw std::exception("Unrecognized date-time format\n");
	return COleDateTime( Year, Month, Day, Hour, Minute, Second ); 
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CDBLogger::CDBLogger(void)
{
}

CDBLogger::~CDBLogger(void)
{
}

void CDBLogger::AddRow(std::string szLoggingDB, 
					   std::vector<std::string> itemlist, 
					   DataDictionary typedata,
					   std::vector<std::string> values) 
{
	HRESULT hr;
	ADODB::_RecordsetPtr rstData ;
	ADODB::_RecordsetPtr rstQuery ;
	ADODB::_RecordsetPtr rstParents ;
	ADODB::_ConnectionPtr  pConnection;
	_variant_t  vtEmpty (DISP_E_PARAMNOTFOUND, VT_ERROR);
	_bstr_t     bstrEmpty(L"");
	_bstr_t     Source1(L"Select * FROM MachineState");
	_variant_t fieldList((BSTR) L"ID");

	bstr_t szConnectionString=StdStringFormat("Provider=Microsoft.Jet.OLEDB.4.0;Data Source= %s;",  szLoggingDB.c_str()).c_str();

	try {

		ERRORCHECK(pConnection.CreateInstance(__uuidof (ADODB::Connection)));
		ERRORCHECK(rstData.CreateInstance(__uuidof (ADODB::Recordset)));

		ERRORCHECK(pConnection->Open(szConnectionString,_bstr_t(""),
			_bstr_t(""),ADODB::adConnectUnspecified));

		ERRORCHECK(rstData->Open( Source1, _variant_t((IDispatch *) pConnection, true), 
			ADODB::adOpenKeyset, 
			ADODB::adLockPessimistic, 
			ADODB::adCmdUnspecified));

		ERRORCHECK(rstData->AddNew(vtEmpty, vtEmpty));


		for(int i=0; i< itemlist.size(); i++)
		{
			bstr_t szField = itemlist[i].c_str();
			bstr_t value = values[i].c_str();

			ADODB::FieldPtr field =  rstData->Fields->GetItem( _variant_t((BSTR) szField));
			if(typedata[itemlist[i]] == "adDBTime")
			{

				bstr_t value =  (LPCSTR) DataDictionary::HrMinSecFormat(ConvertString<double>(values[i],0.0)).c_str();
				field->PutValue(_variant_t((BSTR)value)); 
#if 0
				// assumes data in minutes not hr:min:sec
				int minutes =  ConvertString<int>(values[i],0);
				COleDateTime mytime;
				mytime.SetTime(0,0,minutes);
				field->PutValue(_variant_t(mytime)); 
#endif

			}
			else if(typedata[itemlist[i]] == "adDate")
			{
				COleDateTime mytime =  GetOleDateTime(values[i]);
				field->PutValue(_variant_t(mytime)); 
			}
			else if(typedata[itemlist[i]] == "adDouble")
			{
				double d =  ConvertString<double>(values[i],0);
				field->PutValue(_variant_t(d)); 
			}
			//if(stricmp(itemlist[i].c_str(), "timestamp") ==0)
			//{
			//	COleDateTime mytime =  GetOleDateTime(values[i]);
			//	field->PutValue(_variant_t(mytime)); 
			//}
			else
			{
				field->PutValue(_variant_t((BSTR)value)); 
			}
		}

		ERRORCHECK(rstData->Update());

		// Close up
		ERRORCHECK( rstData->Close());
		ERRORCHECK(pConnection->Close());

	}
	catch(_com_error &e)
	{
		AtlTrace(PrintProviderError(pConnection).c_str());
		AtlTrace((LPCSTR) ("CDBLogger::AddRow " + e.Source() + e.Description()));
	}
	catch(std::string errmsg)
	{
		AtlTrace("CMachineStateLogger::AddRow Error %s\n", errmsg.c_str());
	}
	catch(...)
	{
		AtlTrace("Add Machine State DB Error");
	}
}


HRESULT  CDBLogger::CreateLogDB(std::string sFilepath, 
								std::vector<std::string> itemlist,
								DataDictionary typedata)
{
	HRESULT hr;
	if(GetFileExists(sFilepath))
	{
		return S_FALSE;
	}

	_CatalogPtr m_pCatalog;
	_TablePtr m_pTblNew;
	_ColumnPtr m_pCol = NULL;
	_IndexPtr m_pIndex = NULL;
	ColumnsPtr m_pCols=NULL;

	std::string strCnn;
	//Set ActiveConnection of Catalog to this string
	strCnn=StdStringFormat("Provider=Microsoft.Jet.OLEDB.4.0;" 	"Data Source= %s;", sFilepath.c_str());

	try {

		//Open the catalog
		hr = m_pCatalog.CreateInstance(__uuidof (Catalog));
		m_pCatalog->Create(bstr_t(strCnn.c_str()));

		hr = m_pIndex.CreateInstance(__uuidof (Index));
		hr =m_pCols.CreateInstance(__uuidof (Columns));

		// Connect the catalog.
		if(m_pCatalog!=NULL)
		{
			// Create and append new Table object to the Northwind database.
			hr = m_pTblNew.CreateInstance(__uuidof(Table));

			m_pTblNew->Name = "MachineState";
			m_pTblNew->ParentCatalog = m_pCatalog;

			ERRORCHECK(m_pTblNew->Columns->Append("ID", adInteger,0));
			// Make the Id column and auto incrementing column
			m_pTblNew->Columns->GetItem("ID")->Properties->
				GetItem("AutoIncrement")->Value = true;

			for(int i=0; i< itemlist.size(); i++)
			{
				AtlTrace("Add %s\n", itemlist[i].c_str());
				enum DataTypeEnum eType=adVarWChar;
				long nSize=50;

				if(typedata[itemlist[i]] == "adDBTime")
				{
					eType=adVarWChar; // adDate;
					nSize=0;

				}
				else if(typedata[itemlist[i]] == "adDate")
				{
					eType=adDate;
					nSize=0;
				}
				else if(typedata[itemlist[i]] == "adDouble")
				{
					eType=adDouble;
					nSize=0;
				}
				//	if(stricmp(itemlist[i].c_str(), "timestamp") ==0)
				//{
				//	eType=adDate; // doesn't work adDBTimeStamp - http://support.microsoft.com/kb/266302
				//	nSize=0;
				//}
				bstr_t bstrItem = itemlist[i].c_str();
				m_pTblNew->Columns->Append(_variant_t(bstrItem), eType,nSize);
				m_pTblNew->Columns->GetItem(_variant_t(bstrItem))->Attributes = adColNullable;

			}
			ERRORCHECK(m_pCatalog->Tables->Append(_variant_t((IDispatch*)m_pTblNew)));
			ERRORCHECK(m_pCatalog->Tables->Refresh());

		}
	}
	catch(std::string errmsg)
	{
		AtlTrace("CDBLogger::CreateDB Error %s", errmsg.c_str());
		return E_FAIL;
	}
	catch(_com_error &e)
	{
		AtlTrace((LPCSTR) ("CDBLogger::CreateLogDB " + e.Source() + e.Description()));

	}
	catch(...)
	{
		AtlTrace(L"CDBLogger::CreateDB Error Unknown");
		return E_FAIL;
	}
	return S_OK;

}