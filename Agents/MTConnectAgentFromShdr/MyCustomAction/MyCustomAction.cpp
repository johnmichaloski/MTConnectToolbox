// MyCustomAction.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
//#include "atlstr.h"

// Embedded in this cpp file
//#include "MSI_Logging.h"
//#include "RunProcess.h"
//#include "FilPermission.h"

#include "StdStringFcn.h"
#include "YamlReader.h"

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
	std::string path;
	std::string devices,types,ipaddr,shdrports;
	std::string status;
	std::string ServiceName="MTConnectAgent";
	std::string httpPort="5000";

	try {
		ipaddr="127.0.0.1";

		TCHAR szBuffer1[MAX_PATH] = {'0'};
		DWORD dwLen = MAX_PATH;

		// Returns ERROR_SUCCESS, ERROR_MORE_DATA, ERROR_INVALID_HANDLE, ERROR_BAD_ARGUMENTS
		hRes = MsiGetProperty(hInstall, "CustomActionData", szBuffer1, &dwLen);
		std::vector<std::string> symbols = TrimmedTokenize(std::string(szBuffer1), "$");


		//$Devices=[EDITA1]$Types=[EDITA2]$Ips=[EDITA3]$Ports=[EDITA4]$ServiceName=[EDITB1]$HttpPort=[EDITB2]$Target=[TARGETDIR]
		DbgOut("Parse symbols");
		for(int i=0 ; i< symbols.size() ; i++)
		{
			std::vector<std::string> tokens=TrimmedTokenize(symbols[i],"=");
			if(tokens.size() < 2)
				continue;
			if(tokens[0]=="Target")
				path=tokens[1];
			// Ini file parameters
			if(tokens[0]=="HttpPort") 
				httpPort=tokens[1];     
			if(tokens[0]=="Ips") 
				ipaddr=tokens[1];      
			if(tokens[0]=="Types") 
				types=tokens[1];   
			if(tokens[0]=="Devices") 
				devices=tokens[1];    
			if(tokens[0]=="Ports") 
				shdrports=tokens[1];    
			if(tokens[0]=="ServiceName") 
				ServiceName=tokens[1]; 
		
		}
		//RunSynchronousProcess("sc.exe", " stop MTConnectAgent");
		//RunSynchronousProcess("sc.exe", " delete MTConnectAgent");
		//RunSynchronousProcessUAC("sc.exe", " stop "+ ServiceName, "");
		//::Sleep(3000);
		//RunSynchronousProcessUAC("sc.exe", " delete "+ ServiceName, "");

		SetFilePermission(path+"Agent.log") ;
		SetFilePermission(path+"agent.exe") ;
		SetFilePermission(path+"Agent.cfg") ;
		SetFilePermission(path+"Config.ini") ;
		SetFilePermission(path+"debug.txt") ;
		SetFilePermission(path+"Devices.xml") ;
		SetFilePermission(path+"Install.bat") ;
		SetFilePermission(path+"MTConnectPage-1.4.vbs") ;
		SetFilePermission(path+"RunAgent.bat") ;
		SetFilePermission(path+"superuser.bat") ;
		SetFilePermission(path+"uninstall.bat") ;
		SetFilePermission(path+"uninstall.vbs") ;

		DbgOut( "Done SetFilePermission \n");

#if 1
		std::vector<std::string> ips=TrimmedTokenize(ipaddr,",");
		std::vector<std::string> devs=TrimmedTokenize(devices,",");
		std::vector<std::string> ports=TrimmedTokenize(shdrports,",");
		std::vector<std::string> dtypes=TrimmedTokenize(types,",");

		if(ips.size() != devs.size()||
			ports.size() != devs.size()||
			dtypes.size() != devs.size())
		{
			std::string s = StdStringFormat("Mismatched # ips types port and/or devices: ips = %d  devs = %d", ips.size(), devs.size());
			::MessageBox(NULL, s.c_str(), "Error", MB_OK);
			return  ERROR_INSTALL_USEREXIT;
		}
#endif
		WritePrivateProfileString("GLOBALS", "Config", "NEW", (path + "Config.ini" ).c_str( ) );
		WritePrivateProfileString("GLOBALS", "MTConnectDevice", devices.c_str(), (path + "Config.ini" ).c_str( ) );
		WritePrivateProfileString("GLOBALS", "DeviceType", types.c_str(), (path + "Config.ini" ).c_str( ) );
		WritePrivateProfileString("GLOBALS", "Port", shdrports.c_str(), (path + "Config.ini" ).c_str( ) );
		WritePrivateProfileString("GLOBALS", "Ip", ipaddr.c_str(), (path + "Config.ini" ).c_str( ) );
		WritePrivateProfileString("GLOBALS", "HttpPort", httpPort.c_str(), (path + "Config.ini" ).c_str( ) );
		WritePrivateProfileString("GLOBALS", "ServiceName", ServiceName.c_str(), (path + "Config.ini" ).c_str( ) );

#if 0
		std::string xmldevices = _xmlconfig.WriteDevicesFileXML(devs);
		_xmlconfig.WriteDevicesFile("Devices.xml", path, xmldevices );

		_xmlconfig.WriteAgentCfgFile("Agent.cfg", "Devices.xml", path, httpPort,	devs, ips,  ports,ServiceName);
		// Make all files read writable by everyone - assumes installatino by administrator
		RunSynchronousProcess(path+"superuser.bat","");
#endif
		// Install rvice  
		//RunSynchronousProcess(path+"Agent.exe", " install");
		//RunSynchronousProcessUAC("Agent.exe", " install", path);
		//RunSynchronousProcessUAC("Install.bat", "", path);

		// This works
		RunSynchronousProcessUAC("sc.exe", " create "+ ServiceName + " start= auto binpath= \"" + path+"Agent.exe\"", "");

		// Start service  
		//RunSynchronousProcessUAC("sc.exe", " start "+ ServiceName, "");

	}
	catch(...)
	{
		::MessageBox(NULL,("Custom install exception\n"+status).c_str(), "Error", MB_OK);
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

	HRESULT hRes = ::CoInitialize(NULL);

	std::string path;
	std::string ServiceName="MTConnectAgent";

	TCHAR szBuffer1[MAX_PATH] = {'0'};
	DWORD dwLen = MAX_PATH;

	hRes = MsiGetProperty(hInstall, "CustomActionData", szBuffer1, &dwLen);
	std::vector<std::string> symbols = TrimmedTokenize(std::string(szBuffer1), "/");

	// Parse msi properties - guesss they are preserved?
	for(int i=0 ; i< symbols.size() ; i++)
	{
		std::vector<std::string> tokens=Tokenize(symbols[i],"=");
		if(tokens.size() < 2)
			continue;
		if(tokens[0]=="Target")
			path=tokens[1];
		if(tokens[0]=="ServiceName") 
			ServiceName=tokens[1]; 
	}

	try{
		std::string str;
		YamlReader yaml;
		yaml.LoadFromFile(path+"Agent.cfg");
		ServiceName=yaml.Find("ROOT.ServiceName");
		if(!ServiceName.empty())
		{
			RunSynchronousProcessUAC("sc.exe", " stop "+ServiceName, "");
			::Sleep(3000);
			RunSynchronousProcessUAC("sc.exe", " delete "+ServiceName, "");
		}
	}
	catch(...)
	{
		::MessageBox(NULL,"Custom uninstall exception\n", "Error", MB_OK);
	}

	::CoUninitialize();
	// Returns ERROR_SUCCESS, ERROR_MORE_DATA, ERROR_INVALID_HANDLE, ERROR_BAD_ARGUMENTS
	return ERROR_SUCCESS;
}
extern "C" UINT __stdcall Rollback(MSIHANDLE hInstall)
{


	return ERROR_SUCCESS;
}


