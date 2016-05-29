//
// MainDlg.h : interface of the CMainDlg class
//

#pragma once
#include "ListCtrl.h"

class CMainDlg : public CDialogImpl<CMainDlg>
{
public:
	enum { IDD = IDD_MAINDLG };
	CMainDlg(std::string path)  : _path(path)
	{
		m_wndListCtrl.RegisterClass();
		TABLESIZE=1;
		m_wndListCtrl.RegisterClass();
		m_bShowThemed = FALSE;	
	}



	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_MULTICHECK, OnClick)
		NOTIFY_HANDLER_EX(IDC_LISTCTRL, LCN_SELECTED, OnSelected)	
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	void InitBasicList();
	void FillRow(int i);
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnClick(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	LRESULT OnSelected( LPNMHDR lpNMHDR );
	LRESULT OnListHyperLink( LPNMHDR lpNMHDR )
	{
		CListNotify *pListNotify = reinterpret_cast<CListNotify *>( lpNMHDR );
		
		MessageBox( _T( "Hyperlink clicked" ), _T( "Hyperlink" ) );
		
		return 0;
	}	
	std::string ReadConfigFile();
	void WriteConfigFile(std::string contents, std::string servicename);
protected:
	CListCtrl						m_wndListCtrl;
	CImageList						m_ilItemImages;
	CFont							m_fntCustomFont1;
	CFont							m_fntCustomFont2;
	CImageList						m_ilListItems;
	CButton							m_btnMultiSelect;
	int								TABLESIZE;
	std::string						_path;
	BOOL							m_bShowThemed;


	std::vector<std::string> ips, ports, rates, debugs, devices;
	CListArray < CString, CSimpleArrayEqualHelperFalse<CString> > scanComboList;
	CListArray < CString , CSimpleArrayEqualHelperFalse<CString> > errorComboList;
};
