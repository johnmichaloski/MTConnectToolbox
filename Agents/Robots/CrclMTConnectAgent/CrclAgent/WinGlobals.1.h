//
// WinGlobals.h
// 

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#pragma once
#include <string>

#ifndef LOGONCE
#define LOGONCE  static long nLog##__LINE__=0; if( 0 == nLog##__LINE__++) 
#endif

namespace Globals
{
	bool ReadFile(std::string filename, std::string & contents);
	std::string ExeDirectory();
	void WriteFile(std::string filename, std::string & contents);
	std::string &Trim(std::string &s);
	unsigned int ErrorMessage(std::string errmsg);
	 std::string GetUserDomain();
	 std::string GetUserName();
};