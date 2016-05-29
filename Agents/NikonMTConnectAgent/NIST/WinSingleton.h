
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

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

// Add this to main cpp or where other libraries are included
//#pragma comment(lib, "Psapi.lib")


	/**
	* \brief Tries to determine if an existing MTConnect agent (same name and same http port) exists. 
	* Uses windows psapi lib function (difference version 1 & 2) to determine processes and corresponding process  exe image.
	* Given matching process name to current, look up image folder, read agent.cfg (assuming this is it!) and see what http port it is using.
	* If two exe's exist with same name and same http port - Notify user of potential problem.
	* Example:
	<PRE>
	   MTConnectSingleton singleton;
    std::vector<std::string>  filepaths;
    DWORD  pid = singleton.GetPid();
    std::string modulename = singleton.GetModuleName();
    std::string modulepath = singleton.GetModulePath();
    std::string processname  = singleton.ExtractFilename(argv[0]);
    std::vector<DWORD> pids = singleton.GetModules(processname,  filepaths);

	if(filepaths.size() > 1) // should always be one
	{
		YamlReader  yaml;
		std::string exefolder = singleton.ExtractDirectory(modulepath);
		std::string filename = exefolder + "\\Agent.cfg";
		yaml.LoadFromFile(filename);
		std::string portnum = yaml.Find("GLOBAL.Port");

			for(size_t i=0; i< filepaths.size(); i++)
			{
				if(pids[i] == pid)
					continue;
				std::string exefolder = singleton.ExtractDirectory(filepaths[i]);
				filename = exefolder + "\\Agent.cfg";
				yaml.LoadFromFile(filename);
				std::string portnum2 = yaml.Find("GLOBAL.Port");
				if(portnum==portnum2)
				{
					GLogger.Fatal("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
					GLogger.Fatal("Conflict with another MTConnect agent with same exe name and http port number\n");
					GLogger.Fatal("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				}

			}
	}
	</PRE>
	If a problem will see:
	<PRE>
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Conflict with another MTConnect agent with same exe name and http port number
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	</PRE>
	*/

struct MTConnectSingleton
{

	/**
	* \brief Constructor.. 
	*/
	MTConnectSingleton()
	{

	}
	/**
	* \brief Abstraction for windows pid fetch for current process. 
	*/
	DWORD GetPid()
	{
		return GetCurrentProcessId();
	}
	/**
	* \brief Get current process module name, usually short name with exe. 
	*/
	std::string GetModuleName()
	{
		TCHAR buf[1000];
		GetModuleFileName(NULL, buf, 1000);
		return ExtractFilename(std::string(buf));
	}
	/**
	* \brief Get current process full module path to exe. 
	*/
	std::string GetModulePath()
	{
		TCHAR buf[1000];
		GetModuleFileName(NULL, buf, 1000);
		return buf;
	}
	/**
	* \brief Extracts directory from a path. aka up a level or parent.
	*/
	std::string ExtractDirectory( const std::string& path )
	{
		return path.substr( 0, path.find_last_of( '\\' ) +1 );
	}
		/**
	* \brief Extracts filename from a path. That is filename at end of path is extracted.
	*/
	std::string ExtractFilename( const std::string& path )
	{
		return path.substr( path.find_last_of( '\\' ) +1 );
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
};