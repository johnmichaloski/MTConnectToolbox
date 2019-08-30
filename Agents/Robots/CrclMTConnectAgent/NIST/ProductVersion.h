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
	TCHAR * GetMSVCVersion(int n) ;
	std::string GetOSDisplayString();

	std::string GenerateVersionTable(std::string versionstr);
	std::string GenerateXSLTVersion(std::string versionstr);

	std::string GenerateVersionDocument();
	std::string GetCssStyle();
	std::string GetCssStyle2();
	std::string GetWindowsVersionName();
	std::string  GetProductInstallVersion(std::string configfilename) ;
	////////////////////////////////////////////
	int  major ; 
    int  minor ;
    int  revision ;
    int  build ;

};

