//
// ListViewCtrl.h 
//
#pragma once
#include "Resource.h"
#include "StringEx.h"

#include "ParameterTable.h"
#include "WIN32MessageMeanings.h"
#define PROP_TEXT_INDENT 2

// Custom control messages
#ifndef WM_USER_UPDATEPROPERTY
#define WM_USER_UPDATEPROPERTY   WM_USER+465
#define WM_USER_CANCELPROPERTY   WM_USER+466
#define WM_USER_CHANGEDPROPERTY  WM_USER+467
#define WM_USER_EXPAND           WM_USER+468
#define WM_USER_COLLAPSE         WM_USER+469
#endif // WM_USER_UPDATEPROPERTY

class CListCtrl;

struct Columninfo 
{
	StringEx header;
	int size;
	int format;
	int sortorder;
	bool bReadWrite;
	// add image
	Columninfo(StringEx sHeader, int iSize, int iFormat, bool bReadWrite=false) 
	{
		header=sHeader; size=iSize; format=iFormat;
		sortorder=-1;
		this->bReadWrite=bReadWrite;
	}

};

typedef std::vector<Columninfo> ColumnInfoVector;

template <class T, class TBase = CEdit>
class CListViewInplaceCtrl : public CWindowImpl<T, TBase>
{
public:
	//DECLARE_WND_SUPERCLASS(_T("ListViewEditCtrl"), _T("EDIT"))
	//DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())
		bool m_fCancel;

	BEGIN_MSG_MAP(CListViewInplaceCtrl<T>)
	    MESSAGE_HANDLER(WM_CHAR, OnKeyDown)
	    MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
        MESSAGE_HANDLER(WM_GETDLGCODE, OnGetDlgCode)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()
   LRESULT OnEsc(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
   {

	   return 0;
   }
	LRESULT OnGetDlgCode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		return DefWindowProc(uMsg, wParam, lParam) | DLGC_WANTALLKEYS;
	}

  	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		switch( wParam )
		{
		case VK_ESCAPE:
			m_fCancel = true;
		case VK_RETURN:
			::SetFocus(GetParent());
			return 0;
			break;
		}
		bHandled = FALSE;
		return 0;
	}
};

class CListViewEditCtrl : public CListViewInplaceCtrl<CListViewEditCtrl, CEdit>
{
public:
	DECLARE_WND_SUPERCLASS(_T("ListViewEditCtrl"), _T("EDIT"))
};
class CListViewComboBoxCtrl : public CListViewInplaceCtrl<CListViewComboBoxCtrl, CComboBox>
{
public:
	DECLARE_WND_SUPERCLASS(_T("ListViewComboCtrl"), WC_COMBOBOX)
};

class CListCtrl  :
	public CListViewCtrl
{
public:

	ColumnInfoVector columns;
	bool m_bCreated;
	StringTable  table;
	StringEx m_sWindowTitle;
	int m_iSelectedRow;
	int  m_iSelectedCol;


	CListCtrl()  {}
	~CListCtrl() {}

	int GetRowCount(){ return table.size();}
	int GetColumnCount(){ return columns.size();}

	void SetColumnReadWrite(int col, bool bFlag)
	{
		if(col<0||col>=columns.size())
			return;
		columns[col].bReadWrite=bFlag;
	}
	void _GetInPlaceRect(int row, int col, RECT* pRect) const
   {
      if( col==0 ) {
         GetItemRect(row, pRect, LVIR_BOUNDS);
         if( columns.size() > 0 ) {
            RECT rcFirstColumn;
            GetSubItemRect(row, 1, LVIR_BOUNDS, &rcFirstColumn);
            pRect->right = rcFirstColumn.left-1;
         }
      }
      else {
         GetSubItemRect(row, col, LVIR_BOUNDS, pRect);
      }
      // Adjust (?)
      pRect->left += 1;
      pRect->top -= 1;
   }

   void InvalidateItem(int row, int col)
   {
      RECT rc;
      GetSubItemRect(row, col, LVIR_BOUNDS, &rc);
      InvalidateRect(&rc);
   }
	
	HRESULT CreateTable(HWND staticHwnd, CWindow * pParent)
	{
		CWindow wndStatic(staticHwnd);
		// Get static control rect, convert to parent's client coords.
		CRect rc;
		wndStatic.GetWindowRect(&rc);
		pParent->ScreenToClient(&rc);
		wndStatic.DestroyWindow();
				
//		ATLTRACE2(atlTraceControls,2,_T("CListViewDialog::ConstructTable Right=%d Bottom=%d Left=%d Top=%d\n"), 	rc.right, 	rc.bottom, 	rc.left, 	rc.top);
		this->Create(pParent->m_hWnd,rc,NULL,WS_CHILD|WS_VISIBLE |LVS_REPORT | LVS_SINGLESEL | WS_BORDER | WS_TABSTOP);
		//this->SetExtendedGridStyle(PGS_EX_SINGLECLICKEDIT);
		//::SendMessage(m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_ONECLICKACTIVATE);
		this->SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);
		this->DeleteAllItems();
		for(int j=0; j<columns.size(); j++)
		{
			this->InsertColumn(j, columns[j].header, LVCFMT_LEFT, columns[j].size, 0);
		}		
		//::SetWindowPos(m_hWnd, NULL, rc.left,rc.top, rc.right-rc.left, rc.bottom-rc.top , SWP_NOZORDER | SWP_NOACTIVATE);
		m_bCreated=true;
		return S_OK;
	}
	void ConstructHeader()
	{
		DeleteAllItems();
		for(int j=0; j<columns.size(); j++)
		{  //| LVCF_SUBITEM
			this->InsertColumn(j, columns[j].header, LVCFMT_LEFT, columns[j].size, 0);
			LV_COLUMN lvC;
			lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvC.fmt = LVCFMT_LEFT; // left-align column
			lvC.cx = columns[j].size; // width of column in pixels
			lvC.pszText = columns[j].header;
			ListView_SetColumn(this->m_hWnd, j, &lvC);		
		}
	}
	void UpdateTable()
	{
		ATLTRACE2(atlTraceControls,2,_T("CListCtrl::UpdateTable\n"));
		DeleteAllItems();
		TypeInfoTable::iterator theIterator;
		int i=0;
		for(int k=0; k<table.size(); k++,i++) 
		{
			// Column headers already added			
			StringVector value= table[i];
			for(int j=0; j<columns.size(); j++)
			{
				StringEx item = table[k][j];
				AddItem(k,j, item); // can also use to add column 0 to row
			}
		}
	}
	void UpdateValues()
	{
		ATLTRACE2(atlTraceControls,2,_T("CListCtrl::UpdateValues\n"));
		TypeInfoTable::iterator theIterator;
		int i=0;
		for(int k=0; k<table.size(); k++,i++) 
		{
			for(int j=0; j<columns.size(); j++)
			{
				SetValue(k,j, table[k][j]); // can also use to add column 0 to row
			}
		}
	}
	StringEx GetValue(int row, int col)
	{
		TCHAR szText[1024];
		GetItemText(row, col, szText, 1024);
		return szText;
	}

	double GetDoubleValue(int row, int col)
	{
		StringEx tmp = GetValue(row,  col);
		tmp.Trim();
		return tmp.GetDouble(0.0);
	}
	void SetDoubleValue(int row, int col, double dVal, char * sFormat="%4.2f")
	{
		SetValue(row,col, StringEx(sFormat,dVal));  
	}
	void SetIntValue(int row, int col, long lVal, char *  sFormat="%12d")
	{
		SetValue(row,col, StringEx(sFormat,lVal));  
	}
	void SetValue(int row, int col, StringEx szText)
	{
		StringEx showing= GetValue( row,  col);
		if(!szText.equals(showing))
		{
			table[row][col]= szText;  // row 0, item #
			SetItemText(row, col, szText);
		}
		// FIXME: invalidate the region contained by this row/col
	}
	void SetupColumnHeaders(WCHAR * name, long len, ...)
	{
		long nLen;
		WCHAR * sName=name;
		va_list ap;
		va_start(ap, name);
		while(sName!=NULL) 
		{
			nLen=va_arg( ap, long);
			columns.push_back(Columninfo(sName, nLen, 0));
			sName = va_arg(ap, WCHAR *);
		}
		va_end(ap);
	}
};

class CParameterListViewCtrl : 
	public CWindowImpl<CParameterListViewCtrl, CListCtrl>
{
public:
	DECLARE_WND_SUPERCLASS(_T("ParameterListViewCtrl"), _T("SysListView32"))
	typedef  CWindowImpl<CParameterListViewCtrl, CListCtrl>  CBaseParameterListViewCtrl;
	CListViewEditCtrl hEdit; 
	CListViewComboBoxCtrl hComboBox;
	CListViewComboBoxCtrl hEnumComboBox;
	bool m_bEditing;
	int m_iRow;
	int m_iCol;
	ParameterTable * m_ParameterTable; // this is for type information and initial values
	BEGIN_MSG_MAP(CParameterListViewCtrl)
	    MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnClick)
         MESSAGE_HANDLER(WM_SETFOCUS, OnKillFocus)
		REFLECTED_NOTIFY_CODE_HANDLER(CBN_EDITUPDATE, OnItemChanged)
	DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()
	LRESULT OnItemChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
	{

		return 0;
	}


	CParameterListViewCtrl()
	{
		m_ParameterTable= new ParameterTable;
	}
	StringEx ToString()
	{
		 return this->table.ToString(); 
	}

	void ClearTable()
	{
		m_ParameterTable->Clear();
	}
	void InitializeTable(ParameterTable * pParameterTable)
	{
		m_ParameterTable=pParameterTable;
	}

	BOOL SubclassWindow(HWND hWnd)
	{
		ATLASSERT(m_hWnd == NULL);
		ATLASSERT(::IsWindow(hWnd));
		BOOL bRet = CBaseParameterListViewCtrl::SubclassWindow(hWnd);
		SetupList();
		return  bRet;
	}
	BOOL SetupList()
	{
		m_bEditing=false;
		m_iRow=-1;
		SetExtendedListViewStyle(LVS_EX_GRIDLINES ,0);
		ModifyStyle( 0, LVS_SINGLESEL);

		hEdit.Create(m_hWnd,rcDefault,_T("ParameterEdit") ,ES_AUTOHSCROLL| ES_LEFT| WS_CHILD|WS_BORDER  );
		hEdit.ModifyStyle(WS_TABSTOP, ES_MULTILINE|ES_WANTRETURN | ES_LEFT|ES_AUTOHSCROLL);  // Some standard styles
		hEdit.SetFont( (HFONT)::SendMessage(this->m_hWnd, WM_GETFONT, 0, 0) );

		hComboBox.Create(m_hWnd,rcDefault,_T("ParameterEditDropdown"), WS_CHILD|  WS_BORDER | WS_VSCROLL |CBS_DROPDOWNLIST );
		hComboBox.AddString(_T("TRUE"));
		hComboBox.AddString(_T("FALSE"));
		hComboBox.SetExtendedUI(true);
		hComboBox.ModifyStyle(WS_TABSTOP, CBS_DROPDOWNLIST); // this does not work!
		hComboBox.SetFont( (HFONT)::SendMessage(GetParent(), WM_GETFONT, 0, 0) );

		hEnumComboBox.Create(m_hWnd,rcDefault,_T("ParameterEditDropdown"), WS_CHILD|  WS_BORDER | WS_VSCROLL |CBS_DROPDOWNLIST );
		hEnumComboBox.SetExtendedUI(true);
		hEnumComboBox.ModifyStyle(WS_TABSTOP, CBS_DROPDOWNLIST); // this does not work!
		hEnumComboBox.SetFont( (HFONT)::SendMessage(GetParent(), WM_GETFONT, 0, 0) );


		// not clear how to make this more flexible
		this->columns.push_back(Columninfo(L"Parameter", 200, LVCFMT_LEFT, false));
		this->columns.push_back(Columninfo(L"Value", 200, LVCFMT_LEFT, true));
		this->columns.push_back(Columninfo(L"Description", 500, LVCFMT_LEFT, false));

		ConstructHeader();
		//table.Tablize(m_ParameterTable->ToString(), columns.size(), L"=;");
		GenerateTable();
		UpdateTable();
		return true;
	}

	void GenerateTable()
	{
			TypeInfoTable::iterator it;
			for(it= m_ParameterTable-> entries.begin();it!= m_ParameterTable-> entries.end(); it++)
			{
				TypeInfo &info = (*it).second;
				StringVector  tokens;
				tokens.push_back(info.m_name);
				tokens.push_back(info.Retrieve());
				tokens.push_back(info.m_sDescription);
				table.push_back(tokens);
			}

	}

	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if( wParam==(WPARAM)hEdit.m_hWnd && m_bEditing==true)
		{
			if(hEdit.m_fCancel)
				return 0;
			TCHAR lpszString[255];
			hEdit.GetWindowText(lpszString,255);
			if(m_iRow!=-1)
				SetValue(m_iRow, m_iCol, lpszString);
			hEdit.ShowWindow(SW_HIDE);
			hEdit.MoveWindow(&rcDefault);
			m_bEditing=false;
			return 0;
		}
		else	if( (HWND) wParam==hComboBox.m_hWnd && m_bEditing==true)
		{	
			TCHAR lpszString[255];
			hComboBox.GetWindowText(lpszString,255);
			if(m_iRow!=-1)
				SetValue(m_iRow, m_iCol, lpszString);
			hComboBox.ShowWindow(SW_HIDE);
			hComboBox.MoveWindow(&rcDefault);
			m_bEditing=false;
	
		}
		else	if( (HWND) wParam==hEnumComboBox.m_hWnd && m_bEditing==true)
		{	
			TCHAR lpszString[255];
			hEnumComboBox.GetWindowText(lpszString,255);
			if(m_iRow!=-1)
				SetValue(m_iRow, m_iCol, lpszString);
			hEnumComboBox.ShowWindow(SW_HIDE);
			hEnumComboBox.MoveWindow(&rcDefault);
			m_bEditing=false;
	
		}
		return 1;
	}

	LRESULT OnClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{		
		// Invalidate row,col
		LVHITTESTINFO hti = { 0 };
		DWORD dwPos = ::GetMessagePos();
		POINTSTOPOINT(hti.pt, dwPos);
		ScreenToClient(&hti.pt);

		m_iRow = SubItemHitTest(&hti); //iItem is row number..
		m_iCol = hti.iSubItem;
		LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
		if(m_iRow<0)
			return 0;
		if(m_iCol<0 || m_iCol>=columns.size())
			return 0;
		if(!columns[m_iCol].bReadWrite)
			return 0;
		TypeInfo *typeinfo = this->m_ParameterTable->GetEntryProfile (table[m_iRow][0] );
		if(typeinfo == NULL)
			return 0 ; // errror of some kind

		if( StringEx(typeinfo->m_type).equals(L"OLE_COLOR"))
		{
			COLORREF cColor = table[m_iRow][m_iCol].GetInt(0);
			CColorDialog  dlg ( cColor);
			if(IDOK==dlg.DoModal())
			{
				cColor=  dlg.GetColor() ;
				SetValue(m_iRow,m_iCol,StringEx("0x%x",cColor));
			}

		}
		else if(StringEx(typeinfo->m_type).equals(L"Filename"))
		{
			StringEx extension = table[m_iRow][m_iCol].GetFileExtension();
			
			///static char szFilters[]="OPC Data Item Files (*.opc)\0*.opc\0";
			StringEx szFilters("Data  Files (*.%S)\0*.%S\0", (BSTR) extension, (BSTR) extension);
		
			//CFileDialog fileDlg (FALSE, "opc", table[m_iRow][m_iCol], OFN_HIDEREADONLY, szFilters, this->m_hWnd);//save
			CFileDialog fileDlg (FALSE, _T("opc"), table[m_iRow][m_iCol], OFN_HIDEREADONLY, szFilters, this->m_hWnd); //open
			if( fileDlg.DoModal ()==IDOK )
			{
				SetValue(m_iRow,m_iCol,fileDlg.m_szFileName);
			}

		}
		else if(StringEx(typeinfo->m_type).equals(L"Enumeration"))
		{
			CRect rect;
			_GetInPlaceRect(m_iRow,m_iCol,&rect);
			CEnumWrapper enums ( typeinfo->m_sComments);
			StringVector labels = enums.GetEnumerations();
			hEnumComboBox.ResetContent();
			for(int i=0; i< labels.size(); i++)
			{
				hEnumComboBox.AddString(labels[i]);
			}
			int cy = (hComboBox.GetCount() +1) * hComboBox.GetItemHeight(0);
			rect.bottom=rect.bottom + cy;
			hEnumComboBox.MoveWindow(rect);
			hEnumComboBox.SelectString(-1,table[m_iRow][m_iCol]);
			hEnumComboBox.ShowWindow(SW_SHOW);
			hEnumComboBox.SetFocus();

		}
		else if(StringEx(typeinfo->m_type).equals(L"bool"))
		{		
			CRect rect;
			_GetInPlaceRect(m_iRow,m_iCol,&rect);
			// Resize list to fit all items (but not more than 140 pixels)
			int cy = (hComboBox.GetCount() +1) * hComboBox.GetItemHeight(0);
			rect.bottom=rect.bottom + cy;
			//hComboBox.SelectString(-1, table[m_iRow][m_iCol]);
			hComboBox.MoveWindow(rect);
			hComboBox.SelectString(-1,table[m_iRow][m_iCol]);
			hComboBox.ShowWindow(SW_SHOW);
			hComboBox.SetFocus();
		}
		else
		{
			CRect rect;
			_GetInPlaceRect(m_iRow,m_iCol,&rect);
			hEdit.m_fCancel=false;
			hEdit.MoveWindow(rect);
			hEdit.SetWindowText(table[m_iRow][m_iCol]);
			hEdit.ShowWindow(SW_SHOW);
			hEdit.SetFocus();
		}
		m_bEditing=true;
		return 0;
	}
};

