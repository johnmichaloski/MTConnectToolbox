//

// Firewall.h
//

#pragma once
#include <crtdbg.h>
#include <netfw.h>
#include <objbase.h>
#include <oleauto.h>
#include <stdio.h>
#include "Logging.h"

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

class WindowsFirewall
{
public:
    HRESULT WindowsFirewallInitialize (OUT INetFwProfile **fwProfile)
    {
        HRESULT hr = S_OK;

        CComPtr<INetFwMgr>    fwMgr;
        CComPtr<INetFwPolicy> fwPolicy;

        _ASSERT(fwProfile != NULL);

        *fwProfile = NULL;

        // Create an instance of the firewall settings manager.
        hr = CoCreateInstance(__uuidof(NetFwMgr), NULL, CLSCTX_INPROC_SERVER,
                              __uuidof(INetFwMgr), (void **) &fwMgr);

        if ( FAILED(hr) )
        {
            logFatal("WindowsFirewall CoCreateInstance failed: 0x%08lx\n", hr);
            goto error;
        }

        // Retrieve the local firewall policy.
        hr = fwMgr->get_LocalPolicy(&fwPolicy);

        if ( FAILED(hr) )
        {
            logFatal("WindowsFirewall get_LocalPolicy failed: 0x%08lx\n", hr);
            goto error;
        }

        // Retrieve the firewall profile currently in effect.
        hr = fwPolicy->get_CurrentProfile(fwProfile);

        if ( FAILED(hr) )
        {
            logFatal("WindowsFirewall get_CurrentProfile failed: 0x%08lx\n", hr);
            goto error;
        }

error:

        return hr;
    }

    void WindowsFirewallCleanup (IN INetFwProfile *fwProfile)
    {
        // Release the firewall profile.
        if ( fwProfile != NULL )
        {
            fwProfile->Release( );
        }
    }

    HRESULT WindowsFirewallIsOn (IN INetFwProfile *fwProfile, OUT BOOL *fwOn)
    {
        HRESULT      hr = S_OK;
        VARIANT_BOOL fwEnabled;

        _ASSERT(fwProfile != NULL);
        _ASSERT(fwOn != NULL);

        *fwOn = FALSE;

        // Get the current state of the firewall.
        hr = fwProfile->get_FirewallEnabled(&fwEnabled);

        if ( FAILED(hr) )
        {
            logFatal("WindowsFirewall get_FirewallEnabled failed: 0x%08lx\n", hr);
            goto error;
        }

        // Check to see if the firewall is on.
        if ( fwEnabled != VARIANT_FALSE )
        {
            *fwOn = TRUE;
            logFatal("WindowsFirewall The firewall is on.\n");
        }
        else
        {
            logFatal("WindowsFirewall The firewall is off.\n");
        }

error:

        return hr;
    }

    HRESULT WindowsFirewallPortIsEnabled (IN INetFwProfile *    fwProfile,
                                          IN LONG               portNumber,
                                          IN NET_FW_IP_PROTOCOL ipProtocol,
                                          OUT BOOL *            fwPortEnabled)
    {
        HRESULT          hr = S_OK;
        VARIANT_BOOL     fwEnabled;
        INetFwOpenPort * fwOpenPort  = NULL;
        INetFwOpenPorts *fwOpenPorts = NULL;

        _ASSERT(fwProfile != NULL);
        _ASSERT(fwPortEnabled != NULL);

        *fwPortEnabled = FALSE;

        // Retrieve the globally open ports collection.
        hr = fwProfile->get_GloballyOpenPorts(&fwOpenPorts);

        if ( FAILED(hr) )
        {
            logFatal("WindowsFirewall get_GloballyOpenPorts failed: 0x%08lx\n", hr);
            goto error;
        }

        // Attempt to retrieve the globally open port.
        hr = fwOpenPorts->Item(portNumber, ipProtocol, &fwOpenPort);

        if ( SUCCEEDED(hr) )
        {
            // Find out if the globally open port is enabled.
            hr = fwOpenPort->get_Enabled(&fwEnabled);

            if ( FAILED(hr) )
            {
                logFatal("WindowsFirewall get_Enabled failed: 0x%08lx\n", hr);
                goto error;
            }

            if ( fwEnabled != VARIANT_FALSE )
            {
                // The globally open port is enabled.
                *fwPortEnabled = TRUE;

                logFatal("Port %ld is open in the firewall.\n", portNumber);
            }
            else
            {
                logFatal("Port %ld is not open in the firewall.\n", portNumber);
            }
        }
        else
        {
            // The globally open port was not in the collection.
            hr = S_OK;

            logFatal("Port %ld is not open in the firewall.\n", portNumber);
        }

error:

        // Release the globally open port.
        if ( fwOpenPort != NULL )
        {
            fwOpenPort->Release( );
        }

        // Release the globally open ports collection.
        if ( fwOpenPorts != NULL )
        {
            fwOpenPorts->Release( );
        }

        return hr;
    }

    HRESULT WindowsFirewallPortAdd (IN INetFwProfile *    fwProfile,
                                    IN LONG               portNumber,
                                    IN NET_FW_IP_PROTOCOL ipProtocol,
                                    IN const wchar_t *    name)
    {
        HRESULT          hr = S_OK;
        BOOL             fwPortEnabled;
        BSTR             fwBstrName  = NULL;
        INetFwOpenPort * fwOpenPort  = NULL;
        INetFwOpenPorts *fwOpenPorts = NULL;

        _ASSERT(fwProfile != NULL);
        _ASSERT(name != NULL);

        // First check to see if the port is already added.
        hr = WindowsFirewallPortIsEnabled(fwProfile, portNumber, ipProtocol,
                                          &fwPortEnabled);

        if ( FAILED(hr) )
        {
            logFatal("WindowsFirewallPortIsEnabled failed: 0x%08lx\n", hr);
            goto error;
        }

        // Only add the port if it isn't already added.
        if ( !fwPortEnabled )
        {
            // Retrieve the collection of globally open ports.
            hr = fwProfile->get_GloballyOpenPorts(&fwOpenPorts);

            if ( FAILED(hr) )
            {
                logFatal("WindowsFirewall get_GloballyOpenPorts failed: 0x%08lx\n", hr);
                goto error;
            }

            // Create an instance of an open port.
            hr = CoCreateInstance(__uuidof(NetFwOpenPort), NULL, CLSCTX_INPROC_SERVER,
                                  __uuidof(INetFwOpenPort), (void **) &fwOpenPort);

            if ( FAILED(hr) )
            {
                logFatal("WindowsFirewall CoCreateInstance failed: 0x%08lx\n", hr);
                goto error;
            }

            // Set the port number.
            hr = fwOpenPort->put_Port(portNumber);

            if ( FAILED(hr) )
            {
                logFatal("WindowsFirewall put_Port failed: 0x%08lx\n", hr);
                goto error;
            }

            // Set the IP protocol.
            hr = fwOpenPort->put_Protocol(ipProtocol);

            if ( FAILED(hr) )
            {
                logFatal("WindowsFirewall put_Protocol failed: 0x%08lx\n", hr);
                goto error;
            }

            // Allocate a BSTR for the friendly name of the port.
            fwBstrName = SysAllocString(name);

            if ( SysStringLen(fwBstrName) == 0 )
            {
                hr = E_OUTOFMEMORY;
                logFatal("WindowsFirewall SysAllocString failed: 0x%08lx\n", hr);
                goto error;
            }

            // Set the friendly name of the port.
            hr = fwOpenPort->put_Name(fwBstrName);

            if ( FAILED(hr) )
            {
                logFatal("WindowsFirewall put_Name failed: 0x%08lx\n", hr);
                goto error;
            }

            // Opens the port and adds it to the collection.
            hr = fwOpenPorts->Add(fwOpenPort);

            if ( FAILED(hr) )
            {
                logFatal("WindowsFirewall Add port firewall failed: 0x%08lx\n", hr);
                goto error;
            }
            logFatal("WindowsFirewall Port %ld is now open in the firewall.\n", portNumber);
        }
error:

        // Free the BSTR.
        SysFreeString(fwBstrName);

        // Release the open port instance.
        if ( fwOpenPort != NULL )
        {
            fwOpenPort->Release( );
        }

        // Release the globally open ports collection.
        if ( fwOpenPorts != NULL )
        {
            fwOpenPorts->Release( );
        }
        return hr;
    }

#if 0
    int __cdecl wmain (int argc, wchar_t *argv[])
    {
        HRESULT        hr        = S_OK;
        HRESULT        comInit   = E_FAIL;
        INetFwProfile *fwProfile = NULL;

        // Initialize COM.
        comInit = CoInitializeEx(
            0,
            COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
            );

        // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
        // initialized with a different mode. Since we don't care what the mode is,
        // we'll just use the existing mode.
        if ( comInit != RPC_E_CHANGED_MODE )
        {
            hr = comInit;

            if ( FAILED(hr) )
            {
                printf("CoInitializeEx failed: 0x%08lx\n", hr);
                goto error;
            }
        }

        // Retrieve the firewall profile currently in effect.
        hr = WindowsFirewallInitialize(&fwProfile);

        if ( FAILED(hr) )
        {
            printf("WindowsFirewallInitialize failed: 0x%08lx\n", hr);
            goto error;
        }

        // Turn off the firewall.
        hr = WindowsFirewallTurnOff(fwProfile);

        if ( FAILED(hr) )
        {
            printf("WindowsFirewallTurnOff failed: 0x%08lx\n", hr);
            goto error;
        }

        // Turn on the firewall.
        hr = WindowsFirewallTurnOn(fwProfile);

        if ( FAILED(hr) )
        {
            printf("WindowsFirewallTurnOn failed: 0x%08lx\n", hr);
            goto error;
        }

        // Add Windows Messenger to the authorized application collection.
        hr = WindowsFirewallAddApp(
            fwProfile,
            L"%ProgramFiles%\\Messenger\\msmsgs.exe",
            L"Windows Messenger"
            );

        if ( FAILED(hr) )
        {
            printf("WindowsFirewallAddApp failed: 0x%08lx\n", hr);
            goto error;
        }

        // Add TCP::80 to list of globally open ports.
        hr = WindowsFirewallPortAdd(fwProfile, 80, NET_FW_IP_PROTOCOL_TCP, L"WWW");

        if ( FAILED(hr) )
        {
            printf("WindowsFirewallPortAdd failed: 0x%08lx\n", hr);
            goto error;
        }

error:

        // Release the firewall profile.
        WindowsFirewallCleanup(fwProfile);

        // Uninitialize COM.
        if ( SUCCEEDED(comInit) )
        {
            CoUninitialize( );
        }

        return 0;
    }
#endif
	static void CheckFirewall(int port=7878, BOOL & fwOn)
	{

		HRESULT hr ;
		CComPtr<INetFwProfile> fwProfile ;
		fwOn=false;
		// Initialize COM.
		WindowsFirewall firewall;
		try 
		{
			//long mPort = ::GetPrivateProfileInt(_T("GLOBALS"), _T("port"), 7878, (ExeDirectory() + "Config.ini").c_str());

			hr=firewall.WindowsFirewallInitialize(&fwProfile);
			hr=firewall.WindowsFirewallIsOn(fwProfile,&fwOn);
			if(SUCCEEDED(hr) && fwOn)
			{
				//EventLogger.LogEvent(PROVIDER_NAME + std::string("Firewall ON"));
				logFatal((PROVIDER_NAME + std::string("Firewall ON\n"))..c_str());
				hr=firewall.WindowsFirewallPortIsEnabled(fwProfile,port,NET_FW_IP_PROTOCOL_TCP , &fwOn);
				if(SUCCEEDED(hr) && fwOn)
				{
					logFatal("%s Firewall ON Port %x BLOCKED\n",PROVIDER_NAME , port);

					if(SUCCEEDED(firewall.WindowsFirewallPortAdd(fwProfile,port,NET_FW_IP_PROTOCOL_TCP,L"MTConnect SHDR")))
					{
						logFatal("%s Firewall ON Port %x MADE UNBLOCKED\n",PROVIDER_NAME , port);
					}
				}
				else
				{
					logFatal("%s Firewall ON Port %x NOT BLOCKED\n",PROVIDER_NAME , port);
				}

			}
			else
			{
				logFatal((PROVIDER_NAME + std::string(" Firewall returns OFF\n")).c_str());
			}
		} 
		catch (std::exception errmsg)
		{
			logFatal(errmsg.what());

		}
		catch (...)
		{
			logFatal("Fatal Application Exception in DDE Adapter");

		}

	}
};
