
//
// WinSingleton.h
//

/*
* DISCLAIMER:
* This software was produced by the National Institute of Standards
* and Technology (NIST), an agency of the U.S. government, and by statute is
* not subject to copyright in the United States.  Recipients of this software
* assume all responsibility associated with its operation, modification,
* maintenance, and subsequent redistribution.
*
* See NIST Administration Manual 4.09.07 b and Appendix I.
*/
#pragma once
#include <string>
#include <Psapi.h>
#include "StdStringFcn.h"
#include "File.h"
#include "YamlReader.h"

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

// Add this to main cpp or where other libraries are included
#pragma comment(lib, "Psapi.lib")
template<typename T>
struct MTConnectSingleton
{
	bool bSingleton;

	/**
	* \brief Constructor.. 
	*/
	MTConnectSingleton()
	{
		bSingleton=false;
	}
	/**
	* \brief Abstraction for windows pid fetch for current process. 
	*/
	DWORD GetPid()
	{
		return ::GetCurrentProcessId();
	}
	/**
    * \brief Extracts directory from a path. aka up a level or parent.
    */
    std::string ExtractDirectory (const std::string & path)
    {
        return path.substr(0, path.find_last_of('\\') + 1);
    }

	/**
	* \brief Extracts filename from a path. That is filename at end of path is
	* extracted.
	*/
	std::string ExtractFilename (const std::string & path)
	{
		return path.substr(path.find_last_of('\\') + 1);
	}

	/**
	* \brief Searches task manager for modules matching a processname. 
	* \param filepaths is a vector containing the path to exe matching the processname.
	* \return vector of pids of executables that match processname.
	*/
	std::vector<DWORD> GetModules(std::string processname, std::vector<std::string> & filepaths  )
	{
		DWORD proc_id[1024];    // array for process id's
		DWORD ret_bytes;        // number of bytes returned from EnumProcesses()
		processname=MakeLower(processname);
		std::vector<DWORD> handles;
		// Get list of process id's
		if ( !EnumProcesses( proc_id, sizeof(proc_id), &ret_bytes ) )
		{
			OutputDebugString("Cannot execute EnumProcesses()...\n");
			return handles; // default to true
		}

		// Retriving process id list
		// Calculate how many process identifiers were returned.
		DWORD cProcesses = ret_bytes / sizeof(DWORD);   /**< number of processes */ 

		// Read all the process' names
		for (unsigned int i=0;i<cProcesses;i++)
		{
			if (proc_id[i]!=0) // if the id is not empty
			{
				TCHAR proc_name[MAX_PATH] = TEXT("<unknown>"); // array for storing name of process
				TCHAR file_name[MAX_PATH] = TEXT("<unknown>"); // array for executable of process
				TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS , false, proc_id[i]);   // open the process
				if(hProcess==NULL) 	
					continue; // Problem

				HMODULE hMod;
				DWORD cbNeeded;
				if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod),	&cbNeeded) )
				{
					GetModuleBaseName( hProcess, hMod, szProcessName, 
						sizeof(szProcessName)/sizeof(TCHAR) );
					if(processname.find(MakeLower(szProcessName))!=0)
						continue;
					handles.push_back(proc_id[i]);

					DWORD size=sizeof(file_name)/sizeof(TCHAR);
					QueryFullProcessImageName(hProcess,0,file_name,&size);   // Get the name of the image base file
					filepaths.push_back(file_name);
				}

			}
		}
		return handles; // default to true
	}

	int GetPortNum(std::string exepath)
	{
		YamlReader yaml;
		std::string exefolder = ExtractDirectory(exepath);
		std::string filename = exefolder + "\\Agent.cfg";
		yaml.LoadFromFile(filename);
		std::string portnum2 = yaml.Find("GLOBAL.Port");
		return yaml.Convert<int>(portnum2,0);
	}
	bool IsSameAgent(std::string exepath, int portnum)
	{
        //T * service = (T*) this;
		int portnum2= GetPortNum( exepath);
		return portnum2==portnum;
	}

	BOOL TerminateAgentProcess(DWORD dwProcessId, UINT uExitCode)
	{
		DWORD dwDesiredAccess = PROCESS_TERMINATE;
		BOOL  bInheritHandle  = FALSE;
		HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
		if (hProcess == NULL)
			return FALSE;

		BOOL result = TerminateProcess(hProcess, uExitCode);

		CloseHandle(hProcess);

		return result;
	}
	void KillAllOtherInstances()
	{
		if(!bSingleton)
			return;
		std::vector<std::string>  filepaths;
		DWORD  pid = GetPid();
		std::string modulename = File.ExeDirectory();  // f
		std::string modulepath = File.ExePath();;
		std::string processname  = File.Filename(modulepath);
		std::vector<DWORD> pids = GetModules(processname,  filepaths);
		int portnum = GetPortNum(modulename);
		for(size_t i=0; i<pids.size(); i++)
		{
			if(pid == pids[i])
				continue;
			bool b = IsSameAgent(filepaths[i], portnum);
			if(b)
			{
				logStatus("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                logStatus("Conflict with another MTConnect Agent with same exe name and http port number\n");
				logStatus("Terminated process %s\n", filepaths[i].c_str());
                logStatus("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				TerminateAgentProcess(pids[i], 0);
			}

		}
	}
};