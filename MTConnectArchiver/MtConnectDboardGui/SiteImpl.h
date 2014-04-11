//
//
//

#include <string>
using namespace std;

/**
//http://msdn.microsoft.com/en-us/library/windows/desktop/cc507439(v=vs.85).aspx 
static void SetTrusted(LPCSTR sites)
{
	CComPtr<IInternetSecurityManager> pSecurityMgr;

	ACTIONMAP actionMap[] =
    {
        ACTIONMAP(URLZONE_INTERNET, URLACTION_CROSS_DOMAIN_DATA, URLPOLICY_ALLOW),
    };
    int nActionMapCount = sizeof(actionMap) / sizeof(ACTIONMAP);
    // Create a site object.
    IUnknownPtr pSite = CreateSecureSite(NULL, 0, actionMap, nActionMapCount, bstr_t(sites)); // L"www.microsoft.com");

	//HRESULT hr = CoCreateInstance(CLSID_InternetSecurityManager, NULL, CLSCTX_INPROC_SERVER, IID_IInternetSecurityManager, (void**)&pSecurityMgr);

   // hr=pSecurityMgr->SetZoneMapping(URLZONE_TRUSTED, bstr_t(sites), SZM_CREATE);
}
*/

typedef struct _ZONEMAP
{
    wstring _url;
    DWORD _zoneIndex;
    BOOL _fullCompare;

    _ZONEMAP(wstring url, DWORD zoneIndex, BOOL fullCompare)
        : _url(url), _zoneIndex(zoneIndex), _fullCompare(fullCompare)
    {
    }
} ZONEMAP;

typedef struct _ACTIONMAP
{
    DWORD _zoneIndex;
    DWORD _action;
    BYTE _policy;

    _ACTIONMAP(DWORD zoneIndex, DWORD action, BYTE policy)
        : _zoneIndex(zoneIndex), _action(action), _policy(policy)
    {
    }

} ACTIONMAP;

class CSiteImpl : public IServiceProvider, public IInternetSecurityManager, IOleClientSite
{

private:
    ULONG _ulRefs;
    wstring _secureBaseURL;

    // IInternetSecurityManager
    ZONEMAP* _pZoneMap;
    int _nZoneMapCount;
    ACTIONMAP* _pActionMap;
    int _nActionMapCount;


public:
    CSiteImpl(LPCWSTR secureBaseURL);
    virtual ~CSiteImpl();

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IServiceProvider
    HRESULT STDMETHODCALLTYPE QueryService(REFGUID guidService, REFIID riid, void** ppv);

    // IDispatch
    HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT *pctinfo) { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo,LCID lcid, ITypeInfo** ppTInfo) { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid,DISPID *rgDispId) { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, 
    REFIID riid, LCID lcid, WORD wFlags, 
    DISPPARAMS *pDispParams, VARIANT *pVarResult, 
    EXCEPINFO *pExcepInfo, UINT *puArgErr) { return E_NOTIMPL; }

    // IOleClientSite
    virtual HRESULT STDMETHODCALLTYPE SaveObject() { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign,DWORD dwWhichMoniker, IMoniker** ppmk);
    virtual HRESULT STDMETHODCALLTYPE GetContainer(IOleContainer**) { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE ShowObject() { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL) { return E_NOTIMPL; }
    virtual HRESULT STDMETHODCALLTYPE RequestNewObjectLayout(){ return E_NOTIMPL; }

    // IInternetSecurityManager
public:
    void SetZoneMap(ZONEMAP* pZoneMap, int count) { _pZoneMap = pZoneMap; _nZoneMapCount = count; }
    void SetActionMap(ACTIONMAP* pActionMap, int count) { _pActionMap = pActionMap; _nActionMapCount = count; }

    HRESULT STDMETHODCALLTYPE SetSecuritySite(
        IInternetSecurityMgrSite* pSite)
    {
        return INET_E_DEFAULT_ACTION;
    }

    HRESULT STDMETHODCALLTYPE GetSecuritySite(
        IInternetSecurityMgrSite** ppSite)
    {
        return INET_E_DEFAULT_ACTION;
    }

    virtual HRESULT STDMETHODCALLTYPE MapUrlToZone(
        LPCWSTR pwszUrl,
        DWORD *pdwZone,
        DWORD dwFlags);

    virtual HRESULT STDMETHODCALLTYPE GetSecurityId(
        LPCWSTR pwszUrl,
        BYTE *pbSecurityId,
        DWORD *pcbSecurityId,
        DWORD_PTR dwReserved)
    {
        return INET_E_DEFAULT_ACTION;
    }

    virtual HRESULT STDMETHODCALLTYPE ProcessUrlAction(
        LPCWSTR pwszUrl,
        DWORD dwAction,
        BYTE* pPolicy,
        DWORD cbPolicy,
        BYTE* pContext,
        DWORD cbContext,
        DWORD dwFlags,
        DWORD dwReserved);

    virtual HRESULT STDMETHODCALLTYPE QueryCustomPolicy(
        LPCWSTR pwszUrl,
        REFGUID guidKey,
        BYTE** ppPolicy,
        DWORD* pcbPolicy,
        BYTE* pContext,
        DWORD cbContext,
        DWORD dwReserved)
    {
        return INET_E_DEFAULT_ACTION;
    }

    virtual HRESULT STDMETHODCALLTYPE SetZoneMapping(
        DWORD dwZone,
        LPCWSTR lpszPattern,
        DWORD dwFlags)
    {
        return INET_E_DEFAULT_ACTION;
    }

    virtual HRESULT STDMETHODCALLTYPE GetZoneMappings(
        DWORD dwZone,
        IEnumString** ppenumString,
        DWORD dwFlags)
    {
        return INET_E_DEFAULT_ACTION;
    }
};

extern HRESULT SetSecurityAndSite(IUnknownPtr pObj, IUnknownPtr pSite);
extern IUnknownPtr CreateSecureSite(ZONEMAP* pZoneMap, int nZoneMapCount, ACTIONMAP* pActionMap, int nActionMapCount, LPCWSTR secureBaseURL);