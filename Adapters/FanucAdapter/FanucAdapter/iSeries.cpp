//
// iSeries.cpp
//

#pragma message("Compile iSeries.cpp")

#include "StdAfx.h"
#include "MTCFanucAdapter.h"


// Michaloski
#include "Globals.h"
#include "StdStringFcn.h"

#define FANUCLIBPATH(X)    "C:\\Users\\michalos\\Documents\\Visual Studio 2015\\Projects\\FanucAdapter\\" ## X

#ifdef iSERIESLAN

#pragma comment(lib, FANUCLIBPATH("Distribution\\30iHSSB\\Fwlib32.lib"))

#elif defined( iSERIESHSSB )
#ifdef i160
// no discernible difference to iseries?
#pragma comment(lib, FANUCLIBPATH("Distribution/160iHSSB/Fwlib32.lib"))
#else
#pragma comment(lib, FANUCLIBPATH("Distribution\\30iHSSB\\Fwlib32.lib"))
#endif
#endif


// This is here so the header file include can be cpp file specific
static ODBAXISNAME axisname[MAX_AXIS];


static double ComputeValue(long data, short dec)
{
	return ((double) data ) / pow( 10.0, (double) dec); 
}
CiSeries::CiSeries(FanucShdrAdapter* adapter): _adapter(adapter)
{
	mLag=0;
}

CiSeries::~CiSeries(void)
{
}

int CiSeries::getAlarmInfo()
{

	//char *almmsg[] = {
	//	"P/S 100 ALARM","P/S 000 ALARM",
	//	"P/S 101 ALARM","P/S ALARM (1-255)",
	//	"OT ALARM",     "OH ALARM",
	//	"SERVO ALARM",  "unknown ALARM",
	//	"APC ALARM",    "SPINDLE ALARM",
	//	"P/S ALARM (5000-)"
	//} ;
	
	char *almmsg[] = {
		"P/S alarm",
		"Overtravel alarm",
		"Servo alarm",
		"(Not used)",
		"Overheat alarm",
		"Stroke limit - 1",
		"Stroke limit - 2",
		"Edit alarm",
		"APC alarm",
		"(Not used)",
		"P/S 5000 ... alarm",
		"(Not used)",
		"(Not used)",
		"(Not used)",
		"External alarm",
		"Reverse control alarm"};

	short ret=0;
	ODBALM buf ;
	unsigned short idx ;
	ret=cnc_alarm( _adapter->mFlibhndl, &buf ) ;
	if (ret != EW_OK)
	{
		return 	LogErrorMessage("iSeries::getAlarmInfo() cnc_alarm FAILED\n", ret );
	}

	if ( buf.data == 0 )
	{
			_adapter->SetMTCTagValue("fault", "");
	}
	else
	{
		std::string sAlarm;
		for ( idx = 0 ; idx < 11 ; idx++ ) 
		{
			if ( buf.data & 0x0001 )
				sAlarm+= almmsg[idx]  ;
			buf.data >>= 1 ;
		}
		_adapter->SetMTCTagValue("fault", sAlarm);
	}
	return 0;
}
int CiSeries::getInfo()
{
	short ret=0;
	ODBSYS sysinfo;
	if (EW_OK != ::cnc_sysinfo(_adapter->mFlibhndl, &sysinfo))
	{
		 gLogger->warning(StdStringFormat("Error: iSeries::connect cnc_sysinfo FAILED %d", ret).c_str());
		 return E_FAIL;
	}

	_adapter->_nAxes=atoi(sysinfo.axes);
	gLogger->info(StdStringFormat("iSeries::connect axes number %d\n", _adapter->_nAxes ).c_str());

	if(_adapter->_nAxes >= MAX_AXIS)
	{
		gLogger->warning(StdStringFormat("iSeries::connect axes number %d > MAX_AXIS OF HEADER!!!", _adapter->_nAxes ).c_str());
		_adapter->disconnect();
	}

	if(_adapter->_nAxes< 3)
		_adapter->_nAxes=3;

	short n=_adapter->_nAxes;
	ret = ::cnc_rdaxisname(_adapter->mFlibhndl, &n, axisname);
	if (EW_OK != ret)
	{
		 	gLogger->warning(StdStringFormat("iSeries::connect cnc_rdaxisname FAILED %d", ret ).c_str());
			return E_FAIL;
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

	gLogger->debug(StdStringFormat("Axis # %d", _adapter->_nAxes).c_str());
	for(int i=0; i< n; i++)
	{
		gLogger->info(StdStringFormat("\t%d : %c pos = %d", i, axisname[i].name, _adapter->axisnum[axisname[i].name]).c_str());
	}
	return EW_OK;
}


int CiSeries::getLine()
{
	gLogger->debug("iSeries::getLine Enter\n");
	std::string prognum,progname;

	short ret;
	if (!_adapter->mConnected)
		return -1;

	gLogger->debug("iSeries::getLine\n");
	ODBSEQ block;
	ret = cnc_rdseqnum(_adapter->mFlibhndl, &block ); // 15,16,18,21,0,powermate
	if (ret == EW_OK)
	{
		_adapter->SetMTCTagValue("line", StdStringFormat("%d", block.data));
	}
	else
	{
		gLogger->info(StdStringFormat("Error iSeries::cnc_rdseqnum=%d", ret).c_str());
	}

	ODBPRO buf ;
	ret = cnc_rdprgnum( _adapter->mFlibhndl, &buf ) ; // 15,16,18,21,0,powermate
	if (ret == EW_OK)
	{
		//prognum=StdStringFormat("O%04hd", buf.data); // mdata -  Main program number. data- Running program number.
		prognum=StdStringFormat("O%04d", buf.data); // mdata -  Main program number. data- Running program number.
		gLogger->info(StdStringFormat("iSeries::cnc_rdprgnum prognum = %d", buf.mdata).c_str());
		gLogger->info(StdStringFormat("iSeries::cnc_rdprgnum prognum = %d",(int) (buf.mdata&0xFFFF)).c_str());
		gLogger->info(StdStringFormat("iSeries::cnc_rdprgnum prognum data = %d",(int) (buf.data&0xFFFF)).c_str());
	}
	else
	{
		gLogger->info(StdStringFormat("Error CiSeries::cnc_rdprgnum=%d", ret).c_str());
		LogErrorMessage(" CiSeries::cnc_rdprgnum FAILED\n", ret );
	}
#if 0
	if(1) // Globals.ProgramLogic == "FirstComment")
	{

		short i, blk;
		unsigned short num;
		short top = 0;
		char prg[BUFSIZE+1];
		std::string sLine;
		//do {
			num = BUFSIZE;

			ret = cnc_rdexecprog(_adapter->mFlibhndl,&num, &blk, prg);

			if ( ret == EW_LENGTH  ) 
			{
				LogErrorMessage(" iSeries::cnc_rdexecprog FAILED Length Error", ret );
				goto error;
			}

			if ( ret ) 
			{
				LogErrorMessage("iSeries::cnc_rdexecprog NOSPECIFIC ERROR", ret );
				goto error;
			}

			// Find first comment  (  ) SHOULD BE ON FIRST LINE
			// Extract comment, if not null assign to progname, which is output as program to MTConnect
			{
				//sLine = prg[i].comment;
				sLine = prg; // [i].comment;
				int n = sLine.find("(");
				int m= sLine.find(")");

				if(std::string::npos ==n && std::string::npos ==m)
				{
					LOGONCE GLogger.Fatal(StdStringFormat("Didn't find comment %s\n",sLine.c_str()));
					goto error;
				}

				sLine= sLine.substr(n+1,m-n-1);

				if(!sLine.empty() && sLine != "()")
				{
					// GLogger.Fatal(StdStringFormat("Program Name = %s\n",sLine.c_str()));
					progname=sLine;
				}
			}
	}
#endif
error:
	if(!progname.empty())
	{
		_adapter->SetMTCTagValue("program", progname);
	}
	else if(!prognum.empty())
	{
		_adapter->SetMTCTagValue("program", prognum);
	}
	else
	{
		_adapter->SetMTCTagValue("program", "O1");
	}
	gLogger->debug("iSeries::getLine Leave\n");
	return EW_OK;
}

int CiSeries::getPositions()
{
	if (!_adapter->mConnected)
		return -1;

	gLogger->debug("iSeries::getPositions Enter\n");

	//ODBPOS *PosData = static_cast<ODBPOS *>(alloca(_nAxes * sizeof(ODBPOS)));

	/* type.
	0	:	absolute position
	1	:	machine position
	2	:	relative position
	3	:	distance to go
	-1	:	all type
	*/
	short data_num = MAX_AXIS;
	short  type = -1;
	ODBPOS PosData[MAX_AXIS]; //Position Data
	short ret = cnc_rdposition(_adapter->mFlibhndl, type, &data_num, &PosData[0]); // 15,16,18,21,0,powermate
	if (ret == EW_OK)
	{
		// Fanuc ABC units are in degrees
		try {
			for(int i=0; i< data_num; i++)
			{
				gLogger->info(StdStringFormat( "Axis %c%c%c%c Data=%d Dec=%d\n" ,  PosData[i].abs.name,(int)PosData[i].abs.data,(int)PosData[i].abs.dec ).c_str());
				if(::toupper(PosData[i].abs.name) == 'X' && i==0)
					_adapter->SetMTCTagValue("Xabs", StdStringFormat("%8.4f", ComputeValue( PosData[i].abs.data , PosData[i].abs.dec)));
				if(::toupper(PosData[i].abs.name) == 'Y')
					_adapter->SetMTCTagValue("Yabs", StdStringFormat("%8.4f", ComputeValue( PosData[i].abs.data , PosData[i].abs.dec)));
				if(::toupper(PosData[i].abs.name) == 'Z')
					_adapter->SetMTCTagValue("Zabs", StdStringFormat("%8.4f", ComputeValue( PosData[i].abs.data , PosData[i].abs.dec)));
				if(::toupper(PosData[i].abs.name) == 'A')
					_adapter->SetMTCTagValue("Aabs", StdStringFormat("%8.4f", ComputeValue( PosData[i].abs.data , PosData[i].abs.dec)));
				if(::toupper(PosData[i].abs.name) == 'B')
					_adapter->SetMTCTagValue("Babs", StdStringFormat("%8.4f", ComputeValue( PosData[i].abs.data , PosData[i].abs.dec)));
				if(::toupper(PosData[i].abs.name) == 'C')
					_adapter->SetMTCTagValue("Cabs", StdStringFormat("%8.4f", ComputeValue( PosData[i].abs.data , PosData[i].abs.dec)));
				if(::toupper(PosData[i].abs.name) == 'U')
					_adapter->SetMTCTagValue("Uabs", StdStringFormat("%8.4f", ComputeValue( PosData[i].abs.data , PosData[i].abs.dec)));
				if(::toupper(PosData[i].abs.name) == 'V')
					_adapter->SetMTCTagValue("Vabs", StdStringFormat("%8.4f", ComputeValue( PosData[i].abs.data , PosData[i].abs.dec)));
			}
		}
		catch(...)
		{
			gLogger->info("CiSeries::getPositions Exception\n");
		}

		//GLogger.LogMessage(StdStringFormat("CiSeries::getPositions %.3f %.3f %.3f\n", 
		//	_adapter->mXact.getValue(), _adapter->mYact.getValue(), _adapter->mZact.getValue()) , GLogger.HEAVYDEBUG);
	}
	else
	{
		_adapter->disconnect();
		return -1;
	}
	gLogger->debug("iSeries::getPositions Done\n");
	return EW_OK;
}

int CiSeries::getToolInfo()
{
	IODBTLCTL  toolinfo;
	short	ret;
#ifdef TOOLING
	if(Globals.nToolingFlag)
	{
		ret = cnc_rdtlctldata(_adapter->mFlibhndl, &toolinfo); 
		if(ret) 
		{
			LOGONCE gLogger->info(StdStringFormat("CiSeries::cnc_rdsvmeter Failed\n").c_str());
			return EW_OK;
		}
		if(toolinfo.slct&1)
			_adapter->SetMTCTagValue("toolid", StdStringFormat("%d", toolinfo.used_tool));
		else
			_adapter->SetMTCTagValue("toolid", "");
	}
#endif
	return EW_OK;
}


int CiSeries::getLoads()
{
#ifdef LOADS
	short	ret;
	if(Globals.nAxesLoadFlag)
	{
		ODBSVLOAD sv[MAX_AXIS];
		short num = _adapter->_nAxes;
		GLogger.LogMessage("iSeries::get Loads Enter\n", HEAVYDEBUG);
		try {
			ret = cnc_rdsvmeter(_adapter->mFlibhndl, &num, sv); // 15,16,18,21,0,powermate
			if(ret) 
			{
				LOGONCE GLogger.Fatal(StdStringFormat("CiSeries::cnc_rdsvmeter Failed\n"));
				return EW_OK;
			}

			if(num>0)
				_adapter->SetMTCTagValue("Xload",StdStringFormat("%8.4f", ComputeValue(sv[0].svload.data, sv[0].svload.dec)));
			if(num>1) 
				_adapter->SetMTCTagValue("Yload",StdStringFormat("%8.4f", ComputeValue(sv[1].svload.data, sv[1].svload.dec)));
			if(num>2)
				_adapter->SetMTCTagValue("Zload",StdStringFormat("%8.4f", ComputeValue(sv[2].svload.data, sv[2].svload.dec)));

			for(int i=3; i< _adapter->_nAxes; i++)
			{
				if(::toupper(sv[i].svload.name) == 'A')
					_adapter->SetMTCTagValue("Aload",StdStringFormat("%8.4f", ComputeValue(sv[i].svload.data, sv[i].svload.dec)));
				if(::toupper(sv[i].svload.name) == 'B')
					_adapter->SetMTCTagValue("Bload",StdStringFormat("%8.4f", ComputeValue(sv[i].svload.data, sv[i].svload.dec)));
				if(::toupper(sv[i].svload.name) == 'C')
					_adapter->SetMTCTagValue("Cload",StdStringFormat("%8.4f", ComputeValue(sv[i].svload.data, sv[i].svload.dec)));
			}
		}
	}
	catch(...)
	{


	}
#endif
	gLogger->debug("CiSeries::get Loads leave\n");
	return EW_OK;
}


int CiSeries::getStatus()
{
	gLogger->debug("iSeries::getStatus Enter\n");

	if (!_adapter->mConnected)
		return -1;

	short ret;
	IODBSGNL sgnl;

	/// Difference 15i versus 16/18/.. i series
	if (EW_OK == (ret=cnc_rdopnlsgnl(_adapter->mFlibhndl, 0xFFFF, &sgnl)))
	{
		std::string sFovr = StdStringFormat("%d", (int) sgnl.feed_ovrd );

		// Broken?
		//_adapter->SetMTCTagValue("path_feedrateovr", sFovr);
		_adapter->SetMTCTagValue("path_feedrateovr", "100");
			_adapter->SetMTCTagValue("Sovr", "100");
	}
	else
	{
		_adapter->SetMTCTagValue("Sovr", "100");
		_adapter->SetMTCTagValue("path_feedrateovr", "100");
		gLogger->warning(StdStringFormat("Error: iSeries::cnc_rdopnlsgnl=%d\n", ret).c_str());
		LogErrorMessage("Error: iSeries::cnc_rdopnlsgnl", ret);
	}


	//this one may fail
	// bit 6	:	Spindle override signal (only Series 15)
	//if(EW_OK == cnc_rdopnlsgnl(_adapter->mFlibhndl, 0x40, &sgnl)) 
	//	_adapter->SetMTCTagValue("Sovr", StdStringFormat("%8.4f", sgnl.spdl_ovrd * 10.0));


	ODBST status;
	ret = cnc_statinfo(_adapter->mFlibhndl, &status);
	if (ret == EW_OK)
	{
		gLogger->info(StdStringFormat("Controller mode iSeries::cnc_statinfo=%d\n", status.aut).c_str());
		// Series 16/18/21/0/Power Mate 
		if (status.aut == 0) // MDI for aut
		_adapter->SetMTCTagValue("controllermode", "MANUAL");
		else if (status.aut != 2) // Other than no selection or MDI
			_adapter->SetMTCTagValue("controllermode", "AUTOMATIC");
		else
			_adapter->SetMTCTagValue("controllermode", "MANUAL");

		/** run 
		Status of automatic operation 0  :  ****(reset)  
		1  :  STOP  
		2  :  HOLD  
		3  :  STaRT  
		4  :  MSTR(during retraction and re-positioning of tool retraction and recovery, and operation of JOG MDI)  
		*/
		gLogger->debug(StdStringFormat("Execution mode iSeries::cnc_statinfo=%d\n", status.run).c_str());

		if(status.run == 0 )
			_adapter->SetMTCTagValue("execution", "IDLE");
		else if (status.run == 1 || status.run == 2) //  STOP or HOLD is ePAUSED
			_adapter->SetMTCTagValue("execution", "PAUSED");
		else if (status.run >2) // STaRT
			_adapter->SetMTCTagValue("execution", "EXECUTING");
		//else if (status.run ==4) 
		//	_adapter->SetMTCTagValue("execution", "PAUSED");
		//else
		//	_adapter->SetMTCTagValue("execution", "PAUSED");
	}
	else
	{
		_adapter->disconnect();
	}
	gLogger->debug("iSeries::getStatus Leave\n");
	return EW_OK;
}
void CiSeries::saveStateInfo()
{
	lastFeed=_adapter->GetMTCTagValue("path_feedratefrt"); 
	lastX=_adapter->GetMTCTagValue("Xabs"); 
	lastY=_adapter->GetMTCTagValue("Yabs"); 
	lastZ=_adapter->GetMTCTagValue("Zabs"); 
	lastA=_adapter->GetMTCTagValue("Aabs"); 
	lastB=_adapter->GetMTCTagValue("Babs"); 
}
int CiSeries::getSpeeds()
{
	gLogger->debug("iSeries::getSpeeds enter\n");
	if (!_adapter->mConnected)
		return -1;

	double spindlespeed=0, feedrate=0;


	ODBSPEED speed;
	/* Data type. 0 ( feed rate ), 1 ( spindle speed  ) , -1 (  all  ) */
	short ret = cnc_rdspeed(_adapter->mFlibhndl, -1, &speed);
	if (ret == EW_OK)
	{
		spindlespeed=speed.acts.data;
		if(speed.acts.dec>0)
			 spindlespeed=spindlespeed / pow( 10.0, (double) speed.acts.dec); 

		feedrate=speed.actf.data;
		if(speed.actf.dec>0)
			 feedrate=feedrate / pow( 10.0, (double) speed.actf.dec); 

	}
	else // Error
	{
		gLogger->warning(StdStringFormat("Error: iSeries::getSpeeds=%d\n", ret).c_str());
		LogErrorMessage("Error: iSeries::cnc_rdspeed", ret);

	}
	if(spindlespeed==0)
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
			spindlespeed = 99.0; // _adapter->SetMTCTagValue("Srpm","99.0");
		}
		else
		{
			spindlespeed = 0; // _adapter->SetMTCTagValue("Srpm","0");
		}
	}


	_adapter->SetMTCTagValue("path_feedratefrt", StdStringFormat("%8.4f", feedrate));
	_adapter->SetMTCTagValue("Srpm", StdStringFormat("%8.4f", spindlespeed));


	gLogger->debug("iSeries::getSpeeds done\n");
	gLogger->debug("iSeries::getSpindleLoad\n");
#if 0

	short nspd;
	ret = cnc_rdnspdl(_adapter->mFlibhndl, &nspd);
	if (ret != EW_OK)
	{
		LOGONCE GLogger.Fatal("iSeries::cnc_rdnspdl Failed\n");
		return -1;
	}


	ODBSPN *load = static_cast<ODBSPN *>(alloca(nspd * sizeof(ODBSPN)));
	//ODBSPLOAD load[MAX_SPINDLES];   /* 4 = maximum spinlde number */
	ret = cnc_rdspload (_adapter->mFlibhndl, -1, load);
	if (ret != EW_OK)
	{
		LOGONCE gLogger->error(StdStringFormat("iSeries::cnc_rdspload Failed\n").c_str());
		return ret;
	}
	 _adapter->SetMTCTagValue("Sload", StdStringFormat("%8.4f", load[0].data[0]));
#else
	 _adapter->SetMTCTagValue("Sload", "0");
#endif
	 gLogger->debug("iSeries::getLoads Leave\n");
	return EW_OK;
}

int CiSeries::getVersion()
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
	gLogger->info("FanucMTConnectAdapter::getVersion Enter\n");
	ODBSYS sysinfo;
	short ret = cnc_sysinfo(_adapter->mFlibhndl, &sysinfo); // 15,16,18,21,0,powermate
	if (ret == EW_OK)
	{
		std::string versioninfo =StdStringFormat("CNC=%c%c Series=%c%c%c%c Version=%c%c%c%c ", sysinfo.cnc_type[0], sysinfo.cnc_type[1],
			sysinfo.series[0],sysinfo.series[1],sysinfo.series[2],sysinfo.series[3],
			sysinfo.version[0],sysinfo.version[1],sysinfo.version[2],sysinfo.version[3]
		);
		gLogger->warning(versioninfo.c_str());
	}
	else
	{
		gLogger->error(StdStringFormat("Error: FanucMTConnectAdapter::cnc_sysinfo=%d\n", ret).c_str());
	}
	return EW_OK;
}
void CiSeries::disconnect()
{
	LOGONCE gLogger->debug(StdStringFormat("iSeries::disconnect Using Protocol=%s\n", Globals.FanucProtocol.c_str()).c_str());
	_adapter->SetMTCTagValue("Sload","OFF");
	if (_adapter->mConnected)
	{
		//_adapter->mPower.setValue(PowerState::eOFF);
		if(_adapter->mFlibhndl)
			cnc_freelibhndl(_adapter->mFlibhndl);  
		_adapter->mConnected = false;
		_adapter->mFlibhndl=NULL;
	}
}

int CiSeries::connect()
{
	LOGONCE  gLogger->debug(StdStringFormat("iSeries::connect Using Protocol=%s\n", Globals.FanucProtocol.c_str()).c_str());

	short ret;
	_adapter->mFlibhndl=0;
	if(Globals.FanucProtocol == "HSSB")
	{
		gLogger->error(StdStringFormat("iSeries::connect HSSB cnc_allclibhndl2 Port=%d\n", _adapter->mDevicePort ).c_str());
		//GLogger << FATAL  << "THIS EXECUTABLE DOES NOT HANDLE HSSB - EXCLUSIVELY Focas w/ Processing library for TCP/IP\n";
#ifdef iSERIESHSSB
		ret =  ::cnc_allclibhndl2(_adapter->mDevicePort,&_adapter->mFlibhndl);
#else
		gLogger->error( "THIS EXECUTABLE DOES NOT HANDLE LAN - EXCLUSIVELY Focas w/ Processing library for HSSB\n" );
		return -1;
#endif		
	}
	else
	{
		
		gLogger->error(StdStringFormat("iSeries::connect LAN cnc_allclibhndl3 IP=%s  Port=%d\n",Globals.FanucIpAddress.c_str(), _adapter->mDevicePort ).c_str());
#ifdef iSERIESLAN
		try{
		ret = ::cnc_allclibhndl3(Globals.FanucIpAddress.c_str(), _adapter->mDevicePort, 10, &_adapter->mFlibhndl);
		}
		catch(...)
		{
			gLogger->error(StdStringFormat("iSeries::connect LAN cnc_allclibhndl3 Exception\n").c_str());
		}
#else
		GLogger.Fatal( "THIS EXECUTABLE DOES NOT HANDLE LAN - EXCLUSIVELY Focas w/ Processing library for HSSB\n");
		return -1;
#endif
	}

	gLogger->error(StdStringFormat("iSeries::connect cnc_allclibhndl Result:%x\n " , ret ).c_str());
	if (ret != EW_OK) 
	{
		LogErrorMessage("iSeries::connect cnc_allclibhndl3 failed ",ret);
		_adapter->SetMTCTagValue("avail","UNAVAILABLE");
		_adapter->SetMTCTagValue("power","OFF");
		_adapter->mConnected = false;
		LOGONCE  gLogger->error(StdStringFormat("iSeries::connect FAILED Power = OFF\n", ret ).c_str());
		return -1;
	}

	gLogger->debug(StdStringFormat("iSeries::connect connected ON\n", ret ).c_str());
	_adapter->mConnected = true;
	_adapter->SetMTCTagValue("power","ON");
	_adapter->SetMTCTagValue("avail","AVAILABLE");
	return 0;
}
int CiSeries::LogErrorMessage(std::string msg, short errcode)
{
	if(errcode == EW_OK)
		return 0;
	std::stringstream str;
	str<< msg  + " Error on machine "<< Globals.FanucIpAddress << " : ";
	switch(errcode)
	{
	case  EW_PROTOCOL: str<< "EW_PROTOCOL - protocol error";break; 
	case EW_SOCKET: str<< "EW_SOCKET - Windows socket error ";break; 
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
	case EW_ALARM: str<< "EW_ALARM - alarm has been occurred"; break;  //15 
	case EW_STOP: str<< "EW_STOP -CNC is not running";  break; // 16 
	case EW_PASSWD: str<< "EW_PASSWD -  protection data error ";  break; // 17   
	default: str<< "Fanuc Error not identifed"; break;
	}
	str<< std::endl;
	//GLogger.LogMessage(str.str().c_str(), INFO);
	LOGONCE  gLogger->error(str.str().c_str());
	//_adapter->SetMTCTagValue("status", str.str() );
	_adapter->errmsg= str.str() ;

	return -1;
}
