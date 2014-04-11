#include "StdAfx.h"
#include "Snapshots.h"
#include "StdStringFcn.h"

///////////////////////////////////////////////////////////////////////////////
int CHtmlTableSnapshots::SetSnapshot(CComPtr<IWebBrowser2> pWebBrowser, int i)
{
	if(i == -1)
	{
		if(_nLive==1 && pWebBrowser!=NULL)
			SetElementId(pWebBrowser, "Device", _snapshots[_snapshots.size()-1]);
		return _snapshots.size()-1;
	}
	if(i <0)
		i=0;
	if(i >=_snapshots.size())
		i=_snapshots.size()-1;
	if(i <0)
		return 0; // no data

	if(pWebBrowser!=NULL)
		SetElementId(pWebBrowser, "Device", _snapshots[i]);
	return i;
}
void CHtmlTableSnapshots::StoreSnapshot(CComPtr<IWebBrowser2> pWebBrowser, std::string snapshot)
{
	// Don't save same snapshot
	if(_lastsnapshot==snapshot)
		return;
	_lastsnapshot=snapshot;
	_snapshots.push_back(snapshot); 
	if(_nLive==1 && pWebBrowser!=NULL)
		SetElementId(pWebBrowser, "Device", snapshot);
}

void CHtmlTableSnapshots::SaveSnapshot(std::string snapshot) 
{ 
	if(_lastsnapshot==snapshot)
		return;
	_lastsnapshot=snapshot;
	_snapshots.push_back(snapshot); 
}

void CHtmlTableSnapshots::StoreShift(CComPtr<IWebBrowser2> pWebBrowser, int shift)
{
	std::string sShift="<H1>" + _sHeader + StdStringFormat(" - Shift %d", shift) + "</H1>";
	if(_sShift!=sShift && pWebBrowser!=NULL)
		SetElementId(pWebBrowser, "Header", sShift);
	_sShift=sShift;
}


CComPtr<IHTMLDocument2> CHtmlTableSnapshots::GetDocument(CComPtr<IWebBrowser2> pWebBrowser)
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

CComPtr<IHTMLElement> CHtmlTableSnapshots::GetElement(CComPtr<IWebBrowser2> pWebBrowser, const TCHAR *id)
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

HRESULT CHtmlTableSnapshots::SetElementId(CComPtr<IWebBrowser2> pWebBrowser, std::string id, std::string str)
{
	_bstr_t bstr(str.c_str());
	_bstr_t bid(id.c_str());

	IHTMLDocument2 *document=GetDocument(pWebBrowser);
	CComPtr<IHTMLElement> elem = GetElement(pWebBrowser, id.c_str());
	if(elem==NULL)
		return E_FAIL;
	return	elem->put_innerHTML(bstr);
}
HRESULT CHtmlTableSnapshots::SetDocumentText(CComPtr<IWebBrowser2> pWebBrowser, std::string cstr)
{
	_bstr_t tbstr(cstr.c_str());
	//HRESULT_EXCEPTION hr = S_OK;
	HRESULT hr = S_OK;
	VARIANT *param;
	SAFEARRAY *sfArray;
	CComPtr<IHTMLDocument2> document;
	BSTR  bstr= tbstr.Detach();

	try
	{
		// Creates a new one-dimensional array
		sfArray = SafeArrayCreateVector(VT_VARIANT, 0, 1);

		document=GetDocument(pWebBrowser);
		if (sfArray == NULL || document == NULL) 
			throw std::exception("CMainDlg::SetDocumentText) Failed IHTMLDocument2\n");

		hr = SafeArrayAccessData(sfArray,(LPVOID*) & param);
		param->vt = VT_BSTR;
		param->bstrVal = bstr;
		hr = SafeArrayUnaccessData(sfArray);
		hr = document->write(sfArray);
	}
	catch(std::exception e)
	{
		AtlTrace(e.what());
		throw e;
	}
	catch(HRESULT hResult)
	{
		std::string errmsg =(LPCSTR)( "CMainDlg::SetDocumentText Failed: %s\n" +   ErrorFormatMessage(hResult));
		AtlTrace("CMainDlg::SetDocumentText Failed: %s\n",  (LPCSTR) ErrorFormatMessage(hResult));
		throw std::exception(errmsg.c_str());
	}
	catch(...)
	{
		AtlTrace("CMainDlg::OpenIE() Failed\n");
		throw std::exception("CMainDlg::OpenIE() Failed\n");
	}

	if (sfArray != NULL) 
		SafeArrayDestroy(sfArray);
	return S_OK;
}


std::string CHtmlTableSnapshots::CreateHtmlDocument()
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

std::string CHtmlTableSnapshots::HtmlStyle()
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