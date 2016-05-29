//
// ProductVersion.h
//

#pragma once
#include <string>

class CProductVersion
{
public:
	CProductVersion(void);
	~CProductVersion(void);
	BOOL IsWow64();
	void GetVersionInfo(std::string filename);
	void GetFileVersion( TCHAR *pszFilePath );
	void DumpVersion();
	std::string GenerateVersionDocument();
	std::string GetCssStyle();
	std::string GetCssStyle2();
	////////////////////////////////////////////
	int  major ; 
    int  minor ;
    int  revision ;
    int  build ;

};

