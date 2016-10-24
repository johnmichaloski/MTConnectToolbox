//
// F15B.cpp
//

#pragma message("Compile F15B.cpp")

#pragma comment(lib, "./Distribution/15bHSSB/Fwlib32.lib")

#include "StdAfx.h"
#include "MTCFanucAgent.h"
#include "CmdHandler.h"

#define FS15D
#include "150\fwlib32.h"
//#include "F15B.h"
//#include "Fanuc_adapter.h"

// Michaloski
#include "GLogger.h"
#include "StdStringFcn.h"

// This is here so the header file include can be cpp file specific
//static ODBAXISNAME axisname[MAX_AXIS];


CF15B::CF15B(CCmdHandler* adapter): _adapter(adapter)
{
	mLag=0;
}

CF15B::~CF15B(void)
{
}
void CF15B::saveStateInfo()
{
	lastFeed=_adapter->GetMTCTagValue("path_feedratefrt"); 
	lastX=_adapter->GetMTCTagValue("Xabs"); 
	lastY=_adapter->GetMTCTagValue("Yabs"); 
	lastZ=_adapter->GetMTCTagValue("Zabs"); 
	lastA=_adapter->GetMTCTagValue("Aabs"); 
	lastB=_adapter->GetMTCTagValue("Babs"); 
}

int CF15B::getInfo()
{
	short ret;
	ODBSYS sysinfo;
	ret = ::cnc_sysinfo(_adapter->mFlibhndl, &sysinfo);

	if( EW_OK != ret)
		return LogErrorMessage("CF15B::connect cnc_sysinfo", ret);

	_adapter->_nAxes=atoi(sysinfo.axes);
	GLogger.LogMessage(StdStringFormat("CF15B::connect axes number %d\n", _adapter->_nAxes ), HEAVYDEBUG);

	if(_adapter->_nAxes >= MAX_AXIS)
	{
		GLogger.Warning(StdStringFormat("CF15B::connect axes number %d > MAX_AXIS OF HEADER!!!\n", _adapter->_nAxes ));
		disconnect();
	}

	if(_adapter->_nAxes< 3)
		_adapter->_nAxes=3;

	short n=_adapter->_nAxes;
	// THIS FUNCTIONALITY IS NOT AVAILABLE IN 15M
#if 0 
	ret = ::cnc_rdaxisname(_adapter->mFlibhndl, &n, axisname);
	if (EW_OK != ret)
	{
		CiSeries("CF15B::cnc_rdaxisname", ret);
		return 	GLogger.Warning(StdStringFormat("CF15B::connect cnc_rdaxisname FAILED %d\n", ret ));
	}

	for(int i=3; i< n; i++)
	{
		_adapter->axisnum[axisname[i].name] = i;
		if(::toupper(axisname[i].name) < 'A' ||  ::toupper(axisname[i].name) > 'Z' )
			break;
	}
	_adapter->_nAxes=n;

	// Reads the number of spindles which CNC can control. It is not the number of mounted spindles. 
	// FWLIBAPI short WINAPI cnc_rdnspdl(unsigned short FlibHndl, short *spdl);

	GLogger.LogMessage(StdStringFormat("Axis # %d\n", _adapter->_nAxes) , 3);
	for(int i=0; i< n; i++)
	{
		GLogger.LogMessage(StdStringFormat("\t%d : %c pos = %d\n", i, axisname[i].name, _adapter->axisnum[axisname[i].name]), Logger.HEAVYDEBUG);
	}
#endif
	return EW_OK;
}


int CF15B::getLine()
{
	GLogger.LogMessage("CF15B::getLine Enter\n", 3);
	short ret;
	if (!_adapter->mConnected)
		return -1;

	GLogger.LogMessage("CF15B::getLine\n", HEAVYDEBUG);
	ODBSEQ block;
	ret = cnc_rdseqnum(_adapter->mFlibhndl, &block ); // 15,16,18,21,0,powermate
	if (ret == EW_OK)
	{
		_adapter->SetMTCTagValue("block",StdStringFormat("%d", (long) block.data));
	}
	else
	{
		LogErrorMessage("CF15B::cnc_rdseqnum", ret);
	}
	std::string prognum,progname;
	//ODBPROO8 buf;
	//ret =cnc_rdprgnumo8( _adapter->mFlibhndl, &buf ) ;

	ODBPRO buf ;
	ret = cnc_rdprgnum( _adapter->mFlibhndl, &buf ) ; // 15,16,18,21,0,powermate
	if (ret == EW_OK)
	{
		prognum=StdStringFormat("O%04hd", buf.data); // mdata -  Main program number. data- Running program number.
		GLogger.Error(StdStringFormat("CF15B::cnc_rdprgnum prognum = %d\n",prognum));
	}
	else
	{
		LogErrorMessage("CF15B::cnc_rdprgnum", ret);
	}

#if 0
	if (ret == EW_OK)
		mProgram.setValue((LPCTSTR) ::StdStringFormat("%d", buf.mdata));
#else
#define MAX_PROG_SIZE (1024*2)
	// see if new program - using sequence number or change in program #
	//if(nLastBlockNum > mLine.getValue())  // DOES NOT WORK
	{
		unsigned short length=MAX_PROG_SIZE;
		short blknum;
		static char data[MAX_PROG_SIZE+1];
		ret = cnc_rdexecprog(_adapter->mFlibhndl, &length, &blknum, data);  // 15,16,18,21,0,powermate
		if (ret != EW_OK)
		{
			LogErrorMessage("CF15B::cnc_rdexecprog", ret);
		}
		if (ret == EW_OK)
		{
			GLogger.LogMessage(StdStringFormat("CF15B::cnc_rdexecprog blck counter %d\n", (int) blknum).c_str(), 3);
			GLogger.LogMessage(StdStringFormat("CF15B::cnc_rdexecprog length %d\n", (int) length).c_str(), 3);
			GLogger.LogMessage("CF15B::cnc_rdexecprog Search for O# comment\n", 3);
			GLogger.Info(data);
			// issue here - main program versus subprogram
			progname=_adapter->getProgramName(data);
			GLogger.Info(StdStringFormat("CF15B::cnc_rdexecprog getProgramName Found  %s\n", progname.c_str()));
		}
#if 1
		if(!progname.empty())
		{
			_adapter->SetMTCTagValue("program",progname);
		}
		else if(!prognum.empty())
		{
			_adapter->SetMTCTagValue("program",prognum);
		}
		else
		{
			_adapter->SetMTCTagValue("program","O1");
		}
#else
		_adapter->mProgram.setValue("O0001");

#endif

		
	}
	lastprognum=prognum;
	GLogger.LogMessage("CF15B::getLine Leave\n", 3);
#endif
	return EW_OK;
}

int CF15B::getPositions()
{
	short ret;
	if (!_adapter->mConnected)
		return -1;

	GLogger.LogMessage("CF15B::getPositions Enter\n", HEAVYDEBUG);

	//ODBPOS *PosData = static_cast<ODBPOS *>(alloca(_nAxes * sizeof(ODBPOS)));

	/* type.
	0	:	absolute position
	1	:	machine position
	2	:	relative position
	3	:	distance to go
	-1	:	all type
	*/
	short data_num = MAX_AXIS; // MAX_AXIS different for 15

#if 0
	ODBACT fbuf ;
	ret = cnc_acts( _adapter->mFlibhndl,&fbuf ) ;
	if (ret != EW_OK)
	{
		LogErrorMessage("CF15B::cnc_actf", ret);
	}
	else
	{
		GLogger.LogMessage(StdStringFormat("CF15B::cnc_actf %ld\n",  fbuf.data).c_str(), 3);
		_adapter->mPathFeedrate.setValue( ((double) fbuf.data) / 10.0 );

	}


	ODBDYO8 obdy08;
	ret = cnc_rddynamico8(_adapter->mFlibhndl,-1,sizeof(obdy08), &obdy08);
	//ODBSPEED speed;
	//ret = cnc_rdspeed(_adapter->mFlibhndl,1,&speed);
	if (ret != EW_OK)
	{
		LogErrorMessage("CF15B::cnc_rddynamico8", ret);
	}
	else
	{
		GLogger.LogMessage(StdStringFormat("CF15B::cnc_acts %ld\n", obdy08.acts ).c_str(), 3);
		_adapter->mSpindleSpeed.setValue(obdy08.acts);
	}	

	ODBACT sbuf ;
	ret=cnc_acts( _adapter->mFlibhndl, &sbuf ) ;
	if (ret != EW_OK)
	{
		LogErrorMessage("CF15B::cnc_acts", ret);
	}
	else
	{
		GLogger.LogMessage(StdStringFormat("CF15B::cnc_acts %ld\n",  sbuf.data).c_str(), 3);
		_adapter->mSpindleSpeed.setValue(sbuf.data);
	}	
#endif

	struct odbdy buf ;
	ret=cnc_rddynamic( _adapter->mFlibhndl, -1, sizeof(buf), &buf ) ;
	if (ret != EW_OK)
	{
		LogErrorMessage("CF15B::cnc_rddynamic", ret);
	}
	else
	{
		/*
		The following program displays 
		1:  120005
		2:  -50119
		3:	80
		while the absolute position data for each axes are
		The 1st axis	120.005
		The 2nd axis	-50.119
		The 3rd axis	  0.080
		in 3-axis system.
		*/
		//printf( "Alarm status = %d\n", buf.alarm ) ;
		_adapter->SetMTCTagValue("path_feedratefrt",StdStringFormat("%8.4f",((double) buf.actf) / 10.0));
		_adapter->SetMTCTagValue("Xabs",StdStringFormat("%8.4f",((double)buf.pos.faxis.absolute[0])/10000.0));
		_adapter->SetMTCTagValue("Yabs",StdStringFormat("%8.4f",((double)buf.pos.faxis.absolute[1])/10000.0));
		_adapter->SetMTCTagValue("Zabs",StdStringFormat("%8.4f",((double)buf.pos.faxis.absolute[2])/10000.0));
		_adapter->SetMTCTagValue("Aabs",StdStringFormat("%8.4f",((double)buf.pos.faxis.absolute[3])/10000.0));
		_adapter->SetMTCTagValue("Babs",StdStringFormat("%8.4f",((double)buf.pos.faxis.absolute[4])/10000.0));

		GLogger.LogMessage(StdStringFormat("CF15B::cnc_actf %ld\n",  buf.actf).c_str(), 3);
		GLogger.LogMessage(StdStringFormat("CF15B::cnc_rddynamic X %ld\n",  buf.pos.faxis.absolute[0]).c_str(), 3);
		GLogger.LogMessage(StdStringFormat("CF15B::cnc_rddynamic Y %ld\n",  buf.pos.faxis.absolute[1]).c_str(), 3);
		GLogger.LogMessage(StdStringFormat("CF15B::cnc_rddynamic Z %ld\n",  buf.pos.faxis.absolute[2]).c_str(), 3);
	}
	GLogger.LogMessage("CF15B::getPositions Leave\n", HEAVYDEBUG);
	return EW_OK;
}

int CF15B::getSpeeds()
{
	std::string feed,X, Y, Z, A, B,mode,execution;
	mode=_adapter->GetMTCTagValue("controllermode"); 
	execution=_adapter->GetMTCTagValue("execution"); 
	feed=_adapter->GetMTCTagValue("path_feedratefrt"); 
	X=_adapter->GetMTCTagValue("Xabs"); 
	Y=_adapter->GetMTCTagValue("Yabs"); 
	Z=_adapter->GetMTCTagValue("Zabs"); 
	A=_adapter->GetMTCTagValue("Aabs"); 
	B=_adapter->GetMTCTagValue("Babs"); 
	
	//GLogger.LogMessage(StdStringFormat("CF15B::getSpeeds feed=%s:%6.2f|X=%6.2f:%6.2f|Y=%6.2f:%6.2f|Z=%6.2f:%6.2f|A=%6.2f:%6.2f|B=%6.2f:%6.2f \n", 
	//	lastFeed, _adapter->mPathFeedrate.getValue(),
	//	lastX,_adapter->mXact.getValue(),
	//	lastY,_adapter->mYact.getValue(),
	//	lastZ,_adapter->mZact.getValue()
	//	lastA,_adapter->mAact.getValue(),
	//	lastB,_adapter->mBact.getValue()
	//	).c_str(), 3);



	// If moving and in auto mode, assume spindle on
	if(	(mode == "AUTOMATIC" ) &&
		(execution ==  "EXECUTING" )&&
		( 
		lastFeed!=feed ||
		lastX!=X ||
		lastY!= Y ||
		lastZ!=Z
		//|| lastA!=A
		//||lastB!=B
		)
		)
		mLag=4; // 3 cycles - 3 seconds;
	else
		mLag--;
	
	if(mLag<0) mLag=0;

	if(mLag>0)
	{
		_adapter->SetMTCTagValue("Srpm","99.0");
	}
	else
	{
		_adapter->SetMTCTagValue("Srpm","0");
	}
	GLogger.LogMessage(StdStringFormat("CF15B::getSpeeds speed=%s\n",_adapter->GetMTCTagValue("Srpm").c_str()),3);
	return EW_OK;
}
#pragma message("Compiling " __FILE__ )
int CF15B::getStatus()
{
	short ret;
	GLogger.LogMessage("CF15B::getStatus Enter\n", 3);

	if (!_adapter->mConnected)
		return -1;

#if 0
	IODBSGNL sgnl;

	/// Difference 15i versus 16/18/.. i series
	ret =cnc_rdopnlsgnl(_adapter->mFlibhndl, 0x20, &sgnl);
	if (ret != EW_OK)
	{
		LogErrorMessage("CF15B::cnc_rdopnlsgnl", ret);
	}
	else
	{
		GLogger.LogMessage(StdStringFormat("CF15B::cnc_rdopnlsgnl feed ovrd %d\n",  (int) sgnl.feed_ovrd).c_str(), 3);
		_adapter->mPathFeedrateOvr.setValue(sgnl.feed_ovrd * 10.0);
	}

	if(EW_OK == cnc_rdopnlsgnl(_adapter->mFlibhndl, 0x40, &sgnl)) 
		_adapter->mSpindleSpeedOvr.setValue(sgnl.spdl_ovrd * 10.0);
#endif

	ODBST status;
	ret = cnc_statinfo(_adapter->mFlibhndl, &status);
	if (ret == EW_OK)
	{
		GLogger.LogMessage(StdStringFormat("CF15B::cnc_statinfo status.aut %d\n",  (int) status.aut).c_str(), 3);
		GLogger.LogMessage(StdStringFormat("CF15B::cnc_statinfo status.run %d\n",  (int) status.run).c_str(), 3);
		/*
		AUTOMATIC mode selection
		0	:	****(No selection)
		1	:	MDI
		2	:	TAPE
		3	:	MEMory  <= auto mode
		4	:	EDIT
		5	:	TeacH IN
		*/
		if (status.aut == 1) // MDI for aut
			_adapter->SetMTCTagValue("controllermode","MANUAL_DATA_INPUT");
		else  if (status.aut == 5 ) // job
			_adapter->SetMTCTagValue("controllermode","MANUAL");
		else //(status.manual != 0) 
			_adapter->SetMTCTagValue("controllermode","AUTOMATIC");

		/** run 
		Status of automatic operation
		0	:	STOP
		1	:	HOLD
		2	:	STaRT <= RUNNING
		3	:	MSTR(jog mdi)
		4	:	ReSTaRt(not blinking)
		Except under manual mode and under cutter radius compensation outside corner
		5	:	PRSR(program restart)
		6	:	NSRC(sequence number search)
		7	:	ReSTaRt(blinking)
		Under manual mode or under cutter radius compensation outside corner
		8	:	ReSET		*/

		if (status.run == 0 || status.run == 1) //  STOP or HOLD is ePAUSED
			_adapter->SetMTCTagValue("execution","PAUSED");
		else  if (status.run == 8)  // reset
			_adapter->SetMTCTagValue("execution","IDLE");
		else  // STaRT, etc.
			_adapter->SetMTCTagValue("execution","EXECUTING");


		// Alarms are removed
#if 0
		if (status.emergency == 0)
			mAlarm.setValue(Alarm::eESTOP, "EMerGency", Alarm::eCRITICAL, Alarm::eCLEARED, "EMerGency Status Set");
		else
			mAlarm.setValue(Alarm::eESTOP, "EMerGency", Alarm::eCRITICAL, Alarm::eACTIVE, "EMerGency Status Set");

		if (status.alarm == 0)
			mAlarm.setValue(Alarm::eOTHER, "ALarM", Alarm::eERROR, Alarm::eCLEARED, "ALarM Status Set");
		else
			mAlarm.setValue(Alarm::eOTHER, "ALarM", Alarm::eERROR, Alarm::eACTIVE, "ALarM Status Set");
#endif

	}
	//////////????????????????
	//else if (status.warning == 1)
	//  mAlarm.setValue(Alarm::eOTHER, "WaRNing", Alarm::eWARNING, Alarm::eACTIVE, "WaRNing Status Set");

	GLogger.LogMessage("CF15B::getStatus Leave\n", 3);
	return EW_OK;
}
int CF15B::getVersion()
{
	/** cnc_type
	Kind of CNC (ASCII)
	'15'	:	Series 15
	'16'	:	Series 16
	'18'	:	Series 18
	'21'	:	Series 21
	' 0'	:	Series 0
	'PD'	:	Power Mate i-D
	'PH'	:	Power Mate i-H
	*/
	if (!_adapter->mConnected)
		return -1;
	GLogger.LogMessage("FanucMTConnectAdapter::getVersion Enter\n", HEAVYDEBUG);
	ODBSYS sysinfo;
	short ret = cnc_sysinfo(_adapter->mFlibhndl, &sysinfo); // 15,16,18,21,0,powermate
	if (ret == EW_OK)
	{
		std::string versioninfo =StdStringFormat("CNC=%c%c Series=%c%c%c%c Version=%c%c%c%c ", sysinfo.cnc_type[0], sysinfo.cnc_type[1],
			sysinfo.series[0],sysinfo.series[1],sysinfo.series[2],sysinfo.series[3],
			sysinfo.version[0],sysinfo.version[1],sysinfo.version[2],sysinfo.version[3]
		);
		GLogger.LogMessage(versioninfo.c_str(), FATAL);
#if 0
		if(AutoVersionDetect)
		{
			if(std::string(sysinfo.cnc_type) == "15")
				Globals.FanucVersion = "15i";
			else
				Globals.FanucVersion ="iSeries";
		}
#endif
	}
	else
	{
		LogErrorMessage("FanucMTConnectAdapter::cnc_sysinfo", ret);
	}
	return EW_OK;
}
void CF15B::disconnect()
{
	if (_adapter->mConnected)
	{
		_adapter->SetMTCTagValue("power","OFF");
		cnc_freelibhndl(_adapter->mFlibhndl);  
		_adapter->mConnected = false;
	}
}

int CF15B::connect()
{
	LOGONCE GLogger.Status(StdStringFormat("FanucMTConnectAdapter::connect Using Protocol=%s\n", Globals.FanucProtocol.c_str()));

	short ret;
	_adapter->mFlibhndl=0;
	if(Globals.FanucProtocol == "HSSB")
	{
		ret =  ::cnc_allclibhndl2(_adapter->mDevicePort,&_adapter->mFlibhndl);
	}
	else
	{
			GLogger.LogMessage("Fanuc 15M does not support ethernet, HSSB only\n", HEAVYDEBUG);
		//ret = ::cnc_allclibhndl3(_adapter->mDeviceIP, _adapter->mDevicePort, 60, &_adapter->mFlibhndl);
		return -1;
	}

	GLogger.LogMessage(StdStringFormat("FanucMTConnectAdapter::connect cnc_allclibhndl3 Result: %d\n", ret ), HEAVYDEBUG);
	if (ret != EW_OK) 
	{
		LogErrorMessage("FanucMTConnectAdapter::connect cnc_allclibhndl3",ret);
		_adapter->SetMTCTagValue("power","OFF");
		_adapter->SetMTCTagValue("avail","UNAVAILABLE");
		_adapter->mConnected = false;
		Sleep(500);
		LOGONCE GLogger.Fatal(StdStringFormat("FanucMTConnectAdapter::connect FAILED Power = OFF\n", ret ));
		return -1;
	}

	GLogger.LogMessage(StdStringFormat("FanucMTConnectAdapter::connect connected ON\n", ret ), HEAVYDEBUG);
	_adapter->mConnected = true;
	_adapter->SetMTCTagValue("power","ON");
	_adapter->SetMTCTagValue("avail","AVAILABLE");
	return 0;
}
int CF15B::LogErrorMessage(std::string msg, short errcode)
{
	if(errcode == EW_OK)
		return 0;
	std::stringstream str;
	str<< msg  + " Error : ";
	switch(errcode)
	{
		//case  EW_PROTOCOL: str<< "EW_PROTOCOL - protocol error";break; 
		//	case EW_SOCKET: str<< "EW_SOCKET - Windows socket error ";break; 
	case EW_HANDLE: str<< "EW_HANDLE  - DLL not exist error";break; 
	case EW_VERSION:  str<< "EW_VERSION - CNC/PMC version missmatch";break; 
	case EW_UNEXP: str<< "EW_UNEXP - abnormal error"; break; 
	case EW_NODLL: str<< "EW_NODLL"; break; 
	case EW_BUS: str<< "EW_BUS - bus error"; break; 
	case EW_SYSTEM2: str<< "EW_SYSTEM2 - system error"; break; 
	case EW_HSSB: str<< "EW_HSSB - hssb communication error"; break; 
	case EW_SYSTEM: str<< "EW_SYSTEM"; break; 
	case EW_MMCSYS: str<< "EW_MMCSYS - emm386 or mmcsys install error"; break; 
	case EW_RESET: str<< "EW_RESET - reset or stop occured error"; break; 
	case EW_BUSY: str<< "EW_BUSY - busy error"; break; 
	case EW_PARITY: str<< " EW_PARITY - shared RAM parity error "; break; 
	case EW_FUNC: str<< "EW_FUNC - command prepare error";  ; // 1  
		/*case EW_NOPMC:*/ str<< " or EW_NOPMC - pmc not exist";  break; // 1    
	case EW_LENGTH: str<< "EW_LENGTH - data block length error";  break; // 2   
	case EW_NUMBER: str<< "EW_NUMBER - data number error";    // 3  
		/*case EW_RANGE:*/ str<< " or EW_RANGE -address range error";   break; // 3 
	case EW_ATTRIB: str<< "EW_ATTRIB - data attribute error";     //4 
		/*case EW_TYPE:*/ str<< " or EW_TYPE -data type error";  break;  //4 
	case EW_DATA: str<< "EW_DATA - data error";  break; // 5  
	case EW_NOOPT: str<< "EW_NOOPT - No option The extended driver/library function is necessary.";  break;  //6
	case EW_PROT: str<< "EW_PROT - write protect error";  break;  //7 
	case EW_OVRFLOW: str<< "EW_OVRFLOW - memory overflow error"; break;  //8
	case EW_PARAM: str<< "EW_PARAM - cnc parameter not correct error"; break;  //9 
	case EW_BUFFER: str<< "EW_BUFFER - buffer error";  break;  //10  
	case EW_PATH: str<< "EW_PATH - path error";  break;  //11
	case EW_MODE: str<< "EW_MODE - cnc mode error"; break;  //12  
	case EW_REJECT: str<< "EW_REJECT - execution rejected error";  break;  //13 
	case EW_DTSRVR: str<< "EW_DTSRVR - data server error";  break;  //14  
		//case EW_ALARM: str<< "EW_ALARM - alarm has been occurred"; break;  //15 
		//case EW_STOP: str<< "EW_STOP -CNC is not running";  break; // 16 
		//case EW_PASSWD: str<< "EW_PASSWD -  protection data error ";  break; // 17   
	}
	str<< std::endl;
	GLogger.LogMessage(str.str().c_str(), 3);
	return -1;
}
