// MyCustomAction.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
//#include "MSI_Logging.h"
#include "StdStringFcn.h"
//#include "RunProcess.h"
//#include "FilPermission.h"

//#include "atlstr.h"
#include "Config.h"
#include "DevicesXML.h"
#include <sstream>
#include <algorithm>
#include <numeric>  

#pragma comment(lib, "msi.lib") 
static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg =  StdStringFormat("In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode);
	OutputDebugString(errmsg.c_str() );
	throw std::exception(errmsg.c_str() , pExp->ExceptionRecord->ExceptionCode);
} 
static std::string SanitizeDeviceName(std::string name)
{
	ReplaceAll(name, " ", "_");
	return name;
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
	// If the SetFileSecurity function succeeds, the function returns nonzero.  
	
	if(d != 0)
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
	ReadFile(path, contents);
	ReplacePattern(contents, tag, "\n", tag + "=" + value + "\n");
	WriteFile(path,contents);

}
//////////////////////////////////////////////////////////////////////////////////////
extern "C" UINT __stdcall Install(MSIHANDLE hInstall)
{

	HRESULT hRes = ::CoInitialize(NULL);
	std::string path,agentport,devices,servicename,logfiles;
	std::string status;
	std::string keylist;

	try {

		// Asssign defaults
		devices="M1";
		logfiles="";
		servicename="NikonAgent";
		agentport="5000";

		TCHAR szBuffer1[MAX_PATH] = {'0'};
		DWORD dwLen = MAX_PATH;
		status="MsiGetProperty";

		// Returns ERROR_SUCCESS, ERROR_MORE_DATA, ERROR_INVALID_HANDLE, ERROR_BAD_ARGUMENTS
		hRes = MsiGetProperty(hInstall, "CustomActionData", szBuffer1, &dwLen);
		std::vector<std::string> symbols = TrimmedTokenize(std::string(szBuffer1), "$");

		// Custom action parameterization
		// $ServiceName=[EDITA1]$AgentPort=[EDITA2]$Devices=[EDITA3]$LogFiles=[EDITA4]$Target=[TARGETDIR]
		status="Parse symbols";
		for(size_t i=0 ; i< symbols.size() ; i++)
		{
			std::vector<std::string> tokens=TrimmedTokenize(symbols[i],"=");
			if(tokens.size() < 2)
				continue;
			if(tokens[0]=="Target")
				path=tokens[1];

			// Ini file parameters
			if(tokens[0]=="AgentPort") // http://agent:portno
				agentport=tokens[1];
			if(tokens[0]=="ServiceName") // default NikonAgent
				servicename=tokens[1];    
 			if(tokens[0]=="Devices") // comma separated list of machine names
				devices=tokens[1];    
			if(tokens[0]=="LogFiles") // comma separated list of UNC log files
				logfiles=tokens[1];    
 
		}
		std::vector<std::string> keys = TrimmedTokenize(devices, ",");
		std::vector<std::string> logs = TrimmedTokenize(logfiles, ",");
		for(size_t i=0; i< keys.size(); i++)
		{
			keys[i]=SanitizeDeviceName(keys[i]);
		}
		for(size_t i=0 ; i< keys.size() ; i++)
			keylist+=keys[i]+",";
		keylist=keylist.substr(0,keylist.size()-1); // remove trailing comma
		//std::string keylist = std::accumulate( keys.begin(), keys.end(), std::string("") );; // need comma separator

		if(keys.size() != logs.size())
		{
			return ERROR_BAD_ARGUMENTS;
		}

		RunSynchronousProcess("sc.exe", " stop " + servicename);
		::Sleep(2000);
		RunSynchronousProcess("sc.exe", " delete " + servicename);

		status = "SetFilePermission \n";
		SetFilePermission(path+"debug.txt") ;
		SetFilePermission(path+"Agent.log") ;
		SetFilePermission(path+"Config.ini") ;
		SetFilePermission(path+"NikonAgent.exe") ;
		SetFilePermission(path+"Agent.cfg") ;
		SetFilePermission(path+"Devices.xml") ;
		SetFilePermission(path+"Config.ini") ;

		status = "Done SetFilePermission \n";

		WritePrivateProfileString("GLOBALS", "AgentPort", agentport.c_str(), (path+"Config.ini").c_str());
		WritePrivateProfileString("GLOBALS", "ServiceName", servicename.c_str(), (path+"Config.ini").c_str());
		WritePrivateProfileString("GLOBALS", "MTConnectDevice", keylist.c_str(), (path+"Config.ini").c_str());

		for(size_t i=0; i< keys.size(); i++)
		{
			WritePrivateProfileString(keys[i].c_str(), "ProductionLog", logs[i].c_str(), (path+"Config.ini").c_str());

		}
		DbgOut("Create service with NikonAgent.exe");


		CDevicesXML::WriteAgentCfgFile(servicename, agentport, "Agent.cfg", "Devices.xml", path);
		CDevicesXML::WriteDevicesFile(keys, CDevicesXML::ProbeDeviceXml(),  "Devices.xml",path);
		

		// This can be a problem - since registry access is issue
		//RunSynchronousProcess(path+"NikonAdapter.exe", " install");
		DbgOut("InstallAgent.bat \n");
		//RunSynchronousProcess(path+"InstallAgent.bat","");
		//if(!user.empty())
		//	RunSynchronousProcess("sc.exe"," config "+ servicename + StdStringFormat(" obj= %s password= %s", user.c_str(), pw.c_str()));

		//DbgOut("sc.exe start  servicenamet \n");
		//RunSynchronousProcess("sc.exe", " start " + servicename);

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
		//RunSynchronousProcess("sc.exe", " stop NikonAdapter");
		//::Sleep(3000);
		RunSynchronousProcess("sc.exe", " delete NikonAdapter");
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


