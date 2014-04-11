//
//
//
#include <stdafx.h>
#include "MainFrm.h"
#include "CMTConnectTrayIcon.h"
#include "boost/bind.hpp"

CMainFrame::~CMainFrame()
{

}
#include "urlmon.h"
#include <objsafe.h>
#include "SiteImpl.h"

#include <msxml6.h>
#include "msxml2.h"
using namespace std;
#include <comdef.h>


void CMainFrame::WebPageEvent(const TCHAR * path, std::map<std::string,std::string> queries)
{
	std::string urlpath(path);
	if(urlpath== "/Reset")
	{
		for(int i=0; i< pHtmlViews.size(); i++)
		{
			CCMTConnectTrayIcon * trayicon = pHtmlViews[i];
			trayicon->Fire_Reset(_bstr_t("ALL"));
		}
	}
	if(urlpath== "/monitor")
	{
		std::string arg;// = "file:///"; 
		arg += ::ExeDirectory()+ "SeeMTConnect.html";
		CreateMonitorFile(queries["url"]);
		ShellExecute(NULL, "open", "C:\\progra~1\\intern~1\\iexplore.exe", arg.c_str(),  NULL, SW_SHOWNORMAL);
	}
}
LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	CreateSimpleToolBar();

	CreateSimpleStatusBar();

	m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

	UIAddToolBar(m_hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

			// Remove Max/Min buttons - if using TrayIcon
	DWORD style = ::GetWindowLong(this->m_hWnd, GWL_STYLE);
	style &= ~(WS_MAXIMIZEBOX); // WS_MINIMIZEBOX NOT WS_THICKFRAME
	::SetWindowLong(this->m_hWnd, GWL_STYLE, style);

	// Install tray icon
	hSmileyIcon = LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME));
	hIdleIcon = LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_IDLEICON));
	hStopIcon = LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_STOPICON));
	hClearIcon = LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_CLEARICON));
	InstallIcon(sAppTooltip, hSmileyIcon, IDR_MENU1);

	// Icon mapping
	std::vector<HICON> icons(2);
	icons[0]=hSmileyIcon; icons[1]=hClearIcon;	StateIconMap["EXECUTING"] = icons; // flashing green
	icons[0]=hSmileyIcon; icons[1]=hSmileyIcon;	StateIconMap["IDLE"] = icons; // idle auto
	icons[0]=hStopIcon;    icons[1]=hStopIcon;	StateIconMap["STOPPED"] = icons; // stopped
	icons[0]=hStopIcon;    icons[1]=hClearIcon;	StateIconMap["FAULT"] = icons; // stopped fault
	icons[0]=hIdleIcon;    icons[1]=hIdleIcon;	StateIconMap["MANUAL"] = icons; // stopped fault

	//CMenuHandle menuMain = GetMenu();
	//m_view.SetWindowMenu(menuMain.GetSubMenu(WINDOW_MENU_POSITION));
	pCNCSummaryView = new CWtlHtmlView;
	pCNCSummaryView->Create(m_view, rcDefault, _T("about:blank"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, 0);
	m_view.AddPage(pCNCSummaryView->m_hWnd, _T("Summary"));
	std::string cmd = CCMTConnectTrayIcon::CreateHtmlDocument();
	pCNCSummaryView->SetDocumentText(cmd.c_str());
	std::string sSummaryHeader;
	sSummaryHeader+= "<p><input type=button  class=\"btn\" onClick=\"window.location.href='http://localhost/Reset'\" value='Reset'></p>\n";
	sSummaryHeader+="<H1>Machine Summaries</H1>";
	pCNCSummaryView->AddEventListener(boost::bind(&CMainFrame::WebPageEvent, this,_1,_2));

	pCNCSummaryView->SetElementId("Header", sSummaryHeader );

	pProgramSummaryView= new CWtlHtmlView;
	pProgramSummaryView->Create(m_view, rcDefault, _T("about:blank"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, 0);
	m_view.AddPage(pProgramSummaryView->m_hWnd, _T("Programs"));
	cmd = CCMTConnectTrayIcon::CreateHtmlDocument();
	pProgramSummaryView->SetDocumentText(cmd.c_str());
	pProgramSummaryView->SetElementId("Header", "<H1>Program Summaries</H1>" );

	pFaultView=AddAlarmTab("Alarm History", NULL);

#if 0

	pFaultView->SetDocumentText(cmd.c_str());
	std::string header = "<H1>Alarms History</H1>\n";
	header+="<input type=\"checkbox\" name=\"http://localhost/a\" class=\"styled\" />\n";
	pFaultView->SetElementId("Header", "<H1>Alarms History</H1>");
#endif 

	odbc.open("ConsolidatedData","root","bingo");  // what if failed?

	_faultFields=TrimmedTokenize("Severity,RaisedBy,Shift,Description,Program,RaisedAt,ResolvedAt,TBF, TTR", ",");
	_typedata.Mixin(_faultFields, "adVarWChar");
	_typedata["ResolvedAt"]=_typedata["RaisedAt"]="adDate"; 
	_typedata["TBF"]=_typedata["TTR"]="adTime";// 00:00:00

	_programFields= TrimmedTokenize("Timestamp,Machine,Shift,program,Duration,Machining,Faulted,RpmTotal",",");
	_typedata.Mixin(_programFields, "adVarWChar");
	_typedata["Timestamp"]="adDate"; 
	_typedata["Duration"]=_typedata["Machining"]=_typedata["Faulted"]="adTime"; // 00:00:00


	SetTimer( 99, 1000 );

	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	KillTimer( 99);
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);


	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CWtlHtmlView* pView = new CWtlHtmlView;
	//TODO: Replace with a URL of your choice
	pView->Create(m_view, rcDefault, _T("about:blank"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, 0);
	m_view.AddPage(pView->m_hWnd, _T("DMG2479"));
	std::string cmd = CCMTConnectTrayIcon::CreateHtmlDocument();
	pView->SetDocumentText(cmd.c_str());
	return 0;
}

CWtlHtmlView * CMainFrame::AddWebTab(std::string name, CCMTConnectTrayIcon * tabWnd)
{
	CWtlHtmlView* pView = new CWtlHtmlView;
	//TODO: Replace with a URL of your choice
	pView->Create(m_view, rcDefault, _T("about:blank"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, 0);
	m_view.AddPage(pView->m_hWnd, name.c_str());
	std::string cmd = CCMTConnectTrayIcon::CreateHtmlDocument();
	pView->SetDocumentText(cmd.c_str());
	pHtmlViews.push_back(tabWnd);
	return pView;
}

CWtlHtmlView * CMainFrame::AddAlarmTab(std::string name, CCMTConnectTrayIcon * tabWnd)
{
	CWtlHtmlView* pView = new CWtlHtmlView;
	std::string contents;
	LoadHtmlResource(contents, IDR_HTML3, ::GetModuleHandle(NULL));
	WriteFile(::ExeDirectory()+"FaultHistory.html", contents);
	std::string url = "file:///" + ::ExeDirectory();
	url+="FaultHistory.html";
	ReplaceAll(url,"\\","/");
	pView->Create(m_view, rcDefault, url.c_str(), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, 0);
	m_view.AddPage(pView->m_hWnd, _T("Alarms History"));
	return pView;
}
LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndToolBar);
	::ShowWindow(m_hWndToolBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return 0;
}
void CMainFrame::CreateMonitorFile(std::string url)
{
	std::string filename = ::ExeDirectory() + "SeeMTConnect.html";
	std::string contents;
	LoadHtmlResource(contents, IDR_HTML2, ::GetModuleHandle(NULL));
	contents = ReplaceOnce(contents, "127.0.0.1", url);
	WriteFile(filename, contents);
}
LRESULT CMainFrame::OnWindowClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nActivePage = m_view.GetActivePage();
	if(nActivePage != -1)
		m_view.RemovePage(nActivePage);
	else
		::MessageBeep((UINT)-1);

	return 0;
}

LRESULT CMainFrame::OnWindowCloseAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_view.RemoveAllPages();

	return 0;
}

LRESULT CMainFrame::OnWindowActivate(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nPage = wID - ID_WINDOW_TABFIRST;
	m_view.SetActivePage(nPage);

	return 0;
}
LRESULT CMainFrame::OnSysCommand(UINT message, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
{
	// There is a system command
	// wparam has the exact command type that it is
	switch( wparam )
	{
	case SC_CLOSE:
		PostMessage(WM_CLOSE);
		bHandled=true;
		bHandled=true;
		return 0;
	case SC_MINIMIZE:
		this->ShowWindow(SW_HIDE);
		bHandled=true;
		return 0;

	case SC_MOVE:
		bHandled=false;
		OutputDebugString("You're moving the window!\n");
		// don't interfere with this one, otherwise your window
		// won't move normally when the user tries to move it!
		break;

		/**
		If password protection is enabled by policy, the screen saver is started regardless 
		of what an application does with the SC_SCREENSAVE notification—even if fails to 
		pass it to DefWindowProc.
		*/
	case SC_SCREENSAVE:     // screensaver wants to begin

		return 1;           // returning 0 PREVENTS those things from happening
		// Note about SC_SCREENSAVE:
		// Try this.  Go into your settings and change
		// your screensaver to start after 1 minute.
		// Then run this program and sit and wait
		// for the 1 minute.

		// The funny thing about this, is Windows will
		// keep trying to enter screen saver mode,
		// by sending your app a "message" every
		// half a second or so.

		// If all you do is return 0; from this
		// part, your app will keep on stopping
		// the screensaver from starting.  And
		// Windows will keep asking if it can
		// start the screen saver or not, until
		// the user does something to reset the
		// screensaver-turn-on timer like move the mouse
		// or press a key on the keyboard.

		// if you wait even longer, the same thing
		// happens with SC_MONITORPOWER, except
		// you keep getting 2 messages now.

	case SC_MONITORPOWER:   // monitor wants to shut off - powersaver mode
		return 1;           // returning 0 PREVENTS monitor from turning off
		//case SC_CLOSE:  //??
	case SC_MAXIMIZE:
		bHandled=false;
		return 0;
		break;

	default:
		bHandled=false;
	} // end wparam inner switch
	return 0;
} // end WndProc

LRESULT  CMainFrame::OnTrayIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(lParam)
	{
	case WM_LBUTTONDBLCLK:
		this->ShowWindow(SW_SHOW);
		break;
	case WM_RBUTTONDOWN:
	case WM_CONTEXTMENU:
		{
			CTrayIconImpl<CMainFrame> ::OnTrayIcon(uMsg, wParam, lParam, bHandled);
		}
		break;
	}
	return 0;
}

LRESULT CMainFrame::OnTimer(UINT message, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)\
{
	HICON icon;

	nIcon=++nIcon%2;
	if(StateIconMap.find(state)!=StateIconMap.end())
	{
		std::vector<HICON> icons = StateIconMap[state];
		if(icons[0]!=icons[1])
		{
			ChangeIcon(icons[nIcon]);
		}
	}
	if(pCNCSummaryView->IsWindowVisible())
	{
		std::string html=CreateSummaryHtmlTable();
		pCNCSummaryView->SetElementId("Device",html);
	}

	if(pFaultView->IsWindowVisible())
	{
		std::string html=CreateFaultHtmlTable();
		pFaultView->SetElementId("Device",html);
	}

	if(pProgramSummaryView->IsWindowVisible())
	{
		std::string html=CreateProgramHtmlTable();
		pProgramSummaryView->SetElementId("Device",html);
	}

	return 0;

	

}

std::string  CMainFrame::CreateSummaryHtmlTable()
{
	std::string htmltable;

	htmltable += "<TABLE class=\"gridtable\" >\n";
	htmltable +=   "<TR>";
	if(pHtmlViews.size()==0)
	{
		htmltable +=   "<TH>Machine</TH><TH>Power</TH><TH>Mode</TH><TH>Status</TH><TH>Program</TH><TH>Duration</TH><TH>Utilization</TH><TH>History</TH>\n";\
	}
	else
	{
		htmltable +=  pHtmlViews[0]->GetHeaderRow();
	}

	htmltable +=   "</TR>\n";
	for(int i=0; i< pHtmlViews.size(); i++)
	{
		htmltable +=   "<TR>\n";
		CCMTConnectTrayIcon * trayicon = pHtmlViews[i];
		htmltable += trayicon->GetHtmlStatus() ;
		htmltable +=   "</TR>\n";

	}
	htmltable += "</TABLE>\n";
	return htmltable;
}
std::string  CMainFrame::CreateFaultHtmlTable()
{
	static int _nLastFaultNum=-1;
	static std::string _sFaultTable;	

	//std::string timestamp = "'" + GetTimeStamp(LOCAL | MIDNITE | HUM_READ) +  "'";
	std::string timestamp = "'2013-2-1 00:00:00'";
	std::string faultwhere="RaisedAt >= " + timestamp; // 2013-2-1";

	std::string htmltable;
	htmltable+=CreateDbHtmlTable("Factory", "alarms", faultwhere, _faultFields, _sFaultTable, _nLastFaultNum); 
	return htmltable;
}

std::string  CMainFrame::CreateProgramHtmlTable()
{
	static int _nLastDbCount=-1;
	static std::string _sHtmlTable;	
	
	std::string htmltable;
	htmltable+=CreateDbHtmlTable("Factory", "programs","", _programFields, _sHtmlTable, _nLastDbCount); 
	return htmltable;
}

std::string CMainFrame::CreateDbHtmlTable(std::string schema, std::string table, std::string where, std::vector<std::string> &fields, std::string & sSavedHtml, int &nLastNum ) 
{ 	
	//value="<a href='"+  +" ' target='_blank'>" + value + "</a>";

	int n = odbc.sizeTable(schema, table);
	if(nLastNum==n)
	{
		return sSavedHtml;
	}
	nLastNum=n;

	std::string html;
	html += "<TABLE class=\"gridtable\">\n";
	html+= "<tr>\n";
	for(int i=0; i< fields.size(); i++)
		html+= "<TH>" +  fields[i] + " </TH>";
	html+= "</tr>\n";

	std::vector<std::vector<std::string> > datum;
	datum=odbc.selectTableWhere(schema, table, where, fields, _typedata);

	for(int j=0; j<datum.size(); j++)
	{
		std::vector<std::string>  datamap = datum[j];
		for(int i=0; i< fields.size(); i++)
		{	
			if(fields[i] == "Severity")
			{
				if(stricmp(datamap[i].c_str(),"warning") == 0)
					html+= "<td style='background-color:yellow;'>\n";
				else if(stricmp(datamap[i].c_str(),"fault") == 0)
					html+= "<td style='background-color:#FB7979;'>\n";
			}
			else
				html+= "<td align='right'> \n";

			html+=  datamap[i];
			html+= "</td>\n";
		}
		html+= "</tr>\n";
	}
	html+= "<tr>\n";
	html += "</TABLE>\n";
	 sSavedHtml =html;
	return html;
}