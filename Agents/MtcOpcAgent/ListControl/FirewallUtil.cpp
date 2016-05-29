#include "StdAfx.h"
#include "FirewallUtil.h"

CFirewallUtil::CFirewallUtil(void)
{
    m_pFwMgr = NULL;
    m_pFwPolicy = NULL;
}

CFirewallUtil::~CFirewallUtil(void)
{
}

HRESULT CFirewallUtil::IsPortEnabled(INetFwProfile* fwProfile, BOOL *pEnabled, int iPort, bool bTCP)
{
    HRESULT hr = S_OK;
    *pEnabled = FALSE;

    INetFwOpenPort* fwOpenPort = NULL;
    INetFwOpenPorts* fwOpenPorts = NULL;

    // Retrieve the globally open ports collection.
    hr = fwProfile->get_GloballyOpenPorts(&fwOpenPorts);

    if(SUCCEEDED(hr))
    {
        NET_FW_IP_PROTOCOL ipProtocol = NET_FW_IP_PROTOCOL_TCP;
        if(!bTCP)
            ipProtocol = NET_FW_IP_PROTOCOL_UDP;
        // Attempt to retrieve the globally open port.
        hr = fwOpenPorts->Item(iPort, ipProtocol, &fwOpenPort);
        if (SUCCEEDED(hr))
        {
            VARIANT_BOOL fwEnabled;
            // Find out if the globally open port is enabled.
            hr = fwOpenPort->get_Enabled(&fwEnabled);
            if(SUCCEEDED(hr))
            {
                *pEnabled = fwEnabled != VARIANT_FALSE;
            }
        }
        else
        {
            // The globally open port was not in the collection.
            hr = S_OK;
        }
    }

    // Release the globally open port.
    if (fwOpenPort != NULL)
    {
        fwOpenPort->Release();
    }

    // Release the globally open ports collection.
    if (fwOpenPorts != NULL)
    {
        fwOpenPorts->Release();
    }

    return hr;
}

HRESULT CFirewallUtil::SetPortEnabled(INetFwProfile* fwProfile, int iPort, bool bTCP, bool bEnable)
{
    HRESULT hr = S_OK;

    INetFwOpenPort* fwOpenPort = NULL;
    INetFwOpenPorts* fwOpenPorts = NULL;

    // Retrieve the globally open ports collection.
    hr = fwProfile->get_GloballyOpenPorts(&fwOpenPorts);

    if(SUCCEEDED(hr))
    {
        NET_FW_IP_PROTOCOL ipProtocol = NET_FW_IP_PROTOCOL_TCP;
        if(!bTCP)
            ipProtocol = NET_FW_IP_PROTOCOL_UDP;
        // Attempt to retrieve the globally open port.
        hr = fwOpenPorts->Item(iPort, ipProtocol, &fwOpenPort);
        if (SUCCEEDED(hr))
        {
            VARIANT_BOOL fwEnabled = bEnable;
            // Find out if the globally open port is enabled.
            hr = fwOpenPort->put_Enabled(fwEnabled);
        }
    }

    // Release the globally open port.
    if (fwOpenPort != NULL)
    {
        fwOpenPort->Release();
    }

    // Release the globally open ports collection.
    if (fwOpenPorts != NULL)
    {
        fwOpenPorts->Release();
    }

    return hr;
}

HRESULT CFirewallUtil::IsFirewallEnabled(INetFwProfile* fwProfile, BOOL * pEnabled)
{
    HRESULT hr = S_OK;
    *pEnabled = FALSE;

    VARIANT_BOOL fwEnabled;

    // Get the current state of the firewall.
    hr = fwProfile->get_FirewallEnabled(&fwEnabled);
    if(SUCCEEDED(hr))
    {
        *pEnabled = fwEnabled != VARIANT_FALSE;
    }

    return hr;
}

HRESULT CFirewallUtil::Initialize(INetFwProfile** fwProfile)
{
    HRESULT hr = S_OK;

    // Initialize COM.
    hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hr == RPC_E_CHANGED_MODE)
    {
        hr = S_OK;
    }

    INetFwMgr* fwMgr = NULL;
    if(SUCCEEDED(hr))
    {

        // Create an instance of the firewall settings manager.
        hr = CoCreateInstance(
                __uuidof(NetFwMgr),
                NULL,
                CLSCTX_INPROC_SERVER,
                __uuidof(INetFwMgr),
                (void**)&fwMgr
                );
    }

    INetFwPolicy* fwPolicy = NULL;
    if(SUCCEEDED(hr))
    {
        // Retrieve the local firewall policy.
        hr = fwMgr->get_LocalPolicy(&fwPolicy);
    }

    // Retrieve the firewall profile currently in effect.
    *fwProfile = NULL;
    if(SUCCEEDED(hr))
    {
        hr = fwPolicy->get_CurrentProfile(fwProfile);
    }

    if(FAILED(hr))
    {
       if(fwPolicy != NULL)
            fwPolicy->Release();

        if(fwMgr != NULL)
            fwMgr->Release();
    }
    else
    {
        m_pFwPolicy = fwPolicy;
        m_pFwMgr = fwMgr;
    }

    return hr;
}

void CFirewallUtil::Uninitialize(INetFwProfile* fwProfile)
{
    // Release the firewall profile.
    if (fwProfile != NULL)
    {
        fwProfile->Release();
    }

    if(m_pFwPolicy != NULL)
    {
        m_pFwPolicy->Release();
        m_pFwPolicy = NULL;
    }

    if(m_pFwMgr != NULL)
    {
        m_pFwMgr->Release();
        m_pFwMgr = NULL;
    }

    CoUninitialize();
}