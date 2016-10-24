//
// iSeries.cpp
//

#pragma message("Compile iSeries.cpp")

#include "StdAfx.h"
//#include "iSeries.h"
#include "MTCFanucAgent.h"
#include "CmdHandler.h"


// Michaloski
#include "GLogger.h"
//#include "EnumProc.h"
#include "StdStringFcn.h"


#ifdef iSERIESLAN
#if (_MSC_VER >= 1500 && _MSC_VER < 1600)
#pragma comment(lib, "./Distribution/30iHSSB/Fwlib32.lib")
#endif
#endif

#ifdef iSERIESHSSB
#if (_MSC_VER >= 1500 && _MSC_VER < 1600)
#pragma comment(lib, "./Distribution/30iHSSB/Fwlib32.lib")
#endif
#endif


// This is here so the header file include can be cpp file specific
static ODBAXISNAME axisname[MAX_AXIS];


static double ComputeValue(long data, short dec)
{
	return ((double) data ) / pow( 10.0, (double) dec); 
}
CiSeries::CiSeries(CCmdHandler* adapter): _adapter(adapter)
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
			_adapter->SetMTCTagValue("alarm", "");
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
		_adapter->SetMTCTagValue("alarm", sAlarm);
	}
	return 0;
}
int CiSeries::getInfo()
{
	short ret=0;
	ODBSYS sysinfo;
	if( EW_OK != ::cnc_sysinfo(_adapter->mFlibhndl, &sysinfo))
		return GLogger.Warning(StdStringFormat("Error: iSeries::connect cnc_sysinfo FAILED %d\n", ret));

	_adapter->_nAxes=atoi(sysinfo.axes);
	GLogger.Info(StdStringFormat("iSeries::connect axes number %d\n", _adapter->_nAxes ));

	if(_adapter->_nAxes >= MAX_AXIS)
	{
		GLogger.Warning(StdStringFormat("iSeries::connect axes number %d > MAX_AXIS OF HEADER!!!\n", _adapter->_nAxes ));
		_adapter->disconnect();
	}

	if(_adapter->_nAxes< 3)
		_adapter->_nAxes=3;

	short n=_adapter->_nAxes;
	ret = ::cnc_rdaxisname(_adapter->mFlibhndl, &n, axisname);
	if (EW_OK != ret)
	{
		return 	GLogger.Warning(StdStringFormat("iSeries::connect cnc_rdaxisname FAILED %d\n", ret ));
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
		GLogger.Info(StdStringFormat("\t%d : %c pos = %d\n", i, axisname[i].name, _adapter->axisnum[axisname[i].name]));
	}
	return EW_OK;
}


int CiSeries::getLine()
{
	GLogger.LogMessage("iSeries::getLine Enter\n", 3);
	short ret;
	if (!_adapter->mConnected)
		return -1;

	GLogger.Info("iSeries::getLine\n");
	ODBSEQ block;
	ret = cnc_rdseqnum(_adapter->mFlibhndl, &block ); // 15,16,18,21,0,powermate
	if (ret == EW_OK)
	{
		_adapter->SetMTCTagValue("line", StdStringFormat("%d", block.data));
	}
	else
	{
		GLogger.Info(StdStringFormat("Error iSeries::cnc_rdseqnum=%d\n", ret));
	}
	std::string prognum,progname;
	ODBPRO buf ;
	ret = cnc_rdprgnum( _adapter->mFlibhndl, &buf ) ; // 15,16,18,21,0,powermate
	if (ret == EW_OK)
	{
		prognum=StdStringFormat("O%04hd", buf.mdata); // mdata -  Main program number. data- Running program number.
		GLogger.Info(StdStringFormat("iSeries::cnc_rdprgnum prognum = %d\n", buf.mdata));
		GLogger.Info(StdStringFormat("iSeries::cnc_rdprgnum prognum = %d\n",(int) (buf.mdata&0xFFFF)));
		GLogger.Info(StdStringFormat("iSeries::cnc_rdprgnum prognum data = %d\n",(int) (buf.data&0xFFFF)));
	}
	else
	{
		GLogger.Info(StdStringFormat("Error CiSeries::cnc_rdprgnum=%d", ret));
		LogErrorMessage(" CiSeries::cnc_rdprgnum FAILED\n", ret );
	}

	//PRGDIR2 prg;
	//short i, num;
	//short top = 0;
	//num = 50;
	//ret = cnc_rdprogdir2(_adapter->mFlibhndl, 1, &top, &num, &prg );


	PRGDIR2 prg[BUFSIZE+1];
	short i, num;
	short top = 0;
	std::string sLine;
	do {
		num = BUFSIZE;
		ret = cnc_rdprogdir2( _adapter->mFlibhndl, 1, &top, &num, prg );

		if ( ret == EW_NUMBER ) 
			break;

		if ( ret ) 
		{
			LogErrorMessage("iSeries::cnc_rdprogdir2 ERROR\n", ret );
			break;
		}

		for ( i = 0 ; i < num ; i++ ) 
		{
			sLine = prg[i].comment;
			int n = sLine.find("(");
			int m= sLine.find(")");
			if(std::string::npos !=n && std::string::npos !=m)
				sLine= sLine.substr(n+1,m-n-1);
			if(!sLine.empty() && sLine != "()" )
				progname=sLine;
			if(!sLine.empty())
				break;
		}

		top = prg[num-1].number + 1;

		if(!sLine.empty())
			break;
	} while ( num >= BUFSIZE );

	//if (ret == EW_OK)
	//{
	//	std::string sLine = prg.comment;
	//	int n = sLine.find("(");
	//	int m= sLine.find(")");
	//	if(std::string::npos !=n && std::string::npos !=m)
	//		sLine= sLine.substr(n+1,m-n-1);
	//	if(!sLine.empty() && sLine != "()" )
	//		progname=sLine;
	//}
	//else
	//{
	//	LogErrorMessage("iSeries::cnc_rdprogdir2 FAILED\n", ret );
	//}
	//getProgramName(char * buf) 

	//unsigned short length=MAX_PROG_SIZE;
	//short blknum;
	//static char data[MAX_PROG_SIZE+1];

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
	GLogger.LogMessage("iSeries::getLine Leave\n", 3);
	return EW_OK;
}

int CiSeries::getPositions()
{
	if (!_adapter->mConnected)
		return -1;

	GLogger.LogMessage("iSeries::getPositions Enter\n", HEAVYDEBUG);

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
				GLogger << HEAVYDEBUG << "Axis " << PosData[i].abs.name << " Data:"  << PosData[i].abs.data << "Dec:" << PosData[i].abs.dec << std::endl;
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
			GLogger.LogMessage("CiSeries::getPositions Exception\n", 3);
		}

		//GLogger.LogMessage(StdStringFormat("CiSeries::getPositions %.3f %.3f %.3f\n", 
		//	_adapter->mXact.getValue(), _adapter->mYact.getValue(), _adapter->mZact.getValue()) , GLogger.HEAVYDEBUG);
	}
	else
	{
		_adapter->disconnect();
		return -1;
	}
	GLogger.LogMessage("iSeries::getPositions Done\n", HEAVYDEBUG);

#ifdef LOADS
	ODBSVLOAD sv[MAX_AXIS];
	short num = _nAxes;
	GLogger.LogMessage("iSeries::get Loads Enter\n", GLogger.HEAVYDEBUG);
	try {
		ret = cnc_rdsvmeter(mFlibhndl, &num, sv); // 15,16,18,21,0,powermate
		if(ret) 
		{
			LOGONCE GLogger.Fatal(StdStringFormat("CiSeries::cnc_rdsvmeter Failed\n"));
			return;

		}
		if(num>0)
			_adapter->SetMTCTagValue("Xload",StdStringFormat("%8.4f", ComputeValue(sv[0].svload.data, sv[0].svload.dec));
		if(num>1) 
			_adapter->SetMTCTagValue("Yload",StdStringFormat("%8.4f", ComputeValue(sv[1].svload.data, sv[1].svload.dec));
		if(num>2)
			_adapter->SetMTCTagValue("Zload",StdStringFormat("%8.4f", ComputeValue(sv[2].svload.data, sv[2].svload.dec));
		//mXload.setValue(ComputeValue(sv[0].svload.data, sv[0].svload.dec));
		//if(num>1) mYload.setValue(ComputeValue(sv[1].svload.data, sv[1].svload.dec));
		//if(num>2) mZload.setValue(ComputeValue(sv[2].svload.data, sv[2].svload.dec));

		for(int i=3; i< _nAxes; i++)
		{
			if(::toupper(sv[i].svload.name) == 'A')
				_adapter->SetMTCTagValue("Aload",StdStringFormat("%8.4f", ComputeValue(sv[i].svload.data, sv[i].svload.dec));
				//mAload.setValue(ComputeValue( sv[i].svload.data , sv[i].svload.dec));
			if(::toupper(sv[i].svload.name) == 'B')
				_adapter->SetMTCTagValue("Bload",StdStringFormat("%8.4f", ComputeValue(sv[i].svload.data, sv[i].svload.dec));
				//mBload.setValue(ComputeValue( sv[i].svload.data , sv[i].svload.dec));
			if(::toupper(sv[i].svload.name) == 'C')
				_adapter->SetMTCTagValue("Cload",StdStringFormat("%8.4f", ComputeValue(sv[i].svload.data, sv[i].svload.dec));
				//mCload.setValue(ComputeValue( sv[i].svload.data , sv[i].svload.dec));
			//GLogger.LogMessage(StdStringFormat("CiSeries::getPositions cnc_rdsvmeter Axis %c Data=%d Dec=%d\n", sv[i].svload.name, sv[i].svload.data, sv[i].svload.dec), GLogger.HEAVYDEBUG);

		}
	}
	catch(...)
	{


	}
#endif
	GLogger.Info("CiSeries::get Loads leave\n");
	return EW_OK;
}


int CiSeries::getStatus()
{
	GLogger.LogMessage("iSeries::getStatus Enter\n", 3);

	if (!_adapter->mConnected)
		return -1;

	short ret;
	IODBSGNL sgnl;

	/// Difference 15i versus 16/18/.. i series
	if (EW_OK == (ret=cnc_rdopnlsgnl(_adapter->mFlibhndl, 0xFFFF, &sgnl)))
	{

		_adapter->SetMTCTagValue("path_feedrateovr", "100");
		_adapter->SetMTCTagValue("Sovr", "100");
	}
	else
	{
		_adapter->SetMTCTagValue("Sovr", "100");
		_adapter->SetMTCTagValue("path_feedrateovr", "100");
		GLogger.Warning(StdStringFormat("Error: iSeries::cnc_rdopnlsgnl=%d\n", ret));
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
		GLogger.Info(StdStringFormat("Controller mode iSeries::cnc_statinfo=%d\n", status.aut));
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
		GLogger.Info(StdStringFormat("Execution mode iSeries::cnc_statinfo=%d\n", status.run));

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


		//////////????????????????
		//else if (status.warning == 1)
		//  mAlarm.setValue(Alarm::eOTHER, "WaRNing", Alarm::eWARNING, Alarm::eACTIVE, "WaRNing Status Set");
	}
	else
	{
		_adapter->disconnect();
	}
	GLogger.LogMessage("iSeries::getStatus Leave\n", 3);
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
	GLogger.LogMessage("iSeries::getSpeeds enter\n", 3);
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


	GLogger.LogMessage("iSeries::getSpeeds done\n", 3);
	GLogger.LogMessage("iSeries::getSpindleLoad\n", 5);
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
		LOGONCE GLogger.Fatal(StdStringFormat("iSeries::cnc_rdspload Failed\n"));
		return ret;
	}
	 _adapter->SetMTCTagValue("Sload", StdStringFormat("%8.4f", load[0].data[0]));
#endif
	GLogger.LogMessage("iSeries::getLoads Leave\n", 3);
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
	GLogger.LogMessage("FanucMTConnectAdapter::getVersion Enter\n", INFO);
	ODBSYS sysinfo;
	short ret = cnc_sysinfo(_adapter->mFlibhndl, &sysinfo); // 15,16,18,21,0,powermate
	if (ret == EW_OK)
	{
		std::string versioninfo =StdStringFormat("CNC=%c%c Series=%c%c%c%c Version=%c%c%c%c ", sysinfo.cnc_type[0], sysinfo.cnc_type[1],
			sysinfo.series[0],sysinfo.series[1],sysinfo.series[2],sysinfo.series[3],
			sysinfo.version[0],sysinfo.version[1],sysinfo.version[2],sysinfo.version[3]
		);
		GLogger.Warning(versioninfo);
	}
	else
	{
		GLogger.Fatal(StdStringFormat("Error: FanucMTConnectAdapter::cnc_sysinfo=%d\n", ret));
	}
	return EW_OK;
}
void CiSeries::disconnect()
{
	LOGONCE GLogger.Status(StdStringFormat("iSeries::disconnect Using Protocol=%s\n", Globals.FanucProtocol.c_str()));
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
	LOGONCE GLogger.Status(StdStringFormat("iSeries::connect Using Protocol=%s\n", Globals.FanucProtocol.c_str()));

	short ret;
	_adapter->mFlibhndl=0;
	if(Globals.FanucProtocol == "HSSB")
	{
		GLogger.LogMessage(StdStringFormat("iSeries::connect HSSB cnc_allclibhndl2 Port=%d\n", _adapter->mDevicePort ), FATAL);
		//GLogger << FATAL  << "THIS EXECUTABLE DOES NOT HANDLE HSSB - EXCLUSIVELY Focas w/ Processing library for TCP/IP\n";
#ifdef iSERIESHSSB
		ret =  ::cnc_allclibhndl2(_adapter->mDevicePort,&_adapter->mFlibhndl);
#else
		GLogger << FATAL  << "THIS EXECUTABLE DOES NOT HANDLE LAN - EXCLUSIVELY Focas w/ Processing library for HSSB" << std::endl;
		return -1;
#endif
	}
	else
	{
		
		GLogger.LogMessage(StdStringFormat("iSeries::connect LAN cnc_allclibhndl3 IP=%s  Port=%d\n",_adapter->mDeviceIP.c_str(), _adapter->mDevicePort ), FATAL);
#ifdef iSERIESLAN
		ret = ::cnc_allclibhndl3(_adapter->mDeviceIP.c_str(), _adapter->mDevicePort, 10, &_adapter->mFlibhndl);
#else
		GLogger << FATAL  << "THIS EXECUTABLE DOES NOT HANDLE LAN - EXCLUSIVELY Focas w/ Processing library for HSSB"<< std::endl;
		return -1;
#endif
	}

	GLogger << FATAL  <<"iSeries::connect cnc_allclibhndl Result: " << ret << std::endl;
	if (ret != EW_OK) 
	{
		LogErrorMessage("iSeries::connect cnc_allclibhndl3 failed ",ret);
		_adapter->SetMTCTagValue("avail","UNAVAILABLE");
		_adapter->SetMTCTagValue("power","OFF");
		_adapter->mConnected = false;
		LOGONCE GLogger.Fatal(StdStringFormat("iSeries::connect FAILED Power = OFF\n", ret ));
		return -1;
	}

	GLogger.LogMessage(StdStringFormat("iSeries::connect connected ON\n", ret ), HEAVYDEBUG);
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
	str<< msg  + " Error on machine "<< _adapter->mDeviceIP << " : ";
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
	}
	str<< std::endl;
	GLogger.LogMessage(str.str().c_str(), INFO);
	LOGONCE GLogger.Fatal(str.str().c_str());
	return -1;
}
