


In windows 10 to start service control manager:
services.msc


	// jlmichaloski added
	static std::map<std::string, std::string> Adapter::keymapping; 
	static std::map<std::string, std::string> Adapter::enummapping; 


Adapter.cpp
// jlmichaloski added
std::map<std::string, std::string> Adapter::keymapping; 
std::map<std::string, std::string> Adapter::enummapping; 



template<typename NumberType>
NumberType toNumber(std::string data) 
{
	NumberType result = 0;
	std::istringstream stream(data);

	if(data=="UNKNOWN")
		return 0;
	if(stream >> result)
		return result;
	throw std::runtime_error("Bad number conversion");
}

void Adapter::processData(const string& data)
{
	if (mGatheringAsset)
	{
		if (data == mTerminator)
		{
			mAgent->addAsset(mAssetDevice, mAssetId, mBody.str(), mAssetType, mTime);
			mGatheringAsset = false;
		}
		else
		{
			mBody << data << endl;
		}

		return;
	}

	istringstream toParse(data);
	string key, value;

	getline(toParse, key, '|');
	string time = extractTime(key);


	getline(toParse, key, '|');
	getline(toParse, value, '|');

	
	// Michaloski hack - if srpm2 is > 0 assign it to Srpm 
	if(key.find("rpm"))
	{
		try {
			int rpm = toNumber<int>(value);
			if(rpm>0)
				key="Srpm";
		}
		catch(...)
		{
			// bummer conversion didn't work
		}

	}
	// Michaloski added - Map  shdr key (e.g., mode) into new key (controllermode) 
	if(keymapping.find(key)!= keymapping.end())
	{
		key=keymapping[key];
	}
	// Change enumerations
	if(enummapping.find(key+"."+value)!= enummapping.end())
	{
		value=enummapping[key+"."+value];
	}

Yaml Reader - unnecessary all config now in ini file
				/*
				ROOT.Adapters.MakinoT4.Host=192.168.1.200
				ROOT.Adapters.MakinoT4.Port=7870
				ROOT.AllowPut=true
				ROOT.CheckpointFrequency=10000
				ROOT.Devices=Devices.xml
				ROOT.Port=5001
				ROOT.ServiceName=MTConnectAgent_T4
				ROOT.logger_config.logging_level=fatal
				ROOT.logger_config.output=cout
				*/
				YamlReader yaml;
				std::string filename = File.ExeDirectory()+ "\\Agent.cfg";
				yaml.LoadFromFile(filename);
				std::string HttpPort = yaml.Find("GLOBAL.Port");
				std::string ServiceName = yaml.Find("GLOBAL.ServiceName");




___report_rangecheckfailure unresolved external

In the Solution Explorer, look at the project that you're linking (not the library) and see if the entry is marked "(Visual Studio 2010)".  If so, they you will have to choose between updating that project to Visual Studio 2012 (or later) or turning off compiler option /GS (Buffer Security Check) in that project and all of the libraries from which it pulls in code.

Normally in Visual Studio 2012, the linker uses the "Microsoft Visual Studio 11.0" C libraries.  However, if your project is identified as "(Visual Studio 2010)", the linker uses the "Microsoft Visual Studio 10.0" instead.

In Visual Studio 2012 and later, the /GS option generates calls to __report_rangecheckfailure.  (See http://blogs.msdn.com/b/sdl/archive/2012/01/26/enhancements-to-gs-in-visual-studio-11.aspx for a detailed description.)  In the "Microsoft Visual Studio 11.0" libraries, __report_rangecheckfailure, is in msvcrt[d].lib, but it does not appear at all in the "Microsoft Visual Studio 10.0" libraries.  Hence, a "(Visual Studio 2010)" project that uses code built with /GS under Visual Studio 2012 will get an unresolved external reference to __report_rangecheckfailure.

So, your choices are to upgrade your project to Visual Studio 2012 (from the main menu Project | Upgrade VC++ projects…) or to forgo compiler option /GS (in the project's Property Pages, select Configuration Properties | C/C++ | Code Generation, and set Buffer Security Check to No (/GS-).)


#if 0
		//READY, ACTIVE, INTERRUPTED, or STOPPED 
		// Version 0.9 EXECUTION IDLE, EXECUTING, or PAUSED
		{
			Adapter::enummapping["execution.READY"]="IDLE";
			Adapter::enummapping["execution.ACTIVE"]="EXECUTING";
			Adapter::enummapping["execution.INTERRUPTED"]="PAUSED";
			Adapter::enummapping["execution.STOPPED"]="PAUSED";
			Adapter::keymapping["mode"]="controllermode";
			Adapter::keymapping["fovr"]="path_feedrateovr";
			Adapter::keymapping["fr"]="path_feedratefrt";
#if 1
			Adapter::keymapping["toolid"]="Tool_number";

			Adapter::keymapping["xact"]="Xabs";
			Adapter::keymapping["yact"]="Yabs";
			Adapter::keymapping["zact"]="Zabs";
			Adapter::keymapping["sspeed"]="Srpm";
			Adapter::keymapping["SSovr"]="Sovr";
#endif
		}
#endif

			  //if(bResetAtMidnight)
			  //{
				 // GLogger.Fatal("Agent will Reset At Midnight\n");

				 // COleDateTime now = COleDateTime::GetCurrentTime();
				 // COleDateTime date2 =  COleDateTime(now.GetYear(), now.GetMonth(), now.GetDay(), 0, 0, 0) +  COleDateTimeSpan(1, 0, 0, 1);
				 // //COleDateTime date2 =  now +  COleDateTimeSpan(0, 0, 2, 0); // testing reset time - 2 minutes
				 // COleDateTimeSpan tilmidnight = date2-now;
				 // _resetthread.Initialize();
				 // _resetthread.AddTimer(
					//  (long) tilmidnight.GetTotalSeconds() * 1000,
					//  &_ResetThread,
					//  (DWORD_PTR) this,
					//  &_ResetThread._hTimer  // stored newly created timer handle
					//  ) ;


			  //}


			  //if(bResetAtMidnight)
				 // _resetthread.Shutdown();
			  // Check if invoked from SCM as opposed to midnite reset.

			  #if 0
HRESULT AgentConfigurationEx::CResetThread::Execute(DWORD_PTR dwParam, HANDLE hObject)
{
	static char name[] = "CResetThread::Execute";

	AgentConfigurationEx * agent = (AgentConfigurationEx*) dwParam;
	CancelWaitableTimer(hObject);

	try {
		//GLogger.LogMessage("MTConnect adapter Service Start Resetting %s\n", (LPCSTR) COleDateTime::GetCurrentTime());;
		//OutputDebugString(StdStringFormat( "MTConnect adapter Service Start Resetting %s\n" , (LPCSTR) COleDateTime::GetCurrentTime()).c_str());

		PROCESS_INFORMATION pi;
		ZeroMemory( &pi, sizeof(pi) );

		STARTUPINFO si;
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;	 // set the window display to HIDE	

		// SCM reset command of this service 
		std::string cmd = StdStringFormat("cmd /c net stop \"%s\" & net start \"%s\"", agent->name().c_str(), agent->name().c_str());        // Command line

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
			ReportError("CreateProcess ") ;

		::Sleep(1000); // make sure process has spawned before killing thread
	}
	catch(...)
	{
		 GLogger.Fatal(StdStringFormat( "Exception  - ResetAtMidnightThread(void *oObject)\n" ).c_str());
	}
	return S_OK;
}
#endif
