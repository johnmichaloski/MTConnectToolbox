// MyCustomAction.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
//#include "MSI_Logging.h"
#include "StdStringFcn.h"
//#include "atlstr.h"

#pragma comment(lib, "msi.lib") 
static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg =  StrFormat("In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode);
	OutputDebugString(errmsg.c_str() );
	throw std::exception(errmsg.c_str() , pExp->ExceptionRecord->ExceptionCode);
} 

//////////////////////////////////////////////////////////////////////////////////////

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved )
{
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////

#define DbgOut OutputDebugString
#include "Aclapi.h"
void DbgOutError(DWORD   dwLastError, LPCTSTR errmsg)
{
	TCHAR   lpBuffer[256] = "?";
	if(dwLastError != 0)    // Don't want to see a "operation done successfully" error ;-)
		::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,                 // It´s a system error
		NULL,                                      // No string to be formatted needed
		dwLastError,                               // Hey Windows: Please explain this error!
		MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),  // Do it in the standard language
		lpBuffer,              // Put the message here
		strlen(lpBuffer)-1,                     // Number of bytes to store the message
		NULL);
	strncat(lpBuffer,errmsg, MIN(strlen(lpBuffer)-256, strlen(errmsg)));
	DbgOut(lpBuffer);

}

static void SetFilePermission(std::string szFilename) 
{ 
	LPCTSTR FileName= szFilename.c_str();
	DbgOut(StrFormat("SetFilePermission %s", FileName).c_str());

	PSID pEveryoneSID = NULL; 
	BOOL a= TRUE;
	BOOL b= TRUE;
	BOOL c= TRUE;
	DWORD d= 0;
	DWORD ret = 0;
	PACL pACL = NULL; 
	EXPLICIT_ACCESS ea[1]; 
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY; 

	// Create a well-known SID for the Everyone group. 
	a = AllocateAndInitializeSid(&SIDAuthWorld, 1, 
		SECURITY_WORLD_RID, 
		0, 0, 0, 0, 0, 0, 0, 
		&pEveryoneSID); 
	if(!a)
		DbgOut("SID initialization failed");
	// Initialize an EXPLICIT_ACCESS structure for an ACE. 
	// The ACE will allow Everyone read access to the key. 
	ZeroMemory(&ea, 1 * sizeof(EXPLICIT_ACCESS)); 
	ea[0].grfAccessPermissions = 0xFFFFFFFF; 
	ea[0].grfAccessMode = GRANT_ACCESS; 
	ea[0].grfInheritance= NO_INHERITANCE; 
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID; 
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP; 
	ea[0].Trustee.ptstrName = (LPTSTR) pEveryoneSID; 

	// Create a new ACL that contains the new ACEs. 
	ret = SetEntriesInAcl(1, ea, NULL, &pACL); 

	if(ret != ERROR_SUCCESS)
		DbgOut("ACL entry failed");

	// Initialize a security descriptor.  
	PSECURITY_DESCRIPTOR pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, 
		SECURITY_DESCRIPTOR_MIN_LENGTH); 

	b = InitializeSecurityDescriptor(pSD,SECURITY_DESCRIPTOR_REVISION); 
	if(!b)
		DbgOutError(::GetLastError(),"Security Descriptor initialization failed");

	// Add the ACL to the security descriptor. 
	c = SetSecurityDescriptorDacl(pSD, 
		TRUE,   // bDaclPresent flag  
		pACL, 
		FALSE);  // not a default DACL 
	
	// If the function SetSecurityDescriptorDacl succeeds, the function returns nonzero.
	if(c==0)
	{
		DbgOutError(::GetLastError(),"SetSecurityDescriptorDacl failed");
	}

	//Change the security attributes 
	d = SetFileSecurity(FileName, DACL_SECURITY_INFORMATION, pSD); 
	//       SetNamedSecurityInfo("C:\\Program Files\\Mydir\\My.log",SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,pSD,NULL,NULL,NULL);    
	
	// If the SetFileSecurity function succeeds, the function returns nonzero.  
	if(d == 0)
		DbgOutError(::GetLastError(),"SetFileSecurity failed");
	
	if (pEveryoneSID) 
		FreeSid(pEveryoneSID); 
	if (pACL) 
		LocalFree(pACL); 
	if (pSD) 
		LocalFree(pSD); 
}

//////////////////////////////////////////////////////////////////////////////////////
static void RunSynchronousProcess(std::string cmd, std::string args, int timeout=4000)
{
	STARTUPINFO         siStartupInfo;
	PROCESS_INFORMATION piProcessInfo;

	memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	memset(&piProcessInfo, 0, sizeof(piProcessInfo));

	siStartupInfo.cb = sizeof(siStartupInfo);

	if(CreateProcess(cmd.c_str(),     // Application name
		(LPSTR) args.c_str(),                 // Application arguments
		0,
		0,
		FALSE,
		CREATE_DEFAULT_ERROR_MODE,
		0,
		0,                              // Working directory
		&siStartupInfo,
		&piProcessInfo) != 0)
	{
		WaitForSingleObject (piProcessInfo.hProcess, timeout);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
extern "C" UINT __stdcall Install(MSIHANDLE hInstall)
{

	HRESULT hRes = ::CoInitialize(NULL);
	std::string path,serviceName,fanucIP;
	std::string status;

	try {
		TCHAR szBuffer1[MAX_PATH] = {'0'};
		DWORD dwLen = MAX_PATH;
		status="MsiGetProperty";
		// Returns ERROR_SUCCESS, ERROR_MORE_DATA, ERROR_INVALID_HANDLE, ERROR_BAD_ARGUMENTS
		hRes = MsiGetProperty(hInstall, "CustomActionData", szBuffer1, &dwLen);
		std::vector<std::string> symbols = TrimmedTokenize(std::string(szBuffer1), "$");

		//	MessageBox(NULL, std::string(szBuffer1).c_str(), "Info", MB_OK);

		RunSynchronousProcess("sc.exe", " stop MTCFanucAdapter");
		RunSynchronousProcess("sc.exe", " delete MTCFanucAdapter");

		status="Parse symbols";
		//$Target=[TARGETDIR]$ServiceName=[EDITA2]$IP=[EDITA1]
		for(int i=0 ; i< symbols.size() ; i++)
		{
			std::vector<std::string> tokens=TrimmedTokenize(symbols[i],"=");
			if(tokens.size() < 2)
				continue;
			if(tokens[0]=="Target")
				path=tokens[1];
			if(tokens[0]=="ServiceName") // no change necessary if using csv, program will stip out
				serviceName=Trim(tokens[1]);    
 			if(tokens[0]=="IP") // no change necessary if using csv, program will stip out
				fanucIP=Trim(tokens[1]);    

			}

		SetFilePermission(path+"debug.txt") ;
		SetFilePermission(path+"MTCFanucAdapter.ini") ;
		SetFilePermission(path+"MTCFanucAdapter.exe") ;
		SetFilePermission(path+"InstallAdapter.bat") ;
		SetFilePermission(path+"RunAdapter.bat") ;
		SetFilePermission(path+"superuser.bat") ;
		SetFilePermission(path+"uninstallService.vbs") ;

		//::MessageBox(NULL,ipaddr.c_str(), "ALERT", MB_OK);
		WritePrivateProfileString("CONFIG", "ServiceName", serviceName.c_str(), (path+"MTCFanucAdapter.ini").c_str());
		WritePrivateProfileString("CONFIG", "FanucIpAddress", fanucIP.c_str(), (path+"MTCFanucAdapter.ini").c_str());
		//WritePrivateProfileString("CONFIG", "DeviceName", deviceName.c_str(), (path+"MTCFanucAdapter.ini").c_str());
		::Sleep(4000);
		//::MessageBox(NULL,"Create service with MTCFanucAdapter.exe", "ALERT", MB_OK);
		// Install and start service  
		status="Create service with MTCFanucAdapter.exe";
		//RunSynchronousProcess(path+"MTCFanucAdapter.exe", " install");

		RunSynchronousProcess(path+"InstallAdapter.bat","");
		::Sleep(2000);
		//RunSynchronousProcess("sc.exe", " start MTCFanucAdapter");
	}
	catch(...)
	{
		//LogString(hInstall, (TCHAR*)("Custom install exception\n"+status).c_str());
	}
	::CoUninitialize();


	return hRes==IDOK;
}
//////////////////////////////////////////////////////////////////////////////////////
extern "C" UINT __stdcall Commit(MSIHANDLE hInstall)
{
	return ERROR_SUCCESS;
}



//////////////////////////////////////////////////////////////////////////////////////
// From: https://msdn.microsoft.com/en-us/library/ms686335(VS.85).aspx
bool StopService(const char * servicename)
{
	SERVICE_STATUS_PROCESS ssp;
	DWORD dwStartTime = GetTickCount();
	DWORD dwBytesNeeded;
	DWORD dwTimeout = 3000; // 3-second time-out
	DWORD dwWaitTime;
	SC_HANDLE  schSCManager, schService;
	bool bFlag=true;

	// Get a handle to the SCM database. 

	schSCManager = OpenSCManager( 
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager) 
	{
		//printf("OpenSCManager failed (%d)\n", GetLastError());
		return false;
	}

	// Get a handle to the service.
	schService = OpenService( 
		schSCManager,         // SCM database 
		servicename,            // name of service 
		SERVICE_STOP | 
		SERVICE_QUERY_STATUS | 
		SERVICE_ENUMERATE_DEPENDENTS);  

	if (schService == NULL)
	{ 
		//printf("OpenService failed (%d)\n", GetLastError()); 
		CloseServiceHandle(schSCManager);
		return false;
	}    

	// Make sure the service is not already stopped.
	if ( !QueryServiceStatusEx( 
		schService, 
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssp, 
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded ) )
	{
		// printf("QueryServiceStatusEx failed (%d)\n", GetLastError()); 
		bFlag=false;
		goto stop_cleanup;
	}

	// Make sure the service is not already stopped.
	if ( !QueryServiceStatusEx( 
		schService, 
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssp, 
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded ) )
	{
		bFlag=false;
		//printf("QueryServiceStatusEx failed (%d)\n", GetLastError()); 
		goto stop_cleanup;
	}

	if ( ssp.dwCurrentState == SERVICE_STOPPED )
	{
		//printf("Service is already stopped.\n");
		bFlag=true;
		goto stop_cleanup;
	}

	// If a stop is pending, wait for it.

	while ( ssp.dwCurrentState == SERVICE_STOP_PENDING ) 
	{
		//printf("Service stop pending...\n");

		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth of the wait hint but not less than 1 second  
		// and not more than 10 seconds. 

		dwWaitTime = ssp.dwWaitHint / 10;

		if( dwWaitTime < 1000 )
			dwWaitTime = 1000;
		else if ( dwWaitTime > 10000 )
			dwWaitTime = 10000;

		Sleep( dwWaitTime );

		if ( !QueryServiceStatusEx( 
			schService, 
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp, 
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded ) )
		{
			// printf("QueryServiceStatusEx failed (%d)\n", GetLastError()); 
			bFlag=false;
			goto stop_cleanup;
		}

		if ( ssp.dwCurrentState == SERVICE_STOPPED )
		{
			//printf("Service stopped successfully.\n");
			bFlag=true;
			goto stop_cleanup;
		}

		if ( GetTickCount() - dwStartTime > dwTimeout )
		{
			//printf("Service stop timed out.\n");
			bFlag=false;

			goto stop_cleanup;
		}
	}

	// If the service is running, dependencies must be stopped first.
	// StopDependentServices();

	// Send a stop code to the service.
	if ( !ControlService( 
		schService, 
		SERVICE_CONTROL_STOP, 
		(LPSERVICE_STATUS) &ssp ) )
	{
		//printf( "ControlService failed (%d)\n", GetLastError() );
		bFlag=false;

		goto stop_cleanup;
	}

	// Wait for the service to stop.

	while ( ssp.dwCurrentState != SERVICE_STOPPED ) 
	{
		Sleep( ssp.dwWaitHint );
		if ( !QueryServiceStatusEx( 
			schService, 
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp, 
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded ) )
		{
			//printf( "QueryServiceStatusEx failed (%d)\n", GetLastError() );
			bFlag=false;
			goto stop_cleanup;
		}

		if ( ssp.dwCurrentState == SERVICE_STOPPED )
			break;

		if ( GetTickCount() - dwStartTime > dwTimeout )
		{
			// printf( "Wait timed out\n" );
			bFlag=false;
			goto stop_cleanup;
		}
	}
	//printf("Service stopped successfully\n");
	bFlag=true;

stop_cleanup:
	CloseServiceHandle(schService); 
	CloseServiceHandle(schSCManager);
	return bFlag;
}


extern "C" UINT __stdcall Uninstall (MSIHANDLE hInstall)
{
	HRESULT hRes = ::CoInitialize(NULL);
	try{

		std::string path;

		TCHAR szBuffer1[MAX_PATH] = {'0'};
		DWORD dwLen = MAX_PATH;

		// Returns ERROR_SUCCESS, ERROR_MORE_DATA, ERROR_INVALID_HANDLE, ERROR_BAD_ARGUMENTS
		hRes = MsiGetProperty(hInstall, "CustomActionData", szBuffer1, &dwLen);
		std::vector<std::string> symbols = TrimmedTokenize(std::string(szBuffer1), "/");

		for(int i=0 ; i< symbols.size() ; i++)
		{
			std::vector<std::string> tokens=Tokenize(symbols[i],"=");
			if(tokens.size() < 2)
				continue;
			if(tokens[0]=="Target")
				path=tokens[1];
		}

		// Would be better to read agent.cfg but that requires yaml which is going overboard here
		TCHAR servicename[256];
		GetPrivateProfileString("CONFIG", "ServiceName",  "MTCFanucAdapter", servicename, 256, (path+"MTCFanucAdapter.ini").c_str());
		std::string serviceName(servicename);

		// This may not work, but not worth adding extra pipe code to read service status
		//RunSynchronousProcess("sc.exe", (" stop " + serviceName).c_str());
		StopService(serviceName.c_str());
		::Sleep(3000);
		RunSynchronousProcess("sc.exe", (" delete " + serviceName).c_str());
	}
	catch(...)
	{

	}
	::CoUninitialize();
	return ERROR_SUCCESS;
}
extern "C" UINT __stdcall Rollback(MSIHANDLE hInstall)
{


	return ERROR_SUCCESS;
}


