// MyCustomAction.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
//#include "MSI_Logging.h"
#include "StdStringFcn.h"
//#include "RunProcess.h"
//#include "FilPermission.h"
//#include "atlstr.h"
#include "DevicesXML.h"


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
	std::string path,ipaddr,type,config,httpPort,devices;
	std::string status;
	std::string servicename="MTConnectOpcAgent";
	try {
		ipaddr="0";
		type="1";

		TCHAR szBuffer1[MAX_PATH] = {'0'};
		DWORD dwLen = MAX_PATH;
		DbgOut("MsiGetProperty");
		// Returns ERROR_SUCCESS, ERROR_MORE_DATA, ERROR_INVALID_HANDLE, ERROR_BAD_ARGUMENTS
		hRes = MsiGetProperty(hInstall, "CustomActionData", szBuffer1, &dwLen);
		std::vector<std::string> symbols = TrimmedTokenize(std::string(szBuffer1), "$");


		//$Type=[BUTTON1]$Devices=[EDITA1]$IpValue=[EDITA2]$HttpPort=[EDITA3]$Target=[TARGETDIR]
		DbgOut("Parse symbols");
		for(int i=0 ; i< symbols.size() ; i++)
		{
			std::vector<std::string> tokens=TrimmedTokenize(symbols[i],"=");
			if(tokens.size() < 2)
				continue;
			if(tokens[0]=="Target")
				path=tokens[1];
			// Ini file parameters
			if(tokens[0]=="IpValue") 
				ipaddr=tokens[1];      
			if(tokens[0]=="Devices") 
				devices=tokens[1];    
			if(tokens[0]=="ServiceName") // default LogFileAgent
				servicename=tokens[1];    
			if(tokens[0]=="HttpPort") 
				httpPort=tokens[1];     

		}
		 TCHAR   inBuf[80]; 
		 GetPrivateProfileString (TEXT("AGENT"), 
                            TEXT("ServiceName"), 
                            TEXT("MtcOpcAgent"), 
                            inBuf, 
                            80, 
                            (path+"MtcOpcAgent.ini").c_str()); 
		std::string oldservicename = inBuf;
		RunSynchronousProcess("sc.exe", StdStringFormat(" stop %s", oldservicename.c_str()));
		::Sleep(3000);
		RunSynchronousProcess("sc.exe",  StdStringFormat(" delete %s", oldservicename.c_str())); 

		SetFilePermission(path+"debug.txt") ;
		SetFilePermission(path+"Agent.log") ;
//		SetFilePermission(path+"Config.ini") ;
		SetFilePermission(path+"MtcOpcAgent.ini") ;
		SetFilePermission(path+"MtcOpcAgent.exe") ;
		SetFilePermission(path+"Agent.cfg") ;
		SetFilePermission(path+"Devices.xml") ;
		SetFilePermission(path+"MTConnectPage-1.3.vbs") ;
		SetFilePermission(path+"InstallAgent.bat") ;
		SetFilePermission(path+"UninstallAgent.bat") ;


		WritePrivateProfileString("OPCSERVER", "ServerMachineName", ipaddr.c_str(), (path+"MtcOpcAgent.ini").c_str());
		WritePrivateProfileString("OPCSERVER", "MTConnectDevice", devices.c_str(), (path+"MtcOpcAgent.ini").c_str());
		WritePrivateProfileString("AGENT", "ServiceName", servicename.c_str(), (path+"MtcOpcAgent.ini").c_str());
		WritePrivateProfileString("AGENT", "HttpPort", httpPort.c_str(), (path+"MtcOpcAgent.ini").c_str());

		status="Read MtcOpcAgent.ini";
		std::string contents; 
		ReadFile(path+"MtcOpcAgent.ini", contents);

		// Doesnt work: \n double match: ReplacePattern(contents, "\n\tServerMachineName", "\n", "\n\tServerMachineName=" + ipaddr + "\n");
		//ReplacePattern(contents, "\tServerMachineName", "\n", "\tServerMachineName=" + ipaddr + "\n");
		//ReplacePattern(contents, "\tMTConnectDevice", "\n", "\tMTConnectDevice=" + devices + "\n");
		 
		std::vector<std::string> ips=TrimmedTokenize(ipaddr,",");
		std::vector<std::string> devs=TrimmedTokenize(devices,",");
		if(ips.size() != devs.size())
		{
			::MessageBox(NULL, "Mismatched # ips and devices", "Error", MB_OK);
			return  ERROR_INSTALL_USEREXIT;
		}
		std::string tagsection="SIEMENS";
		for(int i=1; i< ips.size(); i++)
		{
			tagsection+=",SIEMENS";
		}
		//ReplacePattern(contents, "OpcTags", "\n", "OpcTags=" + tagsection + "\n");
		//::MessageBox(NULL,contents.c_str(), "Error", MB_OK);
		//WriteFile(path+"MtcOpcAgent.ini",contents);
		WritePrivateProfileString("OPCSERVER", "OpcTags", tagsection.c_str(), (path+"MtcOpcAgent.ini").c_str());

		DbgOut( "Done SetFilePermission \n");

		CDevicesXML::WriteAgentCfgFile(servicename, httpPort, "Agent.cfg", "Devices.xml", path);
		CDevicesXML::WriteDevicesFile(devs, CDevicesXML::OPCDeviceXml(),  "Devices.xml",path);

		// Install and start service  
		DbgOut("Create service with MtcOpcAgent.exe");
		//RunSynchronousProcess(path+"MtcOpcAgent.exe", " install");
		DbgOut("InstallAgent.bat \n");
		RunSynchronousProcess(path+"InstallAgent.bat","");
	//	RunSynchronousProcess("sc.exe", (" install MtcOpcAgent binPath=\""+path+"MtcOpcAgent.exe\"").c_str());

		RunSynchronousProcess("sc.exe", " start " + servicename);
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
	std::string servicename="MTConnectOpcAgent";

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
	TCHAR   inBuf[80]; 
	GetPrivateProfileString (TEXT("AGENT"), 
		TEXT("ServiceName"), 
		TEXT("MtcOpcAgent"), 
		inBuf, 
		80, 
		(path+"MtcOpcAgent.ini").c_str()); 
	std::string oldservicename = inBuf;
	RunSynchronousProcess("sc.exe", StdStringFormat(" stop %s", oldservicename.c_str()));
	::Sleep(3000);
	RunSynchronousProcess("sc.exe",  StdStringFormat(" delete %s", oldservicename.c_str())); 

#if 0
	try{
		std::string str;
		RunSynchronousProcess("sc.exe", " stop " + servicename);
		::Sleep(3000);
		RunSynchronousProcess("sc.exe", " delete " + servicename);
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


