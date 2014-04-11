//
// ystem.h
//

#pragma once

#include <string>

namespace NIST
{
	std::string GetComputerName()
	{
		TCHAR  infoBuf[32767];

		// Get and display the name of the computer. 
		DWORD bufCharCount = 32767;
		::GetComputerName( infoBuf, &bufCharCount ) ;
		return infoBuf;
	}
	inline  std::string GetUserName()
	{
		TCHAR  infoBuf[32767];
		DWORD bufCharCount = 32767;

		bufCharCount = INFO_BUFFER_SIZE;
		::GetUserName( infoBuf, &bufCharCount ) ;
	}
 
};
