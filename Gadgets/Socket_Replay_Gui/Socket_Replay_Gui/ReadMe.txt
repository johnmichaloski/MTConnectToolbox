

How to use IP control in win32

Setting IP:
		HWND hwndIPCtl = GetDlgItem(hDlg, IDC_IPADDRESS1);
		SendMessage(hwndIPCtl, IPM_SETADDRESS,0, MAKEIPADDRESS(127, 0, 0, 1));
Getting IP:
			HWND hwndIPCtl = GetDlgItem(hDlg, IDC_IPADDRESS1);
			DWORD dwAddr;
			int iCount = (int)SendMessage(hwndIPCtl, IPM_GETADDRESS, 0, (LPARAM)&dwAddr);
			char wchAddr[20];
			StringCchPrintf(wchAddr, _countof(wchAddr), _T("%ld.%ld.%ld.%ld"),
				FIRST_IPADDRESS(dwAddr),
				SECOND_IPADDRESS(dwAddr),
				THIRD_IPADDRESS(dwAddr),
				FOURTH_IPADDRESS(dwAddr));

Thu 11/09/2017_11:57:55.25  
Removed options from config file,not necessary with GUI

	std::string cfgfile = ExeDirectory+"Config.ini";
	Nist::Config config;
	try {

		if(!config.load(cfgfile))
		{
			throw std::runtime_error(StrFormat("No such file %s", cfgfile.c_str()));
		}
		// simpler than using boost program options and requirement to link and load DLL :(
		parser.Repeat()   = config.GetSymbolValue("GLOBALS.Repeat", 0).toNumber<int>( );
		filename          = config.GetSymbolValue("GLOBALS.Filename", ExeDirectory+"out.txt").c_str( );
		bOptionWait       = config.GetSymbolValue("GLOBALS.Wait", 1).toNumber<int>( );
		portnum           = config.GetSymbolValue("GLOBALS.PortNum", portnum).toNumber<int>( );
		ip                = config.GetSymbolValue("GLOBALS.IP", ip).c_str( );
		dTimeMultipler    = config.GetSymbolValue("GLOBALS.TimeMultipler", dTimeMultipler).toNumber<double>( );

	}
	catch(std::exception err)
	{
		std::cout << err.what();
		std::cout << StrFormat("Exception in file %s at line %d\n", filename.c_str(),  parser.LineNumber());
	}