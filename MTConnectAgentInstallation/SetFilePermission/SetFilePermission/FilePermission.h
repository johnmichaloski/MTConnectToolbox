//
// FilePermission.h
//


#pragma once
#include <string>

class CFilePermission
{
public:
	CFilePermission(void);
	~CFilePermission(void);
	static void SetFilePermission(std::string szFilename) ;
};

