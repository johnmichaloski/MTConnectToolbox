// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MainDlg.h"
#include <sstream>
#include <iomanip>
#include <atltime.h>
#include "StdStringFcn.h"
#include "RunProcess.h"

#define CHECKCOL 0
#define IPCOL 1
#define DEVICECOL  2
#define PORTCOL 3
#define SCANCOL 4
#define DEBUGCOL 5

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	//DebugBreak();
	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	m_wndListCtrl.SubclassWindow( GetDlgItem( IDC_LISTCTRL ) );
	m_btnMultiSelect  = GetDlgItem(IDC_MULTICHECK);
	m_btnMultiSelect.SetCheck(1);

	if ( !m_ilItemImages.CreateFromImage( IDB_EXAMPLE, 16, 0, RGB( 255, 0, 255 ), IMAGE_BITMAP, LR_CREATEDIBSECTION ) )
		return FALSE;

	scanComboList.Add( _T( "1 sec" ) );
	scanComboList.Add( _T( "2 sec" ) );
	scanComboList.Add( _T( "5 sec" ) );
	scanComboList.Add( _T( "10 sec" ) );
	scanComboList.Add( _T( "30 sec") );

	errorComboList.Add( _T( "fatal" ) );
	errorComboList.Add( _T( "error" ) );
	errorComboList.Add( _T( "warn" ) );
	errorComboList.Add( _T( "debug" ) );

	InitBasicList();
	m_wndListCtrl.ShowThemed( m_bShowThemed );
	return TRUE;
}

std::string CMainDlg::ReadConfigFile()
{
	std::string contents;
	ReadFile(_path + "Adapter.ini", contents);
	return contents;
}
void CMainDlg::WriteConfigFile(std::string contents, std::string servicename)
{
	// copy exe to servicename

	WriteFile(_path + servicename + ".ini", contents);
	// copy exe to servicname.exe
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	try {
	for(int i=0; i< TABLESIZE ; i++)
	{
		CString tmp;
		tmp = m_wndListCtrl.GetItemText(i,CHECKCOL);
		if(tmp.IsEmpty()) 
			continue;
		tmp =m_wndListCtrl.GetItemText(i,IPCOL);
		ips.push_back((LPCSTR) tmp); 
		tmp =	m_wndListCtrl.GetItemText(i,DEVICECOL);
		devices.push_back((LPCSTR)tmp); 
		tmp =	m_wndListCtrl.GetItemText(i,PORTCOL);
		ports.push_back((LPCSTR)tmp); 
		tmp =m_wndListCtrl.GetItemText(i,SCANCOL);
		// strip off units and convert to milliseconds
		if(tmp.Find("sec"))
		{
			tmp.Replace("sec","");
			tmp=tmp.TrimLeft();
			tmp=tmp.TrimRight();
			tmp+="000";
		}
		rates.push_back((LPCSTR)tmp); 
		tmp =m_wndListCtrl.GetItemText(i,DEBUGCOL);
		debugs.push_back((LPCSTR)tmp); 
	}
	std::string config = ReadConfigFile();
	std::string contents;
	std::string services;
	CButton c(GetDlgItem(IDC_MULTICHECK));

	if(c.GetCheck())
	{
		for(int i=0; i< ips.size(); i++)
		{
			std::string servicename=StdStringFormat("MTConnectOPCAdapter%d", i+1);
			contents=ReplaceOnce(config,"XXXX", servicename);
			contents=ReplaceOnce(contents,"YYYY", rates[i]);
			contents=ReplaceOnce(contents,"IPIPIP", ips[i]);			
			contents=ReplaceOnce(contents,"PPPP", ports[i]);
			contents=ReplaceOnce(contents,"DDDD", debugs[i]);
			contents=ReplaceOnce(contents,"MCMCMC", devices[i]);
			contents=ReplaceOnce(contents,"ZZZZ", "SIEMENS");
			WriteConfigFile(contents, servicename);
			
			::WriteFile("Run" + servicename + ".bat", _path+servicename+".exe debug\npause\n");
				
			CopyFile((_path+"MtcOpcAgent.exe").c_str(), (_path+servicename+".exe").c_str(), false);
			RedirectedIOProcess shell;
			std::string str=_path+servicename + "  install";
			str=shell.RunProcess(str.c_str());
			services+=servicename+",";

		}

	}
	else
	{
		std::string servicename="MTConnectOPCAdapter";
		contents=ReplaceOnce(config,"XXXX", servicename);
		contents=ReplaceOnce(contents,"YYYY", (rates.size()>0)? rates[0]: "1000");
		std::string ip, device, adapter;
		for(int i=0; i< ips.size(); i++)
		{
			if(i>0)  ip+=","; ip+=ips[i]; 
			if(i>0)  adapter+=","; adapter+="SIEMENS"; 
			if(i>0)  device+=","; device+=devices[i]; 

		}
		contents=ReplaceOnce(contents,"MCMCMC", device);
		contents=ReplaceOnce(contents,"ZZZZ", adapter);
		contents=ReplaceOnce(contents,"IPIPIP", ip);			
		contents=ReplaceOnce(contents,"PPPP", ports[0]);
		WriteConfigFile(contents, servicename);
		::WriteFile("Run" + servicename + ".bat", _path+servicename+".exe debug\npause\n");
		CopyFile((_path+"MtcOpcAgent.exe").c_str(), (_path+servicename+".exe").c_str(), false);
		RedirectedIOProcess shell;
		std::string str=_path+servicename + ".exe  install";
		str=shell.RunProcess(str.c_str());
		services+=servicename+",";
	}

	// Save services created.
	//HKLM\Software\MTConnect\AdapterOpc840D
	CRegKey key;
	long nError = key.Open(HKEY_LOCAL_MACHINE, "Software\\MTConnect\\AdapterOpc840D", KEY_WRITE);
	if(nError == ERROR_SUCCESS)
	{
		key.SetStringValue("Services", services.c_str());
	}
	key.Close();
	}
	catch(...) 
	{

	}

	EndDialog(IDOK);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

void CMainDlg::FillRow(int nNewItem)
{

	// Row i
	m_wndListCtrl.SetItemFormat(nNewItem,0, ITEM_IMAGE_3STATE_ON, ITEM_FORMAT_CHECKBOX );
	m_wndListCtrl.SetItemCheck(nNewItem, 0, 1 );
	m_wndListCtrl.SetItemFormat( nNewItem, IPCOL, ITEM_FORMAT_EDIT,ITEM_FLAGS_RIGHT);
	m_wndListCtrl.SetItemText( nNewItem, IPCOL, _T( "127.0.0.1" ));

	m_wndListCtrl.SetItemFormat( nNewItem, DEVICECOL, ITEM_FORMAT_EDIT,ITEM_FLAGS_RIGHT);
	m_wndListCtrl.SetItemFormat( nNewItem, PORTCOL, ITEM_FORMAT_EDIT, ITEM_FLAGS_EDIT_NUMBER|ITEM_FLAGS_RIGHT );
	if(	m_btnMultiSelect.GetCheck())
	{
		m_wndListCtrl.SetItemText( nNewItem, DEVICECOL, _T( "CNC1" ));
		m_wndListCtrl.SetItemText( nNewItem, PORTCOL,_T("7878") );
	}
	else
	{
		m_wndListCtrl.SetItemText( nNewItem, DEVICECOL,StdStringFormat("CNC%d", nNewItem ).c_str());
		m_wndListCtrl.SetItemText( nNewItem, PORTCOL, StdStringFormat("%d", 7878 + nNewItem ).c_str() );
	}
	m_wndListCtrl.SetItemFormat( nNewItem, SCANCOL, ITEM_FORMAT_COMBO,ITEM_FLAGS_RIGHT| ITEM_FLAGS_NONE, (CListArray<CString> &) scanComboList );
	m_wndListCtrl.SetItemComboIndex( nNewItem, SCANCOL, 0 ); // "Item 1"		
	m_wndListCtrl.SetItemFormat( nNewItem, DEBUGCOL, ITEM_FORMAT_COMBO,ITEM_FLAGS_RIGHT| ITEM_FLAGS_NONE,(CListArray<CString> &) errorComboList );
	m_wndListCtrl.SetItemComboIndex( nNewItem, DEBUGCOL, 0 ); // "Item 1"		

}

void CMainDlg::InitBasicList()
{
	LOGFONT logFont = { 0 };
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = 90;
	lstrcpy( logFont.lfFaceName, _T( "New Times Roman" ) );
	logFont.lfWeight = FW_BOLD;
	logFont.lfItalic = (BYTE)TRUE;

	m_fntCustomFont1.CreatePointFontIndirect( &logFont );

	logFont.lfHeight = 100;
	lstrcpy( logFont.lfFaceName, _T( "Arial" ) );
	logFont.lfUnderline = (BYTE)TRUE;
	m_fntCustomFont2.CreatePointFontIndirect( &logFont );

	m_wndListCtrl.SetImageList( m_ilItemImages );
	m_wndListCtrl.SetFocusSubItem( TRUE );



	m_wndListCtrl.AddColumn( _T( "" ), 0, ITEM_IMAGE_CHECKBOX, TRUE, ITEM_FORMAT_CHECKBOX );

	m_wndListCtrl.AddColumn( _T( "OPC IP" ), 100,0,FALSE,ITEM_FORMAT_EDIT,ITEM_FLAGS_RIGHT );
	m_wndListCtrl.AddColumn( _T( "Device" ), 100,0,FALSE,ITEM_FORMAT_EDIT,ITEM_FLAGS_RIGHT );
	m_wndListCtrl.AddColumn( _T( "SHDR Port" ), 100,0,FALSE,ITEM_FORMAT_EDIT,ITEM_FLAGS_RIGHT );

	m_wndListCtrl.AddColumn( _T( "Scan Time" ), 70, -1, FALSE, ITEM_FORMAT_NONE, ITEM_FLAGS_RIGHT );
	m_wndListCtrl.AddColumn( _T( "Debug" ), 70, -1, FALSE, ITEM_FORMAT_NONE, ITEM_FLAGS_RIGHT );
	//m_wndListCtrl.AddColumn( _T( "Progress" ), 170, ITEM_IMAGE_ATTACHMENT, FALSE, ITEM_FORMAT_PROGRESS );

	for(int i=0; i< TABLESIZE; i++)
	{
		int nNewItem = m_wndListCtrl.AddItem("");
		FillRow( nNewItem);
		//// Row i
		//int nNewItem = m_wndListCtrl.AddItem("");
		//m_wndListCtrl.SetItemFormat(nNewItem,0, ITEM_IMAGE_3STATE_ON, ITEM_FORMAT_CHECKBOX );
		//m_wndListCtrl.SetItemCheck(nNewItem, 0, 1 );
		//m_wndListCtrl.SetItemFormat( nNewItem, IPCOL, ITEM_FORMAT_EDIT,ITEM_FLAGS_RIGHT);
		//m_wndListCtrl.SetItemText( nNewItem, IPCOL, _T( "127.0.0.1" ));

		//m_wndListCtrl.SetItemFormat( nNewItem, DEVICECOL, ITEM_FORMAT_EDIT,ITEM_FLAGS_RIGHT);
		//m_wndListCtrl.SetItemText( nNewItem, DEVICECOL, _T( "CNC1" ));

		//m_wndListCtrl.SetItemFormat( nNewItem, PORTCOL, ITEM_FORMAT_EDIT, ITEM_FLAGS_EDIT_NUMBER|ITEM_FLAGS_RIGHT );
		//m_wndListCtrl.SetItemText( nNewItem, PORTCOL, StdStringFormat("%d", 7878 + i ).c_str() );
		//m_wndListCtrl.SetItemFormat( nNewItem, SCANCOL, ITEM_FORMAT_COMBO,ITEM_FLAGS_RIGHT| ITEM_FLAGS_NONE, (CListArray<CString> &) aComboList );
		//m_wndListCtrl.SetItemComboIndex( nNewItem, SCANCOL, 0 ); // "Item 1"		
		//m_wndListCtrl.SetItemFormat( nNewItem, DEBUGCOL, ITEM_FORMAT_COMBO,ITEM_FLAGS_RIGHT| ITEM_FLAGS_NONE,(CListArray<CString> &) bComboList );
		//m_wndListCtrl.SetItemComboIndex( nNewItem, DEBUGCOL, 0 ); // "Item 1"		
	}
	int nNewItem = m_wndListCtrl.AddItem("");
	m_wndListCtrl.SetItemFormat(nNewItem,0,  ITEM_FORMAT_ADDBOX,ITEM_IMAGE_ADDBOX );
	//m_wndListCtrl.SetItemImage(nNewItem,0, ITEM_IMAGE_ADDBOX );
		m_wndListCtrl.SetItemCheck(nNewItem, 0, 1 );
	//m_wndListCtrl.SetItemCheck(nNewItem, 0, 0 );

}
LRESULT CMainDlg::OnSelected( LPNMHDR lpNMHDR )
{
	CListNotify *pListNotify = reinterpret_cast<CListNotify *>( lpNMHDR );
	if(pListNotify->m_nItem == TABLESIZE &&  pListNotify->m_nSubItem == 0)
	{
		FillRow(TABLESIZE);
		TABLESIZE++;
		int nNewItem = m_wndListCtrl.AddItem("");
		m_wndListCtrl.SetItemFormat(nNewItem,0,  ITEM_FORMAT_ADDBOX,ITEM_IMAGE_ADDBOX );

	}

	return 0;
}	

LRESULT CMainDlg::OnClick(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
{ 
	CButton c(hWndCtl);

	for(int i=0; i< TABLESIZE ; i++)
	{
		std::string device, shdrport;
		if(c.GetCheck())
		{
			device="CNC1";
			shdrport =StdStringFormat("%d", 7878+i);
		}
		else
		{
			device = StdStringFormat("CNC%d", i+1);
			shdrport="7878";
		}
		m_wndListCtrl.SetItemText( i, DEVICECOL,device.c_str() );
		m_wndListCtrl.SetItemText( i, PORTCOL,shdrport.c_str() );

	}
	m_wndListCtrl.Invalidate();
	return 0;
}