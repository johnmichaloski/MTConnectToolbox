// MyCustomAction.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "MSI_Logging.h"
#include "Resource.h"
#include "StdStringFcn.h"
#include "atlstr.h"

#pragma comment(lib, "msi.lib") 

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
	hRes = MsiGetProperty(hInstall, _T("CustomActionData"), szBuffer1, &dwLen);
	std::vector<std::string> symbols = TrimmedTokenize(std::string(szBuffer1), "$");

	//	MessageBox(NULL, std::string(szBuffer1).c_str(), "Info", MB_OK);

	RunSynchronousProcess("sc.exe", " stop MTCFanucAgent");
	RunSynchronousProcess("sc.exe", " delete MTCFanucAgent");

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
		if(tokens[0]=="Type") // no change necessary if using csv, program will stip out
			type=tokens[1];
		if(tokens[0]=="IpValue") // no change necessary if using csv, program will stip out
			ipaddr=tokens[1];    
		if(tokens[0]=="Config") // no change necessary if using csv, program will stip out
			config=tokens[1];    
		if(tokens[0]=="HttpPort") // no change necessary if using csv, program will stip out
			httpPort=tokens[1];    
		if(tokens[0]=="FanucPort") // no change necessary if using csv, program will stip out
			fanucPort=tokens[1];    
	}

	status="Read MTCFanucAgent.ini";
	std::string contents; 
	ReadFile(path+"MTCFanucAgent.ini", contents);

	//FanucIpAddress=192.168.1.102
	ReplacePattern(contents, "FanucIpAddress=", "\n", "FanucIpAddress=" + ipaddr + "\n");
	//Protocol=LAN 
	if(type=="1")
		ReplacePattern(contents, "Protocol=", "\n", "Protocol=HSSB\n"); 
	else
		ReplacePattern(contents, "Protocol=", "\n", "Protocol=LAN\n"); 
		
	ReplacePattern(contents, "FanucPort=", "\n", "FanucPort=" + fanucPort +"\n"); 

	WriteFile(path+"MTCFanucAgent.ini",contents);

	status="Read agent.cfg";
	
	if(!httpPort.empty())
	{
		contents.clear();
		ReadFile(path+"agent.cfg", contents);
		ReplacePattern(contents, "Port ", "\n", "Port=" + httpPort + "\n");
		WriteFile(path+"agent.cfg",contents);
	}

	//contents=ReplaceOnce(contents,"IP=127.0.0.1", "IP=" + ipaddr);
	//contents=ReplaceOnce(contents,"Machine=DMG2796", "Machine=" + machinename);
	// Install and start service  
	status="Create service with MTCFanucAgent.exe";
	RunSynchronousProcess(path+"MTCFanucAgent.exe", " install");
	status="sc Start service MTCFanucAgent";

		//RunSynchronousProcess("sc.exe", " start MTCFanucAgent");
	}
	catch(...)
	{
		LogString(hInstall, (TCHAR*)("Custom install exception\n"+status).c_str());
	}
	::CoUninitialize();


	return hRes==IDOK;
}
extern "C" UINT __stdcall Commit(MSIHANDLE hInstall)
{
	return ERROR_SUCCESS;
}

static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg =  StdStringFormat("COpcAdapter In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode);
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
		RunSynchronousProcess("sc.exe", " stop MTCFanucAgent");
		RunSynchronousProcess("sc.exe", " delete MTCFanucAgent");
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


