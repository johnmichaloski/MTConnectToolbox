// 
// ExcelArchiver.cpp
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#include "StdAfx.h"
#include "ExcelArchiver.h"
#include "float.h"
#include <comdef.h>
#include "StdStringFcn.h"

static bool fexists(const char *filename)
{
	std::ifstream ifile(filename);
	return ifile;
} 

static long Variant2Int (_variant_t &vItem)
{
	USES_CONVERSION;
	return atoi(W2A( _bstr_t( vItem )));
}

static CString Variant2String (_variant_t &vItem)
{
	USES_CONVERSION;
	_bstr_t b( vItem );
	return (LPCTSTR)b;
}

static double Variant2Double (_variant_t &vItem)
{
	USES_CONVERSION;
	CString str = Variant2String (vItem);
	if("DBL_MAX" == str.Trim())
		return DBL_MAX;
	return atof(str);
}

static CString ComErrorMessage(_com_error &e)
{
	CString ErrStr("Error:\n"); 
	ErrStr.AppendFormat("ErrorCode = %08lx\n",e.Error());
	ErrStr.AppendFormat("ErrorsCode meaning = %s\n",  (TCHAR*) e.ErrorMessage());
	ErrStr.AppendFormat("ErrorSource = %s\n",  (TCHAR*) e.Source());
	ErrStr.AppendFormat("ErrorsDescription = %s", (TCHAR*) e.Description());
	return ErrStr;

}

static Excel::RangePtr GetRange(Excel::_WorksheetPtr pWSheet, int rowstart,int colstart, int rowend, int colend)
{
	Excel::RangePtr pRange  = pWSheet->GetRange( _bstr_t( _T("A2") ), _bstr_t( _T("Z16384" ) ) );
}
static		CString GetHeaderColumn(int row, int col)
{
	std::string str= StdStringFormat("%c%d", 65+col,row);
	return str.c_str();
}
////////////////////////////////////////////////
CExcelArchiver::CExcelArchiver(void)
{
}

CExcelArchiver::~CExcelArchiver(void)
{
}

void CExcelArchiver::Close() 
{
	WS=NULL;;
	WB=NULL;;
	if(XL!=NULL)
		XL->Quit();
	XL=NULL;
		
}
#if 0
#include <boost/filesystem.hpp>
namespace fs=boost::filesystem;

std::string CExcelArchiver::CreateArchiveDirectory(std::string path)
{
	if(! boost::filesystem::exists(path))
	{
		fs::create_directories(path);
	}
	if(!EndsWith(path,"\\"))
		path+="\\";
	return false;
}
#endif
std::string CExcelArchiver::CreateArchiveDirectory(std::string path)
{
	USES_CONVERSION;
	if ((GetFileAttributes(path.c_str())) == INVALID_FILE_ATTRIBUTES)
	{
		SHCreateDirectory(0,A2W(path.c_str()));
		//fs::create_directories(path);
	}
	if(!EndsWith(path,"\\"))
		path+="\\";
	return path;
}


HRESULT CExcelArchiver::CreateDataArchive(_bstr_t filename, _bstr_t sheetname , std::vector<DataDictionary> datum)
{
	AtlTrace("ExcelService::OpenFile()%s\n", (LPCTSTR) filename);
	// VARIANT used in place of optional-parameters.
	VARIANT varOpt;
	varOpt.vt = VT_ERROR;
	varOpt.scode = DISP_E_PARAMNOTFOUND;
	HRESULT hr= S_OK;
	std::string tmpName;
	_variant_t covOptional(DISP_E_PARAMNOTFOUND,VT_ERROR);

	  try
	{
		::CoInitialize(NULL);
		VARIANT* p = &vtMissing;
		HRESULT hr = XL.CreateInstance(L"Excel.Application"); 
		// OR
		// Get an interface to the running instance, if any..
		//IUnknown *pUnk;
		//CLSID clsid;
		//CLSIDFromProgID(L"Excel.Application", &clsid);  
		//HRESULT hr = GetActiveObject(clsid, NULL, (IUnknown**)&pUnk);

		//XL->PutVisible(0,VARIANT_FALSE);
		XL->PutVisible(0,VARIANT_TRUE);
		//XL->Interactive = VARIANT_FALSE;
	//	XL->put_Interactive(0, VARIANT_FALSE); 
		XL->put_DisplayAlerts(0, VARIANT_FALSE); // DisplayAlerts

		Excel::WorkbooksPtr pBooks = XL->Workbooks;
		// FIXME: copy to safe place so others can look at daily excel and still update
		if (fexists(filename)) 
		{
			WB = pBooks->Open(filename);	
			//::remove(filename);
			WS = WB->Sheets->Add();
		}
		else
		{
			WB = pBooks->Add(Excel::xlWorksheet); 
			WS = XL->ActiveSheet; 
		}

		WS->Name = sheetname;  

		Excel::RangePtr  pRange = WS->GetRange( _bstr_t( _T("A1") ), _bstr_t( _T("Z16384" ) ) );
		Excel::RangePtr  pColRange = WS->GetRange( _bstr_t( _T("A1") ), _bstr_t(GetHeaderColumn(1,_fields.size()) ) );
		Excel::RangePtr pCols = pColRange->Columns;
		pCols->AutoFit();
		pCols->Font->Bold = _variant_t(TRUE);
		pCols->Interior->ColorIndex = 5;
		pCols->Interior->Pattern = 1;
		pCols->Font->ColorIndex = 2;
		pCols->HorizontalAlignment = Excel::xlCenter; // xlCenter

		for(int i=0; i< _fields.size(); i++)
		{
			pRange->Item[ 1 ][ i +1 ] = _bstr_t(_fields[i].c_str());
		}
		for(int i=0; i< datum.size(); i++)
		{
			CreateRow(i, datum[i]);
		}
		tmpName = GetTempFileName("WB"); 
		WB->SaveAs(tmpName.c_str(),Excel::xlWorkbookNormal,varOpt,varOpt,VARIANT_FALSE,VARIANT_FALSE,Excel::xlExclusive); 
		WB->Close(VARIANT_FALSE);

	}
	catch(_com_error e)
	{
		AtlTrace(ComErrorMessage(e));
		hr= E_FAIL;
	}
	catch(...)
	{
		hr= E_FAIL;
	}
	//::remove(filename);// can't remove it if excel has it open.
	//::MoveFile(tmpfile.c_str(),filename);
	//::remove(tmpfile.c_str());
	Close();
	::MoveFileEx(tmpName.c_str(), filename,MOVEFILE_REPLACE_EXISTING);
	::remove(tmpName.c_str());
	::CoUninitialize();
	return  hr;
}

void  CExcelArchiver::CreateRow(int row, DataDictionary data)
{
	std::string style;
	double dVal=0.0;

	Excel::RangePtr  pRange = WS->GetRange( _bstr_t( _T("A1") ), _bstr_t( _T("Z16384" ) ) );

	for(int i=0; i< _fields.size(); i++)
	{
		std::string value = data[_fields[i]]; //(*it).second;
		std::string field = _fields[i]; //  (*it).first
		pRange->Item[ row+2 ][ i+1  ] = _bstr_t(value.c_str());

	}
}