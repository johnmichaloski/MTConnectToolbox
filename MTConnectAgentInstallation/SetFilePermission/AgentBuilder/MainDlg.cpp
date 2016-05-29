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
#include "UTils.h"
#include <stdlib.h>
#include "DcomUtils.h"
#include "AtlUtils.h"
#include "Yaml.h"

#pragma comment( lib, "msxml2.lib" )
#import <msxml6.dll>

#define CHECKCOL 0
#define DEVICECOL  1
#define NAMECOL  2
#define IPCOL 3
#define PORTCOL 4
#define SCANCOL 5
#define DEBUGCOL 6

static TCHAR strFilter[] = _T("CFG Files (*.cfg)\0*.cfg\0");

int CMainDlg::m_nMachines=1;

std::vector<std::string> ParseDevices(_bstr_t xmlfile)
{ 
	std::vector<std::string> devices;
	_bstr_t parseError(L"");
	MSXML2::IXMLDOMDocument2Ptr        m_pXMLDoc;
	try{
		IXMLDOMParseErrorPtr  pError;

		// load the XML file
		// ****** you need to use IXMLDOMDocument2 interface *********
		HRESULT hr = m_pXMLDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
		m_pXMLDoc->async =  VARIANT_FALSE; 

		hr = m_pXMLDoc->load(xmlfile);      
	
		//check on the parser error      
		if(hr!=VARIANT_TRUE)
		{
			return devices;
		}
		MSXML2::IXMLDOMNodePtr root = m_pXMLDoc->GetdocumentElement();
		MSXML2::IXMLDOMNodeListPtr nodes = root->selectNodes(_bstr_t("//Device/Description"));
		for(int i=0; i< nodes->length; i++)
		{
			MSXML2::IXMLDOMNodePtr pNode = NULL;					
			nodes->get_item(i, &pNode);
			bstr_t device = pNode->Gettext();
			devices.push_back((LPCSTR) device);
		}

	}
	catch(_com_error &e)
	{
		std::string errmsg=e.ErrorMessage();
		AtlTrace(errmsg.c_str());
	}
	return devices;
}

void CMainDlg::AddLastRow()
{		
	int nNewItem = m_wndListCtrl.AddItem("");
	m_wndListCtrl.SetItemFormat(nNewItem,0,  ITEM_FORMAT_ADDBOX,ITEM_IMAGE_ADDBOX );
	m_wndListCtrl.SetItemText( nNewItem, 0,"1");
	for(int i=1; i< m_wndListCtrl. GetColumnCount(); i++)
	{
		m_wndListCtrl.SetItemFormat(nNewItem,i,  ITEM_FORMAT_EDIT,ITEM_FLAGS_READ_ONLY );
		m_wndListCtrl.SetItemText( nNewItem, i, _T( "" ));
	}
}
void CMainDlg::InsertListItem(int n)
{
	m_wndListCtrl.InsertItem(n, "");
	FillRow(n);
}
void CMainDlg::DeleteListItem(int nSelectedRow)
{
	m_wndListCtrl.DeleteItem( nSelectedRow );
}

CString FolderRecurse(LPCTSTR pstr, CString startingdir)
{
   CFindFile finder;
   CString tmp;

   // build a string with wildcards
   CString strWildcard(pstr);
   strWildcard += _T("\\*.*");

   // start working for files
   BOOL bWorking = finder.FindFile(strWildcard);

   while (bWorking)
   {
      bWorking = finder.FindNextFile();
	  if(!bWorking)
		  break;

      if (finder.IsDots())
         continue;
      if (finder.IsDirectory())
      {
		  CString str = finder.GetFilePath();
		  str.Replace(startingdir, "");
		  str = str.Mid(1);
		  str.Replace("\\", ".");
		  str+="\n";
		  tmp += str;
		  tmp+=FolderRecurse(finder.GetFilePath(),startingdir);
		  continue;
	  }
	  CString str = finder.GetFilePath();
	  str.Replace(startingdir, "");
	  str = str.Mid(1);
	  str.Replace("\\", ".");
	  str.Replace(".txt", "");
	  str+="\n";
	  tmp += str;
  }
   finder.Close();
   return tmp;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	if ( !m_ilItemImages.CreateFromImage( IDB_EXAMPLE, 16, 0, RGB( 255, 0, 255 ), IMAGE_BITMAP, LR_CREATEDIBSECTION ) )
		return FALSE;
	m_wndListCtrl.SubclassWindow( GetDlgItem( IDC_LISTCTRL ) );
	DlgResize_Init(true);

	scanComboList.Add( _T( "1 sec" ) );
	scanComboList.Add( _T( "2 sec" ) );
	scanComboList.Add( _T( "5 sec" ) );
	scanComboList.Add( _T( "10 sec" ) );
	scanComboList.Add( _T( "30 sec") );

	errorComboList.Add( _T( "fatal" ) );
	errorComboList.Add( _T( "error" ) );
	errorComboList.Add( _T( "warn" ) );
	errorComboList.Add( _T( "debug" ) );

	CString sPattern = (ExeDirectory() + "Devices").c_str();
	devicemenustr = FolderRecurse(sPattern, sPattern);
	InitBasicList();
	return TRUE;
}

std::string CMainDlg::ReadConfigFile(std::string name)
{
	std::string contents;
	ReplaceAll(name, ".", "\\");
	std::string filename(::ExeDirectory() + "Devices\\" + name + ".txt");
	std::ifstream in( filename.c_str() );
	if(!in)
	{
		filename = RecursiveFileFind(::ExeDirectory() + "Devices\\", name + ".txt");
	}

	ReadFile(filename,contents);
	return contents;
}
void CMainDlg::WriteConfigFile(std::string contents)
{
	//WriteFile("C:\\Program Files\\MTConnect\\Agent\\devices.xml", contents);
	WriteFile(::ExeDirectory() + "\\devices.xml", contents);
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

void  CMainDlg::Clear()
{
	ips.clear(); 
	ports.clear(); 
	rates.clear();
	debugs.clear();
	devices.clear(); 
	names.clear();
}

void CMainDlg::UpdateLists(std::string config)
{
	Clear();
	std::vector<std::string> lines = Tokenize(config,"\n");
	for(int i=0;i<lines.size(); i++)
	{
		std::vector<std::string> tokens = Tokenize(lines[i],",");
		if(tokens.size() < 4)
			continue;
		names.push_back(tokens[0]); 
		devices.push_back(tokens[1]); 
		ips.push_back(tokens[2]); 
		ports.push_back(tokens[3]); 
	}
}	
void CMainDlg::UpdateLists()
{
	Clear();

	int i;
	for(i=0; i< Tablesize() -1 ; i++)
	{
		CString tmp;
		tmp = m_wndListCtrl.GetItemText(i,CHECKCOL);
		if(tmp.IsEmpty()) 
			continue;
		tmp =m_wndListCtrl.GetItemText(i,IPCOL);
		ips.push_back((LPCSTR) tmp); 

		tmp =m_wndListCtrl.GetItemText(i,NAMECOL);
		names.push_back((LPCSTR) tmp); 

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

	// Error checking 
	std::set<std::string> uniquenames;
	for(int i=0; i< names.size(); i++)
		uniquenames.insert(names[i]); 
	if(names.size() != uniquenames.size())
	{
		throw std::exception("Names must be unique - please fix");
	}
}

LRESULT CMainDlg::OnNewCfg(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_wndListCtrl.DeleteAllItems(  );
	AddLastRow();
	// Fixme: check for save file first
//	_cfgfolder="C:\\Program Files\\MTConnect\\Agent";
	_cfgfolder=ExeDirectory();
	return 0;
}

LRESULT CMainDlg::OnSaveCfg(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
//	CFolderDialog dlg;
//	dlg.DoModal();
	UpdateLists();
//	WriteCfgFile(dlg.m_szFolderPath);
	WriteCfgFile(ExeDirectory());

	return 0;

}
LRESULT CMainDlg::OnOpenCfg(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
{

	CFileDialog dlg(TRUE,
		_T("*"),
		(::ExeDirectory() + "\\Agent.cfg").c_str(), // "C:\\Program Files\\MTConnect\\Agent\\Agent.cfg",
		OFN_HIDEREADONLY,
		strFilter);
	if( dlg.DoModal() != IDOK )
		return 0;
	OnNewCfg(0,0,0,bHandled);

	_cfgfolder=dlg.m_szFileName;
	size_t endpos = _cfgfolder.find_last_of("\\"); 
	if(std::string::npos != endpos ) 
		_cfgfolder = _cfgfolder.substr( 0, endpos+1 ); 		 

	//std::vector<std::string> devices = ParseDevices((_cfgfolder+"devices.xml").c_str());
	Clear();

	Yaml storage;
	try {
		ParseIniFile(dlg.m_szFileName, storage );
		Yaml yaml = storage.GetSection("Adapters");
		std::vector<std::string> adapters = yaml.GetSections();
		for(int i=0; i< adapters.size(); i++)
		{
			Yaml section =  yaml.GetSection(adapters[i]);
			devices.push_back(section.keys["DeviceXml"]);
			names.push_back(adapters[i]);
			ips.push_back(section.keys["Host"]);
			ports.push_back(section.keys["Port"]);
		}

		for(int i=0; i< devices.size(); i++)
		{
			int nNewItem = m_wndListCtrl.InsertItem(i, "");
			FillRow(nNewItem, 
				names[i],
				ips[i],
				devices[i],
				ports[i]
			);
		}
	}
	catch(...)
	{

	}
	return 0;
}

LRESULT CMainDlg::WriteCfgFile(std::string cfgfolder)
{
	try {
	}
	catch(std::exception e)
	{
		std::string errmsg("Configuration not written - ");
		errmsg+=e.what();
		MessageBox(errmsg.c_str(), "Error", MB_OK);
		return 0;
	}

	std::string contents;
	contents+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	contents+="<MTConnectDevices xmlns=\"urn:mtconnect.org:MTConnectDevices:1.1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"urn:mtconnect.org:MTConnectDevices:1.1 http://www.mtconnect.org/schemas/MTConnectDevices_1.1.xsd\">\n";
	contents+=StdStringFormat("<Header bufferSize=\"130000\" instanceId=\"1\" creationTime=\"%s\" sender=\"local\" version=\"1.1\"/>\n",
		GetTimeStamp().c_str());
	contents+="<Devices>\n";

	// Check all machine names unique

	// Generate Devices.xml file with all devices in it.
	for(UINT i=0; i<devices.size(); i++)
	{

		std::string config = ReadConfigFile(devices[i]);
		ReplaceAll(config,"####",names[i]); // StdStringFormat("Dev%d",i));
		config=ReplaceOnce(config,"name=\"NNNNNN\"","name=\""+names[i]+"\"");
		contents+=config+"\n";
	}
	contents+="</Devices>\n";
	contents+="</MTConnectDevices>\n";

	WriteFile(cfgfolder + "\\devices.xml", contents);

	// Generate agent.cfg file with all devices in it.
	std::string cfg;
	//MyLoadResource(_Module.GetModuleInstance(), IDR_TXT1, cfg);
	//ReadFile(::ExeDirectory() + "Agent.cfg",cfg);
	cfg="Devices = Devices.xml\n";
	cfg+="Port = " + _mtport + "\n";
	cfg+="ServiceName = MTConnectAgent\n";
	cfg+="CheckpointFrequency=10000\n";
	cfg+="Adapters \n{\n";
	for(UINT i=0; i<devices.size(); i++)
	{
		cfg+="\t"+names[i] + "\n";
		cfg+="\t{\n";
		cfg+="\t\tHost = " + ips[i] +  "\n";
		cfg+="\t\tPort = " + ports[i] + "\n";
		cfg+="\t\tDeviceXml = " + devices[i] + "\n";
		cfg+="\t}\n";
	}
	cfg+="}\n";
	cfg+="# Logger Configuration\n";
	cfg+="logger_config\n";
	cfg+="{\n";
	cfg+="\tlogging_level = fatal\n";
	cfg+="\toutput = cout\n";
	cfg+="}\n";
	WriteFile(cfgfolder+"\\agent.cfg", cfg);
	return 0;
}

HRESULT CMainDlg::Validate()
{
	if(ConvertString<int>(_mtport, -1)<0)
		return E_INVALIDARG;

	// FIXME: test to see if all devicesxml exist

	return S_OK;
}

LRESULT CMainDlg::OnRestartService(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int resp = MessageBox("Restart Agent Service?", "Service Request",MB_OKCANCEL);
	if(resp==IDOK)
	{
		int err = system("sc.exe stop MTConnectAgent");
		::Sleep(3000);
		err = system("sc.exe start MTConnectAgent");
	}
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
{

	UpdateLists();
	// Fixme: any changes to config files?
	WriteCfgFile(_cfgfolder);
	//if(_cfgfolder=="C:\\Program Files\\MTConnect\\Agent")
	//{
	//	OnRestartService(0,0,0,bHandled);
	//}
	EndDialog(wID);
	return 0;
}
LRESULT CMainDlg::OnTest(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	UpdateLists();
	for(UINT i=0; i<devices.size(); i++)
	{
		if(TestPing(devices[i])==0)
		{ // failed
		}
		
	}
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}


void CMainDlg::FillRow(int nNewItem )
{
	FillRow(nNewItem, 
		StdStringFormat("M%03d", m_nMachines++ ),
		"127.0.0.1",
		StdStringFormat("Generic", nNewItem ),
		StdStringFormat("%d", 7878   ) //7878 + nNewItem 
		);
}
void CMainDlg::FillRow(int nNewItem, 
					   std::string machinename, 
					   std::string ipaddr,
					   std::string devicetype,
					   std::string portcol
					   )
{

	// Row i
	m_wndListCtrl.SetItemText( nNewItem, 0, "0"); 
	m_wndListCtrl.SetItemFormat(nNewItem,0,  ITEM_FORMAT_ADDBOX,ITEM_IMAGE_ADDBOX );
	m_wndListCtrl.SetItemCheck(nNewItem, 0, 1 );

//	m_wndListCtrl.SetItemFormat( nNewItem, DEVICECOL, ITEM_FORMAT_COMBO,ITEM_FLAGS_RIGHT| ITEM_FLAGS_NONE, (CListArray<CString> &) allDevicesComboList);
	m_wndListCtrl.SetItemFormat( nNewItem, DEVICECOL, ITEM_FORMAT_NONE, ITEM_FLAGS_RIGHT );

	m_wndListCtrl.SetItemFormat( nNewItem, PORTCOL, ITEM_FORMAT_EDIT, ITEM_FLAGS_EDIT_NUMBER|ITEM_FLAGS_RIGHT );
	
	
	m_wndListCtrl.SetItemFormat( nNewItem, NAMECOL, ITEM_FORMAT_EDIT,ITEM_FLAGS_RIGHT);
	m_wndListCtrl.SetItemText( nNewItem, NAMECOL,machinename.c_str());


	m_wndListCtrl.SetItemFormat( nNewItem, IPCOL, ITEM_FORMAT_EDIT,ITEM_FLAGS_RIGHT);
	m_wndListCtrl.SetItemText( nNewItem, IPCOL, ipaddr.c_str());


	m_wndListCtrl.SetItemText( nNewItem, DEVICECOL,devicetype.c_str());
	m_wndListCtrl.SetItemText( nNewItem, PORTCOL, portcol.c_str() );

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

	//m_wndListCtrl.AddColumn( _T( "" ), 0, ITEM_IMAGE_CHECKBOX, TRUE, ITEM_FORMAT_CHECKBOX );
	m_wndListCtrl.AddColumn( _T( "" ), 0, ITEM_IMAGE_ADDBOX /*ITEM_IMAGE_CHECKBOX*/, TRUE, ITEM_FORMAT_CHECKBOX );

	m_wndListCtrl.AddColumn( _T( "Device" ), 200,ITEM_IMAGE_NONE,FALSE,ITEM_FORMAT_EDIT| LVCF_WIDTH,ITEM_FLAGS_RIGHT );
	m_wndListCtrl.AddColumn( _T( "Name" ), 100,ITEM_IMAGE_NONE,FALSE,ITEM_FORMAT_EDIT| LVCF_WIDTH,ITEM_FLAGS_RIGHT );
	m_wndListCtrl.AddColumn( _T( "IP" ), 100,ITEM_IMAGE_NONE,FALSE,ITEM_FORMAT_EDIT,ITEM_FLAGS_RIGHT );
	m_wndListCtrl.AddColumn( _T( "SHDR Port" ), 100,ITEM_IMAGE_NONE,FALSE,ITEM_FORMAT_EDIT,ITEM_FLAGS_RIGHT );

	m_wndListCtrl.AddColumn( _T( "SampleRate" ), 70, ITEM_IMAGE_NONE, FALSE, ITEM_FORMAT_NONE, ITEM_FLAGS_RIGHT );
	m_wndListCtrl.SetColumnFormat( SCANCOL, ITEM_FORMAT_COMBO, ITEM_FLAGS_RIGHT, (CListArray<CString> &) scanComboList );

	m_wndListCtrl.AddColumn( _T( "Debug" ), 70, ITEM_IMAGE_NONE, FALSE, ITEM_FORMAT_NONE, ITEM_FLAGS_RIGHT );
	m_wndListCtrl.SetColumnFormat( DEBUGCOL, ITEM_FORMAT_COMBO, ITEM_FLAGS_RIGHT, (CListArray<CString> &) errorComboList );

	//m_wndListCtrl.AddColumn( _T( "Progress" ), 170, ITEM_IMAGE_ATTACHMENT, FALSE, ITEM_FORMAT_PROGRESS );
	m_wndListCtrl.ShowHeaderSort( FALSE );

	for(int i=0; i<  Tablesize(); i++)
	{
		int nNewItem = m_wndListCtrl.AddItem("");
		FillRow( nNewItem);
	}
	AddLastRow();
}
LRESULT CMainDlg::OnDoubleClicked( LPNMHDR lpNMHDR )
{
	CListNotify *pListNotify = reinterpret_cast<CListNotify *>( lpNMHDR );
	// + Add image
	if(pListNotify->m_nItem == (Tablesize()-1) &&  pListNotify->m_nSubItem == 0)
	{
		// FIXME: problem with zero length vector
		InsertListItem(Tablesize()-1);
	}
	// - remove image
	else if(pListNotify->m_nItem>= 0 && pListNotify->m_nItem < Tablesize() &&  pListNotify->m_nSubItem == 0)
	{
		DeleteListItem(pListNotify->m_nItem); 
	}
	return 0;
}	
LRESULT CMainDlg::OnClicked( LPNMHDR lpNMHDR )
{
	CListNotify *pListNotify = reinterpret_cast<CListNotify *>( lpNMHDR );
	int nNewItem = pListNotify->m_nItem;
	int k = pListNotify->m_nSubItem; 
	// skip last row
	if(pListNotify->m_nItem >= ( Tablesize() - 1)  && ( k > 0))
	{
		SetMsgHandled( TRUE );
		return 0;
	}
	if(k==1)
	{
		CPoint		pt;
		GetCursorPos(&pt);
		CAtlPopup popup;
		std::string selection =  popup.TrackPopupMenu(m_hWnd,pt.x, pt.y, devicemenustr);
		m_wndListCtrl.SetItemText( nNewItem, 1, selection.c_str());
		m_wndListCtrl.InvalidateItem(nNewItem, 1);
	}
	SetMsgHandled( FALSE );
	return 1;

}
LRESULT CMainDlg::OnClick(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
{ 
	CButton c(hWndCtl);

	for(int i=0; i<  Tablesize() ; i++)
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