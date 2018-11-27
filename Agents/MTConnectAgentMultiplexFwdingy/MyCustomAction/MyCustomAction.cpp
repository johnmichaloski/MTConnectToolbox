// MyCustomAction.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
//#include "MSI_Logging.h"
#include "StdStringFcn.h"
#include "Config.h"
#include <sstream>
#include <algorithm>
#include <numeric>  

//#defube DEBUG
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

static std::vector<std::string> get_all_files_names_within_folder(std::string folder)
{
    std::vector<std::string> names;
    std::string search_path = folder + "/*.*";
    WIN32_FIND_DATA fd; 
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd); 
    if(hFind != INVALID_HANDLE_VALUE) { 
        do { 
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                names.push_back(fd.cFileName);
            }
        }while(::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind); 
    } 
    return names;
}

//static bool EndsWith (std::string const &fullString, std::string const &ending) {
//    if (fullString.length() >= ending.length()) {
//        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
//    } else {
//        return false;
//    }
//}

static void Alert(HWND hwnd, std::string msg, std::string banner, DWORD mb)
{
#ifdef DEBUG
	::MessageBoxA(hwnd, msg.c_str(), banner.c_str(), mb);
#endif
}
//////////////////////////////////////////////////////////////////////////////////////
extern "C" UINT __stdcall Install(MSIHANDLE hInstall)
{

	HRESULT hRes = ::CoInitialize(NULL);
	std::string path,backendurl,servicename;
	std::string alldevices,allbackurls,allports;
	std::vector<std::string> devices,ports,urls;
	std::string status;

	try {
		Alert(NULL, "Enter  MTConnectAgentMultiplexFwding custom action", "Alert", MB_OK);

		// Asssign defaults
		servicename="MTConnectAgentMultiplexFwding";

		TCHAR szBuffer1[MAX_PATH] = {'0'};
		DWORD dwLen = MAX_PATH;

		// Returns ERROR_SUCCESS, ERROR_MORE_DATA, ERROR_INVALID_HANDLE, ERROR_BAD_ARGUMENTS
		hRes = MsiGetProperty(hInstall, "CustomActionData", szBuffer1, &dwLen);
		std::vector<std::string> symbols = TrimmedTokenize(std::string(szBuffer1), "$");

		// Custom action parameterization
		// NOW: $ServiceName=[EDITA1]$Devices=[EDITA2]$Ports=[EDITA3]$URLS=[EDITA4]$Target=[TARGETDIR]
		for(size_t i=0 ; i< symbols.size() ; i++)
		{
			std::vector<std::string> tokens=TrimmedTokenize(symbols[i],"=");
			if(tokens.size() < 2)
				continue;
			if(tokens[0]=="Target")
				path=tokens[1];

			// Ini file parameters
			if(tokens[0]=="ServiceName") // default MTConnectAgentFwding
				servicename=tokens[1];    
			if(tokens[0]=="Devices") // comma separated list of machine names
				alldevices=tokens[1];       
			if(tokens[0]=="Ports") // http://agent:portno
				allports=tokens[1];
  			if(tokens[0]=="URLS") // comma separated list of machine names
				allbackurls=tokens[1];   
		}
		Alert(NULL, StdStringFormat("Devices=%s", alldevices.c_str()).c_str(), "Alert", MB_OK);
		Alert(NULL, StdStringFormat("URLS=%s", allbackurls.c_str()).c_str(), "Alert", MB_OK);

		RunSynchronousProcess("sc.exe", " stop " + servicename);
		::Sleep(2000);
		RunSynchronousProcess("sc.exe", " delete " + servicename);

		status = "SetFilePermission \n";
		//Alert(NULL, status.c_str(), "Alert", MB_OK);

		//std::vector<std::string> files = get_all_files_names_within_folder(path);
		//for(size_t j=0; j< files.size(); j++)
		//{
		//	::MessageBoxA(NULL, files[j].c_str(), "Alert", MB_OK);
		//	if(EndsWith(files[j],"dll"))
		//		continue;
		//	SetFilePermission(path+files[j]) ;
		//}

		SetFilePermission(path+"debug.txt") ;
		SetFilePermission(path+"Config.ini") ;
		SetFilePermission(path+"MTConnectAgentMultiplexFwding.exe") ;
		SetFilePermission(path+"InstallFwdAgent.bat") ;
		SetFilePermission(path+"MTConnectPage-1.4.vbs") ;
		SetFilePermission(path+"superuser.bat") ;
		SetFilePermission(path+"uninstallService.vbs") ;
		SetFilePermission(path+"RunFwdAgent.bat") ;
		

		status = "Done SetFilePermission \n";
		WritePrivateProfileString("MTCONNECT", "servicename", servicename.c_str(), (path+"Config.ini").c_str());
		WritePrivateProfileString("MTCONNECT", "backends", alldevices.c_str(), (path+"Config.ini").c_str());

		devices=TrimmedTokenize(alldevices,",");
		ports=TrimmedTokenize(allports,",");
		urls=TrimmedTokenize(allbackurls,",");
		for(size_t i=0; i< devices.size(); i++)
		{
			WritePrivateProfileString(devices[i].c_str(), "backurl", urls[i].c_str(), (path+"Config.ini").c_str());
			WritePrivateProfileString(devices[i].c_str(), "fwdport", ports[i].c_str(), (path+"Config.ini").c_str());
		}
		
		RunSynchronousProcess(path+"InstallFwdAgent.bat","");
		// Use service control manager DOS command
		// https://stackoverflow.com/questions/3663331/when-creating-a-service-with-sc-exe-how-to-pass-in-context-parameters
		//RunSynchronousProcess("sc.exe", StdStringFormat("  create %s binpath= \"%s\"", servicename.c_str(), (path+"MTConnectAgentMultiplexFwding.exe").c_str() ));

		//RunSynchronousProcess("sc.exe", StdStringFormat(" description %s \"MTConnect Agent for Zeiss CMM with Calypso\"", servicename.c_str()).c_str()   );
		//if(!user.empty())
		//	RunSynchronousProcess("sc.exe"," config "+ servicename + StdStringFormat(" obj= %s password= %s", user.c_str(), pw.c_str()));
		//RunSynchronousProcess("sc.exe", " start " + servicename);
	}
	catch(...)
	{
		Trace(("Custom install exception\n"+status).c_str());
		::MessageBoxA(NULL, ("Custom install exception\n"+status).c_str(), "Alert", MB_OK);
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

	// Get the service name from the ini file, hopefully hasn't changed.
	TCHAR serviceName[32];
	int a = GetPrivateProfileString("MTCONNECT", "servicename", "MTConnectAgentMultiplexFwding", serviceName, 32, (path+"Config.ini").c_str());

	// change service name into std string
	std::string servicename= serviceName;

#if 1
	try{
		std::string str;
		// FIXME: need to lookup service name before deleting - should have been saved in registry or 
		// installation folder.
		//RunSynchronousProcess("sc.exe", " stop MTConnectAgentFwding");
		//::Sleep(3000);
		RunSynchronousProcess("sc.exe", (" delete "+servicename).c_str());
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


