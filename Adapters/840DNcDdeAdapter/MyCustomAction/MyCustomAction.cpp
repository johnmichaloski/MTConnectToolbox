// MyCustomAction.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
//#include "MSI_Logging.h"
#include "StdStringFcn.h"
//#include "RunProcess.h"
//#include "FilPermission.h"

//#include "atlstr.h"
//#include "Firewall.h"


#pragma comment(lib, "msi.lib") 
static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg =  StdStringFormat("In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode);
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
#if 0
static void CheckFirewall(int port=7878)
{
		_set_se_translator( trans_func );  

	CComPtr<INetFwProfile> fwProfile ;
	BOOL fwOn=false;
	// Initialize COM.
	WindowsFirewall firewall;
	try 
	{
		//long mPort = ::GetPrivateProfileInt(_T("GLOBALS"), _T("port"), 7878, (ExeDirectory() + "Config.ini").c_str());

		hr=firewall.WindowsFirewallInitialize(&fwProfile);
		hr=firewall.WindowsFirewallIsOn(fwProfile,&fwOn);
		if(SUCCEEDED(hr) && fwOn)
		{
			//EventLogger.LogEvent(PROVIDER_NAME + std::string("Firewall ON"));
			GLogger.Fatal(PROVIDER_NAME + std::string("Firewall ON\n"));
			hr=firewall.WindowsFirewallPortIsEnabled(fwProfile,port,NET_FW_IP_PROTOCOL_TCP , &fwOn);
			if(SUCCEEDED(hr) && fwOn)
			{
				std::string msg = StdStringFormat("%s Firewall ON Port %x BLOCKED\n",PROVIDER_NAME , port);
				//				EventLogger.LogEvent(msg);
				GLogger.Fatal(msg);

				if(SUCCEEDED(firewall.WindowsFirewallPortAdd(fwProfile,port,NET_FW_IP_PROTOCOL_TCP,L"MTConnect SHDR")))
				{
					std::string msg = StdStringFormat("%s Firewall ON Port %x MADE UNBLOCKED\n",PROVIDER_NAME , port);
					//EventLogger.LogEvent(msg);
					GLogger.Fatal(msg);

				}
			}
			else
			{
				std::string msg = StdStringFormat("%s Firewall ON Port %x NOT BLOCKED\n",PROVIDER_NAME , port);
				GLogger.Fatal(msg);
			}

		}
	} 
	catch (std::exception errmsg)
	{
		GLogger.Fatal(errmsg.what());

	}
	catch (...)
	{
		GLogger.Fatal("Fatal Application Exception in DDE Adapter");

	}
}
#endif
// DbgOutError(::GetLastError());
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
	DbgOut(StdStringFormat("SetFilePermission %s", FileName).c_str());

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

static void RunSynchronousProcessUAC(std::string cmd, std::string args, std::string dir)
{
	DbgOut(StdStringFormat("RunSynchronousProcessUAC %s", cmd.c_str()).c_str());
	SHELLEXECUTEINFO shExecInfo;

	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);

	shExecInfo.fMask = NULL;
	shExecInfo.hwnd = NULL;
	shExecInfo.lpVerb = "runas";
	shExecInfo.lpFile = cmd.c_str();
	shExecInfo.lpParameters = args.c_str();
	shExecInfo.lpDirectory = dir.c_str();
	shExecInfo.nShow = SW_HIDE;
	shExecInfo.hInstApp = NULL;

	ShellExecuteEx(&shExecInfo);
}

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

static void Trace(LPCSTR pszFormat, ...) 
{
	va_list Arguments;
	va_start(Arguments, pszFormat);
	CHAR pszText[16 << 10] = { 0 };
	LPSTR pszTextPointer = pszText;
	vsprintf_s(pszTextPointer, pszText + _countof(pszText) - pszTextPointer, pszFormat, Arguments);
	OutputDebugStringA(pszText);
	va_end(Arguments);
}


//////////////////////////////////////////////////////////////////////////////////////
extern "C" UINT __stdcall Install(MSIHANDLE hInstall)
{

	HRESULT hRes = ::CoInitialize(NULL);
	std::string path,ipaddr,type,config,httpPort,fanucPort;
	std::string status;

	try {
		ipaddr="0";
		type="1";
		fanucPort="0"; // hssb is port 0

		TCHAR szBuffer1[MAX_PATH] = {'0'};
		DWORD dwLen = MAX_PATH;
		status="MsiGetProperty";
		// Returns ERROR_SUCCESS, ERROR_MORE_DATA, ERROR_INVALID_HANDLE, ERROR_BAD_ARGUMENTS
		hRes = MsiGetProperty(hInstall, "CustomActionData", szBuffer1, &dwLen);
		std::vector<std::string> symbols = TrimmedTokenize(std::string(szBuffer1), "$");

		RunSynchronousProcess("sc.exe", " stop MTConnectDdeAdapter");
		RunSynchronousProcess("sc.exe", " delete MTConnectDdeAdapter");

		//$Target=[TARGETDIR]
		status="Parse symbols";
		for(int i=0 ; i< symbols.size() ; i++)
		{
			std::vector<std::string> tokens=TrimmedTokenize(symbols[i],"=");
			if(tokens.size() < 2)
				continue;
			if(tokens[0]=="Target")
				path=tokens[1];
			// Ini file parameters
			if(tokens[0]=="Type") // no change necessary if using csv, program will stip out
				type=tokens[1];
		}

		SetFilePermission(path+"agent.log") ;
		SetFilePermission(path+"debug.txt") ;
		SetFilePermission(path+"Config.ini") ;
		SetFilePermission(path+"840DNcDdeAdapter.exe") ;
		SetFilePermission(path+"InstallAdapter.bat") ;
		SetFilePermission(path+"KillAdapterTask.bat") ;
		SetFilePermission(path+"RunAdapter.bat") ;
		SetFilePermission(path+"UninstallAdapter.bat") ;

	//	RunSynchronousProcessUAC(path+"superuser.bat", "", path);

		status = "Done SetFilePermission \n";
		//::MessageBox(NULL,(path+"superuser.bat").c_str(), "ALERT", MB_OK);
		//RunSynchronousProcess(path+"superuser.bat","");
//		::Sleep(4000);
//		::MessageBox(NULL,"Create service with MTConnectDdeAdapter.exe", "ALERT", MB_OK);
		// Install and start service  
		status="Create service with MTConnectDdeAdapter.exe";
		//RunSynchronousProcess(path+"MTConnectDdeAdapter.exe", " install");
		status = "InstallAdapter.bat \n";
		RunSynchronousProcess(path+"InstallAdapter.bat","");
	//	RunSynchronousProcess("sc.exe", (" install MTConnectDdeAdapter binPath=\""+path+"MTConnectDdeAdapter.exe\"").c_str());
	//	status="sc Start service MTConnectDdeAdapter";
		status = "sc start MTConnectDdeAdapter \n";
		RunSynchronousProcess("sc.exe", " start MTConnectDdeAdapter");

		//RunSynchronousProcess("sc.exe", " start MTConnectDdeAdapter");
	}
	catch(...)
	{
		Trace(("Custom install exception\n"+status).c_str());
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
extern "C" UINT __stdcall Uninstall (MSIHANDLE hInstall)
{
	//_set_se_translator( trans_func );  // correct thread?

	HRESULT hRes = ::CoInitialize(NULL);
	std::string path;
	std::string status;

	TCHAR szBuffer1[MAX_PATH] = {'0'};
	DWORD dwLen = MAX_PATH;
	// Returns ERROR_SUCCESS, ERROR_MORE_DATA, ERROR_INVALID_HANDLE, ERROR_BAD_ARGUMENTS
	hRes = MsiGetProperty(hInstall, "CustomActionData", szBuffer1, &dwLen);
	std::vector<std::string> symbols = TrimmedTokenize(std::string(szBuffer1), "/");
	//	MessageBox(NULL, std::string(szBuffer1).c_str(), "Info", MB_OK);
	//	DebugBreak();
	for(int i=0 ; i< symbols.size() ; i++)
	{
		std::vector<std::string> tokens=Tokenize(symbols[i],"=");
		if(tokens.size() < 2)
			continue;
		if(tokens[0]=="Target")
			path=tokens[1];
	}

	/*
	TCHAR* szPropertyValue = NULL;
	DWORD cchPropertyValue = 0;

	// read the CustomActionData property which has information for my custom action
	UINT uiRet = MsiGetProperty(hInstall, TEXT("CustomActionData"), TEXT(""), &cchPropertyValue);
	if (ERROR_MORE_DATA == uiRet)
	{
	// add 1 because on return does not include terminating null in count
	++cchPropertyValue;
	szPropertyValue = new TCHAR[cchPropertyValue];
	*/
	// Returns ERROR_SUCCESS, ERROR_MORE_DATA, ERROR_INVALID_HANDLE, ERROR_BAD_ARGUMENTS

	//	MessageBox(NULL, ("Rollback" + path+"adapter.ini").c_str(), "0", MB_OK);
#if 1
	try{
		std::string str;
		//RunSynchronousProcess("sc.exe", " stop MTConnectDdeAdapter");
		//::Sleep(3000);
		RunSynchronousProcess("sc.exe", " delete MTConnectDdeAdapter");
	}
	catch(...)
	{
		Trace(("Custom uninstall exception\n"+status).c_str());

	}
#endif
	::CoUninitialize();
	return ERROR_SUCCESS;
}
extern "C" UINT __stdcall Rollback(MSIHANDLE hInstall)
{


	return ERROR_SUCCESS;
}


