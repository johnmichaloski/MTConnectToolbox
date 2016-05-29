#pragma once
#include <netfw.h>

/**
 HRESULT hr;
    BOOL bEnabled = FALSE;
    INetFwProfile* fwProfile = NULL;

    CFirewallUtil pUtil;
    hr = pUtil.Initialize(&fwProfile);
    if(SUCCEEDED(hr))
    {
        hr = pUtil.IsFirewallEnabled(fwProfile, &bEnabled);
    }

    pUtil.Uninitialize(fwProfile);
*/
class CFirewallUtil
{
public:
    CFirewallUtil(void);
    ~CFirewallUtil(void);

    HRESULT IsFirewallEnabled(INetFwProfile* fwProfile, BOOL * pEnabled);
    HRESULT IsPortEnabled(INetFwProfile* fwProfile, BOOL *pEnabled, int iPort, bool bTCP);
    HRESULT SetPortEnabled(INetFwProfile* fwProfile, int iPort, bool bTCP, bool bEnable);

    HRESULT Initialize(INetFwProfile** fwProfile);
    void Uninitialize(INetFwProfile* fwProfile);

private:
    INetFwMgr* m_pFwMgr;
    INetFwPolicy* m_pFwPolicy;
};
