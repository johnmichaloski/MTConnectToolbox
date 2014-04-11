// MyCustomAction.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "MSI_Logging.h"
#include "Resource.h"
#include "StdStringFcn.h"
#include "atlstr.h"
#include "RunProcess.h"

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


extern "C" UINT __stdcall Install(MSIHANDLE hInstall)
{

	HRESULT hRes = ::CoInitialize(NULL);
	std::string path;
	std::string ipaddr;
	std::string machinename;

	TCHAR szBuffer1[MAX_PATH] = {'0'};
	DWORD dwLen = MAX_PATH;

	// Returns ERROR_SUCCESS, ERROR_MORE_DATA, ERROR_INVALID_HANDLE, ERROR_BAD_ARGUMENTS
	hRes = MsiGetProperty(hInstall, _T("CustomActionData"), szBuffer1, &dwLen);
	std::vector<std::string> symbols = TrimmedTokenize(std::string(szBuffer1), "$");
//	MessageBox(NULL, std::string(szBuffer1).c_str(), "Info", MB_OK);
	//DebugBreak();
	for(int i=0 ; i< symbols.size() ; i++)
	{
		std::vector<std::string> tokens=TrimmedTokenize(symbols[i],"=");
		if(tokens.size() < 2)
			continue;
		if(tokens[0]=="Target")
			path=tokens[1];
		// Ini file parameters
		if(tokens[0]=="NameValue") // no change necessary if using csv, program will stip out
			machinename=tokens[1];
		if(tokens[0]=="IpValue") // no change necessary if using csv, program will stip out
			ipaddr=tokens[1];    
	}
	//std::stringstream sb;
	//sb<< "Target " << path << " ";
	//sb<< "NameValue " << machinename << " ";
	//sb<< "IpValue " << ipaddr << " ";
	//MessageBox(NULL, sb.str().c_str(), "Info", MB_OK);

	std::string contents; 
	ReadFile(path+"MtConnectDboard.ini", contents);

	ReplacePattern(contents, "IP=", "\n", "IP=" + ipaddr + "\n");
	ReplacePattern(contents, "Machine=", "\n", "Machine=" + machinename + "\n");

	//contents=ReplaceOnce(contents,"IP=127.0.0.1", "IP=" + ipaddr);
	//contents=ReplaceOnce(contents,"Machine=DMG2796", "Machine=" + machinename);
	WriteFile(path+"MtConnectDboard.ini",contents);
#if 1
	// Install and start service  
	{
		RedirectedIOProcess shell;
		std::string str=path+"MtConnectDboard.exe  install";
		str=shell.RunProcess(str.c_str());
	}
	{
		RedirectedIOProcess shell;
		std::string str="cmd.exe /C sc start MtConnectDboard";
		str=shell.RunProcess(str.c_str());
	}
#endif
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
		{
			RedirectedIOProcess shell;
			str="cmd.exe /C sc stop MtConnectDboard";
			str=shell.RunProcess(str.c_str());
		}
		{
			RedirectedIOProcess shell;
			str=path+"MtConnectDboard.exe  remove";
			str=shell.RunProcess(str.c_str());
		}
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


