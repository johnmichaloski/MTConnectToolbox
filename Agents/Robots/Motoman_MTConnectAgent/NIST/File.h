
// File.h

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#pragma once
#include "Logger.h"
#include <map>
#include <stdio.h>
#include <vector>

#include "ATLComTime.h"

/**
 * @brief The CFile class Windows oriented file manipulation
 */
class CFile
{
private:
    BY_HANDLE_FILE_INFORMATION GetFileInformation (std::string filename)
    {
        BY_HANDLE_FILE_INFORMATION fInfo = { 0 };
        HANDLE                     hFile = CreateFile(filename.c_str( ),
                                                      GENERIC_READ,          // access (read) mode
                                                      FILE_SHARE_READ | FILE_SHARE_WRITE
                                                      | FILE_SHARE_DELETE,   // share mode
                                                      NULL,                  // pointer to security attributes
                                                      OPEN_EXISTING,         // how to create
                                                      FILE_ATTRIBUTE_NORMAL, // file attributes
                                                      NULL);                 // handle to file with attributes to copy

        if ( hFile == INVALID_HANDLE_VALUE )                                 // did we succeed?
        {
            logWarn("File::Size INVALID_HANDLE_VALUE File \"%s\" \n",
                    filename.c_str( ));
            return fInfo;
        }
        GetFileInformationByHandle(hFile, &fInfo);
        CloseHandle(hFile);
        return fInfo;
    }

public:
    bool Exists (std::string filename)
    {
        HANDLE hFile = CreateFile(filename.c_str( ),
                                  GENERIC_READ,            // access (read) mode
                                  FILE_SHARE_READ | FILE_SHARE_WRITE
                                  | FILE_SHARE_DELETE,     // share mode
                                  NULL,                    // pointer to security attributes
                                  OPEN_EXISTING,           // how to create
                                  FILE_ATTRIBUTE_NORMAL,   // file attributes
                                  NULL);                   // handle to file with attributes to copy

        if ( hFile == INVALID_HANDLE_VALUE )               // did we succeed?
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

        // return boost::filesystem::exists( unc );
        // struct stat filestatus;
        //	return !stat(filename.c_str(), &filestatus );
    }

    HRESULT Size (std::string filename, DWORD & size)
    {
        size = 0;
        HANDLE hFile = CreateFile(filename.c_str( ),
                                  GENERIC_READ,            // access (read) mode
                                  FILE_SHARE_READ | FILE_SHARE_WRITE
                                  | FILE_SHARE_DELETE,     // share mode
                                  NULL,                    // pointer to security attributes
                                  OPEN_EXISTING,           // how to create
                                  FILE_ATTRIBUTE_NORMAL,   // file attributes
                                  NULL);                   // handle to file with attributes to copy

        if ( hFile == INVALID_HANDLE_VALUE )               // did we succeed?
        {
            return E_FAIL;
        }

        size = GetFileSize(hFile, NULL);
        CloseHandle(hFile);

        if ( size != INVALID_FILE_SIZE )
        {
            return S_OK;
        }
        return E_FAIL;

        // BY_HANDLE_FILE_INFORMATION fInfo = GetFileInformation (filename); // {0};
        // BY_HANDLE_FILE_INFORMATION fEmpty = {0};
        // int n=memcmp ((void *) &fInfo,(void *) &fEmpty,
        // sizeof(BY_HANDLE_FILE_INFORMATION) );
        // if(n!= 0)
        //	return (fInfo.nFileSizeHigh * (MAXDWORD+1)) + fInfo.nFileIndexLow;
        // return 0;

        // OFSTRUCT TestOF;
        // HFILE hFile= OpenFile(unc.c_str(),&TestOF, OF_SHARE_DENY_NONE );
        // if(hFile)
        //	return GetFileSize((HANDLE) hFile,  NULL);
        // return 0;

        // return boost::filesystem::file_size(unc);
        // struct stat filestatus;
        //	if(stat(filename.c_str(), &filestatus )!=0) return 0;
        // return filestatus.st_size;
    }

    COleDateTime GetFileModTime (std::string filename)
    {
        BY_HANDLE_FILE_INFORMATION fInfo  = GetFileInformation(filename); // {0};
        BY_HANDLE_FILE_INFORMATION fEmpty = { 0 };
        int                        n      = memcmp((void *) &fInfo, (void *) &fEmpty,
                                                   sizeof( BY_HANDLE_FILE_INFORMATION ));

        if ( n != 0 )
        {
            return fInfo.ftLastWriteTime;
        }
        return COleDateTime(0, 0, 0, 0, 0, 0);

        // HANDLE hFile = CreateFile(filename.c_str(),
        //	GENERIC_READ, // access (read) mode
        //	FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, // share mode
        //	NULL, // pointer to security attributes
        //	OPEN_EXISTING, // how to create
        //	FILE_ATTRIBUTE_NORMAL,// file attributes
        //	NULL); // handle to file with attributes to copy

        // if (hFile == INVALID_HANDLE_VALUE)    // did we succeed?
        // {
        //	DebugMessage(StdStringFormat("File::Size INVALID_HANDLE_VALUE File
        // \"%s\" \n", filename.c_str()));
        //	return 0;
        // }
        // BY_HANDLE_FILE_INFORMATION fInfo = {0};
        // GetFileInformationByHandle(hFile, &fInfo);
        // CloseHandle(hFile);
        // return fInfo.ftLastWriteTime;

        /*FILETIME creationTime,
                lpLastAccessTime,
                lastWriteTime;
        OFSTRUCT TestOF;
        HANDLE hFile= (HANDLE) OpenFile(filename.c_str(),&TestOF, OF_SHARE_DENY_NONE
        );
        bool err = GetFileTime( hFile, &creationTime, &lpLastAccessTime,
        &lastWriteTime );
        return  COleDateTime (lastWriteTime);*/
    }

    std::string TempDirectory ( )
    {
        TCHAR lpTempPathBuffer[MAX_PATH];
        DWORD dwRetVal = GetTempPath(MAX_PATH,             // length of the buffer
                                     lpTempPathBuffer);    // buffer for path

        if ( ( dwRetVal > MAX_PATH ) || ( dwRetVal == 0 ) )
        {
            return "C:\\TEMP";
        }
        return lpTempPathBuffer;
    }

    std::string ExeDirectory ( )
    {
        TCHAR buf[1000];

        GetModuleFileName(NULL, buf, 1000);
        std::string path(buf);
        path = path.substr(0, path.find_last_of('\\') + 1);
        return path;
    }

    std::string GetModuleName ( )
    {
        TCHAR buf[1000];

        GetModuleFileName(NULL, buf, 1000);
        return ExtractFilename(std::string(buf));
    }

    std::string Filename (std::string path)
    {
        size_t sep = path.find_last_of("\\");

        if ( sep != std::string::npos )
        {
            return path.substr(sep + 1);
        }
        return path;                                       // throw?
    }

    std::string Filetitle (std::string path)
    {
        path = Filename(path);
        size_t dot = path.find_last_of(".");

        if ( dot != std::string::npos )
        {
            return path.substr(0, dot);
        }
        return path;
    }

    std::string Extension (std::string path)
    {
        size_t dot = path.find_last_of(".");

        if ( dot != std::string::npos )
        {
            return path.substr(dot);
        }
        return "";                                         // throw?
    }

    inline std::string ExtractDirectory (const std::string & path)
    {
        return path.substr(0, path.find_last_of('\\') + 1);
    }

    std::string ExtractFilename (const std::string & path)
    {
        return path.substr(path.find_last_of('\\') + 1);
    }

    std::string ExtractFiletitle (const std::string & path)
    {
        std::string filename = path.substr(path.find_last_of('\\') + 1);
        return filename.substr(0, filename.find_last_of('.'));
    }

    std::string ChangeExtension (const std::string & path, const std::string & ext)
    {
        std::string filename = ExtractFilename(path);
        return ExtractDirectory(path)
               + filename.substr(0, filename.find_last_of('.')) + ext;
    }
};
__declspec(selectany) CFile File;
