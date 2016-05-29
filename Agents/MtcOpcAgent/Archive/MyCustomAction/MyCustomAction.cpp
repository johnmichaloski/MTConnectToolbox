// MyCustomAction.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "MSI_Logging.h"
#include "Resource.h"
#include "StdStringFcn.h"
#include "atlstr.h"
#include <vector>
#include "AccCtrl.h"
#include "AclApi.h"

#include <sddl.h>

#pragma comment(lib, "msi.lib") 
#pragma comment(lib, "advapi32.lib")

class CConfigDlg : public CDialogImpl<CConfigDlg>
{
public:
	enum { IDD = IDD_DIALOG1 };
	BEGIN_MSG_MAP(CConfigDlg)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)  
	END_MSG_MAP()
	CEdit port;
	CIPAddressCtrl ip;
	CComboBox debug;
	CComboBox rate;

	CString sPort,sIp,sDebug,sRate;

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		EndDialog(ERROR_SUCCESS);
		return 0;
	}

	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{

		port.GetWindowText(sPort);
		ip.GetWindowText(sIp);
		debug.GetWindowText(sDebug);
		rate.GetWindowText(sRate);

		EndDialog(IDOK );
		return 0;
	}
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		port.GetWindowText(sPort);
		ip.GetWindowText(sIp);
		debug.GetWindowText(sDebug);
		rate.GetWindowText(sRate);
		EndDialog(ERROR_SUCCESS);
		return 0;
	}

	LRESULT OnInitDialog(...)
	{
		CenterWindow();
		port = GetDlgItem(IDC_PORTEDIT);
		port.SetWindowTextA(_T("7878"));
		ip = GetDlgItem(IDC_IPADDRESS1);
		ip.SetWindowTextA(_T("127.0.0.1"));
		debug =  GetDlgItem(IDC_DEBUGCOMBO);
		debug.AddString(_T("fatal"));
		debug.AddString(_T("error"));
		debug.AddString(_T("warn"));
		debug.AddString(_T("debug"));
		debug.SetCurSel(0);

		rate=  GetDlgItem(IDC_RATECOMBO);
		rate.AddString(_T("1"));
		rate.AddString(_T("2"));
		rate.AddString(_T("5"));
		rate.AddString(_T("10"));
		rate.SetCurSel(0);
		return 0;
	}
};

BOOL APIENTRY DllMain( HANDLE hModule, 
	DWORD  ul_reason_for_call, 
	LPVOID lpReserved )
{
	return TRUE;
}

static int RunAsSynchronousProcess(std::string cmd, std::string args, std::string directory, int timeout=4000)
{
	SHELLEXECUTEINFO shExInfo = {0};
	shExInfo.cbSize = sizeof(shExInfo);
	shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shExInfo.hwnd = 0;
	shExInfo.lpVerb = _T("runas");                // Operation to perform
	shExInfo.lpFile = cmd.c_str();       // Application to start    
	shExInfo.lpParameters = args.c_str();                  // Additional parameters
	shExInfo.lpDirectory = directory.c_str();
	shExInfo.nShow = SW_SHOW;
	shExInfo.hInstApp = 0;  

	if (ShellExecuteEx(&shExInfo))
	{
		WaitForSingleObject(shExInfo.hProcess, timeout);
		CloseHandle(shExInfo.hProcess);		
	}
	return S_OK;
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

static std::string GetTimeStamp()
{
	char aBuffer[256];
	SYSTEMTIME st;
	GetSystemTime(&st);
	sprintf(aBuffer, "%4d-%02d-%02dT%02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, 
		st.wMinute, st.wSecond);
	return aBuffer;
}
static HRESULT	WriteDevicesFile(std::vector<std::string> devicenames, std::string probexml,  std::string xmlFile, std::string destFolder)
{
	std::string contents;
	contents+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	contents+="<MTConnectDevices xmlns=\"urn:mtconnect.org:MTConnectDevices:1.1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"urn:mtconnect.org:MTConnectDevices:1.1 http://www.mtconnect.org/schemas/MTConnectDevices_1.1.xsd\">\n";
	contents+=StdStringFormat("<Header bufferSize=\"130000\" instanceId=\"1\" creationTime=\"%s\" sender=\"local\" version=\"1.1\"/>\n",
		GetTimeStamp().c_str());
	contents+="<Devices>\n";

	// Check all machine names unique

	// Generate Devices.xml file with all devices in it.
	for(UINT i=0; i<devicenames.size(); i++)
	{
		std::string config = probexml; 
		ReplaceAll(config,"####", devicenames[i]); 
		config=ReplaceOnce(config,"name=\"NNNNNN\"","name=\""+devicenames[i]+"\"");
		contents+=config+"\n";
	}
	contents+="</Devices>\n";
	contents+="</MTConnectDevices>\n";

	if(!xmlFile.empty())
		WriteFile(destFolder + "\\" + xmlFile  , contents);
	return 0;
}

BOOL ElevateSecurityAttribute(SECURITY_ATTRIBUTES * pSA)
{

	static TCHAR secDescriptor[SECURITY_DESCRIPTOR_MIN_LENGTH];

	PSECURITY_DESCRIPTOR pSecDesc;

	PACL pAcl = NULL;

	BOOL fAclPresent = FALSE;

	BOOL fAclDefaulted = FALSE;

	BOOL bRetval = FALSE;

	pSA->nLength = sizeof(SECURITY_ATTRIBUTES);

	pSA->bInheritHandle = FALSE;

	pSA->lpSecurityDescriptor = &secDescriptor;

	InitializeSecurityDescriptor(pSA->lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);

	SetSecurityDescriptorDacl(pSA->lpSecurityDescriptor, TRUE, 0, FALSE);

	bRetval = ConvertStringSecurityDescriptorToSecurityDescriptor(_T("S:(ML;;NW;;;LW)"), // "low integrity"
		SDDL_REVISION_1, &pSecDesc, NULL);

	bRetval = GetSecurityDescriptorSacl(pSecDesc, &fAclPresent, &pAcl, &fAclDefaulted);

	return SetSecurityDescriptorSacl(pSA->lpSecurityDescriptor, TRUE, pAcl, FALSE);

}

// CreateMyDACL.
//    Create a security descriptor that contains the DACL 
//    you want.
//    This function uses SDDL to make Deny and Allow ACEs.
//
// Parameter:
//    SECURITY_ATTRIBUTES * pSA
//    Pointer to a SECURITY_ATTRIBUTES structure. It is your
//    responsibility to properly initialize the 
//    structure and to free the structure's 
//    lpSecurityDescriptor member when you have
//    finished using it. To free the structure's 
//    lpSecurityDescriptor member, call the 
//    LocalFree function.
// 
// Return value:
//    FALSE if the address to the structure is NULL. 
//    Otherwise, this function returns the value from the
//    ConvertStringSecurityDescriptorToSecurityDescriptor 
//    function.
BOOL CreateMyDACL(SECURITY_ATTRIBUTES * pSA)
{
	// Define the SDDL for the DACL. This example sets 
	// the following access:
	//     Built-in guests are denied all access.
	//     Anonymous logon is denied all access.
	//     Authenticated users are allowed 
	//     read/write/execute access.
	//     Administrators are allowed full control.
	// Modify these values as needed to generate the proper
	// DACL for your application. 
	TCHAR * szSD = TEXT("D:")       // Discretionary ACL
		TEXT("(D;OICI;GA;;;BG)")     // Deny access to 
		// built-in guests
		TEXT("(D;OICI;GA;;;AN)")     // Deny access to 
		// anonymous logon
		TEXT("(A;OICI;GRGWGX;;;AU)") // Allow 
		// read/write/execute 
		// to authenticated 
		// users
		TEXT("(A;OICI;GA;;;BA)");    // Allow full control 
	// to administrators

	if (NULL == pSA)
		return FALSE;

	return ConvertStringSecurityDescriptorToSecurityDescriptor(
		szSD,
		SDDL_REVISION_1,
		&(pSA->lpSecurityDescriptor),
		NULL);
}

BOOL GrantFullAccess (MSIHANDLE hInstall, const TCHAR * lpszOwnFile) 
{
	BOOL bRetval = FALSE;

	PSID pSIDEveryone = NULL;
	PSID pUserSID = NULL;
	PACL pACL = NULL;
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
	EXPLICIT_ACCESS ea;
	DWORD dwRes;

	const DWORD ACCESS_DIRECTORY_PROTECTED
		= READ_CONTROL
		| GENERIC_READ
		| GENERIC_WRITE
		| GENERIC_EXECUTE
		| SYNCHRONIZE
		| FILE_LIST_DIRECTORY
		| FILE_ADD_FILE
		| FILE_ADD_SUBDIRECTORY
		| FILE_READ_EA
		| FILE_TRAVERSE
		| FILE_DELETE_CHILD
		| FILE_READ_ATTRIBUTES;

	const DWORD ACCESS_DIRECTORY_UNPROTECTED = ACCESS_DIRECTORY_PROTECTED
		| DELETE;

	try { 

		// Create a SID for the Everyone group.
		if (! (AllocateAndInitializeSid (& SIDAuthWorld, 1,
			SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, & pUserSID))) 
		{
			/* error freatment */
			return FALSE;
		}


		// Set full access for Everyone.
		ZeroMemory (& ea, sizeof (EXPLICIT_ACCESS));
		ea. grfAccessPermissions = MAXIMUM_ALLOWED;
		ea. grfAccessMode = SET_ACCESS;
		ea. grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
		ea. Trustee. TrusteeForm = TRUSTEE_IS_SID;
		ea. Trustee. TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
		ea. Trustee. ptstrName   = (LPTSTR) pUserSID;

		if (FAILED (ERROR_SUCCESS == SetEntriesInAcl (1, & ea, NULL, & pACL)))
		{
			LogString(hInstall, "SetEntriesInAcl failed");
			return FALSE;
		}

		// Try to modify the object's DACL.
		if (FAILED (SetNamedSecurityInfo (
			(LPSTR) lpszOwnFile,                 // name of the object
			SE_FILE_OBJECT,              // type of object
			DACL_SECURITY_INFORMATION,   // change only the object's DACL
			NULL, NULL,                  // do not change owner or group
			pACL,                        // DACL specified
			NULL)))                       // do not change SACL
		{
			LogString(hInstall,"SetNamedSecurityInfo failed to change the DACL Maximum Allowed Access");
			return FALSE;
		}

		return TRUE;
	}
	catch(...) {}
	if (pSIDEveryone)
		FreeSid(pSIDEveryone); 
	if (pACL)
		LocalFree(pACL);
	return TRUE;

}
extern "C" UINT __stdcall Install(MSIHANDLE hInstall)
{

	HRESULT hRes = ::CoInitialize(NULL);
	std::string path,user, ipaddr,type,devices,httpPort;
	std::string status;

	try 
	{
		TCHAR szBuffer1[MAX_PATH] = {'0'};
		DWORD dwLen = MAX_PATH;
		status="MsiGetProperty";
		// Returns ERROR_SUCCESS, ERROR_MORE_DATA, ERROR_INVALID_HANDLE, ERROR_BAD_ARGUMENTS
		hRes = MsiGetProperty(hInstall, _T("CustomActionData"), szBuffer1, &dwLen);
		//::MessageBox(NULL, szBuffer1, "Hey", MB_OK);
		std::vector<std::string> symbols = TrimmedTokenize(std::string(szBuffer1), "$");

		//	MessageBox(NULL, std::string(szBuffer1).c_str(), "Info", MB_OK);

		RunSynchronousProcess("sc.exe", " stop MtcOpcAgent");
		RunSynchronousProcess("sc.exe", " delete MtcOpcAgent");

		//$Type=[BUTTON1]$IpValue=[EDITA1]$HttpPort=[EDITA2]$Target=[TARGETDIR]$Config=[EDITA3]
		status="Parse symbols";
		for(int i=0 ; i< symbols.size() ; i++)
		{
			std::vector<std::string> tokens=TrimmedTokenize(symbols[i],"=");
			if(tokens.size() < 2)
				continue;
			if(tokens[0]=="Target")
				path=tokens[1];
			// Ini file parameters
			if(tokens[0]=="IpValue") // no change necessary if using csv, program will stip out
				ipaddr=tokens[1];    
			if(tokens[0]=="Devices") // no change necessary if using csv, program will stip out
				devices=tokens[1];    
			if(tokens[0]=="HttpPort") // no change necessary if using csv, program will stip out
				httpPort=tokens[1];    
			if(tokens[0]=="User") 
				user=tokens[1]+"MtcOpcAgent\\";    
		}
		//path="C:\\MTConnect\\MtcOpcAgent\\";
		// \"C:\\Program Files\\MTConnect\\MtcOpcAgent\\ \   
		std::string args = "  \"" + path + "\"  /t  /c  /GRANT Everyone:F";

		RunAsSynchronousProcess("icacls", args, path, 4000);
		//RunAsSynchronousProcess(path+"superuser.bat", "", path, 4000);

		status="Read MtcOpcAgent.ini";
		std::string contents; 
		ReadFile(path+"MtcOpcAgent.ini", contents);

		//GrantFullAccess (hInstall, (path+"MtcOpcAgent.ini").c_str()) ;
		//GrantFullAccess (hInstall, (path+"Devices.xml").c_str()) ;

		ReplacePattern(contents, "ServerMachineName", "\n", "ServerMachineName=" + ipaddr + "\n");
		ReplacePattern(contents, "MTConnectDevice", "\n", "MTConnectDevice=" + devices + "\n");

		std::vector<std::string> ips=TrimmedTokenize(ipaddr,",");
		std::vector<std::string> devs=TrimmedTokenize(devices,",");
		if(ips.size() != devs.size())
			::MessageBox(NULL, "Mismatched # ips and devices", "Error", MB_OK);
		std::string tagsection="SIEMENS";
		for(int i=1; i< ips.size(); i++)
		{
			tagsection+=",SIEMENS";
		}
		ReplacePattern(contents, "OpcTags", "\n", "OpcTags=" + tagsection + "\n");
		//::MessageBox(NULL,contents.c_str(), "Error", MB_OK);
		WriteFile(path+"MtcOpcAgent.ini",contents);

		std::string _probexml;
		ReadFile(path + "probexml.txt", _probexml);
		WriteDevicesFile(devs, _probexml,  "Devices.xml", path);

		// Install and start service  
		status="Create service with MtcOpcAgent.exe";
		RunSynchronousProcess(path+"MtcOpcAgent.exe", " install");
		status="sc Start service MtcOpcAgent";
		//RunSynchronousProcess("sc.exe", " start MTCFanucAgent");
		//	LocalFree(sa.lpSecurityDescriptor);
	}
	catch(...)
	{
		LogString(hInstall, (TCHAR*)("Custom install exception"+status+"\n").c_str());
	}
	::CoUninitialize();

	return ERROR_SUCCESS;
}
extern "C" UINT __stdcall Commit(MSIHANDLE hInstall)
{
	return ERROR_SUCCESS;
}

static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg =  StdStringFormat("MtcOpcAgent Install In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode);
	OutputDebugString(errmsg.c_str() );
	throw std::exception(errmsg.c_str() , pExp->ExceptionRecord->ExceptionCode);
} 

extern "C" UINT __stdcall Uninstall (MSIHANDLE hInstall)
{
	//_set_se_translator( trans_func );  // correct thread?

	HRESULT hRes = ::CoInitialize(NULL);
	std::string path;

	TCHAR szBuffer1[MAX_PATH] = {'0'};
	DWORD dwLen = MAX_PATH;
	// Returns ERROR_SUCCESS, ERROR_MORE_DATA, ERROR_INVALID_HANDLE, ERROR_BAD_ARGUMENTS
	hRes = MsiGetProperty(hInstall, _T("CustomActionData"), szBuffer1, &dwLen);
	std::vector<std::string> symbols = TrimmedTokenize(std::string(szBuffer1), "/");
	//	MessageBox(NULL, std::string(szBuffer1).c_str(), "Info", MB_OK);
	//	DebugBreak();
	//for(int i=0 ; i< symbols.size() ; i++)
	//{
	//	std::vector<std::string> tokens=Tokenize(symbols[i],"=");
	//	if(tokens.size() < 2)
	//		continue;
	//	if(tokens[0]=="Target")
	//		path=tokens[1];
	//}

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
		RunSynchronousProcess("sc.exe", " stop MtcOpcAgent");
		RunSynchronousProcess("sc.exe", " delete MtcOpcAgent");
	}
	catch(...)
	{

	}
#endif
	::CoUninitialize();
	return ERROR_SUCCESS;
}
extern "C" UINT __stdcall Rollback(MSIHANDLE hInstall)
{


	return ERROR_SUCCESS;
}


