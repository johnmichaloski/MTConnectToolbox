// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Resource.h"
#include "atlstr.h"
#define _WTL_NO_CSTRING
#define _CSTRING_NS	ATL
#include "trayiconimpl.h"
#include <string>
#include <vector>
#include <map>
#include "Thread.h"
#include "Snapshots.h"

extern 	DWORD	dwWebBrowserCookie; //Save this to get the interface later

class CDlgThread;

class CMainDlg : public CAxDialogImpl<CMainDlg>, 
	public CUpdateUI<CMainDlg>,
	public CMessageFilter, 
	public CIdleHandler,
	public CDialogResize<CMainDlg>,
	public CTrayIconImpl<CMainDlg> 
{
public:
	enum { IDD = IDD_MAINDLG };

	//CMainDlg(CDlgThread * thread);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_DLGRESIZE_MAP(CMainDlg)
		 DLGRESIZE_CONTROL(IDC_EXPLORER1, DLSZ_SIZE_X|DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
		REFLECT_NOTIFICATIONS()		
		CHAIN_MSG_MAP(CTrayIconImpl<CMainDlg>)
		CHAIN_MSG_MAP(CDialogResize<CMainDlg>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	virtual LRESULT OnTrayIcon(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT message, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnSysCommand(UINT message, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled);

	void CloseDialog(int nVal);
	std::string HtmlStyle();
	std::string	CreateHtmlDocument();

	// EXECUTING,IDLE,STOPPED,FAULT, MANUAL
	void SetState(std::string s) { state=s; }
	void SetTooltip(std::string s) ;

	HICON								hSmileyIcon, hIdleIcon, hStopIcon, hClearIcon;
	int									nIcon;
	std::string							state;
	std::map<std::string,std::vector<HICON> > StateIconMap;
	CString								sAppTooltip;
	CComPtr<IWebBrowser2>				m_spWebBrowser;
	CHtmlTableSnapshots                 _htmlsnapshot;
	CDlgThread *                        _thread;

};
class CDlgThread : public CGuiThreadImpl<CDlgThread> 
{
public:
	CDlgThread(CAppModule* pModule)
		: CGuiThreadImpl<CDlgThread>(pModule) //, dlgMain(this) 
	{ }
	CMainDlg dlgMain;
	BEGIN_MSG_MAP(CDlgThread)
		MESSAGE_HANDLER(WM_ENDSESSION, OnLogoff)
	END_MSG_MAP()
	LRESULT OnLogoff(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	BOOL InitializeThread();
	//Causing a GUI thread to quit is usually a matter of posting the WM_QUIT message to its queue. 
	void CleanupThread(DWORD dwExitCode);
	CMessageLoop * GetMessageLoop() { return &msgLoop; }
};
