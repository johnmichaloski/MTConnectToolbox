//
// ListExeProcesses.h
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.


#pragma once

#include "psapi.h"
#include <stdlib.h>     /* getenv */
#include "StdStringFcn.h"

#include <tlhelp32.h>

class ExeList
{
public:
	DWORD cProcesses;
	DWORD aProcesses[1024];
	////////////////////////////////////////////////////////////////
	// Look in  HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths 
	void BuildAppList(std::vector<std::string> & appPaths, std::string key, std::string IS_KEY = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths")
	{
		CRegKey reg;
		if (reg.Open(HKEY_LOCAL_MACHINE, IS_KEY.c_str(), KEY_READ) != ERROR_SUCCESS)
			return AtlTrace("Fail ; reg.Open(HKEY_LOCAL_MACHINE, IS_KEY, KEY_READ)");

		DWORD dwIndex = 0;
		DWORD cbName = IS_KEY.size();
		TCHAR * szSubKeyName = new TCHAR [cbName];
		LONG lRet;

		while ((lRet = reg.EnumKey(dwIndex, szSubKeyName, &cbName)) != ERROR_NO_MORE_ITEMS)
		{
			if (lRet!= ERROR_SUCCESS)
				return AtlTrace("Fail ; reg.EnumKey(dwIndex, szSubKeyName, &cbName)) != ERROR_NO_MORE_ITEMS)");
			CRegKey regItem;
			TCHAR szBuffer[FILENAME_MAX]={0};
			ULONG cchBuffer = FILENAME_MAX;
			if (regItem.Open(reg, szSubKeyName, KEY_READ) == ERROR_SUCCESS)
			{
				LONG errhr;
				//if ((errhr=regItem.QueryStringValue("", szBuffer, &cchBuffer)) == ERROR_SUCCESS)
				if ((errhr=regItem.QueryStringValue("Path", szBuffer, &cchBuffer)) == ERROR_SUCCESS)
				{
					// Add to the array
					appPaths.push_back(szBuffer);
				}
			}

			dwIndex++;
			cbName = IS_KEY.size();
		}
	}
	bool ExeExists(std::string shortexename)
	{
		std::string descr = getenv("PATH");
		std::vector<std::string> paths = TrimmedTokenize(descr,";");
		for(int i=0; i< paths.size(); i++)
		{
			if(FileExists(paths[i]+"\\"+shortexename))
				return true;
		}
		std::vector<std::string>  appPaths;
		BuildAppList(appPaths, shortexename) ; 
		for(int i=0; i< appPaths.size(); i++)
		{
			if(FileExists(appPaths[i]+shortexename))
				return true;
		}		
		return false;
	}
	bool Find(std::string exename)
	{
		// Get the list of process identifiers.  
		DWORD cbNeeded ;
		unsigned int i;

		//This returns a list of handles to processes running on the system as an array.
		if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
			return false;

		// Calculate how many process identifiers were returned.  
		cProcesses = cbNeeded / sizeof(DWORD);

		// Display the name and process identifier for each process.  
		for ( i = 0; i < cProcesses; i++ )
		{
			if( aProcesses[i] != 0 )
			{
				HMODULE hMod;
				cbNeeded=0;
				HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
					PROCESS_VM_READ,
					FALSE, aProcesses[i] ); 
				if (NULL == hProcess )
					continue;
				if(EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
				{
					TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
					GetModuleBaseName( hProcess, hMod, szProcessName,
						sizeof(szProcessName)/sizeof(TCHAR) );
					if(0==stricmp(exename.c_str(), szProcessName))
						return true;
				}
				CloseHandle( hProcess );
			}
		}
		return false;
	}
	HRESULT Launch( std::string exename, int nWait=0 /*INFINITE*/  )
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );


		// Start the child process. 
		if( !CreateProcess( NULL,   // No module name (use command line)
			(LPSTR) exename.c_str(),        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi )           // Pointer to PROCESS_INFORMATION structure
			) 
		{
			AtlTrace( "CreateProcess failed (%d).\n", GetLastError() );
			return E_FAIL;
		}

		// Wait until child process exits.
		if(nWait)
			WaitForSingleObject( pi.hProcess, nWait );

		// Close process and thread handles. 
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
		return S_OK;
	}
	void KillProgram(std::string progname)
	{
		HANDLE hProcessSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0 );

		// Initialize the process entry structure.
		PROCESSENTRY32 ProcessEntry = { 0 };
		ProcessEntry.dwSize = sizeof( ProcessEntry );

		// Get the first process info.
		BOOL Return = FALSE;
		if(! Process32First( hProcessSnapShot,&ProcessEntry ))
			return;

		do
		{
			if (_tcsicmp(ProcessEntry.szExeFile, progname.c_str()) ==0) 
			{ 
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, ProcessEntry.th32ProcessID); //Open Process to terminate
				TerminateProcess(hProcess,0);
				CloseHandle(hProcess); //Close Handle 
				break;

			}
		}
		while( Process32Next( hProcessSnapShot, &ProcessEntry ));

		// Close the handle
		CloseHandle( hProcessSnapShot );

	}
};