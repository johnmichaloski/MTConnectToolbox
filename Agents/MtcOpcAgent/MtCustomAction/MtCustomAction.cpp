// MtCustomAction.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "MSI_Logging.h"
#include "Resource.h"
#include "StdStringFcn.h"
#include "atlstr.h"
#include "RunProcess.h"

#pragma comment(lib, "msi.lib") 

static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg =  StdStringFormat("COpcAdapter In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode);
	OutputDebugString(errmsg.c_str() );
	throw std::exception(errmsg.c_str() , pExp->ExceptionRecord->ExceptionCode);
}

#if 0
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
#endif
#include "MainDlg.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved )
{
	return TRUE;
}

CAppModule _Module;

extern "C" UINT __stdcall Install(MSIHANDLE hInstall)
{

	//_set_se_translator( trans_func );  
	//HRESULT hRes = ::CoInitialize(NULL);
	HRESULT hRes = ::OleInitialize(NULL);
	try{
		AtlInitCommonControls(ICC_BAR_CLASSES | ICC_DATE_CLASSES);	// add flags to support other controls

		TCHAR szBuffer1[MAX_PATH] = {'0'};
		DWORD dwLen = MAX_PATH;
		// Returns ERROR_SUCCESS, ERROR_MORE_DATA, ERROR_INVALID_HANDLE, ERROR_BAD_ARGUMENTS
		MsiGetProperty(hInstall, _T("CustomActionData"), szBuffer1, &dwLen);
		std::string path(szBuffer1);

		// Get user configuration for installation
		CMainDlg dlg(path);
		hRes = dlg.DoModal();

	}
	catch(...)
	{
		MsiMessageBox(hInstall, "Internal install error", 0);

	}
	::CoUninitialize();

	return hRes!=IDOK;
}
extern "C" UINT __stdcall Commit(MSIHANDLE hInstall)
{
	return ERROR_SUCCESS;
}
 
extern "C" UINT __stdcall Uninstall (MSIHANDLE hInstall)
{
	//_set_se_translator( trans_func );  

	HRESULT hRes = ::CoInitialize(NULL);
	TCHAR szBuffer1[MAX_PATH] = {'0'};
	DWORD dwLen = MAX_PATH;
	std::string str;

	// Returns ERROR_SUCCESS, ERROR_MORE_DATA, ERROR_INVALID_HANDLE, ERROR_BAD_ARGUMENTS
	UINT hr = MsiGetProperty(hInstall, _T("CustomActionData"), szBuffer1, &dwLen);
	std::string path(szBuffer1);
	//MessageBox(NULL, ("Rollback" + path+"adapter.ini").c_str(), "0", MB_OK);

	// Install and start service  
	try{
		std::string adapters;
		CRegKey key;
		long nError = key.Open(HKEY_LOCAL_MACHINE, "Software\\MTConnect\\AdapterOpc840D", KEY_READ);
		if(nError == ERROR_SUCCESS)
		{
			TCHAR buffer[2048];
			DWORD dwSize=2048;
			key.QueryStringValue("Services", buffer, &dwSize);
			adapters=buffer;
		}
		key.Close();
		std::vector<std::string> services = Tokenize(adapters,",");
		for(int i=0; i<services.size(); i++)
		{
			RedirectedIOProcess shell;
			str="cmd.exe /C sc stop " + services[i]; 
			str=shell.RunProcess(str.c_str());
			str="cmd.exe /C sc delete " +  services[i]; 
			str=shell.RunProcess(str.c_str());
		}
	}
	catch(...)
	{
#ifdef DEBUG
		MsiMessageBox(hInstall, "Uninstall error", 0);
#endif
	}
	//MessageBox(NULL, (str).c_str(), "0", MB_OK);
	::CoUninitialize();
	return ERROR_SUCCESS;
}
extern "C" UINT __stdcall Rollback(MSIHANDLE hInstall)
{


	return ERROR_SUCCESS;
}


