//
// DdeAdapter.cpp
// 


#include "stdafx.h"
#include "DdeAdapter.h"
#include "ATLComTime.h"
#include "Logger.h"
#include "Firewall.h"
#include "StdStringFcn.h"

int DdeAdapter::mCyclesUntilUp;
int DdeAdapter::mDebug;
static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg =  StdStringFormat("In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode);
	OutputDebugString(errmsg.c_str() );
	throw std::exception(errmsg.c_str() , pExp->ExceptionRecord->ExceptionCode);
} 

static void CheckFirewall(int port=7878)
{
	_set_se_translator( trans_func );  

	HRESULT hr ;
	CComPtr<INetFwProfile> fwProfile ;
	BOOL fwOn=false;
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
			GLogger.Fatal(PROVIDER_NAME + std::string("Firewall ON\n"));
			hr=firewall.WindowsFirewallPortIsEnabled(fwProfile,port,NET_FW_IP_PROTOCOL_TCP , &fwOn);
			if(SUCCEEDED(hr) && fwOn)
			{
				std::string msg = StdStringFormat("%s Firewall ON Port %x BLOCKED\n",PROVIDER_NAME , port);
				//				EventLogger.LogEvent(msg);
				GLogger.Fatal(msg);

				if(SUCCEEDED(firewall.WindowsFirewallPortAdd(fwProfile,port,NET_FW_IP_PROTOCOL_TCP,L"MTConnect SHDR")))
				{
					std::string msg = StdStringFormat("%s Firewall ON Port %x MADE UNBLOCKED\n",PROVIDER_NAME , port);
					//EventLogger.LogEvent(msg);
					GLogger.Fatal(msg);

				}
			}
			else
			{
				std::string msg = StdStringFormat("%s Firewall ON Port %x NOT BLOCKED\n",PROVIDER_NAME , port);
				GLogger.Fatal(msg);
			}

		}
		else
		{
			GLogger.Fatal(PROVIDER_NAME + std::string(" Firewall returns OFF\n"));
		}
	} 
	catch (std::exception errmsg)
	{
		GLogger.Fatal(errmsg.what());

	}
	catch (...)
	{
		GLogger.Fatal("Fatal Application Exception in DDE Adapter");

	}

}

void DdeAdapter::ErrMsg( std::string err)
{
	AtlTrace(err.c_str());
	if(mDebug>0)
	{
		//EventLogger.LogEvent(err);
		GLogger.Fatal(err+"\n");
	}
}

bool DdeAdapter::ResetAtMidnite()
{
	COleDateTime now = COleDateTime::GetCurrentTime();
	COleDateTime date2 =  COleDateTime(now.GetYear(), now.GetMonth(), now.GetDay(), 0, 0, 0) +  COleDateTimeSpan(1, 0, 0, 1);
	//COleDateTime date2 =  now +  COleDateTimeSpan(0, 0, 2, 0); // testing reset time - 2 minutes
	COleDateTimeSpan tilmidnight = date2-now;
	_resetthread.Initialize();
	_resetthread.AddTimer(
		(long) tilmidnight.GetTotalSeconds() * 1000,
		&_ResetThread,
		(DWORD_PTR) this,
		&_ResetThread._hTimer  // stored newly created timer handle
		) ;
	return true;

}
HRESULT DdeAdapter::CResetThread::Execute(DWORD_PTR dwParam, HANDLE hObject)
{
	static char name[] = "CResetThread::Execute";

	DdeAdapter * adapter = (DdeAdapter*) dwParam;
	CancelWaitableTimer(hObject);
	//DebugBreak();

	try {

		PROCESS_INFORMATION pi;
		ZeroMemory( &pi, sizeof(pi) );

		STARTUPINFO si;
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;	 // set the window display to HIDE	

		// SCM reset command of this service 
		std::string cmd = StdStringFormat("cmd /c net stop \"%s\" & net start \"%s\"", adapter->name(), adapter->name());        // Command line

		if(!::CreateProcess( NULL,   // No module name (use command line)
			const_cast<char *>(cmd.c_str()),
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi ))           // Pointer to PROCESS_INFORMATION structure
			AtlTrace("CreateProcess FAIL ") ;

		::Sleep(5000); // make sure process has spawned before killing thread
	}
	catch(...)
	{
		adapter->AbortMsg("Exception  - ResetAtMidnightThread(void *oObject");
	}
	return S_OK;
}


void DdeAdapter::initialize(int aArgc, const char *aArgv[])
{
	GLogger.Timestamping()=true;
	GLogger.Open();

	ParseOptions();
	CheckFirewall(mPort);

	MTConnectService::initialize(aArgc, aArgv);
	if (aArgc > 1) {
		mPort = atoi(aArgv[1]);
	}

}

HRESULT DdeAdapter::ParseOptions()
{
	crp::Config config;
	std::string inifile = ::ExeDirectory() + "Config.ini";

	std::vector<std::string> tags ; 
	try 
	{

		config.load(inifile);
		app=config.GetSymbolValue("GLOBALS.app", "ncdde").c_str();
		topic=config.GetSymbolValue("GLOBALS.topic", "machineswitch").c_str();
		cnc=config.GetSymbolValue("GLOBALS.cnc", "run_hmi.exe").c_str();
		tagdefs=config.GetSymbolValue("GLOBALS.tags", "GENERIC").c_str();

		mPort=config.GetSymbolValue("GLOBALS.port", "7878").toNumber<long>();
		mSleep=config.GetSymbolValue("GLOBALS.sleep", "2000").toNumber<long>();
		mDebug=config.GetSymbolValue("GLOBALS.debug", "0").toNumber<long>();
		//mPort = ::GetPrivateProfileInt(_T("GLOBALS"), _T("port"), mPort, inifile.c_str());
		//mSleep = ::GetPrivateProfileInt(_T("GLOBALS"), _T("sleep"), mSleep, inifile.c_str());
		GLogger.DebugLevel() = ::GetPrivateProfileInt(_T("GLOBALS"), _T("debug"), 0, inifile.c_str());
		///mSpawnDDE=config.GetSymbolValue("GLOBALS.spawndde", "F:\\mmc2\\ncdde.exe").c_str();
		mSpawnDDE=config.GetSymbolValue("GLOBALS.spawndde", "ncdde.exe").c_str();


		mCyclesUntilUp=config.GetSymbolValue("GLOBALS.cyclesuntilup", "10").toNumber<long>();

		tags = config.getsection(tagdefs.c_str()); 

		std::vector<std::string> keys = config.getkeys(tagdefs);
		if(tags.size() < 1 || keys.size() < 1 )
			throw std::exception(StdStringFormat("Error: No [%s] Section or Keys?\n", tagdefs.c_str()).c_str());

		for(int i=0; i<keys.size(); i++)
		{
			Item * item = new Item();
			item->_alias=keys[i];

			// make sure its is an OPC Tag
			if(item->_alias.find(_T("Tag.")) == 0)
			{
				item->_alias=ReplaceOnce(item->_alias, _T("Tag."), _T(""));
				item->vt=VT_BSTR;
			}
			else if(item->_alias.find(_T("Enum."))==0)
			{
				item->_type=_T("Enum");
				item->_subtype="Enumeration";
				item->_value=item->_lastvalue= config.GetSymbolValue(tagdefs+"."+ keys[i]).c_str();
				//item->_tagname No tagname
				items.push_back(item);
				continue;
			}
			else if(item->_alias.find(_T("Const."))==0)
			{
				item->_alias=ReplaceOnce(item->_alias,_T("Const."), _T(""));
				item->vt= VT_USERDEFINED;
				item->_subtype="Constant";
			}

			// THese must be DDE lookup
			if(item->_alias.find(_T("Sample."))==0)
			{
				item->_alias=ReplaceOnce(item->_alias,_T("Sample."), _T(""));
				item->_type=_T("Sample");
			}
			else if(item->_alias.find(_T("Event."))==0)
			{
				item->_alias=ReplaceOnce(item->_alias,_T("Event."), _T(""));
				item->_type=_T("Event");
			}
			else if(item->_alias.find(_T("DDE."))==0)  // reads opc values does not send to MT Connect
			{
				item->_alias=ReplaceOnce(item->_alias, _T("DDE."), _T(""));  // OPC value for scripting
				item->_type=_T("DDE");
			}
			else if(item->_alias.find(_T("Condition."))==0)
			{
				item->_alias=ReplaceOnce(item->_alias,_T("Condition."), _T(""));
				item->_type="Condition";
			}
			std::string tmp= config.GetSymbolValue(tagdefs+"."+ keys[i]).c_str();

			item->_tagname=tmp;
			items.push_back(item);
		}
		for(int i = 0 ; i < items.size(); i++)
		{
			if(items[i]->_type==_T("Event") || items[i]->_type==_T("Sample"))
			{
				strncpy(items[i]->_event.mName,items[i]->_alias.c_str(),NAME_LEN);
				addDatum(items[i]->_event);
			}
		}
	}
	catch(std::exception e)
	{
		ErrMsg( e.what());
		return E_FAIL;
	}
	return S_OK;
}

void DdeAdapter::gatherDeviceData()
{
	ExeList taskmgr;
	static int counter=mCyclesUntilUp;
	counter--;
	try{
#ifdef SIEMENS
		if(_p840DShutdownSink->CanSubscriptionSink())
		{
			if(!bConnected && !_p840DShutdownSink->IsConnected())
			{
				if(FAILED(_p840DShutdownSink->Register()))
					return;

			}
			if(bConnected && !_p840DShutdownSink->IsConnected())
			{
				throw std::string("Error disconnecting from CNC\n");
			}
			bConnected=true;
			if(_p840DShutdownSink->bShutdown)
			{
				throw std::string("Disconnecting Shutdown from CNC\n");
			}
		}
#endif
#ifdef REALTHING
		if(!taskmgr.Find(cnc))
		{
			ErrMsg("Did not find active CNC.exe yet!");
			clear();
			counter=6*mCyclesUntilUp;
			if(taskmgr.Find("ncdde.exe"))
			{
				ErrMsg("KillProgram ncdde.exe");
				taskmgr.KillProgram("ncdde.exe");
			}
			return;
			//throw std::string("Did not find CNC.exe!");
		}


		if(counter >0)
		{
			::Sleep(1000); // wait 10*10 seconds so cnc actually starts
			return; 
		}

		if((counter == 0) && !taskmgr.Find("ncdde.exe"))
		{
			ErrMsg(StdStringFormat("Launching %s\n",mSpawnDDE.c_str()));
			if(FAILED(taskmgr.Launch(mSpawnDDE, 2000)))
				throw std::string("Error launching ncdde.exe\n");

		}
		if(!taskmgr.Find("ncdde.exe"))
		{
			throw std::string("Did not launch NCDDE.EXE and then find NCDDE.EXE!");
		}

		if(! ddeAdapter.IsConnected())
		{
			// check for error
			if(FAILED(init()))
				throw "Cannot init 840D DDE";
		}
		if(! ddeAdapter.IsConnected())
			throw ddeAdapter.DDEErrorMessage(DMLERR_NO_CONV_ESTABLISHED);
#else
		::Sleep(1000);
#endif

		mAvailability.available();
		mPower.setValue(PowerState::eON);

#ifdef REALTHING
		// Read DDE Values
		for(int i=0; i< items.size(); i++)
		{
			Item * item = items[i];
			std::string data;

			// Skip lookup for constants and enumerations
			if(	item->_subtype=="Constant" || item->_subtype=="Enumeration")
				continue;

			// DDE read of tagname
			if(FAILED(ddeAdapter.GetData((LPSTR) item->_tagname.c_str(), data)))
			{
				// should we abort if error? Change to throw
				this->ErrMsg(data); // error message - maybe throw?
				continue;
			}
			else
			{
				item->_lastvalue=item->_value;
				item->_value=data;
			}
		}
#else 
		for(int i=0; i< items.size(); i++)
		{
			Item * item = items[i];

			item->_lastvalue=item->_value;
			int n = ConvertString<long>(item->_value,99)+1;
			item->_value=StdStringFormat("%d", n);
		}
#endif
#ifdef REALTHING
		// Change DDE Enum to value
		for(int i=0; i< items.size(); i++)
		{				
			std::string alias =  items[i]->_alias; // NOTE!!! alias points to MTConnect name or id 
			std::string value =  items[i]->_value;
			std::string szEnumTag= "Enum." + alias + "." + value;
			if(! items.FindEnum("Enum." + alias) )  // look for enumeration
				continue;
			//szEnumTag.Format(_T("Enum.%s.%s"), alias, value);
			value = items.GetSymbolValue( szEnumTag, (LPCSTR) "UNAVAILABLE");

			// Use default of -1 if unavailable value
			if(value == "UNAVAILABLE")
			{
				szEnumTag=alias + ".-1";
				value = items.GetSymbolValue(szEnumTag, (LPCSTR) "UNAVAILABLE");
			}
			else
			{
				items[i]->_lastvalue=items[i]->_value;
				items[i]->_value=value;
			}
		}
#endif
		for(int i = 0 ; i < items.size(); i++)
		{
			if(items[i]->_type==_T("Event") || items[i]->_type==_T("Sample"))
			{
				if(items[i]->_value != items[i]->_lastvalue)
					items[i]->_event.setValue(items[i]->_value.c_str());
			}
		}
	}
	catch(std::string errmsg)
	{
		ErrMsg(errmsg.c_str());
		clear();
		counter=10;
		if(taskmgr.Find("ncdde.exe"))
		{
			ErrMsg("KillProgram ncdde.exe");
			taskmgr.KillProgram("ncdde.exe");
		}
	}
	catch(...)
	{
		ErrMsg("Throw error in gatherDeviceData()\n");
		clear();
		counter=10;
		if(taskmgr.Find("ncdde.exe"))
		{
			ErrMsg("KillProgram ncdde.exe");
			taskmgr.KillProgram("ncdde.exe");
		}
	}
}