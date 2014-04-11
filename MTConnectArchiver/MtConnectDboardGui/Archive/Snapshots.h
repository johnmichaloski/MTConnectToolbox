//
//
//
#pragma once
#pragma comment( lib, "msxml2.lib" )
#import <msxml6.dll>
#include <string>
#include <vector>

class CHtmlTableSnapshots
{
public:
	CHtmlTableSnapshots() 
	{
		_nLive=1;
		_nSnapshot=-1;
		_sShift="<BR><H3>Shift 1</H3>";
	}
	void							SaveSnapshot(std::string snapshot) ;
	CComPtr<IHTMLDocument2>			GetDocument(CComPtr<IWebBrowser2>);
	CComPtr<IHTMLElement>			GetElement(CComPtr<IWebBrowser2>, const TCHAR *id);
	HRESULT							SetDocumentText(CComPtr<IWebBrowser2>, std::string cstr);
	HRESULT							SetElementId(CComPtr<IWebBrowser2>, std::string id, std::string str);
	void							StoreShift(CComPtr<IWebBrowser2> pWebBrowser, int shift);
	int								SetSnapshot(CComPtr<IWebBrowser2> pWebBrowser, int i=-1);
	void							StoreSnapshot(CComPtr<IWebBrowser2>, std::string snapshot);
	std::string						_lastsnapshot;
	std::vector<std::string>		_snapshots ;
	unsigned int					_nSnapshot;
	int								_nLive;
	std::string						_sShift;
	std::string						_sHeader;
	std::string						_fields;
	static std::string CHtmlTableSnapshots::CreateHtmlDocument();
	static std::string CHtmlTableSnapshots::HtmlStyle();
};