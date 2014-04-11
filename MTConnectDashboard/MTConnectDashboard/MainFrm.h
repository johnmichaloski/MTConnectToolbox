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
#include <ATLComTime.h>
#include "Config.h"

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


struct CMachineState
{
	CCMTConnectTrayIcon* _pView;
	int _row;
	int _counter;
	UINT _pageId;
	std::vector<std::vector<std::string> >  _events;
	std::string _lastState;
	int _elapsedSeconds;
	int _CycleTime;

	CMachineState()
	{
		_pageId=_counter=_row=_elapsedSeconds=0;
	}

};
class CMainFrame : public CFrameWindowImpl<CMainFrame>
	, public CUpdateUI<CMainFrame>
	, public CMessageFilter
	, public CIdleHandler
	, public CTrayIconImpl<CMainFrame> 

{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)
	CMainFrame();
	~CMainFrame();

	CTabView m_view;
	CWtlHtmlView* pCNCSummaryView;
	CWtlHtmlView* pFaultView;
	CWtlHtmlView* pProgramSummaryView;
	std::vector<CCMTConnectTrayIcon*> pHtmlViews;
	crp::Config config;
	
	OdbcArchiver odbc;

	std::vector<std::string> _faultFields;
	std::vector<std::string> _machineItems, _eventItems, _programItems, _summaryItems;
	std::string _programFields, _machineFields, _eventFields,_summaryFields;
	DataDictionary _typedata;
	COleDateTime today;
	bool _bSimulation;
	COleDateTime startTime;
	COleDateTime endTime;

	std::vector<std::vector<std::string> >  GetMachinesDb(std::string schema, std::string table, std::string fields);


	std::map<std::string, CMachineState> _machine;
	CMachineState dummy;
	void CMainFrame::Configure();
	CMachineState & find(int pageid) 
	{
		for(std::map<std::string, CMachineState>::iterator it=_machine.begin(); it!=_machine.end(); it++)
			if((*it).second._pageId == pageid)
				return (*it).second;
		return dummy;
	}

	//std::map<std::string,CCMTConnectTrayIcon* > _namedeventviews;

	//std::map<std::string, int > _roweventviews;
	//std::map<std::string, int > _countereventviews;
	void Off(DataDictionary &data, std::string machine, int shift, COleDateTime timestamp);
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
		MESSAGE_HANDLER(WM_NOTIFY, OnSelChange)
		COMMAND_RANGE_HANDLER(ID_WINDOW_TABFIRST, ID_WINDOW_TABLAST, OnWindowActivate)
#if 0
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(ID_WINDOW_CLOSE, OnWindowClose)
		COMMAND_ID_HANDLER(ID_WINDOW_CLOSE_ALL, OnWindowCloseAll)
		
#endif
		MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
		CHAIN_MSG_MAP(CTrayIconImpl<CMainFrame>)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

    LRESULT OnSelChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
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
		DataDictionary _formats;
		bool _bDone;

};
