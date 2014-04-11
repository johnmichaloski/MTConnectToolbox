#include "stdafx.h"
#include <comdef.h>
#include "SiteImpl.h"

#pragma comment( lib, "urlmon.lib")



// Prepares the security manager object.
IUnknownPtr CreateSecureSite(ZONEMAP* pZoneMap, int nZoneMapCount, ACTIONMAP* pActionMap, int nActionMapCount, LPCWSTR secureBaseURL)
{
    if (secureBaseURL == NULL)
    {
        ATLTRACE("\nNo secureBaseURL, returning NULL for CreateSecureSite\r\n");
        return NULL;
    }

    // Create the Site object.
    CSiteImpl * pSite = new CSiteImpl(secureBaseURL);

    // Set the Zone and Action maps.
    pSite->SetZoneMap(pZoneMap, nZoneMapCount);
    pSite->SetActionMap(pActionMap, nActionMapCount);

    IUnknownPtr pSitePtr(pSite);
    return pSitePtr;
}

// Sets the safety options on the DOM object and 
// sets the Site object on the DOM object. 
HRESULT SetSecurityAndSite(IUnknownPtr pObj, IUnknownPtr pSite)
{
    HRESULT hr;

    // Set the safety options on the DOM object.
    IObjectSafetyPtr pObjectSafety(pObj);
    if (pObjectSafety != NULL)
    {
        DWORD dwSafetyOpt = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
        hr = pObjectSafety->SetInterfaceSafetyOptions(IID_IUnknown, dwSafetyOpt, dwSafetyOpt);
        if(FAILED(hr))
        {
            return hr;
        }
    }

    // Set the site object on the DOM object.
    if (pSite != NULL)
    {
        IObjectWithSitePtr pObjectWithSite(pObj);
        if (pObjectWithSite != NULL)
        {
            hr = pObjectWithSite->SetSite(pSite);
            if (FAILED(hr))
            {
                return hr;
            }
        }
    }
    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSiteImpl::CSiteImpl(LPCWSTR secureBaseURL)
{
    _ulRefs = 0;
    _secureBaseURL = secureBaseURL;

    _pZoneMap = NULL;
    _pActionMap = NULL;
    _nZoneMapCount = 0;
    _nActionMapCount = 0;
}

CSiteImpl::~CSiteImpl()
{
}

HRESULT CSiteImpl::QueryInterface(REFIID riid, void** ppv)
{
    HRESULT hr = S_OK;

    if (riid == IID_IUnknown || riid == IID_IServiceProvider)
    {
        *ppv = static_cast<IServiceProvider*>(this);
    }
    else if (riid == IID_IOleClientSite)
    {
        *ppv = static_cast<IOleClientSite*>(this);
    }
    else if (riid == IID_IInternetSecurityManager)
    {
        *ppv = static_cast<IInternetSecurityManager*>(this);
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

ULONG CSiteImpl::AddRef()
{
    return ++_ulRefs;
}

ULONG CSiteImpl::Release()
{
    ULONG ul = --_ulRefs;
    if (0 == ul)
        delete this;
    return ul;
}

HRESULT CSiteImpl::QueryService(REFGUID guidService, REFIID riid, void ** ppv)
{
    return  QueryInterface(riid, ppv);
}

// Either URLMON or MSXML can trigger this method. 
// The implementation should be caller agnostic. 
// This method is used to determine the zone of the URL. 
// Further, MSXML will use this method to determine the zone of a referenced document. 
// It can then determine whether the referring document 
// is attempting to load a document that is in a different zone.
HRESULT CSiteImpl::MapUrlToZone(LPCWSTR pwszUrl, DWORD *pdwZone, DWORD dwFlags)
{
    // Check if URL is in our map.
    for (int i=0; i<_nZoneMapCount; i++)
    {
        ZONEMAP * p = &_pZoneMap[i];
        wstring tempurl = pwszUrl;
        if ((p->_fullCompare && p->_url == pwszUrl) ||
             (!p->_fullCompare && tempurl.find(p->_url) == 0))
        {
            *pdwZone = p->_zoneIndex;
            return S_OK;
        }
    }
    return INET_E_DEFAULT_ACTION;
}

// Determines the policy for a specified security action. 
// URLMON triggers a call to this method whenever it encounters a security action 
// and needs to enlist a security manager to make a security-related decision 
// for that action. 
HRESULT CSiteImpl::ProcessUrlAction(
                        LPCWSTR pwszUrl,
                        DWORD dwAction,
                        BYTE *pPolicy,
                        DWORD cbPolicy,
                        BYTE *pContext,
                        DWORD cbContext,
                        DWORD dwFlags,
                        DWORD dwReserved)
{
    DWORD dwZone;
    HRESULT hr = MapUrlToZone(pwszUrl, &dwZone, 0);
    // The goal is that MapUrlToZone returns a Zone that 
    // either the custom security manager recognizes, or
    // the default Internet Security Manager recognizes. 
    // (One of which should always be the case.) 
    // Thus we ALWAYS enter the ACTIONMAP for the custom security manager 
    // whenever a ProcessUrlAction is called.
    if (hr == INET_E_DEFAULT_ACTION)
    {
        // The custom MapUrlToZone gets the zone
        // from the default security manager.
        IInternetSecurityManagerPtr pDefSecMgr = NULL;
        hr = ::CoCreateInstance(CLSID_InternetSecurityManager, NULL, CLSCTX_INPROC, IID_IInternetSecurityManager, (LPVOID*)&pDefSecMgr);
        if (hr == S_OK)
            hr = pDefSecMgr->MapUrlToZone(pwszUrl, &dwZone, 0);
    }
    if (hr == S_OK)
    {
        for (int i=0; i<_nActionMapCount; i++)
        {
            ACTIONMAP* p = &_pActionMap[i];
            if (p->_zoneIndex == dwZone && p->_action == dwAction)
            {
                *((DWORD*)pPolicy) = p->_policy;
                if (p->_policy == URLPOLICY_ALLOW)
                {
                    return S_OK;
                }
                else
                {
                    return S_FALSE;
                }
            }
        }
    }
    return INET_E_DEFAULT_ACTION;
}
// Returns the Secure Base URL to MSXML.
HRESULT CSiteImpl::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
{
    return CreateURLMoniker(NULL, _secureBaseURL.c_str(), ppmk);
}