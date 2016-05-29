// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ListCtrl.h"

class CMainDlg : public CAxDialogImpl<CMainDlg>,
		public CDialogResize<CMainDlg>

{
public:

	CMainDlg()
	{
		m_wndListCtrl.RegisterClass();
		_cfgfolder="C:\\Program Files\\MTConnect\\Agent";
		_mtport="80";
	//	TABLESIZE=1;
	}

	CListCtrl m_wndListCtrl;
	CImageList m_ilItemImages;
	CFont m_fntCustomFont1;
	CFont m_fntCustomFont2;
	CImageList m_ilListItems;
	//int TABLESIZE;
	static int m_nMachines;


	enum { IDD = IDD_MAINDLG };

	std::vector<std::string> ips, ports, rates, debugs, devices,names, alldevices;
	CListArray < CString > scanComboList;
	CListArray < CString > errorComboList;
	CListArray < CString > allDevicesComboList;
	std::string devicemenustr;
	std::string _cfgfolder;
	std::string _mtport;


	BEGIN_DLGRESIZE_MAP(CMainDlg)
		DLGRESIZE_CONTROL(IDC_LISTCTRL, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_TESTBUTTON, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_TESTBUTTON, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X | DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_MULTICHECK, OnClick)
		COMMAND_ID_HANDLER(IDC_TESTBUTTON, OnTest)
		COMMAND_ID_HANDLER(ID_FILE_SAVE, OnSaveCfg)
		COMMAND_ID_HANDLER(ID_FILE_OPEN, OnOpenCfg)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnNewCfg)
	
		NOTIFY_HANDLER_EX(IDC_LISTCTRL, LCN_LEFTCLICK, OnClicked)
		NOTIFY_HANDLER_EX(IDC_LISTCTRL, LCN_RIGHTCLICK, OnClicked)
		
//		NOTIFY_HANDLER_EX(IDC_LISTCTRL, LCN_HYPERLINK, OnListHyperLink)
		NOTIFY_HANDLER_EX(IDC_LISTCTRL, LCN_DBLCLICK, OnDoubleClicked)
		NOTIFY_HANDLER_EX(IDC_LISTCTRL, LCN_SELECTED, OnClicked)
		
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<CMainDlg>)
	END_MSG_MAP()

	std::string & HttpPort() { return  _mtport; }
	void InitBasicList();
	void FillRow(int i);
	void FillRow(int nNewItem, 
		std::string machinename, 
		std::string ipaddr,
		std::string devicetype,
		std::string portcol
		);

	void DeleteListItem(int nSelectedRow);
	void InsertListItem(int n);
	void AddLastRow();	
	std::string ReadConfigFile(std::string name);
	void WriteConfigFile(std::string contents);
	HRESULT Validate();
	void UpdateLists();
	LRESULT WriteCfgFile(std::string cfgfilename);
	void  Clear();
	// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnClick(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTest(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSaveCfg(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOpenCfg(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNewCfg(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRestartService(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	void UpdateLists(std::string config);
	int Tablesize() { return m_wndListCtrl.GetItemCount(); }
	LRESULT OnClicked( LPNMHDR lpNMHDR );
	LRESULT OnDoubleClicked( LPNMHDR lpNMHDR );
	LRESULT OnListHyperLink( LPNMHDR lpNMHDR )
	{
		CListNotify *pListNotify = reinterpret_cast<CListNotify *>( lpNMHDR );
		
		MessageBox( _T( "Hyperlink clicked" ), _T( "Hyperlink" ) );
		
		return 0;
	}	

};
