

Handle tag and enumeration renaming:
#ifdef MAZAKAGENT
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
#ifdef LINUXSOLUTIONLINEAGENT
			Adapter::keymapping["toolid"]="Tool_number";

			Adapter::keymapping["xact"]="Xabs";
			Adapter::keymapping["yact"]="Yabs";
			Adapter::keymapping["zact"]="Zabs";
			Adapter::keymapping["sspeed"]="Srpm";
			Adapter::keymapping["SSovr"]="Sovr";
#endif
		}
#endif

		// Now parse the tag information into MTConnect information
		std::vector<std::string> tags = GetSectionKeys(_sTagSectionName);
		if(tags.size() < 1)
			throw std::exception(StdStringFormat("Error: No [%s] Section\n", _sTagSectionName.c_str()).c_str());
		for(int i=0; i<tags.size(); i++)
		{
			VarTypes vt;

			// make sure its is an OPC Tag
			if(tags[i].find(_T("Tag.")) == 0)
			{
				OpcItem * item = new OpcItem();
				item->_alias=tags[i];
				item->_alias=ReplaceOnce(item->_alias, _T("Tag."), _T(""));
				item->vt=VT_BSTR;
				if(item->_alias.find(_T("Enum."))==0)
				{
					item->_alias=ReplaceOnce(item->_alias, _T("Enum."), _T(""));
					item->_subtype="enumeration";
				}
				else if(item->_alias.find(_T("Const."))==0)
				{
					item->_alias=ReplaceOnce(item->_alias,_T("Const."), _T(""));
					item->vt= VT_USERDEFINED;
				}
				else if(item->_alias.find(_T("Prog."))==0)
				{
					item->_alias=ReplaceOnce(item->_alias,_T("Prog."), _T(""));
					item->vt= VT_USERDEFINED;
					item->_subtype="program";
				}

				if(item->_alias.find(_T("Sample."))==0)
				{
					item->_alias=ReplaceOnce(item->_alias,_T("Sample."), _T(""));
					item->_type=_T("Sample");
					//_mtcagent->AddSample(this->_device, item->_alias);
				}
				else if(item->_alias.find(_T("Event."))==0)
				{
					item->_alias=ReplaceOnce(item->_alias,_T("Event."), _T(""));
					item->_type=_T("Event");
					//_mtcagent->AddEvent(this->_device, item->_alias);
				}
				else if(item->_alias.find(_T("OPC."))==0)  // reads opc values does not send to MT Connect
				{
					item->_alias=ReplaceOnce(item->_alias, _T("OPC."), _T(""));  // OPC value for scripting
					item->_type=_T("OPC");
				}
				else if(item->_alias.find(_T("Condition."))==0)
				{
					item->_alias=ReplaceOnce(item->_alias,_T("Condition."), _T(""));
					item->_type="Condition";
					//_mtcagent->AddCondition(this->_device, item->_alias);
				}
				std::string tmp= GetSymbolValue(_sTagSectionName, tags[i],"");
				item->_tagname=tmp;
				opcitems.push_back(item);
			}

		}
		// Test to see if we have a power, if not add one.
		//if(opcitems.FindItem("power")==NULL)
		//	_mtcagent->AddEvent(this->_device, "power");

#ifdef ALARMS
		//Alarm Management
		_alarmconfig.load(ExeDirectory() + "English\\Alarms.cfg");
#endif

MTConnect Data Items Types Sample:
http://www.mtcup.org/wiki/Data_Item_Types:_SAMPLE


Borrowed Heavily from:
https://github.com/ThomasTimm/moto_modern_driver




STL Goodies!
http://www.bogotobogo.com/cplusplus/stl5_function_objects.php

Unsure about UNC naming for local file system




1) You will need to install boost to get its include and libraries.

2) You will need to modify the include directive in

C:\Users\michalos\Documents\GitHub\moto_MTConnectAgent\moto_Agent\IncludeDirX32.txt

to point to the location of you moto_Agent (now in C:\Users\michalos\Documents\GitHub\moto_MTConnectAgen)
which contains:

-I"C:\Program Files\NIST\src\boost_1_54_0"
-I.
-I"C:\Users\michalos\Documents\GitHub\moto_MTConnectAgent\MTConnectAgent\agent"
-I"C:\Users\michalos\Documents\GitHub\moto_MTConnectAgent\MTConnectAgent\lib"
-I"C:\Users\michalos\Documents\GitHub\moto_MTConnectAgent\MTConnectAgent\win32\libxml2-2.7.7\include"
-I"C:\Users\michalos\Documents\GitHub\moto_MTConnectAgent\MTConnectAgent"
-I"C:\Users\michalos\Documents\GitHub\moto_MTConnectAgent\NIST"

3) IN the moto_Agent project setting for the linker, you will need to add the folders that point to your file location for:
a) C:\Users\michalos\Documents\GitHub\moto_MTConnectAgent\MTConnectAgent\win32\libxml2-2.7.7\lib
b) C:\Program Files\NIST\src\boost_1_54_0\stage or wherever you have built the libraries for boost - Note 32 bit is different
that 64 bit!!


On Target Machine (Installation)
1) Create  install directory C:\Program Files\MTConnect\moto_Agent
2) Copy files and exe to install directory
3) Change file permission via cmd and InstallShield in install directory to everyone full control.
4) Register Agent in Windows service as automatic with reference to moto_Agent BUT NOT STARTED
5) User will need to configure  Config.ini for 
	MTConnectDevice=M1 
or whatever device name anb then configure ini file section:
	[M1]
	IPAddress=127.0.0.1
	ProductionLog=C:\Users\michalos\AppData\Local\MTConnect\ProductionLog.csv
	logging_level=ERROR
	Simulation=1
 Most important for now is UNC location for ProductionLog. IPAddress is ignored.

Prerequisite assumptions:

1) Starting midday service - otherwise may have to go back a day to look for Tags
2) Install changes permissions of Program Files (agent might write some logs to install directory) not temp directory.
cd /d %~dp0

echo y|  cacls .  /t  /c  /GRANT Everyone:F
NOTE changed already with addition of echo y and obsoleted cacls recently.
3) Fatal debug stoppage will go to Application Event Log (hopefullly)
4) Used sc.exe to install agent (and to stop and delete)
5) Agent is not configured! You will need to setup config.ini and then start agent.

Potential improvements:
1) check modification time stamp before copy


TESTS:
UNC remote fetch


SCRAPS:
#if 0
		FILE *stream;
		if( (stream = _fsopen( filename.c_str(), "rb", _SH_DENYNO )) == NULL )
		{
			DebugMessage(StdStringFormat("Couldn't open File \"%s\" for device %s\n", filename.c_str(), mDevice.c_str()));
			return E_FAIL;
		}

//		fseek(stream,0,SEEK_END);
//		long ulFileSize = ftell(stream);
		if(filesize>_lastfilesize && _lastfilesize != 0 )
		{
			fseek(stream,_lastfilesize,SEEK_SET);
			ulFileSize=filesize-_lastfilesize;
		}
		void  * pAddr = &data[0];
		if((numread= fread( pAddr, 1, ulFileSize, stream )) == 0)
		{
			DebugMessage(StdStringFormat("numread File %s is zero for device %s\n", filename.c_str(), mDevice.c_str()));
				re
				
				turn E_FAIL;

		}
		fclose( stream );
#endif

#if 0
		if(filesize>_lastfilesize && _lastfilesize != 0 )
		{	
			// Skip file until new lines
			//data=data.substr(_lastfilesize);			
		}
		else
		{
			size_t n=0;
			std::string date = StdStringFormat("%4d-%02d-%02d", today.GetYear(), today.GetMonth(), today.GetDay()); 
			// FIXME: maybe substract a day then search
			if((n=data.find(date)) == std::string::npos)
			{
				// Go back extra day - since could be 12:05
				COleDateTime newdate = today - COleDateTimeSpan(1,0,0,0);
				date = StdStringFormat("%4d-%02d-%02d", newdate.GetYear(), newdate.GetMonth(), newdate.GetDay()); 
				if((n=data.find(date)) == std::string::npos)
					return WarnWithMsg(E_FAIL, StdStringFormat("Cant find date \"%s\" for device %s\n", date.c_str(),  mDevice.c_str())) ;
			}

			data=data.substr(n); //return E_FAIL;


		}
#endif



	memset((char *) &sec_serv_addr_, 0, sizeof(sec_serv_addr_));
	sec_sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (sec_sockfd_ < 0) {
		return _Moto_Adapter->ErrorMessage("ERROR opening socket sec_sockfd host %s\n", _host.c_str());
	}

	sec_serv_addr_.sin_family = AF_INET;
	memcpy ((char *) server_->h_addr, (char *)&sec_serv_addr_.sin_addr.s_addr, server_->h_length);
	sec_serv_addr_.sin_port = htons(30002);

	mFlag = 1;

	setsockopt(sec_sockfd_, IPPROTO_TCP, TCP_NODELAY, (char *) &mFlag,
			sizeof(int));
#ifndef _WINDOWS
	setsockopt(sec_sockfd_, IPPROTO_TCP, TCP_QUICKACK, (char *) &mFlag,
			sizeof(int));
#endif
	setsockopt(sec_sockfd_, SOL_SOCKET, SO_REUSEADDR, (char *) &mFlag,
			sizeof(int));
#ifdef _WINDOWS
    unsigned long ulValue = 1;
    ioctlsocket (sec_sockfd_, FIONBIO, &ulValue);
#else
    iValue = fcntl (m_hSocket, F_GETFL);
    iValue |= O_NONBLOCK;
	fcntl(sec_sockfd_, F_SETFL, O_NONBLOCK);
#endif

        MTConnectSingleton       singleton;
        std::vector<std::string> filepaths;
        DWORD                    pid         = singleton.GetPid( );
        std::string              modulename  = singleton.GetModuleName( );
        std::string              modulepath  = singleton.GetModulePath( );
        std::string              processname = singleton.ExtractFilename(argv[0]);
        std::vector<DWORD>       pids        = singleton.GetModules(processname, filepaths);
        remove(( File.ExeDirectory( ) + "MaydayMayday.txt" ).c_str( ));

        std::vector<std::string>::iterator it = std::find(filepaths.begin( ), filepaths.end( ), modulepath);

        if ( it != filepaths.end( ) )
        {
            filepaths.erase(it);                           // delete 1 - my running image
        }

        // if another one - bad news
        it = std::find(filepaths.begin( ), filepaths.end( ), modulepath);

        if ( it != filepaths.end( ) )
        {
            Logging::CLogger Mayday;
            Mayday.Timestamping( )  = true;
            Mayday.DebugString( )   = "moto_Agent";
            Mayday.OutputConsole( ) = false;
            Mayday.Open(File.ExeDirectory( ) + "MaydayMayday.txt");
            Mayday.DebugLevel( ) = 5;
            Mayday.Fatal("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            Mayday.Fatal("Conflict with another MTConnect agent with same exe name and http port number\n");
            Mayday.Fatal("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            Mayday.Close( );
        }
		   /**
   * @brief ErrorMessage variable argument list to log if logging enabled.
   * @param format printf string and associated parameters
   * @return E_FAIL
   */
    virtual HRESULT ErrorMessage (const char *format, ...);

    /**
   * @brief DebugMessage variable argument list to log if logging enabled.
   * @param format printf string and associated parameters
   * @return E_FAIL if level <= logging level
   */
    virtual HRESULT DebugMessage (int level, const char *format, ...);

HRESULT AdapterT::ErrorMessage (const char *format, ...)
{
    va_list ap;

    va_start(ap, format);

    int         m;
    int         n = strlen(format) + 1028;
    std::string tmp(n, '0');

    // Kind of a bogus way to insure that we don't
    // exceed the limit of our buffer
    while ( ( m = _vsnprintf(&tmp[0], n - 1, format, ap) ) < 0 )
    {
        n = n + 1028;
        tmp.resize(n, '0');
    }

    va_end(ap);

    // AtlTrace(tmp.c_str( ) );
    GLogger.LogMessage(tmp.substr(0, m), LOWERROR);
    return E_FAIL;
}
HRESULT AdapterT::DebugMessage (int level, const char *format, ...)
{
    va_list ap;

    va_start(ap, format);

    int         m;
    int         n = strlen(format) + 1028;
    std::string tmp(n, '0');

    // Kind of a bogus way to insure that we don't
    // exceed the limit of our buffer
    while ( ( m = _vsnprintf(&tmp[0], n - 1, format, ap) ) < 0 )
    {
        n = n + 1028;
        tmp.resize(n, '0');
    }

    va_end(ap);

    // AtlTrace(tmp.c_str( ) );
    GLogger.LogMessage(tmp.substr(0, m), level);

    if ( level < 3 )
    {
        return E_FAIL;
    }
    return S_OK;
}


//#ifdef CASE_SENSITIVE
//			iter = inimap.find(keyName);
//#else
//			// Key is now uppercase
//			std::transform(keyName.begin( ), keyName.end( ), keyName.begin( ), toupper);
//
//			for(iter=inimap.begin(); iter!=inimap.end(); iter++)
//			{
//				std::string key = (*iter).first;
//				std::transform(key.begin( ), key.end( ), key.begin( ), toupper);
//				if(key==keyName)
//					break;
//			}
//#endif

			//#ifdef CASE_SENSITIVE
			//            iter = inimap.find(keyName);
			//#else
			//			// Key is now uppercase
			//			std::transform(keyName.begin( ), keyName.end( ), keyName.begin( ), toupper);
			//
			//			for(iter=inimap.begin(); iter!=inimap.end(); iter++)
			//			{
			//				std::string key = (*iter).first;
			//				std::transform(key.begin( ), key.end( ), key.begin( ), toupper);
			//				if(key==keyname)
			//					break;
			//			}
			//#endif

				bool comp(const std::string& a, const std::string& b)
	{  
		//if(!Config::bCaseSensitive)
		{
			MakeUpper(a);
			MakeUpper(b);
		}
		return a==b;
	};

		//struct string_icompare : public std::unary_function<std::string, bool>
	//{
	//	explicit string_icompare(const std::string &baseline) : baseline(baseline) 
	//	{
	//		MakeUpper(baseline); 
	//	}
	//	bool operator() (const std::string &arg)
	//	{ 
	//		MakeUpper(arg); 
	//		return baseline==arg;
	//	}
	//	std::string baseline;
	//};

	struct string_icompare 
	{
		bool operator()(const std::string& a, const std::string& b) const {
			MakeUpper(a); 
			MakeUpper(b); 
			return a==b;
		}
	};

	  // MSVC::SetPriorityClass(GetCurrentProcess( ), ABOVE_NORMAL_PRIORITY_CLASS);
//	MSVC::SetThreadPriority(GetCurrentThread( ), ABOVE_NORMAL_PRIORITY_CLASS);
