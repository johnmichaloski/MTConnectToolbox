// MyCustomAction.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
//#include "MSI_Logging.h"
#include "StdStringFcn.h"
//#include "RunProcess.h"
//#include "FilPermission.h"

//#include "atlstr.h"
#include "Config.h"

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
static void SetFilePermission(std::string szFilename) 
{ 
	LPCTSTR FileName= szFilename.c_str();
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
		DbgOut("Security Descriptor initialization failed");

	// Add the ACL to the security descriptor. 
	c = SetSecurityDescriptorDacl(pSD, 
		TRUE,   // bDaclPresent flag  
		pACL, 
		FALSE);  // not a default DACL 
	if(!c)
		DbgOut("SetSecurityDescriptorDacl failed");

	//Change the security attributes 
	d = SetFileSecurity(FileName, DACL_SECURITY_INFORMATION, pSD); 
	//       SetNamedSecurityInfo("C:\\Program Files\\Mydir\\My.log",SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,pSD,NULL,NULL,NULL);    
	if(d == 0)
		DbgOut("SetFileSecurity failed");
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

void ChangeIniParameter(std::string path, std::string tag, std::string value)
{
	std::string contents;
	//ReadFile(path+"MtcOpcAgent.ini", contents);
	//ReplacePattern(contents, "ServerMachineName", "\n", "ServerMachineName=" + ipaddr + "\n");
	//WriteFile(path+"MtcOpcAgent.ini",contents);

	//ReadFile(path, contents);
	//ReplacePattern(contents, tag, "\n", tag + "=" + value + "\n");
	//WriteFile(path,contents);
	/*WritePrivateProfileStringA(
    __in_opt LPCSTR lpAppName,
    __in_opt LPCSTR lpKeyName,
    __in_opt LPCSTR lpString,
    __in_opt LPCSTR lpFileName
    );
	*/
	WritePrivateProfileString(
		"GLOBALS",
		tag.c_str(),
		value.c_str(),
		path.c_str()
		);

}
//////////////////////////////////////////////////////////////////////////////////////
extern "C" UINT __stdcall Install(MSIHANDLE hInstall)
{

	HRESULT hRes = ::CoInitialize(NULL);
	std::string path,agentport,devices,servicename,iplist, portlist;
	std::string status;
	std::string ServiceName = "CrclAgent";

	try {

		// Asssign defaults
		devices="M1";
		servicename="CrclAgent";
		agentport="5000";

		TCHAR szBuffer1[MAX_PATH] = {'0'};
		DWORD dwLen = MAX_PATH;
		status="MsiGetProperty";

		// Returns ERROR_SUCCESS, ERROR_MORE_DATA, ERROR_INVALID_HANDLE, ERROR_BAD_ARGUMENTS
		hRes = MsiGetProperty(hInstall, "CustomActionData", szBuffer1, &dwLen);
		std::vector<std::string> symbols = TrimmedTokenize(std::string(szBuffer1), "$");

		RunSynchronousProcess("sc.exe", " stop " + ServiceName);
		RunSynchronousProcess("sc.exe", " delete " + ServiceName);


		//
		//$ServiceName=[EDITA1]$Devices=[EDITA2]$UNCFileName=[EDITA3]$Target=[TARGETDIR]
		//
		status="Parse symbols";
		for(int i=0 ; i< symbols.size() ; i++)
		{
			std::vector<std::string> tokens=TrimmedTokenize(symbols[i],"=");
			if(tokens.size() < 2)
				continue;
			if(tokens[0]=="Target")
				path=tokens[1];
			// Ini file parameters
			if(tokens[0]=="AgentPort") 
				agentport=tokens[1];
			if(tokens[0]=="Devices") 
				devices=tokens[1];    
			if(tokens[0]=="ServiceName") 
				servicename=tokens[1];    
			if(tokens[0]=="Ips") 
				iplist=tokens[1];  			
			if(tokens[0]=="Ports") 
				portlist=tokens[1];  
		}

		status = "SetFilePermission \n";
		SetFilePermission(path+"debug.txt") ;
		SetFilePermission(path+"Agent.log") ;
		SetFilePermission(path+"Config.ini") ;
		SetFilePermission(path+"CrclAgent.exe") ;
		SetFilePermission(path+"Agent.cfg") ;
		SetFilePermission(path+"Devices.xml") ;
		SetFilePermission(path+"InstallAgent.bat") ;
		SetFilePermission(path+"RunAgent.bat") ;
		status = "Done SetFilePermission \n";

		ChangeIniParameter(path+"Config.ini", "HttpPort", agentport); //AgentPort = 5000
		ChangeIniParameter(path+"Config.ini", "ServiceName", servicename); //ServiceName=CrclAgent
		ChangeIniParameter(path+"Config.ini", "MTConnectDevice", devices); //MTConnectDevice=M,M2
	
#if 1
		std::vector<std::string> keys = TrimmedTokenize(devices, ",");
		std::vector<std::string> ips = TrimmedTokenize(iplist, ",");
		std::vector<std::string> ports = TrimmedTokenize(portlist, ",");
		for(int i=0; i< keys.size(); i++)
		{
			//if(ips.size() >= i && !ips[i].empty())
				WritePrivateProfileString(
				keys[i].c_str(),
				"Ip",
				ips[i].c_str(),
				(path+"Config.ini").c_str()
				);
			//if(ports.size() >= i && !ports[i].empty())
				WritePrivateProfileString(
				keys[i].c_str(),
				"Port",
				ports[i].c_str(),
				(path+"Config.ini").c_str()
				);
		}
		
#endif
		status="Create service with CrclAgent.exe";

		//RunSynchronousProcess(path+"CrclAgent.exe", " install");

		status = "InstallAgent.bat \n";
		//RunSynchronousProcess(path+"InstallAgent.bat","");

		//status = "sc config CrclAgent \n";
		//if(!user.empty())
		//	RunSynchronousProcess("sc.exe"," config "+ ServiceName + StrFormat(" obj= %s password= %s", user.c_str(), pw.c_str()));
		//status = "sc start CrclAgent \n";
		//RunSynchronousProcess("sc.exe", " start " + ServiceName);

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
		//RunSynchronousProcess("sc.exe", " stop CrclAgent");
		//::Sleep(3000);
		RunSynchronousProcess("sc.exe", " delete CrclAgent");
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


