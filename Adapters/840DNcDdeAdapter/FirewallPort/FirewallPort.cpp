// FirewallPort.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "firewall.h"


int _tmain(int argc, _TCHAR* argv[])
{
	CComPtr<INetFwProfile> fwProfile ;
	BOOL fwOn;

    // Initialize COM.
    HRESULT hr = CoInitializeEx(
                0,
                COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
                );


	WindowsFirewall firewall;
	hr=firewall.WindowsFirewallInitialize(&fwProfile);
	hr=firewall.WindowsFirewallIsOn(fwProfile,&fwOn);
	hr=firewall.WindowsFirewallPortIsEnabled(fwProfile,7879,NET_FW_IP_PROTOCOL_TCP , &fwOn);


	CoUninitialize();
	return 0;
}

