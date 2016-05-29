//
// ATLUtils.h
//

#pragma once
#include "StdStringFcn.h"
#include <streambuf>
#include <boost/algorithm/string.hpp> 

inline int MyLoadResource(HMODULE hMod, UINT nID, std::string& str)
{
    int nRetLen = 0;
	DWORD dwSize;
	HRSRC hrsrc;
	HGLOBAL hglob;

     str.empty();

   // Find the stringtable
    hrsrc = FindResource(hMod, MAKEINTRESOURCE(nID), "txt");
    if (hrsrc == 0) return 0;

    // load the stringtable resource
    hglob = LoadResource(hMod, hrsrc);
    if (hglob == 0) return 0;

	if( 0 == (dwSize = ::SizeofResource(hMod, hrsrc) / sizeof(char) ))
		return 0;

    // get pointer to stringtable data
    LPSTR lpResLock = static_cast<LPSTR>(LockResource(hglob));

	std::stringstream  s;
	s.write((const char *) lpResLock, dwSize);
	str=s.str();
	boost::replace_all(str, "\n", "\r\n");
	return str.size();
}
/**
Sample:
	inside a dialog .... with WM_RBUTTONDOWN command
	LRESULT OnPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
			int xPos = GET_X_LPARAM(lParam);   // horizontal position of cursor 
			int yPos = GET_Y_LPARAM(lParam);    // vertical position of cursor
				
	   		CAtlPopup menu(L"Properties",L"Save",L"-", L"Refresh", 0);
			String selection =menu.TrackPopupMenu(m_hWnd, xPos, yPos );
		return 0;

	}
	inside a dialog .... with NM_RCLICK notify from Listview
  	CPoint		pt;
    GetCursorPos(&pt);
    ScreenToClient(&pt);	
	CAtlPopup  pPopupMenu( L"Properties",L"-", L"Refresh", 0);
	String selection =pPopupMenu.TrackPopupMenu( m_hWnd, pt.x, pt.y);

*/
class CAtlPopup
{
public:
	std::vector<std::string> entries;
	std::vector<CMenuHandle> cmenus;

	CAtlPopup()
	{
	}
	~CAtlPopup()
	{
		cmenus.clear();
	}

	void CreateRecursiveMenu(CMenu & menu, std::string menulist)
	{
		entries = Tokenize(menulist,"\n");
		std::vector<HMENU> hMenus;
		std::vector<std::string> menustack;
		hMenus.push_back((HMENU) menu);
		cmenus.clear();
		for(int i=0; i< entries.size(); i++)
		{
			std::vector<std::string> items = Tokenize(entries[i],".");
			if(items.size() < 1)
				continue;

			std::string entry = items[items.size()-1];
			items.pop_back();

			// now we see if we have to pop stack
			while(menustack.size() > 0 && menustack!=items)
			{
				menustack.pop_back();
				hMenus.pop_back();
			}
			//Check branch - submenu
			if((i+1) < entries.size() && entries[i+1].find(entries[i])== 0)
			{
				CMenu * menu = new CMenu();
				menu->CreatePopupMenu();
				::AppendMenu(hMenus.back(),MF_STRING|MF_POPUP,(UINT_PTR)(HMENU)menu->m_hMenu,entry.c_str());
				hMenus.push_back(menu->m_hMenu);
				cmenus.push_back(menu->Detach());
				menustack.push_back(entry); 
				continue;
			}
			if(entry=="-")
			{
				::AppendMenu(hMenus.back(),MF_SEPARATOR,0,0);	
			}
			else
			{
				::AppendMenu(hMenus.back(),MF_STRING, i , entry.c_str());
			}
		}
	}
	std::string TrackPopupMenu(HWND hWnd, int xPos, int yPos, std::string listitems)
	{
		CMenu pPopupMenu;
		pPopupMenu.CreatePopupMenu();
		CreateRecursiveMenu(pPopupMenu, listitems);
		DWORD selection =pPopupMenu.TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,    xPos, yPos, hWnd);
		if(selection < 0 || selection >= entries.size())
			return "";
		return entries[selection];
	}
};

/**
EZFONT.C has only one function, called EzCreateFont, which you can use like so: 

hFont = EzCreateFont (hdc, szFaceName, iDeciPtHeight, iDeciPtWidth,
                      iAttributes, fLogRes) ;

*/
#if 0
inline HFONT EzCreateFont (HDC hdc, TCHAR * szFaceName, int iDeciPtHeight,
                    int iDeciPtWidth, int iAttributes, BOOL fLogRes)
{
     FLOAT      cxDpi, cyDpi ;
     HFONT      hFont ;
     LOGFONT    lf ;
     POINT      pt ;
     TEXTMETRIC tm ;
     
     SaveDC (hdc) ;
     
     SetGraphicsMode (hdc, GM_ADVANCED) ;
     ModifyWorldTransform (hdc, NULL, MWT_IDENTITY) ;
     SetViewportOrgEx (hdc, 0, 0, NULL) ;
     SetWindowOrgEx   (hdc, 0, 0, NULL) ;
     
     if (fLogRes)
     {
          cxDpi = (FLOAT) GetDeviceCaps (hdc, LOGPIXELSX) ;
          cyDpi = (FLOAT) GetDeviceCaps (hdc, LOGPIXELSY) ;
     }
     else
     {
          cxDpi = (FLOAT) (25.4 * GetDeviceCaps (hdc, HORZRES) /
                                        GetDeviceCaps (hdc, HORZSIZE)) ;
          
          cyDpi = (FLOAT) (25.4 * GetDeviceCaps (hdc, VERTRES) /
                                        GetDeviceCaps (hdc, VERTSIZE)) ;
     }
     
     pt.x = (int) (iDeciPtWidth  * cxDpi / 72) ;
     pt.y = (int) (iDeciPtHeight * cyDpi / 72) ;
     
     DPtoLP (hdc, &pt, 1) ;
     lf.lfHeight         = - (int) (fabs (pt.y) / 10.0 + 0.5) ;
     lf.lfWidth          = 0 ;
     lf.lfEscapement     = 0 ;
     lf.lfOrientation    = 0 ;
     lf.lfWeight         = iAttributes & EZ_ATTR_BOLD      ? 700 : 0 ;
     lf.lfItalic         = iAttributes & EZ_ATTR_ITALIC    ?   1 : 0 ;
     lf.lfUnderline      = iAttributes & EZ_ATTR_UNDERLINE ?   1 : 0 ;
     lf.lfStrikeOut      = iAttributes & EZ_ATTR_STRIKEOUT ?   1 : 0 ;
     lf.lfCharSet        = DEFAULT_CHARSET ;
     lf.lfOutPrecision   = 0 ;
     lf.lfClipPrecision  = 0 ;
     lf.lfQuality        = 0 ;
     lf.lfPitchAndFamily = 0 ;
     
     lstrcpy (lf.lfFaceName, szFaceName) ;
     
     hFont = CreateFontIndirect (&lf) ;
     
     if (iDeciPtWidth != 0)
     {
          hFont = (HFONT) SelectObject (hdc, hFont) ;
          
          GetTextMetrics (hdc, &tm) ;
          
          DeleteObject (SelectObject (hdc, hFont)) ;
          
          lf.lfWidth = (int) (tm.tmAveCharWidth *
                                        fabs (pt.x) / fabs (pt.y) + 0.5) ;
          
          hFont = CreateFontIndirect (&lf) ;
     }
     
     RestoreDC (hdc, -1) ;
     return hFont ;
}
// Derive from this class to implement the stock Font property
// This class is responsible for holding the m_pFont member and 
// updating the view in response to property change notifications
// from the font object.
template <class T>
class CStockFontImpl
{
public:
	CComPtr<IFontDisp> m_pFont; // Must be called m_pFont
	CComPtr<IAtlEventSink> m_pFontNotify;

	~CStockFontImpl() throw()
	{
		if (m_pFontNotify)
		{
			m_pFontNotify->Unadvise();
			m_pFontNotify.Release();
		}
	}

	void OnFontChanged() throw() // Must be called OnFontChanged
	{
		HRESULT hr = E_UNEXPECTED;

		if (m_pFontNotify)
			hr = m_pFontNotify->Unadvise();
		else
			hr = CPropertyNotifySinkImpl<CStockFontImpl>::Create(this, &m_pFontNotify);
		
		ATLASSERT(SUCCEEDED(hr) && (m_pFontNotify != NULL));
		
		hr = m_pFontNotify->Advise(m_pFont);
		ATLASSERT(SUCCEEDED(hr));
	}

	// Called by Font object when one of its properties changes
	HRESULT OnChanged(DISPID dispid) throw() // Must be called OnChanged
	{
		dispid; // eliminate C4100 warning

		// Update the display
		__if_exists(T::FireViewChange)
		{
			T* pT = static_cast<T*>(this);
			pT->FireViewChange();
		}
		return S_OK;
	}
};
#endif

// Simplified creation for COM font objects
template <class Q>
HRESULT AtlOleCreateFont(Q** pp, LPCOLESTR pszFontName = OLESTR("Arial"),
					  UINT nPointSize = 14, SHORT sCharset = ANSI_CHARSET,
					  SHORT sWeight = FW_NORMAL, BOOL bItalic = FALSE,
					  BOOL bUnderline = FALSE, BOOL bStrikethrough = FALSE) throw()
{
	FONTDESC fd;
	fd.cbSizeofstruct = sizeof(FONTDESC); 
	fd.lpstrName = const_cast<LPOLESTR>(pszFontName);
	fd.cySize.Lo = 10000 * nPointSize; // TODO - this matches FONTSIZE macro, but doesn't actually work
	fd.cySize.Hi = 0;
    fd.sWeight = sWeight; 
    fd.sCharset = sCharset; 
    fd.fItalic = bItalic; 
    fd.fUnderline = bUnderline; 
    fd.fStrikethrough = bStrikethrough;
	return OleCreateFontIndirect(&fd, __uuidof(Q), reinterpret_cast<void**>(pp));
}

// This class selects a standard COM font object into
// a device context, and deselects the font on destruction
class CSelectFont
{
public: 
	HFONT m_hFont;
	CSelectFont(HDC hDC, IUnknown* pFont) throw()
	{
		// Initialization
		m_hDC = hDC;
		m_hOldFont = NULL;
		m_hFont = NULL;

		if (pFont)
		{
			// Get the vtable interface
			HRESULT hr = pFont->QueryInterface(&m_pFont);
			ATLASSERT(SUCCEEDED(hr));

			// Get the Windows font handle
			if (SUCCEEDED(hr))
				hr = m_pFont->get_hFont(&m_hFont);

			// Lock the font handle
			if (SUCCEEDED(hr))
				hr = m_pFont->AddRefHfont(m_hFont);

			ATLASSERT(SUCCEEDED(hr));
		}

		// Select the font
		if (NULL != m_hFont)
			m_hOldFont = static_cast<HFONT>(SelectObject(hDC, m_hFont));
	}

	~CSelectFont() throw()
	{
		// Reset the font
		if (NULL != m_hFont)
		{
			SelectObject(m_hDC, m_hOldFont);
			HRESULT hr = m_pFont->ReleaseHfont(m_hFont);
			hr; // remove warning C4189
			ATLASSERT(SUCCEEDED(hr));
		}
	}

private:
	CComPtr<IFont> m_pFont;
	HFONT m_hOldFont;
	HDC m_hDC;
};



