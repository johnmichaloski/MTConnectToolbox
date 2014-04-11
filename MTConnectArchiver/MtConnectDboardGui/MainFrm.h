//
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "Resource.h"
#include "atlstr.h"
#define _WTL_NO_CSTRING
#define _CSTRING_NS	ATL
#include "trayiconimpl.h"
#include <string>
#include <vector>
#include <map>
#define WINDOW_MENU_POSITION	3
#include "WtlHtmlView.h"
#include "OdbcArchiver.h"

class CMTConnectStatusSheetView : public CWindowImpl<CMTConnectStatusSheetView, CAxWindow>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CAxWindow::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		   (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
			return FALSE;

		// give HTML page a chance to translate this message
		return (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
	}

	BEGIN_MSG_MAP(CMTConnectStatusSheetView)
	END_MSG_MAP()
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};

class CCMTConnectTrayIcon;

class CMainFrame : public CFrameWindowImpl<CMainFrame>
	, public CUpdateUI<CMainFrame>
	, public CMessageFilter
	, public CIdleHandler
	, public CTrayIconImpl<CMainFrame> 

{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	CTabView m_view;
	CWtlHtmlView* pCNCSummaryView;
	CWtlHtmlView* pFaultView;
	CWtlHtmlView* pProgramSummaryView;
	std::vector<CCMTConnectTrayIcon*> pHtmlViews;
	OdbcArchiver odbc;

	DataDictionary _typedata;
	std::vector<std::string> _faultFields;
	std::vector<std::string> _programFields;
	

	~CMainFrame();
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return m_view.PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateToolBar();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
#if 0
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(ID_WINDOW_CLOSE, OnWindowClose)
		COMMAND_ID_HANDLER(ID_WINDOW_CLOSE_ALL, OnWindowCloseAll)
		COMMAND_RANGE_HANDLER(ID_WINDOW_TABFIRST, ID_WINDOW_TABLAST, OnWindowActivate)
#endif
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
		CHAIN_MSG_MAP(CTrayIconImpl<CMainFrame>)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

	virtual LRESULT OnTrayIcon(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);

	LRESULT OnSysCommand(UINT message, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnWindowActivate(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowCloseAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	CWtlHtmlView * AddWebTab(std::string name, CCMTConnectTrayIcon *);
	CWtlHtmlView * AddAlarmTab(std::string name, CCMTConnectTrayIcon * tabWnd);

	void WebPageEvent(const TCHAR * path, std::map<std::string,std::string> queries);

	std::string HtmlStyle();
	std::string	CreateHtmlDocument();
	std::string  CreateSummaryHtmlTable();
	std::string  CreateDbHtmlTable(std::string schema, 
		std::string table, 
		std::string where,
		std::vector<std::string> &fields, 
		std::string & sSavedHtml, 
		int &nLastNum);
	static void CreateMonitorFile(std::string url);
	std::string CreateFaultHtmlTable() ;
	std::string CreateProgramHtmlTable() ;
	// EXECUTING,IDLE,STOPPED,FAULT, MANUAL
	void SetState(std::string s) { state=s; }
	void SetTooltip(std::string s)
	{
		CTrayIconImpl<CMainFrame>::SetTooltipText(s.c_str());
	}

	std::map<std::string,std::vector<HICON> > StateIconMap;
	HICON								hSmileyIcon, hIdleIcon, hStopIcon, hClearIcon;
	int									nIcon;
	std::string							state;
	CString								sAppTooltip;


};
