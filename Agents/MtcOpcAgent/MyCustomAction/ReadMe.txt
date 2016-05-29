
$Type=[BUTTON1]$IpValue=[EDITA1]$HttpPort=[EDITA2]$Target=[TARGETDIR]$Config=[EDITA3]


Problem with 

Matched a string within a comment: 
# 2) MTConnectDevice=M234


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