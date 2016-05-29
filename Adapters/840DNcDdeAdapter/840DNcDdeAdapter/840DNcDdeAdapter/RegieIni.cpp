#include "StdAfx.h"
#include "RegieIni.h"


HRESULT CreateSubscriptions();
HRESULT GetObject(LPCTSTR szMoniker, REFIID iid, void **ppvObject);
HRESULT SupportsErrorInfo( LPUNKNOWN pUnk, REFIID riid, LPTSTR szErrorDesc );
void DisplayLastError( LPCTSTR szFunc, HRESULT hrError, BOOL bErrorInfo, LPUNKNOWN pUnk, REFIID riid );


CComPtr<IMcEventList>					g_pEventList				= NULL;
CComPtr<IMcEventSubscription>			g_pEventSubscription		= NULL;
CComPtr<IMcEventSubscription>			g_pAlarmSubscription		= NULL;

CComObject<CMcEventCreationSink>*	g_pMcEventSubscriptionSink	= NULL;
CComObject<CMcEventCreationSink>*	g_pMcAlarmSubscriptionSink	= NULL;


CMcEventCreationSink::CMcEventCreationSink(void)
{
	bShutdown=false;
	_hServerHandle=NULL;
	_pEventCreation	=NULL	;
	_pMcEventCreationSink=NULL	;
}


CMcEventCreationSink::~CMcEventCreationSink(void)
{
}


void CMcEventCreationSink::Init()
{
	// get the IMcEventList object via moniker string
	HRESULT hr = GetObject("@SinHMIEventSvr.EventListSvrObj", IID_IMcEventList, (void**)&g_pEventList);
	CreateSubscriptions();
}

STDMETHODIMP CMcEventCreationSink::Refresh()
{
	return S_OK;
}


STDMETHODIMP CMcEventCreationSink::Acknowledge( long Handle, VARIANT EventIDs, VARIANT TimeStamps, VARIANT Cookies)
{
	return S_OK;
}


STDMETHODIMP CMcEventCreationSink::ShutDownRequest( long Time )
{
	bShutdown=true;
	return S_OK;
}


HRESULT CreateSubscriptions()
{
	HRESULT	hr			= S_OK;

	if (g_pEventList!=NULL )
	{
		
		CComObject<CMcEventCreationSink>::CreateInstance(&g_pMcEventSubscriptionSink);

		hr = g_pEventList->CreateEventSubscription( (LONG)g_pMcEventSubscriptionSink, (IMcEventSubscriptionSink *) g_pMcEventSubscriptionSink, &g_pEventSubscription );
		if (SUCCEEDED(hr))
		{
			hr = g_pEventSubscription->put_Active( VARIANT_TRUE );
		}
		else
		{
			DisplayLastError( _T("IMcEventList::CreateEventSubscription()"), hr, TRUE, g_pEventList, IID_IMcEventList );
		}
		

	/*	CComObject<CRegieIni>::CreateInstance(&g_pMcAlarmSubscriptionSink);
		g_pMcAlarmSubscriptionSink->m_bAlarmList = TRUE;

		hr = g_pEventList->CreateAlarmSubscription( (LONG)g_pMcAlarmSubscriptionSink, (IMcEventSubscriptionSink *) g_pMcAlarmSubscriptionSink, &g_pAlarmSubscription );
		if (SUCCEEDED(hr))
		{
			hr = g_pAlarmSubscription->put_Active( VARIANT_TRUE );
		}
		else
		{
			DisplayLastError( _T("IMcEventList::CreateAlarmSubscription()"), hr, TRUE, g_pEventList, IID_IMcEventList );
		}*/
	}
	return S_OK;
}



// Gets an COM object based on a moniker string
HRESULT GetObject(LPCTSTR szMoniker, REFIID iid, void **ppvObject)
{
	IBindCtx*			pbc		= NULL;
	IMoniker*			pmk		= NULL;
	ULONG				chEaten	= 0;	
	HRESULT				hr = S_OK;
	
	USES_CONVERSION;

	// Create Bind Context
	hr = CreateBindCtx ( 0, &pbc ); 
	if ( hr!= S_OK )
	{
		//DisplayLastError( _T("CreateBindCtx()"), hr );
		return hr;
	}
	
	// Get Moniker
	hr = MkParseDisplayName( pbc, T2OLE(szMoniker), &chEaten, &pmk );
	if ( hr!= S_OK )
	{
		//DisplayLastError( _T("MkParseDisplayName()"), hr );
		pbc->Release ( );
		return hr;
	}
	// Bind Moniker to Server Object and get the desired Interface
	hr = pmk->BindToObject ( pbc, NULL, iid, ppvObject );
	if (hr!=S_OK)
	{
		//DisplayLastError( _T("IMoniker::BindToObject()"), hr );
	}
	pmk->Release ( );
	pbc->Release ( );

	return hr;
}

// asks a com interface for supporting error info
// if there is an error object then it retrieves the error description from it
HRESULT SupportsErrorInfo( LPUNKNOWN pUnk, REFIID riid, LPTSTR szErrorDesc )
{
	HRESULT						hr					= S_OK;
	CComPtr<ISupportErrorInfo>	pISupportErrorInfo	= NULL;
	CComPtr<IErrorInfo>			pErrorInfo			= NULL;
	BSTR						bstrDesc			= NULL;

	USES_CONVERSION;

	if (pUnk!=NULL)
	{
		hr = pUnk->QueryInterface(IID_ISupportErrorInfo, (void**)&pISupportErrorInfo);
		if (SUCCEEDED(hr))
		{
			hr = pISupportErrorInfo->InterfaceSupportsErrorInfo(riid);
			if (hr==S_OK)
			{
				hr = GetErrorInfo( 0, &pErrorInfo );
				if (hr==S_OK)
				{
					hr = pErrorInfo->GetDescription( &bstrDesc );
					_tcscpy( szErrorDesc, OLE2T(bstrDesc) );
					SysFreeString( bstrDesc );
				}
			}
		}
	}
	else
	{
		return E_FAIL;
	}

	return hr;
}


// displays the function, the error and a describing text in the last error line		
void DisplayLastError( LPCTSTR szFunc, HRESULT hrError, BOOL bErrorInfo, LPUNKNOWN pUnk, REFIID riid )
{
	HRESULT	hr		= S_OK;
	TCHAR	szErrorDesc[1000];
	TCHAR	szErrorDisp[1500];
	LPVOID lpMsgBuf	= NULL;

	if (bErrorInfo)
	{
		hr = SupportsErrorInfo( pUnk, riid, szErrorDesc );
	}

	if ((!bErrorInfo) || (hr!=S_OK))
	{
	
		if ( FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER | 
							FORMAT_MESSAGE_FROM_SYSTEM |     
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL,
							hrError,
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
							(LPTSTR) &lpMsgBuf,
							0,
							NULL ) )
		{
			_tcscpy( szErrorDesc, (LPTSTR)lpMsgBuf );
			LocalFree(lpMsgBuf);
		}
	}

	_stprintf( szErrorDisp, "%s: 0x%X - %s", szFunc, hrError, szErrorDesc );
}

bool CMcEventCreationSink::CanSubscriptionSink()
{
	if (_pEventCreation!=NULL)
		return true;

	// get the IMcEventList object via moniker string
	if(FAILED( GetObject("@SinHMIEventSvr.EventListSvrObj", IID_IMcEventList, (void**)&_pEventCreation)))
		return false;

	return true;
}
// register our client to the IMcEventCreation server
HRESULT CMcEventCreationSink::Register()
{
	HRESULT	hr			= E_FAIL;

	if (_pEventCreation==NULL)
	{
			// get the IMcEventList object via moniker string
		if(FAILED( GetObject("@SinHMIEventSvr.EventListSvrObj", IID_IMcEventList, (void**)&_pEventCreation)))
			return E_FAIL;

	}
	if (_pEventCreation!=NULL )
	{
		CComObject<CMcEventCreationSink>::CreateInstance(&_pMcEventCreationSink);

		if(FAILED(hr = _pEventCreation->Register( _pMcEventCreationSink, &_hServerHandle )))
		{
			bShutdown=true;
			return E_FAIL;
		}

		if(FAILED( g_pEventSubscription->put_Active( VARIANT_TRUE )))
		{
			bShutdown=true;
			return E_FAIL;
		}
		
		bShutdown=false;

	}
	return hr;

}


// unregister our client from the IMcEventCreation server
HRESULT CMcEventCreationSink::Unregister()
{
	HRESULT	hr			= E_FAIL;
	
	if (_pEventCreation!=NULL )
	{
		hr = _pEventCreation->Unregister( _hServerHandle );
	}
	_hServerHandle=NULL;
	_pEventCreation=NULL;
	bShutdown=true;
	return hr;
}
