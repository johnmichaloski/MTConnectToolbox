//
// RegieIni.h
//


#pragma once
#include <atlbase.h>
extern CComModule _Module;		
#include <atlcom.h>
#include "EventListSvr.tlh"
//#import "EventServer.dll"
//#import "EventSvr.dll"

class CMcEventCreationSink :	public CComObjectRoot,
								public IDispatchImpl< IMcEventCreationSink, &IID_IMcEventCreationSink, &LIBID_EVENTLISTSVRLib>  
{
	LONG								_hServerHandle	;
	CComPtr<IMcEventCreation>			_pEventCreation		;
	CComObject<CMcEventCreationSink>*	_pMcEventCreationSink	;
public:
	CMcEventCreationSink(void);
	~CMcEventCreationSink(void);

	BEGIN_COM_MAP (CMcEventCreationSink)
		COM_INTERFACE_ENTRY (IMcEventCreationSink)
	END_COM_MAP ()
	void Init();
// IMcEventCreationSink methods
public:
	STDMETHOD(Refresh)();
	STDMETHOD(Acknowledge)( long Handle, VARIANT EventIDs, VARIANT TimeStamps, VARIANT Cookies);
	STDMETHOD(ShutDownRequest)( long Time );
	HRESULT Register();
	HRESULT Unregister();
	bool bShutdown;
	bool IsConnected() { return _pEventCreation!=NULL; }
	bool CanSubscriptionSink();
};

typedef CComObject<CMcEventCreationSink> CComMcEventCreationSink;

