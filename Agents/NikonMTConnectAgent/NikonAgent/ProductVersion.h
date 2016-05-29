//

// ProductVersion.h
//

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#pragma once
#include <string>

class CProductVersion
{
public:
  CProductVersion(void);
  ~CProductVersion(void);
  BOOL        IsWow64 ( );
  void        GetVersionInfo (std::string filename);
  void        GetFileVersion (TCHAR *pszFilePath);
  void        DumpVersion ( );
  TCHAR *     GetMSVCVersion (int n);
  std::string GetOSDisplayString ( );

  std::string GenerateVersionTable (std::string versionstr);
  std::string GenerateXSLTVersion (std::string versionstr);

  std::string GenerateVersionDocument ( );
  std::string GetCssStyle ( );
  std::string GetCssStyle2 ( );
  std::string GetWindowsVersionName ( );
  std::string GetNikonInstallVersion (std::string configfilename);

  // //////////////////////////////////////////
  int major;
  int minor;
  int revision;
  int build;
};
