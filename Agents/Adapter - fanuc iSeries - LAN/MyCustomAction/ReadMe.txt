
$Type=[BUTTON1]
$Target=[TARGETDIR]$IpValue=[EDITA1]$FanucPort=[EDITA2]$ShdrPort=[EDITA3]

	// Get user configuration for installation
	CConfigDlg dlg;
	INT_PTR res = dlg.DoModal();

2014-10-23T18:38:14.0530Z|power|OFF|avail|UNAVAILABLE|block|UNAVAILABLE|line|UNA
VAILABLE|program|UNAVAILABLE|controllermode|UNAVAILABLE|execution|UNAVAILABLE|Xa
bs|UNAVAILABLE|Yabs|UNAVAILABLE|Zabs|UNAVAILABLE|Aabs|UNAVAILABLE|Babs|UNAVAILAB
LE|Cabs|UNAVAILABLE|Uabs|UNAVAILABLE|Vabs|UNAVAILABLE|Srpm|UNAVAILABLE|Sovr|UNAV
AILABLE|path_feedrateovr|UNAVAILABLE|path_feedratefrt|UNAVAILABLE|heartbeat|UNAV
AILABLE|Sload|UNAVAILABLE|fault|UNAVAILABLE|toolid|UNAVAILABLE
	

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