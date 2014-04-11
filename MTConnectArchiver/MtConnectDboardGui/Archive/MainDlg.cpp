// MainDlg.cpp
// c:\Program Files\NIST\proj\MTConnect\Nist\MTConnectGadgets\MtConnectDboard\MtConnectDboardGui\MainDlg.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"
#include "StdStringFcn.h"

CComPtr<IGlobalInterfaceTable> pGIT;
DWORD	dwWebBrowserCookie; //Save this to get the interface later

#ifdef THREAD
CMainDlg::CMainDlg(CDlgThread * thread):_thread(thread)
{

}
#endif
BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DlgResize_Init();
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
#ifdef THREAD
	CMessageLoop* pLoop = _thread->GetMessageLoop();
#else
	CMessageLoop* pLoop = _Module.GetMessageLoop();
#endif
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);


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


	CAxWindow wndIE = GetDlgItem(IDC_EXPLORER1);
	HRESULT hr;
	hr = wndIE.QueryControl ( &m_spWebBrowser );

	if ( m_spWebBrowser )
	{
		CComVariant v;  // empty variant
		m_spWebBrowser->Navigate ( CComBSTR("about:blank"), 
			&v, &v, &v, &v );
	}
	//Handle NavigateComplete2 event from the browser, get the document  element, call DispEventAdvise(pDocument). onclick event bubbles, so you  can handle it on the document and get clicks from all contained  elements.
	//DispEventAdvise(m_spWebBrowser); 

	//Then you need to register your interface to be used proccess wide:
	hr = pGIT->RegisterInterfaceInGlobal( (IUnknown*)(IWebBrowser2*)m_spWebBrowser, IID_IWebBrowser2, &dwWebBrowserCookie);
	std::string html= this->CreateHtmlDocument();
	_htmlsnapshot.SetDocumentText(m_spWebBrowser, html);
	this->SetTimer(1, 250); 

	return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	//CMessageLoop* pLoop = _Module.GetMessageLoop();
#ifdef THREAD
	CMessageLoop* pLoop = _thread->GetMessageLoop();
#else
	CMessageLoop* pLoop = _Module.GetMessageLoop();
#endif	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	// if UI is the last thread, no need to wait
	if(_Module.GetLockCount() == 1)
	{
		_Module.m_dwTimeOut = 0L;
		_Module.m_dwPause = 0L;
	}
	_Module.Unlock();

	return 0;
}

void CMainDlg::SetTooltip(std::string s)
{
	CTrayIconImpl<CMainDlg>::SetTooltipText(s.c_str());
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	CloseDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	RemoveIcon();
	KillTimer(1);
	HRESULT hr =(pGIT!=NULL) ? pGIT->RevokeInterfaceFromGlobal( dwWebBrowserCookie ) : S_OK;

	DestroyWindow();
	::PostQuitMessage(nVal);
}

LRESULT  CMainDlg::OnTrayIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(lParam)
	{
	case WM_LBUTTONDBLCLK:
		this->ShowWindow(SW_SHOW);
		break;
	case WM_RBUTTONDOWN:
	case WM_CONTEXTMENU:
		{
			CTrayIconImpl<CMainDlg> ::OnTrayIcon(uMsg, wParam, lParam, bHandled);
		}
		break;
	}
	return 0;
}
LRESULT CMainDlg::OnTimer(UINT message, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)\
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
	return 0;
}
LRESULT CMainDlg::OnSysCommand(UINT message, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
{
	// There is a system command
	// wparam has the exact command type that it is
	switch( wparam )
	{
	case SC_CLOSE:
		this->CloseDialog(0);
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

std::string CMainDlg::CreateHtmlDocument()
{
	std::string html;
	html += "<HTML>\n";
	html += "<HEAD><STYLE>" + HtmlStyle() + "</STYLE>"; 
	html +=" <title>MTConnect Data Trace</title>";
	html +="</HEAD>\n";
	html +="<BODY>\n";

	html += "<DIV id=""Header""> MTConnect Readings</DIV>\n";
	html += "<DIV id=""Device""> Loading... </DIV>\n";

	//html +="<input type=\"button\" value=\"BACK\" onClick=\"window.location='http://back.com' \">";
	//html +="<input type=\"button\" value=\"FWD\" onClick=\"window.location='http://fwd.com' \">";
	//html +="<input type=\"button\" value=\"RUN\" onClick=\"window.location='http://run.com' \">";

	html += "</BODY>\n";
	html += "</HTML>\n";
	return html;
}

std::string CMainDlg::HtmlStyle()
{
	std::string style;
	style+= "P\n";
	style+= "{\n";
	style+= "	FONT-FAMILY: ""Myriad"", sans-serif;\n";
//	style+= "	FONT-SIZE: 70%;\n";
	style+= "	LINE-HEIGHT: 12pt;\n";
	style+= "	MARGIN-BOTTOM: 0px;\n";
	style+= "	MARGIN-LEFT: 10px;\n";
	style+= "	MARGIN-TOP: 10px\n";
	style+= "}\n";

	style+= "H1\n";
	style+= "{\n";
	style+= "	BACKGROUND-COLOR: #003366;\n";
	style+= "	BORDER-BOTTOM: #336699 6px solid;\n";
	style+= "	COLOR: #ffffff;\n";
	style+= "	FONT-SIZE: 130%;\n";
	style+= "	FONT-WEIGHT: normal;\n";
	style+= "	MARGIN: 0em 0em 0em -20px;\n";
	style+= "	PADDING-BOTTOM: 8px;\n";
	style+= "	PADDING-LEFT: 30px;\n";
	style+= "	PADDING-TOP: 16px\n";
	style+= "}\n";
	style+= "table {\n";
	style+= " 	BACKGROUND-COLOR: #f0f0e0;\n";
	style+= "	BORDER-BOTTOM: #ffffff 0px solid;\n";
	style+= "	BORDER-COLLAPSE: collapse;\n";
	style+= "	BORDER-LEFT: #ffffff 0px solid;\n";
	style+= "	BORDER-RIGHT: #ffffff 0px solid;\n";
	style+= "	BORDER-TOP: #ffffff 0px solid;\n";
	//style+= "	FONT-SIZE: 70%;\n";
	style+= "	FONT-SIZE: 9pt;\n";
	style+= "	MARGIN-LEFT: 10px\n";
	style+= "  }\n";

	style+= "td {\n";
	style+= "	BACKGROUND-COLOR: #e7e7ce;\n";
	style+= "	BORDER-BOTTOM: #ffffff 1px solid;\n";
	style+= "	BORDER-LEFT: #ffffff 1px solid;\n";
	style+= "	BORDER-RIGHT: #ffffff 1px solid;\n";
	style+= "	BORDER-TOP: #ffffff 1px solid;\n";
	style+= "	PADDING-LEFT: 3px\n";
	style+= "  }\n";
	style+= "th {\n";
	style+= "	BACKGROUND-COLOR: #cecf9c;\n";
	style+= "	BORDER-BOTTOM: #ffffff 1px solid;\n";
	style+= "	BORDER-LEFT: #ffffff 1px solid;\n";
	style+= "	BORDER-RIGHT: #ffffff 1px solid;\n";
	style+= "	BORDER-TOP: #ffffff 1px solid;\n";
	style+= "	COLOR: #000000;\n";
	style+= "	FONT-WEIGHT: bold\n";
	style+= "  }\n";
	return style;
}
///////////////////////////////////////////////////////////////

BOOL CDlgThread::InitializeThread()
{
	AtlTrace("CDlgThread::InitializeThread()");
	HRESULT hRes = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));
	::DefWindowProc(NULL, 0, 0, 0L);
	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls
	AtlAxWinInit(); // NEW

	if(dlgMain.Create(NULL) == NULL)
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}

	dlgMain.ShowWindow(SW_HIDE);
	return TRUE;
}

void CDlgThread::CleanupThread(DWORD dwExitCode)
{
	AtlTrace("CDlgThread::CleanupThread");
	if(dlgMain.IsWindow())
		dlgMain.DestroyWindow();
	pGIT=NULL;
	::CoUninitialize();

}