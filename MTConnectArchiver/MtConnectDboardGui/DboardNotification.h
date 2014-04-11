// DboardNotification.h : Declaration of the CDboardNotification

#pragma once
#include "resource.h"       // main symbols

#include "MtConnectDboardGui.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CDboardNotification

class ATL_NO_VTABLE CDboardNotification :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CDboardNotification, &CLSID_DboardNotification>,
	public IConnectionPointContainerImpl<CDboardNotification>,
	public CProxy_IDboardNotificationEvents<CDboardNotification>,
	public IDispatchImpl<IDboardNotification, &IID_IDboardNotification, &LIBID_MtConnectDboardGuiLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CDboardNotification()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_DBOARDNOTIFICATION)

DECLARE_NOT_AGGREGATABLE(CDboardNotification)

BEGIN_COM_MAP(CDboardNotification)
	COM_INTERFACE_ENTRY(IDboardNotification)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

};

OBJECT_ENTRY_AUTO(__uuidof(DboardNotification), CDboardNotification)
