// MyCustomAction.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "StdStringFcn.h"
#include "Config.h"
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

LRESULT WriteAgentCfgFile (std::string ServerName,
                                        std::string HttpPort,
                                        std::string cfgfile, 
										std::string xmlFile,
                                        std::string destFolder)
{
    // Generate agent.cfg file with all devices in it.
    std::string cfg;

    cfg  = "Devices = " + xmlFile + "\n";
    cfg += "ServiceName = " + ServerName + "\n";           // MTConnectAgent\n";
    cfg += "Port = " + HttpPort + "\n";

    cfg += "CheckpointFrequency=10000\n";
    cfg += "AllowPut=true\n";

    cfg += "Adapters \n{\n";
    cfg += "}\n";

    cfg += "# Logger Configuration\n";
    cfg += "logger_config\n";
    cfg += "{\n";
    cfg += "\tlogging_level = debug\n";
    cfg += "\toutput = cout\n";
    cfg += "}\n";

    if ( !cfgfile.empty( ) )
    {
        WriteFile(destFolder + cfgfile, cfg);
    }
    return 0;
}

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
	std::string path;   // $TARGET
	std::string servicename,agentport;
	std::string devices,ip, model,version;
	std::string status;
	std::string keylist;

	try {

		// Asssign defaults
		servicename="moto_Agent";
		agentport="5000";
		devices="M1";
		ip="";
		model="";
		version="";

		TCHAR szBuffer1[MAX_PATH] = {'0'};
		DWORD dwLen = MAX_PATH;
		status="MsiGetProperty";

		// Returns ERROR_SUCCESS, ERROR_MORE_DATA, ERROR_INVALID_HANDLE, ERROR_BAD_ARGUMENTS
		hRes = MsiGetProperty(hInstall, "CustomActionData", szBuffer1, &dwLen);
		std::vector<std::string> symbols = TrimmedTokenize(std::string(szBuffer1), "$");

		// Custom action parameterization
		// $ServiceName=[EDITA1]$AgentPort=[EDITA2]$
		// $Devices=[EDITB1]$IP=[EDITB2]$Model=[EDITB3]$Version=[EDITB4]
		// $Target=[TARGETDIR]
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
			if(tokens[0]=="ServiceName") // default moto_Agent
				servicename=tokens[1];    
 			if(tokens[0]=="Devices") // csv list of machine names
				devices=tokens[1];    
			if(tokens[0]=="IP") // csv list of ips
				ip=tokens[1];    
			if(tokens[0]=="Model") // csv list of models (ur3,ur5,ur10)
				model=tokens[1];    
 			if(tokens[0]=="Version") // csv list of versions(1.8,1.9,3.0,3.1,..3.4)
				version=tokens[1];    

		}
		std::vector<std::string> keys = TrimmedTokenize(devices, ",");
		std::vector<std::string> ips = TrimmedTokenize(ip, ",");
		std::vector<std::string> models = TrimmedTokenize(model, ",");
		std::vector<std::string> versions = TrimmedTokenize(version, ",");

		for(size_t i=0; i< keys.size(); i++)
		{
			keys[i]=SanitizeDeviceName(keys[i]);
		}

		for(size_t i=0 ; i< keys.size() ; i++)
			keylist+=keys[i]+",";
		keylist=keylist.substr(0,keylist.size()-1); // remove trailing comma

		if(keys.size() != ips.size())
		{
			return ERROR_BAD_ARGUMENTS;
		}

		RunSynchronousProcess("sc.exe", " stop " + servicename);


		status = "SetFilePermission \n";
		SetFilePermission(path+"debug.txt") ;
		SetFilePermission(path+"Agent.log") ;
		SetFilePermission(path+"Config.ini") ;
		SetFilePermission(path+"moto_Agent.exe") ;
		SetFilePermission(path+"Agent.cfg") ;
		SetFilePermission(path+"Devices.xml") ;
		SetFilePermission(path+"Config.ini") ;
		SetFilePermission(path+"InstallAgent.bat") ;
		SetFilePermission(path+"tags.csv") ;

		status = "Done SetFilePermission \n";

		WritePrivateProfileString("GLOBALS", "AgentPort", agentport.c_str(), (path+"Config.ini").c_str());
		WritePrivateProfileString("GLOBALS", "ServiceName", servicename.c_str(), (path+"Config.ini").c_str());
		WritePrivateProfileString("GLOBALS", "MTConnectDevice", keylist.c_str(), (path+"Config.ini").c_str());

		// This ensures that a devices.xml will be generated upon first app or service execution
		WritePrivateProfileString("GLOBALS", "Config", "NEW", (path+"Config.ini").c_str());

		std::string jointnames = "shoulder_pan_joint,shoulder_lift_joint,elbow_joint,wrist_1_joint,wrist_2_joint,wrist_3_joint";
		for(size_t i=0; i< keys.size(); i++)
		{
			WritePrivateProfileString(keys[i].c_str(), "IP", ips[i].c_str(), (path+"Config.ini").c_str());
			WritePrivateProfileString(keys[i].c_str(), "urdf", (models[i]+".urdf").c_str(), (path+"Config.ini").c_str());
			WritePrivateProfileString(keys[i].c_str(), "version", versions[i].c_str(), (path+"Config.ini").c_str());
			WritePrivateProfileString(keys[i].c_str(), "jointnames", jointnames.c_str(), (path+"Config.ini").c_str());

		}
		DbgOut("Create service with moto_Agent.exe");

		WriteAgentCfgFile(servicename, agentport, "Agent.cfg", "Devices.xml", path);
		//CDevicesXML::WriteDevicesFile(keys, CDevicesXML::ProbeDeviceXml(),  "Devices.xml",path);
		

		// This can be a problem - since registry access is issue
		//RunSynchronousProcess(path+"Moto_Adapter.exe", " install");
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
		//RunSynchronousProcess("sc.exe", " stop Moto_Adapter");
		//::Sleep(3000);
		RunSynchronousProcess("sc.exe", " delete Moto_Adapter");
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


