#ifndef __BEVELLINE_H__
#define __BEVELLINE_H__

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CBevelLine - Gradient label control implementation
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2000.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#ifndef __cplusplus
#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
#error BevelLine.h requires atlapp.h to be included first
#endif

#ifndef __ATLCTRLS_H__
#error BevelLine.h requires atlctrls.h to be included first
#endif

#if (_WIN32_IE < 0x0400)
#error BevelLine.h requires _WIN32_IE >= 0x0400
#endif


template< class T, class TBase = CWindow, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CBevelLineImpl : public CWindowImpl< T, TBase, TWinTraits >
{
public:
	bool m_bSunken;
	int m_iLineWidth;
	int m_iLineHeight;
	CRect room;
	CString strText;

	CBevelLineImpl(int _iLineWidth=1,int iLineHeight=1, bool _bSunken=true) : 
	m_bSunken(_bSunken),
		m_iLineWidth(_iLineWidth),
		m_iLineHeight(iLineHeight)
	{ };

	~CBevelLineImpl()
	{
	}

	// Operations

	BOOL SubclassWindow(HWND hWnd)
	{
		ATLASSERT(m_hWnd == NULL);
		ATLASSERT(::IsWindow(hWnd));
		BOOL bRet = CWindowImpl< T, TBase, TWinTraits >::SubclassWindow(hWnd);
		return bRet;
	}

	// Implementation

	// Message map and handlers

	BEGIN_MSG_MAP(CBevelLineImpl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		T* pT = static_cast<T*>(this);
		if(wParam != NULL)
		{
			pT->DoPaint((HDC)wParam);
		}
		else
		{
			CPaintDC dc(m_hWnd);
			pT->DoPaint(dc.m_hDC);
		}
		return 0;
	}

	// Fixme, add left, center, right, top, middle, bottom
	void SetCaption(CString _text) { this->strText=_text; }
	void SetDimensions(CRect rect) { this->room = rect; }

	void Draw3dRect(CDCHandle dc, CRect r,  int width, COLORREF clrTopLeft, COLORREF clrBottomRight)
	{
		int x=r.left, y=r.top,  cx=r.Width(),  cy=r.Height();
		dc.FillSolidRect(x, y, cx - width, width, clrTopLeft);
		dc.FillSolidRect(x, y, width, cy - width, clrTopLeft);
		dc.FillSolidRect(x + cx, y, -width, cy, clrBottomRight);
		dc.FillSolidRect(x, y + cy, cx, -width, clrBottomRight);
	}
	void Draw3dRectDiagCross(CDCHandle dc, CRect r,  COLORREF clrTopLeft, COLORREF clrBottomRight)
	{
		CBrush brDiagCross;
		brDiagCross.CreateHatchBrush(HS_DIAGCROSS, RGB(0xff, 128, 0xff));
		CBrushHandle oldBrush = dc.SelectBrush(brDiagCross);
		dc.Rectangle(r.left,r.top,r.right,r.bottom);
		dc.SelectBrush(oldBrush);
	}

	void DrawGradientRect(CDCHandle dc, CRect rect,  COLORREF clr1, COLORREF clr2)
	{
		TRIVERTEX tv[2];
		tv[0].x=rect.left;
		tv[0].y=rect.top;
		tv[0].Red = (clr1&0xFF ) * 256;
		tv[0].Green =  ((clr1&0xFF00 )/ 0x100) * 256;
		tv[0].Blue = ((clr1&0xFF0000 )/ 0x10000) * 256;

		tv[1].x=rect.right;
		tv[1].y=rect.bottom;
		tv[1].Red = (clr2&0xFF ) * 256;
		tv[1].Green =  ((clr2&0xFF00 )/ 0x100) * 256;
		tv[1].Blue = ((clr2&0xFF0000 )/ 0x10000) * 256;

		GRADIENT_RECT gr = { 0, 1 };

		dc.GradientFill ( tv, 2, &gr, 1, GRADIENT_FILL_RECT_V );
	}

	void DoPaint(CDCHandle dc)
	{
		CRect r;
		//GetClientRect(&r); 
		DWORD hiCol = ::GetSysColor(!m_bSunken ? COLOR_3DHIGHLIGHT : COLOR_3DSHADOW); 
		DWORD loCol = ::GetSysColor(m_bSunken ? COLOR_3DHIGHLIGHT : COLOR_3DSHADOW); 
		//if( r.bottom > r.right) { // vertical
		//  r.right /= 2; 
		//  r.left = r.right - m_iLineWidth; 
		//  r.right += m_iLineWidth; 
		//} 
		//else { // horizonzal 
		//  r.bottom /= 2; 
		//  r.top = r.bottom - m_iLineHeight; 
		//  r.bottom += m_iLineHeight; 
		//}

		r=this->room; 
		Draw3dRect(dc, r,  2, COLOR_3DSHADOW,COLOR_3DHIGHLIGHT);
		r.DeflateRect( 2, 2 );		 			
		DrawGradientRect(dc, r,  RGB(230,230,218),RGB(194,189,174));  // off gray
		//Draw3dRectDiagCross( dc, r, RGB(0xFF,0xFF,0xFF), RGB(0xFF,0xFF,0x88));


		if(strText.IsEmpty())
			return;
		//CSize size; 
		//dc.GetTextExtent(strText, strText.GetLength(),&size);

		//CRect rcText(room.left+4,room.top+4 , room.right-8,room.top+size.cy+8); //  this->room );
		CRect rcText(room);
		rcText.OffsetRect( 2, 2 );		 			
		//dc.SelectFont( m_fntComboFont );
		//dc.SetTextColor( m_rgbStaticText );

		//dc.SetBkMode( TRANSPARENT );
		dc.DrawText( strText, strText.GetLength(), rcText, DT_CENTER| DT_SINGLELINE | DT_NOPREFIX | DT_TOP); //  DT_VCENTER );

	}
};


class CBevelSquare : public CBevelLineImpl<CBevelSquare>
{
public:
	DECLARE_WND_CLASS(_T("WTL_BevelSquare"))
	CStatic * _static;
	CBevelSquare(HWND hwnd, CString caption, CRect rect, int _iLineWidth=1,int iLineHeight=1, bool _bSunken=true)
		: CBevelLineImpl<CBevelSquare>(_iLineWidth, iLineHeight,  _bSunken)
	{
		_static = new CStatic();
		// 0,0 offset so you draw inside the parent dialog
		_static->Create(hwnd, CRect(0,0,2000,2000) ,_T("TEST"), WS_CHILD|WS_VISIBLE);
		this->SetDimensions(rect);
		if(!caption.IsEmpty())
		this->SetCaption(caption);

		this->SubclassWindow(_static->m_hWnd);

	}
	~CBevelSquare() 
	{
		_static->DestroyWindow();
	}
};


#endif //__BEVELLINE_H__
