//
// WinGlobals.h
//

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#include "targetver.h"
#include "Windows.h"

#include "WinGlobals.h"
#include "StdStringFcn.h"
#include <Lmcons.h>


#define SECURITY_WIN32
#include "security.h"
#pragma comment(lib, "Secur32.lib")

namespace Globals
{
	unsigned int ErrorMessage(std::string errmsg) 
	{
		OutputDebugString(errmsg.c_str());
		return E_FAIL;
	}

	std::string &LeftTrim(std::string &str) 
	{

		size_t startpos = str.find_first_not_of(" \t\r\n"); 
		if( std::string::npos != startpos )
			str = str.substr( startpos ); 
		return str;
	}
	// trim from end
	std::string &RightTrim(std::string &str, std::string trim=" \t\r\n") 
	{
		size_t endpos = str.find_last_not_of(trim); 
		if(std::string::npos != endpos ) 
			str = str.substr( 0, endpos+1 ); 		 
		return str;
	}
	// trim from both ends
	std::string &Trim(std::string &s) 
	{
		return LeftTrim(RightTrim(s));
	}
	bool ReadFile(std::string filename, std::string & contents)
	{
		std::ifstream in(filename.c_str()); 
		std::stringstream buffer; 
		buffer << in.rdbuf(); 
		contents= buffer.str();
		return true;
	}
	void WriteFile(std::string filename, std::string & contents)
	{
		std::ofstream outFile(filename.c_str());
		//Write the string and its null terminator !!
		outFile << contents.c_str();
	}
	std::string ExeDirectory()
	{
		TCHAR buf[1000];
		GetModuleFileName(NULL, buf, 1000);
		std::string path(buf);
		path=path.substr( 0, path.find_last_of( '\\' ) +1 );
		return path;
	}
std::string GetUserName()
{
	TCHAR username[UNLEN + 1];
	DWORD size = UNLEN + 1;
	::GetUserName((TCHAR*)username, &size);
	return username;
}
 std::string GetUserDomain()
{
	TCHAR username[UNLEN + 1];
	DWORD size = UNLEN + 1;
	// NameDnsDomain campus.nist.gov
	if( GetUserNameEx(NameSamCompatible  , (TCHAR*)username, &size))
	{
		std::string domain = username;
		domain = domain.substr( 0, domain.find_first_of( '\\' )  ); 
		return domain;
	}
	return "";
}

};