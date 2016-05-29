//
//
//

#pragma once
#include <vector>
#include <map>
#include "StdStringFcn.h"
#include <stdio.h>
#include <sys/types.h> 
#include "dirent.h"

// Windows oriented file manipulation 

//#include <boost/filesystem.hpp>
#include "ATLComTime.h"
class CFile
{
public:
	void Warning(std::string str)
	{
		OutputDebugString(str.c_str());
	}
	void Fatal(std::string str)
	{
		OutputDebugString(str.c_str());
	}
	void Abort(std::string str)
	{
		OutputDebugString(str.c_str());
	}

private:
BY_HANDLE_FILE_INFORMATION GetFileInformation(std::string filename)
{
		BY_HANDLE_FILE_INFORMATION fInfo = {0};
		HANDLE hFile = CreateFile(filename.c_str(),
			GENERIC_READ, // access (read) mode
			FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, // share mode
			NULL, // pointer to security attributes
			OPEN_EXISTING, // how to create
			FILE_ATTRIBUTE_NORMAL,// file attributes
			NULL); // handle to file with attributes to copy

		if (hFile == INVALID_HANDLE_VALUE)    // did we succeed?
		{
			Warning(StdStringFormat("File::Size INVALID_HANDLE_VALUE File \"%s\" \n", filename.c_str()));
			return fInfo;
		}
		GetFileInformationByHandle(hFile, &fInfo);
		CloseHandle(hFile);
		return fInfo;
}
public:
	bool Exists(std::string filename)
	{
		HANDLE hFile = CreateFile(filename.c_str(),
			GENERIC_READ, // access (read) mode
			FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, // share mode
			NULL, // pointer to security attributes
			OPEN_EXISTING, // how to create
			FILE_ATTRIBUTE_NORMAL,// file attributes
			NULL); // handle to file with attributes to copy

		if (hFile == INVALID_HANDLE_VALUE)    // did we succeed?
		{
			return false;
		}
		CloseHandle(hFile);
		return true;

	/*	OFSTRUCT TestOF;
		HFILE hFile= OpenFile(unc.c_str(),&TestOF, OF_SHARE_DENY_NONE );	
		if(hFile)
			return true;
		return false;*/

		//return boost::filesystem::exists( unc );
		//struct stat filestatus;
		//	return !stat(filename.c_str(), &filestatus );
	}
	HRESULT  Size(std::string filename, DWORD & size)
	{
		size=0;
		HANDLE hFile = CreateFile(filename.c_str(),
			GENERIC_READ, // access (read) mode
			FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, // share mode
			NULL, // pointer to security attributes
			OPEN_EXISTING, // how to create
			FILE_ATTRIBUTE_NORMAL,// file attributes
			NULL); // handle to file with attributes to copy

		if (hFile == INVALID_HANDLE_VALUE)    // did we succeed?
		{
			return E_FAIL;
		}

		size = GetFileSize(hFile, NULL);
		CloseHandle(hFile);
		if(size!=INVALID_FILE_SIZE ) 
			return S_OK;
		return E_FAIL;

		//BY_HANDLE_FILE_INFORMATION fInfo = GetFileInformation (filename); // {0};
		//BY_HANDLE_FILE_INFORMATION fEmpty = {0};
		//int n=memcmp ((void *) &fInfo,(void *) &fEmpty, sizeof(BY_HANDLE_FILE_INFORMATION) );
		//if(n!= 0)
		//	return (fInfo.nFileSizeHigh * (MAXDWORD+1)) + fInfo.nFileIndexLow;
		//return 0;

		//OFSTRUCT TestOF;
		//HFILE hFile= OpenFile(unc.c_str(),&TestOF, OF_SHARE_DENY_NONE );	
		//if(hFile) 
		//	return GetFileSize((HANDLE) hFile,  NULL);
		//return 0;

		//return boost::filesystem::file_size(unc);
		//struct stat filestatus;
		//	if(stat(filename.c_str(), &filestatus )!=0) return 0;
		// return filestatus.st_size;
	}
	COleDateTime GetFileModTime(std::string filename)
	{

		BY_HANDLE_FILE_INFORMATION fInfo = GetFileInformation (filename); // {0};
		BY_HANDLE_FILE_INFORMATION fEmpty = {0};
		int n=memcmp ((void *) &fInfo, (void *) &fEmpty, sizeof(BY_HANDLE_FILE_INFORMATION) );
		if(n!= 0)
			return fInfo.ftLastWriteTime;
		return COleDateTime(0,0,0,0,0,0);
		//HANDLE hFile = CreateFile(filename.c_str(),
		//	GENERIC_READ, // access (read) mode
		//	FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, // share mode
		//	NULL, // pointer to security attributes
		//	OPEN_EXISTING, // how to create
		//	FILE_ATTRIBUTE_NORMAL,// file attributes
		//	NULL); // handle to file with attributes to copy

		//if (hFile == INVALID_HANDLE_VALUE)    // did we succeed?
		//{
		//	DebugMessage(StdStringFormat("File::Size INVALID_HANDLE_VALUE File \"%s\" \n", filename.c_str()));
		//	return 0;
		//}
		//BY_HANDLE_FILE_INFORMATION fInfo = {0};
		//GetFileInformationByHandle(hFile, &fInfo);
		//CloseHandle(hFile);
		//return fInfo.ftLastWriteTime;

		/*FILETIME creationTime,
			lpLastAccessTime,
			lastWriteTime;
		OFSTRUCT TestOF;
		HANDLE hFile= (HANDLE) OpenFile(filename.c_str(),&TestOF, OF_SHARE_DENY_NONE );	
		bool err = GetFileTime( hFile, &creationTime, &lpLastAccessTime, &lastWriteTime );
		return  COleDateTime (lastWriteTime);*/
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
	bool IsDirectory(std::string path)
	{
		//CString szPath("c:\\windows");
		//CString szFile("c:\\test.txt");
		const char * szPath=path.c_str();

		DWORD dwAttr = GetFileAttributes(szPath);
		if(dwAttr == 0xffffffff)
		{
			DWORD dwError = GetLastError();
			if(dwError == ERROR_FILE_NOT_FOUND)
			{
				return false; // file not found
			}
			else if(dwError == ERROR_PATH_NOT_FOUND)
			{
				return false;  // path not found
			}
			else if(dwError == ERROR_ACCESS_DENIED)
			{
				return false;  // file or directory exists, but access is denied
			}
			else
			{
				return false;  // some other error has occured
			}
		}
		else
		{
			if(dwAttr & FILE_ATTRIBUTE_DIRECTORY)
			{
				//// this is a directory
				return true;
				
			// Directory is part or used exclusively by the operating system
			}
			else
			{
				//// this is an ordinary file
				return false;
			}
		}
	}
	//std::vector<std::string> FolderRecurse(LPCTSTR pstr, std::string startingdir)
	//{
	//	CFindFile finder;
	//	std::string tmp;

	//	// build a string with wildcards
	//	std::string strWildcard(pstr);
	//	strWildcard += "\\*.*";

	//	// start working for files
	//	BOOL bWorking = finder.FindFile(strWildcard.c_str());

	//	while (bWorking)
	//	{
	//		bWorking = finder.FindNextFile();
	//		if(!bWorking)
	//			break;

	//		if (finder.IsDots())
	//			continue;
	//		if (finder.IsDirectory())
	//		{
	//			std::string str = finder.GetFilePath();
	//			ReplaceOnce(str, startingdir, "");
	//			str = str.substr(1);
	//			str=ReplaceOnce(str,"\\", ".");
	//			tmp += str;
	//			tmp+=FolderRecurse(finder.GetFilePath(),startingdir);
	//			continue;
	//		}
	//		std::string str = finder.GetFilePath();
	//		str.ReplaceOnce(std, startingdir, "");
	//		str = str.Mid(1);
	//		str.Replace("\\", ".");
	//		str.Replace(".txt", "");
	//		str+="\n";
	//		tmp += str;
	//	}
	//	finder.Close();
	//	return tmp;
	//}
	std::vector<std::string> FileList(std::string folder)
	{
		std::vector<std::string> files;

		DIR *dp;
		struct dirent *dirp;
		if (folder[folder.length()-1] != '\\') 
			folder += "\\";

		if((dp = opendir(folder.c_str())) == NULL) 
		{
//			std::stringstream ss (std::stringstream::in);
//			ss<< "Error(" << errno << ") opening " << folder << std::endl;
//			throw ss.str();
			return files;
		}

		while ((dirp = readdir(dp)) != NULL) 
		{
			if((strcmp(dirp->d_name, ".") == 0) || (strcmp(dirp->d_name, "..") == 0))
				continue;
			files.push_back(std::string(folder+dirp->d_name));
		}
		closedir(dp);
		return files;
	}
	std::vector<std::string> RecursiveFindFiles(std::string path)
	{
		struct dirent *fname;
		DIR *od;   
		std::string filename;
		std::vector<std::string> files;

		if (path[path.length()-1] != '\\') 
			path += "\\";
		od = opendir(path.c_str());

		while((od != NULL) && (fname = readdir(od)) != NULL)
		{
			if((strcmp(fname->d_name, ".") != 0) && (strcmp(fname->d_name, "..") != 0))
			{
				if(fname->d_type == DT_DIR)
				{
					filename = path + fname->d_name; 
					std::vector<std::string> pathfiles = RecursiveFindFiles(filename);
					files.insert(files.begin(), pathfiles.begin(), pathfiles.end());
				}

				if(fname->d_type == DT_REG)
				{
					files.push_back(std::string(path + fname->d_name));
				}
			}
		}
		closedir(od);
		return files;
	}

};
__declspec(selectany)  CFile File;

