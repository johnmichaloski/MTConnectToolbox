// CMTConnectTrayIcon.h : Declaration of the CCMTConnectTrayIcon

#pragma once
#include "resource.h"       // main symbols

#include "MtConnectDboardGui.h"
#include "_IDboardNotificationEvents_CP.h"

#include "MainFrm.h"
#include "WtlHtmlView.h"
#include "StdStringFcn.h"
#include <boost/timer.hpp>
#include "DataDictionary.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

extern CMainFrame wndMain;

// CCMTConnectTrayIcon

class ATL_NO_VTABLE CCMTConnectTrayIcon :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IConnectionPointContainerImpl<CCMTConnectTrayIcon>,
	public CComCoClass<CCMTConnectTrayIcon, &CLSID_CMTConnectTrayIcon>,
	public CProxy_IDboardNotificationEvents<CCMTConnectTrayIcon>,
	public IDispatchImpl<ICMTConnectTrayIcon, &IID_ICMTConnectTrayIcon, &LIBID_MtConnectDboardGuiLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CCMTConnectTrayIcon()
	{
		wpx = 300.0;
		dTraceWidth=600.0;
		nSampleTimeSecs = 30;
	}
	std::vector<int>					_historian;
	std::vector<std::string>			_historiantimestamp;
	std::vector<std::string>			_fields;
	//std::vector<std::string>			_values;
	std::map<std::string,std::string>	_datamap;
	static std::vector<DataDictionary>	_faultdatum;
	static std::vector<std::string>		_faultfields;
	boost::timer						statetimer;
	static std::string					_faultitems; ;
	int  _laststate;
	int  _state;
	int GetState();
	double wpx;
	double dTraceWidth;
	int nSampleTimeSecs;
	std::string _ipaddr;

DECLARE_REGISTRY_RESOURCEID(IDR_CMTCONNECTTRAYICON)
//DECLARE_CLASSFACTORY_SINGLETON(CCMTConnectTrayIcon)

BEGIN_COM_MAP(CCMTConnectTrayIcon)
	COM_INTERFACE_ENTRY(ICMTConnectTrayIcon)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CCMTConnectTrayIcon)
	CONNECTION_POINT_ENTRY(__uuidof(_IDboardNotificationEvents))
END_CONNECTION_POINT_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();

	void FinalRelease()
	{
	}
 
	std::string GetHtmlStatus();
	static std::string CreateHtmlDocument();
	static std::string HtmlStyle();
	static std::string GetFaultSummaryHtml() ;
	std::string GetHeaderRow();
	std::string CreateHistory() ;
	DataDictionary						_typedatum;
	DataDictionary						_formats;
public:
	CWtlHtmlView * _htmlpage;

	// GLobal information
	STDMETHOD(Formats)(BSTR csvFields, BSTR csvFormats);
	STDMETHOD(Types)(BSTR csvFields, BSTR csvTypes);
	STDMETHOD(GetPointerBrowser)(IUnknown** pWebBrowser);


	// Machine summary tab
	STDMETHOD(CreateTabWindow)(BSTR name);
	STDMETHOD(SetHeaderColumns)(BSTR csv); // adds history to end of tab
	STDMETHOD(AddRow)(BSTR csvFields, BSTR values);
	STDMETHOD(SetIpAddr)(BSTR ipaddr);

	STDMETHOD(SetTrayIconColor)(BSTR traycolor,VARIANT_BOOL flashing);
	STDMETHOD(SetTooltip)(BSTR bstr);

	// Alarm tab
	STDMETHOD(CreateAlarmTab)(BSTR name);
	STDMETHOD(Clear)(void);
	STDMETHOD(AddFaultRow)(BSTR csvFields, BSTR csvValues);
	STDMETHOD(SetFaultColumns)(BSTR csv); // csv representation of fault column fields
};

OBJECT_ENTRY_AUTO(__uuidof(CMTConnectTrayIcon), CCMTConnectTrayIcon)
