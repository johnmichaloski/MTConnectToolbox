// P21toHtmlView.cpp : implementation of the CWtlHtmlView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "WtlHtmlView.h"
#include "docobj.h"
#include "comutil.h"
#include "StdStringFcn.h"

CWtlHtmlView::CWtlHtmlView()
{
	m_pBody = NULL;
	m_spHTMLDocument= NULL;
	m_spWebBrowser2= NULL;
	bBackEnable=true;
	bForwardEnable=true;

}
CWtlHtmlView::~CWtlHtmlView()
{
	if(m_spWebBrowser2!=NULL)
	{
		DispEventUnadvise ( m_spWebBrowser2 );
	}
}
BOOL CWtlHtmlView::PreTranslateMessage(MSG* pMsg)
{
	if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
	   (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
		return FALSE;

	// give HTML page a chance to translate this message
	return (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
}
LRESULT CWtlHtmlView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}
CComPtr<IWebBrowser2> CWtlHtmlView::GetWebBrowser()
{
	CComPtr<IWebBrowser2> pWebBrowser2;
	pWebBrowser2=NULL;

	if( SUCCEEDED(QueryControl(__uuidof(pWebBrowser2), reinterpret_cast<void**>(&pWebBrowser2))) )
	{
		return pWebBrowser2;
	}
	return NULL;
}
BOOL CWtlHtmlView::GetBody()
{
	HRESULT hr;
	CComPtr<IDispatch> pDisp;
	CComVariant ve;
	// navOpenInNewWindow, navNoHistory,navNoReadFromCache,navNoWriteToCache
	// navAllowAutosearch,navBrowserBar,navHyperlink
	CComVariant vBrowserNav; 
	if(m_spHTMLDocument!=NULL)
		return 0;
	
	if((m_spWebBrowser2!=NULL && m_spHTMLDocument == NULL))
	{
		hr=DispEventAdvise ( m_spWebBrowser2 );
		if(FAILED(m_spWebBrowser2->get_Document( &pDisp)))
			OutputDebugString("get_Document Failed");
		if(pDisp!=NULL) 
		{
			if(FAILED(pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&m_spHTMLDocument))) 
				OutputDebugString("QI IID_IHTMLDocument2 Failed");
		}
		return 0;
	}
	// Get the browser interface from the embedded HTML control
	//if((m_spWebBrowser==NULL) && FAILED(QueryControl(__uuidof(IWebBrowser), reinterpret_cast<void**>(&m_spWebBrowser))) )
	//	m_spWebBrowser=NULL;

	if(m_spWebBrowser2==NULL &&  SUCCEEDED(QueryControl(__uuidof(m_spWebBrowser2), reinterpret_cast<void**>(&m_spWebBrowser2))) )
	{
		if(m_spWebBrowser2!=NULL)
		{
			hr=DispEventAdvise ( m_spWebBrowser2 );
			if(FAILED(m_spWebBrowser2->get_Document( &pDisp)))
				OutputDebugString("get_Document Failed");
			if(pDisp!=NULL) 
			{
				if(FAILED(pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&m_spHTMLDocument))) 
					OutputDebugString("QI IID_IHTMLDocument2 Failed");
			}
		}
	}
	return FALSE;
}
HRESULT CWtlHtmlView::SetDocumentText( CString cstr)
{
	_bstr_t str(cstr);
	HRESULT hr = S_OK;
	VARIANT *param;
	SAFEARRAY *sfArray;
	IHTMLDocument2 *document;
	BSTR bstr = str.Detach();

	// Creates a new one-dimensional array
	sfArray = SafeArrayCreateVector(VT_VARIANT, 0, 1);

	if(m_spHTMLDocument==NULL)
		this->GetBody();

	document= m_spHTMLDocument;
	if (sfArray == NULL || document == NULL) {
		hr=E_FAIL;
		goto cleanup;
	}

	hr = SafeArrayAccessData(sfArray,(LPVOID*) & param);
	param->vt = VT_BSTR;
	param->bstrVal = bstr;
	hr = SafeArrayUnaccessData(sfArray);
	hr = document->write(sfArray);

cleanup:
	if (sfArray != NULL) {
		SafeArrayDestroy(sfArray);
	}
	return hr;
}
HRESULT CWtlHtmlView::Navigate(CString url)
{
	USES_CONVERSION;
	CComVariant vurl((BSTR) T2W( url));
	CComVariant ve;
	CComVariant vBrowserNav; 
	GetBody();
	return m_spWebBrowser2->Navigate2(&vurl,
					&vBrowserNav, 
					&ve, //TargetFrameName _BLANK  _PARENT  _SELF _TOP  
					&ve, &ve);

}

bool CWtlHtmlView::QueryStatus( OLECMDID nCmdID, int value) 
{
	OLECMDF eQuery ; //      'return value type for QueryStatusWB
	if(m_spWebBrowser2==NULL)
		return false;

	if(FAILED( m_spWebBrowser2->QueryStatusWB(OLECMDID_PRINT, &eQuery))) //  'get print command status
		return false;

	if(( eQuery & value) )
		return true;
	return false;
}



//OLECMDID_PRINT	= 6,
HRESULT CWtlHtmlView::ExecCommand( OLECMDID nCmdID, OLECMDEXECOPT nCmdExecOpt, VARIANTARG* pvarargIn, VARIANTARG* pvarargOut) 
{
	HRESULT hr=S_OK;
	LPDISPATCH lpDispatch = NULL;
	LPOLECOMMANDTARGET lpOleCommandTarget = NULL;
	try {

		m_spWebBrowser2->get_Document(&lpDispatch);
		if(lpDispatch==NULL)
			throw;
		OLECMDF eQuery ; //      'return value type for QueryStatusWB

		if(FAILED( m_spWebBrowser2->QueryStatusWB(OLECMDID_PRINT, &eQuery))) //  'get print command status
			throw;

		if(!( eQuery & OLECMDF_ENABLED) )
			throw;

		lpDispatch->QueryInterface(IID_IOleCommandTarget, (void**)&lpOleCommandTarget);
		if(lpOleCommandTarget==NULL)
			throw;

		// Print contents of WebBrowser control.
		hr=lpOleCommandTarget->Exec(NULL, nCmdID, nCmdExecOpt, pvarargIn, pvarargOut);
	}
	catch(...)
	{
		hr=E_FAIL;
	}
	if(lpDispatch)
		lpDispatch->Release();
	if(lpOleCommandTarget)
		lpOleCommandTarget->Release();
	return hr;
}

//OLECMDID_PRINT	= 6,
HRESULT CWtlHtmlView::OnExecCommand( OLECMDID nCmdID, OLECMDEXECOPT nCmdExecOpt, VARIANTARG* pvarargIn, VARIANTARG* pvarargOut) 
{
	return m_spWebBrowser2->ExecWB(nCmdID, nCmdExecOpt, pvarargIn, pvarargOut);
	
}

void CWtlHtmlView::GoBack() 
{
	if(m_spWebBrowser2==NULL && FAILED(QueryControl(__uuidof(IWebBrowser2), reinterpret_cast<void**>(&m_spWebBrowser2))) )
		return;
	m_spWebBrowser2->GoBack();


}
void CWtlHtmlView::GoForward() 
{
	if(m_spWebBrowser2==NULL && FAILED(QueryControl(__uuidof(IWebBrowser2), reinterpret_cast<void**>(&m_spWebBrowser2))) )
		return;
	m_spWebBrowser2->GoForward();

}
void CWtlHtmlView::Refresh()
{
	if (m_spWebBrowser2 != NULL) {
		m_spWebBrowser2->Refresh();
	}
}
IHTMLDocument2 *CWtlHtmlView::GetDocument()
{
	IHTMLDocument2 *document = NULL;
	
	if (m_spWebBrowser2 != NULL) {
	
		// get browser document's dispatch interface

		IDispatch *document_dispatch = NULL;

		HRESULT hr = m_spWebBrowser2->get_Document(&document_dispatch);

		if (SUCCEEDED(hr) && (document_dispatch != NULL)) {

			// get the actual document interface

			hr = document_dispatch->QueryInterface(IID_IHTMLDocument2,
			                                       (void **)&document);

			// release dispatch interface					
			document_dispatch->Release();	
		}	
	}	
	return document;
}
void CWtlHtmlView::Write(LPCTSTR string)
{
	if (m_spWebBrowser2 == NULL)
	GetBody();

	if (m_spWebBrowser2 != NULL) {

		// get document interface

		IHTMLDocument2 *document = GetDocument();
		
		if (document != NULL) {

			// construct text to be written to browser as SAFEARRAY

			SAFEARRAY *safe_array = SafeArrayCreateVector(VT_VARIANT,0,1);
			
			VARIANT	*variant;
			
			SafeArrayAccessData(safe_array,(LPVOID *)&variant);
			
			variant->vt      = VT_BSTR;
			variant->bstrVal = CString(string).AllocSysString();
			
			SafeArrayUnaccessData(safe_array);

			// write SAFEARRAY to browser document

			document->write(safe_array);
			
			document->Release();
			document = NULL;

		}

	}
}

void CWtlHtmlView::Clear()
{
	if (m_spWebBrowser2 != NULL) {

		// if document interface available, close/re-open document to clear display

		IHTMLDocument2	*document	= GetDocument();
		HRESULT			hr			= S_OK;
		
		if (document != NULL) {

			// close and re-open document to empty contents

			document->close();
			
			VARIANT		open_name;
			VARIANT		open_features;
			VARIANT		open_replace;
			IDispatch	*open_window	= NULL;

			::VariantInit(&open_name);

			open_name.vt      = VT_BSTR;
			open_name.bstrVal = ::SysAllocString(L"_self");

			::VariantInit(&open_features);
			::VariantInit(&open_replace);
			
			hr = document->open(::SysAllocString(L"text/html"),
			                    open_name,
			                    open_features,
			                    open_replace,
			                    &open_window);

			if (hr == S_OK) {
				Refresh();
			}

			if (open_window != NULL) {
				open_window->Release();
			}

		}

		// otherwise, navigate to about:blank and wait for document ready

		else {

			Navigate(_T("about:blank"));

			IHTMLDocument2 *document = NULL;
			HRESULT			hr       = S_OK;

			while ((document == NULL) && (hr == S_OK)) {

				Sleep(0);

				IDispatch *document_dispatch = NULL;

				hr = m_spWebBrowser2->get_Document(&document_dispatch);

				// if dispatch interface available, retrieve document interface
				
				if (SUCCEEDED(hr) && (document_dispatch != NULL)) {

					// retrieve document interface
					
					hr = document_dispatch->QueryInterface(IID_IHTMLDocument2,(void **)&document);

					document_dispatch->Release();

				}
				
			}
			
			if (document != NULL) {
				document->Release();	
			}

		}
		
	}
}

CComPtr<IHTMLDocument2> CWtlHtmlView::GetDocument(CComPtr<IWebBrowser2> pWebBrowser)
{
	//HRESULT_EXCEPTION hr;
	HRESULT hr;
	CComPtr<IDispatch> pDisp;
	CComPtr<IHTMLDocument2> pHTMLDocument;

	if(pWebBrowser==NULL)
		throw std::exception("CMainDlg::GetDocument() pWebBrowser NULL\n");

	hr=pWebBrowser->get_Document( &pDisp);

	hr=pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pHTMLDocument);
	if(pHTMLDocument==NULL )
		throw std::exception("CMainDlg::GetDocument() pHTMLDocument NULL\n");

	return pHTMLDocument;
}

CComPtr<IHTMLElement> CWtlHtmlView::GetElement(CComPtr<IWebBrowser2> pWebBrowser, const TCHAR *id)
{ 
	CComPtr<IHTMLElement> ret(NULL); 
	if (id==0) 
		return 0;

	CComPtr<IHTMLDocument2>  doc	= this->GetDocument(pWebBrowser);
	if (doc==0) 
		throw std::exception("CMainDlg::GetElement() pHTMLDocument NULL\n");

	CComPtr<IHTMLElementCollection> doc_all;
	HRESULT hr = doc->get_all(&doc_all);      // this is like doing document.all
	if (hr==S_OK)
	{ 
		VARIANT v0; VariantInit(&v0);

		CComPtr<IDispatch> disp;
		_variant_t vid((BSTR) _bstr_t(id));
		hr = doc_all->item(vid,v0,&disp);       // this is like doing document.all["messages"]
		if (hr==S_OK && disp!=0)
		{ 
			hr = disp->QueryInterface(IID_IHTMLElement,(void **)&ret); // it's the caller's responsibility to release ret
			if(FAILED(hr))
				throw HRESULT(hr);
		}
	}
	return ret;
}

HRESULT CWtlHtmlView::SetElementId( std::string id, std::string str)
{
	_bstr_t bstr(str.c_str());
	_bstr_t bid(id.c_str());

	try {
		CComPtr<IWebBrowser2> pWebBrowser = this->GetWebBrowser();
		if(pWebBrowser==NULL)
			return E_FAIL;
		IHTMLDocument2 *document=GetDocument(pWebBrowser);
		CComPtr<IHTMLElement> elem = GetElement(pWebBrowser, id.c_str());
		if(elem==NULL)
			return E_FAIL;
		return	elem->put_innerHTML(bstr);
	}
	catch(...)
	{
		return E_FAIL;
	}
	return S_OK;
}
LRESULT CWtlHtmlView::TurnOffContextMenu()
{
	HRESULT hr;
	CAxWindow wnd(m_hWnd);
	CComPtr<IUnknown> spUnk;					
	AtlAxGetHost(m_hWnd, &spUnk);				
	CComQIPtr<IAxWinAmbientDispatch> spWinAmb(spUnk);
	hr=spWinAmb->put_AllowContextMenu(VARIANT_FALSE);
	return SUCCEEDED(hr) ? 0 : -1;
}
#include "url.h"
void __stdcall CWtlHtmlView::OnBeforeNavigate2 (
	IDispatch* pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName,
	VARIANT* PostData, VARIANT* Headers, VARIANT_BOOL* Cancel )
{
	// Return TRUE to cancel
	//*Cancel = VARIANT_FALSE;
	int i=0;
	Url url ((LPCSTR) bstr_t(URL));
	
	if(url._host == "localhost") //  
	{
		for(int i=0; i< listeners.size(); i++)
			listeners[i](url._path.c_str(), url.queries );

	}
	*Cancel = VARIANT_TRUE;
}