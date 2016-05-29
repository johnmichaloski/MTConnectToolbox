#ifndef _BUTTONMENU_H_
#define _BUTTONMENU_H_

// Copyright (c) 2000 David Peterson
// All rights reserved.
//
// 7-19-2000
// - Initial release
//
// NO WARRANTIES ARE EXTENDED. USE AT YOUR OWN RISK.
//
// To contact the author with suggestions or comments, use gt7891a@hotmail.com.

#include <atlutils.h>
#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLCTRLS_H__
	#error ButtonMenu.h requires atlctrls.h to be included first
#endif

#ifndef __ATLMISC_H__
	#error ButtonMenu.h requires atlmisc.h to be included first
#endif

// TODO: In OnDraw code, the line "GetWindowLong(GWL_STYLE) & BS_DEFPUSHBUTTON"
//		 always returns true, regardless of what the style is.  Why???
// TODO: Add Create method that properly initializes control

template <class T, class TBase = CButton, class TWinTraits = CControlWinTraits>
class CButtonMenuImpl : public CWindowImpl<T, TBase, TWinTraits>
{
public:
//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
	CButtonMenuImpl():
	  m_Parent(this,1)
	{
		m_menu.CreatePopupMenu();
	}
//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
	~CButtonMenuImpl()
	{
		if ( m_Parent.IsWindow() )
			m_Parent.UnsubclassWindow();
		m_menu.DestroyMenu();
	}
//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
	BOOL AddMenuItem(UINT nMenuId,const CString& strMenu, UINT nMenuFlags = MF_STRING)
	{
		return m_menu.AppendMenu(nMenuFlags | MF_STRING, nMenuId, (LPCTSTR)strMenu);
	}
//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
	void AttachMenu(HMENU hMenu)
	{
		m_menu.DestroyMenu();
		m_menu.Attach(hMenu);
	}
//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
	BOOL DeleteMenuItem(UINT id, UINT nFlags = MF_BYCOMMAND)
	{
		return m_menu.DeleteMenu(id,nFlags);
	}
//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
	BOOL DeleteMenuItems()
	{
		m_menu.DestroyMenu();
		return m_menu.CreatePopupMenu();
	}

	DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())
protected:
	typedef CWindowImpl<T, TBase, TWinTraits> baseClass;
	BEGIN_MSG_MAP(CButtonMenuImpl)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
	END_MSG_MAP()

//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CPoint pt;
		UINT Flags = 0;
		Flags = _CalculateTrackMenuPosition(pt);
		m_menu.TrackPopupMenu(Flags|TPM_LEFTBUTTON,pt.x,pt.y,m_Parent);

		return 0;
	}
//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
	BOOL SubclassWindow(HWND hWnd)
	{
		if ( ::IsWindow(hWnd) )
		{
			BOOL bRet = FALSE;
			m_Parent.SubclassWindow(::GetParent(hWnd));
			if ( bRet = baseClass::SubclassWindow(hWnd) )
			{
				SetButtonStyle(BS_OWNERDRAW);
			}
			return bRet;
		}
		return FALSE;
	}
//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
	// Inspired by Norm Almond
	// http://www.codeproject.com/buttonctrl/pushmenubutton.asp
	// 29 June 2000
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DRAWITEMSTRUCT* lpDIS = (DRAWITEMSTRUCT*)lParam;
		if (lpDIS->CtlType != ODT_BUTTON)
			return 0;

		CFont Font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		CRect rect;
		CRect rectFocus;
		CRect rectText;
		CRect rectDropArrow;
		const int FocusMargin = 6;
		const int TextMargin = 2;
		const int DropArrowWidth = 16;
		CDC DC = lpDIS->hDC;

		rect = lpDIS->rcItem;

		////////////////////////////////////////
		// Button Background                  //
		////////////////////////////////////////
		DC.FillRect(rect,HBRUSH(GetSysColor(COLOR_BTNFACE)));

		// TODO:
		// BS_DEFPUSHBUTTON is always true, regardless of how we set it!
		BOOL bDefaultBtn = FALSE;//GetWindowLong(GWL_STYLE) & BS_DEFPUSHBUTTON;
		BOOL bDisabled = ODS_DISABLED & lpDIS->itemState;

		if (bDefaultBtn)
			rect.DeflateRect(1,1);

		rectFocus = rect;
		rectFocus.DeflateRect(FocusMargin,2);
		rectText = rectFocus;
		rectText.DeflateRect(TextMargin,TextMargin);

		DC.DrawFrameControl(&rect,DFC_BUTTON,DFCS_BUTTONPUSH);


		////////////////////////////////////////
		// Default Button State               //
		////////////////////////////////////////
		if (bDefaultBtn && !bDisabled)
			DC.FrameRect(&lpDIS->rcItem,(HBRUSH)GetStockObject(BLACK_BRUSH));


		////////////////////////////////////////
		// State Focus                        //
		////////////////////////////////////////
		if (lpDIS->itemState & ODS_FOCUS)
			DC.DrawFocusRect(&rectFocus);


		////////////////////////////////////////
		// Draw out text                      //
		////////////////////////////////////////
		DC.SelectFont(Font);

		long lLength = ::GetWindowTextLength(lpDIS->hwndItem);
		CComBSTR bstrCaption(lLength);
		::GetWindowText(lpDIS->hwndItem,bstrCaption.m_str,lLength);
		DC.SetBkMode(TRANSPARENT);
		DC.SetBkColor(GetSysColor(COLOR_BTNFACE));

		if (ODS_DISABLED & lpDIS->itemState)
		{
			rectText.OffsetRect(1,1);
			DC.SetTextColor(GetSysColor(COLOR_WINDOW));
			::DrawTextW(DC,bstrCaption,lstrlenW(bstrCaption),rectText,DT_SINGLELINE|DT_CENTER|DT_VCENTER);

			rectText.OffsetRect(-1,-1);
			DC.SetTextColor(GetSysColor(COLOR_GRAYTEXT));
			::DrawTextW(DC,bstrCaption,lstrlenW(bstrCaption),rectText,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		}
		else
		{
			DC.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
			::DrawTextW(DC,bstrCaption,lstrlenW(bstrCaption),rectText,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		}

		rectDropArrow = rect;
		rectDropArrow.DeflateRect(FocusMargin,FocusMargin);
		rectDropArrow.left = rectDropArrow.right - DropArrowWidth;

		CPoint pt(rectDropArrow.CenterPoint());
		pt += CPoint(0,1);

		DrawArrow(DC,pt,ODS_DISABLED & lpDIS->itemState);

		return 0;
	}
//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
	// Inspired by Norm Almond
	// http://www.codeproject.com/buttonctrl/pushmenubutton.asp
	// 29 June 2000
	void DrawArrow(CDC DC,CPoint ArrowTip, BOOL bDisabled = FALSE)
	{
		CPoint ptDest;
		CPoint ptOrig = ArrowTip;

		COLORREF cr = bDisabled ? GetSysColor(COLOR_GRAYTEXT) : RGB(0,0,0);
		CPen penArrow, penOld;
		
		penArrow.CreatePen(PS_SOLID,1,cr);
		penOld = DC.SelectPen(penArrow);

		DC.SetPixel(ArrowTip,cr);

		ArrowTip -= CPoint(1,1);
		DC.MoveTo(ArrowTip);

		ptDest = ArrowTip;
		ptDest += CPoint(3,0);
		DC.LineTo(ptDest);

		ArrowTip -= CPoint(1,1);
		DC.MoveTo(ArrowTip);

		ptDest = ArrowTip;
		ptDest += CPoint(5,0);
		DC.LineTo(ptDest);

		ArrowTip -= CPoint(1,1);
		DC.MoveTo(ArrowTip);

		ptDest = ArrowTip;
		ptDest += CPoint(7,0);
		DC.LineTo(ptDest);

		if ( bDisabled )
		{
			ptOrig += CPoint(0,1);
			penArrow.DeleteObject();
			penArrow.CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DHILIGHT));	
			DC.SelectPen(penArrow);
			DC.LineTo(ptOrig);
		}

		DC.SelectPen(penOld);
	}

private:
//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
	UINT _CalculateTrackMenuPosition(CPoint& pt)
	{
		CRect rect;
		GetWindowRect(&rect);
		pt.x = rect.left < 0 ? 0 : rect.left;
		pt.y = rect.bottom;
		return TPM_LEFTALIGN;
	}

	CMenu m_menu;
	CContainedWindow m_Parent;
	

};

class CButtonMenu : public CButtonMenuImpl<CButtonMenu>
{
public:
	DECLARE_WND_SUPERCLASS(_T("WTL_CButtonMenu"), GetWndClassName())
//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
	CButtonMenu& operator=(HWND hWnd)
	{
		if ( m_hWnd )
			DestroyWindow();
		if ( ::IsWindow(hWnd) )
			SubclassWindow(hWnd);
		return *this;
	}
};
#endif