
Fri 10/20/2017_11:15:05.47  

Agent crashes
  C:\Users\michalos\Documents\GitHub\Agents\moto_MTConnectAgent\MTConnectAgent\agent\agent.cpp(75):        sLogger << LFATAL << "Error loading xml configuration: " + configXmlPath;
  C:\Users\michalos\Documents\GitHub\Agents\moto_MTConnectAgent\MTConnectAgent\agent\agent.cpp(76):        sLogger << LFATAL << "Error detail: " << e.what( );
  C:\Users\michalos\Documents\GitHub\Agents\moto_MTConnectAgent\MTConnectAgent\agent\agent.cpp(186):                sLogger << LFATAL << "Duplicate DataItem id " << d->getId( )
                sLogger << LFATAL << "Duplicate DataItem id " << d->getId( )
                        << " for device: " << ( *device )->getName( ) << " and data item name: "
                        << d->getName( );
                exit(1);  
 C:\Users\michalos\Documents\GitHub\Agents\moto_MTConnectAgent\MTConnectAgent\agent\agent.cpp(219):        sLogger << LFATAL << "Cannot start server: " << e.what( );
				\        sLogger << LFATAL << "Cannot start server: " << e.what( );
        exit(1);

  C:\Users\michalos\Documents\GitHub\Agents\moto_MTConnectAgent\MTConnectAgent\agent\config.cpp(138):        sLogger << LFATAL << "Agent failed to load: " << e.what( );
       configureLogger( );
        ifstream file(mConfigFile.c_str( ));
        loadConfig(file);
    }
    catch ( std::exception & e )
    {
        OutputDebugString("Agent failed to load");
        OutputDebugString(e.what( ));

        sLogger << LFATAL << "Agent failed to load: " << e.what( );



$Type=[BUTTON1]$IpValue=[EDITA1]$HttpPort=[EDITA2]$Target=[TARGETDIR]$Config=[EDITA3]

C:\Users\michalos\AppData\Local\MTConnect\Ata\ATA_4_Status.tsv

//////////////////////////////////////////////////////
	//	RunSynchronousProcess("sc.exe", (" install AtaAgent binPath=\""+path+"AtaAgent.exe\"").c_str());
	//	status="sc Start service AtaAgent";

	//::MessageBox(NULL,(path+"superuser.bat").c_str(), "ALERT", MB_OK);
		//RunSynchronousProcess(path+"superuser.bat","");
//		::Sleep(4000);
//		::MessageBox(NULL,"Create service with AtaAgent.exe", "ALERT", MB_OK);
		// Install and start service  
	//RunSynchronousProcess("sc.exe", " start AtaAgent");

//////////////////////////////////////////////////////
Dialog
	// Get user configuration for installation
	CConfigDlg dlg;
	INT_PTR res = dlg.DoModal();

	
	#if 0
#include "Resource.h"
class CConfigDlg : public CDialogImpl<CConfigDlg>
{
public:
	enum { IDD = IDD_DIALOG1 };
	BEGIN_MSG_MAP(CConfigDlg)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)  
	END_MSG_MAP()
	CEdit port;
	CIPAddressCtrl ip;
	CComboBox debug;
	CComboBox rate;

	CString sPort,sIp,sDebug,sRate;

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		EndDialog(ERROR_SUCCESS);
		return 0;
	}

	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{

		port.GetWindowText(sPort);
		ip.GetWindowText(sIp);
		debug.GetWindowText(sDebug);
		rate.GetWindowText(sRate);

		EndDialog(IDOK );
		return 0;
	}
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		port.GetWindowText(sPort);
		ip.GetWindowText(sIp);
		debug.GetWindowText(sDebug);
		rate.GetWindowText(sRate);
		EndDialog(ERROR_SUCCESS);
		return 0;
	}

	LRESULT OnInitDialog(...)
	{
		CenterWindow();
		port = GetDlgItem(IDC_PORTEDIT);
		port.SetWindowTextA(_T("7878"));
		ip = GetDlgItem(IDC_IPADDRESS1);
		ip.SetWindowTextA(_T("127.0.0.1"));
		debug =  GetDlgItem(IDC_DEBUGCOMBO);
		debug.AddString(_T("fatal"));
		debug.AddString(_T("error"));
		debug.AddString(_T("warn"));
		debug.AddString(_T("debug"));
		debug.SetCurSel(0);

		rate=  GetDlgItem(IDC_RATECOMBO);
		rate.AddString(_T("1"));
		rate.AddString(_T("2"));
		rate.AddString(_T("5"));
		rate.AddString(_T("10"));
		rate.SetCurSel(0);
		return 0;
	}
};
#endif
	//contents=ReplaceOnce(contents,"IP=127.0.0.1", "IP=" + ipaddr);
	//contents=ReplaceOnce(contents,"Machine=DMG2796", "Machine=" + machinename);

// REmoved not necessary 
	status="Read MTCFanucAgent.ini";
	std::string contents; 
	ReadFile(path+"MTCFanucAgent.ini", contents);

	//FanucIpAddress=192.168.1.102
	ReplacePattern(contents, "FanucIpAddress=", "\n", "FanucIpAddress=" + ipaddr + "\n");
	//Protocol=LAN 
	if(type=="1")
		ReplacePattern(contents, "Protocol=", "\n", "Protocol=HSSB\n"); 
	else
		ReplacePattern(contents, "Protocol=", "\n", "Protocol=LAN\n"); 
		
	ReplacePattern(contents, "FanucPort=", "\n", "FanucPort=" + fanucPort +"\n"); 

	WriteFile(path+"MTCFanucAgent.ini",contents);

	status="Read agent.cfg";

					// holds address info for socket to connect to
				struct addrinfo *result = NULL;
				struct addrinfo	*ptr = NULL;
				struct addrinfo	hints;

				// set address info
				ZeroMemory( &hints, sizeof(hints) );
				hints.ai_family = AF_UNSPEC;
				hints.ai_socktype = SOCK_STREAM;
				hints.ai_protocol = IPPROTO_TCP;  //TCP connection!!!

				//resolve server address and port
				iResult = getaddrinfo(mHost.c_str( ), 30002, &hints, &result);

				// Attempt to connect to an address until one succeeds
				for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

					// Create a SOCKET for connecting to server
					server = socket(ptr->ai_family, ptr->ai_socktype,
						ptr->ai_protocol);

					if (server == INVALID_SOCKET) {
						printf("socket failed with error: %ld\n", WSAGetLastError());
					}

					// Connect to server.
					iResult = connect( server, ptr->ai_addr, (int)ptr->ai_addrlen);
					if (iResult == SOCKET_ERROR)
					{
						closesocket(server);
						int errnum = WSAGetLastError( );
						logError("Error %s connecting to host %s\n",
							WhatIsWSAError(errnum).c_str( ), mHost.c_str( ));

					}
					else
					{
						mConnected.set(true);
					}
				}
#if 0