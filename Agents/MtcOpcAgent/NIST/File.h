//
//
//

#pragma once
#include <vector>
#include <map>
#include "StdStringFcn.h"
#include "Logger.h"
#include <stdio.h>


//#include <boost/filesystem.hpp>
#include "ATLComTime.h"
class CFile
{
public:
	bool Exists(std::string unc)
	{
		OFSTRUCT TestOF;
		HFILE hFile= OpenFile(unc.c_str(),&TestOF, OF_SHARE_DENY_NONE );	
		if(hFile)
			return true;
		return false;
		//return boost::filesystem::exists( unc );
		//struct stat filestatus;
		//	return !stat(filename.c_str(), &filestatus );

	}
	size_t  Size(std::string unc)
	{
		OFSTRUCT TestOF;
		HFILE hFile= OpenFile(unc.c_str(),&TestOF, OF_SHARE_DENY_NONE );	
		if(hFile) 
			return GetFileSize((HANDLE) hFile,  NULL);
		return 0;

		//return boost::filesystem::file_size(unc);
		//struct stat filestatus;
		//	if(stat(filename.c_str(), &filestatus )!=0) return 0;
		// return filestatus.st_size;

		//OFSTRUCT TestOF;
		// HFILE hFile= OpenFile(unc.c_str(),&TestOF, OF_SHARE_DENY_NONE );	
		// if(hFile) return GetFileSize(hFile,  NULL);
		// return 0;


	}
	COleDateTime GetFileModTime(std::string filename)
	{
		FILETIME creationTime,
			lpLastAccessTime,
			lastWriteTime;
		OFSTRUCT TestOF;
		HANDLE hFile= (HANDLE) OpenFile(filename.c_str(),&TestOF, OF_SHARE_DENY_NONE );	
		bool err = GetFileTime( hFile, &creationTime, &lpLastAccessTime, &lastWriteTime );
		return  COleDateTime (lastWriteTime);
	}

	std::string TempDirectory()
	{
		TCHAR lpTempPathBuffer[MAX_PATH];
		DWORD  dwRetVal = GetTempPath(MAX_PATH,          // length of the buffer
			lpTempPathBuffer); // buffer for path 
		if (dwRetVal > MAX_PATH || (dwRetVal == 0))
			return "C:\\TEMP";
		return lpTempPathBuffer;
	}
	std::string ExeDirectory()
	{
		TCHAR buf[1000];
		GetModuleFileName(NULL, buf, 1000);
		std::string path(buf);
		path=path.substr( 0, path.find_last_of( '\\' ) +1 );
		return path;
	}
	std::string Filename(std::string path)
	{
		size_t sep = path.find_last_of("\\");
		if (sep != std::string::npos)
			 return path.substr(sep + 1);
		return path; // throw?
	}
	std::string Filetitle(std::string path)
	{
		path= Filename(path);
		size_t dot = path.find_last_of(".");
		if (dot != std::string::npos)
			return path.substr(0, dot);	
		return path;
	}
	std::string Extension(std::string path)
	{
		size_t dot = path.find_last_of(".");
		if (dot != std::string::npos)
			return path.substr(dot);
		return ""; // throw?
	}
};
__declspec(selectany)  CFile File;

