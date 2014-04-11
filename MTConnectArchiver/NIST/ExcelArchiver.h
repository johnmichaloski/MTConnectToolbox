//
// ExcelArchiver.h
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#pragma once
#include "DataDictionary.h"

#pragma region Import the type libraries

#import "libid:2DF8D04C-5BFA-101B-BDE5-00AA0044DE52" \
	rename("RGB", "MSORGB") \
	rename("DocumentProperties", "MSODocumentProperties")
// [-or-]
//#import "C:\\Program Files\\Common Files\\Microsoft Shared\\OFFICE12\\MSO.DLL" \
//	rename("RGB", "MSORGB") \
//	rename("DocumentProperties", "MSODocumentProperties")

using namespace Office;

#import "libid:0002E157-0000-0000-C000-000000000046"
// [-or-]
//#import "C:\\Program Files\\Common Files\\Microsoft Shared\\VBA\\VBA6\\VBE6EXT.OLB"

using namespace VBIDE;

#import "libid:00020813-0000-0000-C000-000000000046" \
	rename("DialogBox", "ExcelDialogBox") \
	rename("RGB", "ExcelRGB") \
	rename("CopyFile", "ExcelCopyFile") \
	rename("ReplaceText", "ExcelReplaceText") \
	no_auto_exclude

class CExcelArchiver
{
public:
	CExcelArchiver(void);
	~CExcelArchiver(void);

	HRESULT  CreateDataArchive(_bstr_t filename, _bstr_t sheetname, std::vector<DataDictionary> datum);
	void  CreateRow(int row, DataDictionary data);
	void LoadFields(std::string fields);
	std::string CreateArchiveDirectory(std::string path);
private:
	void Close();
	Excel::_ApplicationPtr  XL;
	Excel::WorkbooksPtr WBS;
	Excel::_WorkbookPtr WB;
	Excel::_WorksheetPtr WS;
	std::vector<std::string> _fields;
};
