//

// FS15D.cpp
//
// If you make an application for Series 15i, you must specify the compiling option, which is "FS15D".

#define FS15D

/*
In case of Series 15i, the number of the program number digits is always 8.
So, it is necessary to switch API to the one for the program number 8 digits in case of Series 15i.

In case of program number 8 digits, the argument and the member of the structure
which are related to program number of Data window function, is changed from "short" to "long".

In case of C/C++ language, define character constant "ONO8D" at compiling by option "-D" to switch library for program number 8 digits.

System	Option(-D)
Program number 4 digits	(None)
Program number 8 digits	ONO8D

The window function compiled for the program number 8 digits operates normally when CNC side is
program number 4 digits. Conversely, if the window function for the program number 4 digits is used
when CNC side is the program number 8 digits, it becomes an error(EW_FUNC).
*/
#define ONO8D

#include "StdAfx.h"
#include "FS15D.h"
#include "fwlib32.h"
#include "MTCFanucAgent.h"
#include "CmdHandler.h"

// Michaloski
#include "Logger.h"

// #include "EnumProc.h"
/ # include "StdStringFcn.h"

// This is here so the header file include can be cpp file specific
static ODBAXISNAME axisname[MAX_AXIS];

static double ComputeValue (long data, short dec)
{
    return ( (long double) data ) / pow(10.0, (double) dec);
}
CFS15D::CFS15D(FanucMTConnectAdapter *adapter) : _adapter(adapter)
{
    nLastBlockNum = HUGE;
}
CFS15D::~CFS15D(void)
{ }
int CFS15D::getInfo ( )
{
    short  ret;
    ODBSYS sysinfo;

    if ( EW_OK != ::cnc_sysinfo(_adapter->mFlibhndl, &sysinfo) )
    {
        return CiSeries("CFS15D::connect cnc_sysinfo", ret);
    }

    _adapter->_nAxes = atoi(sysinfo.axes);
    Logger.LogMessage(StrFormat("CFS15D::connect axes number %d\n", _adapter->_nAxes), Logger.HEAVYDEBUG);

    if ( _adapter->_nAxes >= MAX_AXIS )
    {
        Logger.Warning(StrFormat("CFS15D::connect axes number %d > MAX_AXIS OF HEADER!!!\n", _adapter->_nAxes) );
        _adapter->disconnect( );
    }

    if ( _adapter->_nAxes < 3 )
    {
        _adapter->_nAxes = 3;
    }

    short n = _adapter->_nAxes;
    ret = ::cnc_rdaxisname(_adapter->mFlibhndl, &n, axisname);

    if ( EW_OK != ret )
    {
        CiSeries("CFS15D::cnc_rdaxisname", ret);
        return Logger.Warning(StrFormat("CFS15D::connect cnc_rdaxisname FAILED %d\n", ret) );
    }

    for ( int i = 3; i < n; i++ )
    {
        _adapter->axisnum[axisname[i].name] = i;

        if ( ( ::toupper(axisname[i].name) < 'A' ) || ( ::toupper(axisname[i].name) > 'Z' ) )
        {
            break;
        }
    }
    _adapter->_nAxes = n;

    // Reads the number of spindles which CNC can control. It is not the number of mounted spindles.
    // FWLIBAPI short WINAPI cnc_rdnspdl(unsigned short FlibHndl, short *spdl);

    Logger.LogMessage(StrFormat("Axis # %d\n", _adapter->_nAxes), 3);

    for ( int i = 0; i < n; i++ )
    {
        Logger.LogMessage(StrFormat("\t%d : %c pos = %d\n", i, axisname[i].name, _adapter->axisnum[axisname[i].name]), Logger.HEAVYDEBUG);
    }
    return EW_OK;
}
int CFS15D::getLine ( )
{
    short ret;

    Logger.LogMessage("CFS15D::getLine Enter\n", 3);

    if ( !_adapter->mConnected )
    {
        return -1;
    }

    Logger.LogMessage("CFS15D::getLine\n", Logger.HEAVYDEBUG);
    ODBSEQ block;
    ret = cnc_rdseqnum(_adapter->mFlibhndl, &block);  // 15,16,18,21,0,powermate

    if ( ret == EW_OK )
    {
        _adapter->mLine.setValue(block.data);
    }
    else
    {
        CiSeries("CFS15D::cnc_rdseqnum", ret);
    }
    std::string prognum, progname;
    ODBPRO      buf;
    ret = cnc_rdprgnum(_adapter->mFlibhndl, &buf);    // 15,16,18,21,0,powermate

    if ( ret == EW_OK )
    {
        prognum = StdStringFormat("O%d", buf.mdata); // mdata -  Main program number. data- Running program number.
        Logger.LogErrorMessage(StrFormat("CFS15D::cnc_rdprgnum prognum = %d\n", prognum), 3);
    }
    else
    {
        CiSeries("CFS15D::cnc_rdprgnum", ret);
    }

#if 0
    if ( ret == EW_OK )
    {
        mProgram.setValue( ( LPCTSTR ) ::StrFormat("%d", buf.mdata) );
    }
#else
#define MAX_PROG_SIZE    ( 1024 * 2 )

    // see if new program - using sequence number or change in program #
    // if(nLastBlockNum > mLine.getValue())  // DOES NOT WORK
    {
        unsigned short length = MAX_PROG_SIZE;
        short          blknum;
        static char    data[MAX_PROG_SIZE + 1];
        ret = cnc_rdexecprog(_adapter->mFlibhndl, &length, &blknum, data);  // 15,16,18,21,0,powermate

        if ( ret != EW_OK )
        {
            CiSeries("CFS15D::cnc_rdexecprog", ret);
        }

        if ( ret == EW_OK )
        {
            Logger.LogMessage(StdStringFormat("CFS15D::cnc_rdexecprog blck counter %d\n", (int) blknum).c_str( ), 3);
            Logger.LogMessage(StdStringFormat("CFS15D::cnc_rdexecprog length %d\n", (int) length).c_str( ), 3);
            Logger.LogMessage("CFS15D::cnc_rdexecprog Search for O# comment\n", 3);
            Logger.Info(data);

            // issue here - main program versus subprogram
            progname = _adapter->getProgramName(data);
            Logger.Info(StrFormat("CFS15D::cnc_rdexecprog getProgramName Found  %s\n", progname.c_str( ) ) );
        }

        if ( !progname.empty( ) )
        {
            _adapter->mProgram.setValue(progname.c_str( ) );
        }
        else if ( !prognum.empty( ) )
        {
            _adapter->mProgram.setValue(prognum.c_str( ) );
        }
        else
        {
            _adapter->mProgram.setValue("O1");
        }
    }
    lastprognum   = prognum;
    nLastBlockNum = _adapter->mLine.getValue( );
#endif
    Logger.LogMessage("CFS15D::getLine Leave\n", 3);
    return EW_OK;
}
int CFS15D::getPositions ( )
{
    if ( !_adapter->mConnected )
    {
        return -1;
    }

    Logger.LogMessage("CFS15D::getPositions Enter\n", Logger.HEAVYDEBUG);

    // ODBPOS *PosData = static_cast<ODBPOS *>(alloca(_nAxes * sizeof(ODBPOS)));

    /* type.
    0	:	absolute position
    1	:	machine position
    2	:	relative position
    3	:	distance to go
    -1	:	all type
    */
    short  data_num = MAX_AXIS; // MAX_AXIS different for 15
    short  type     = -1;
    ODBPOS PosData[MAX_AXIS];

    short ret = cnc_rdposition(_adapter->mFlibhndl, type, &data_num, &PosData[0]); // 15,16,18,21,0,powermate

    if ( ret != EW_OK )
    {
        CiSeries("CFS15D::cnc_rdposition", ret);
    }

    if ( ret == EW_OK )
    {
        for ( int i = 0; i < data_num; i++ )
        {
            Logger.Info(StrFormat("Abs[%d] %c = %d Dec=%d Unit=%d \n", i, PosData[i].abs.name, PosData[i].abs.data, (int) PosData[i].abs.dec, (int) PosData[i].abs.unit) );
            Logger.Info(StrFormat("Mach[%d] %c = %d Dec=%d Unit=%d \n", i, PosData[i].mach.name, PosData[i].mach.data, (int) PosData[i].mach.dec, (int) PosData[i].mach.unit) );
            Logger.Info(StrFormat("Rel[%d] %c = %d Dec=%d Unit=%d\n", i, PosData[i].rel.name, PosData[i].rel.data, (int) PosData[i].rel.dec, (int) PosData[i].rel.unit) );
            Logger.Info(StrFormat("Dist[%d] %c = %d Dec=%d Unit=%d \n", i, PosData[i].dist.name, PosData[i].dist.data, (int) PosData[i].dist.dec, (int) PosData[i].dist.unit) );
        }
#if 0
        _adapter->mXact.setValue(ComputeValue(PosData[0].abs.data, PosData[0].abs.dec) );
        _adapter->mYact.setValue(ComputeValue(PosData[1].abs.data, PosData[1].abs.dec) );
        _adapter->mZact.setValue(ComputeValue(PosData[2].abs.data, PosData[2].abs.dec) );
#endif

        // Fanuc ABC units are in degrees
        try
        {
            for ( int i = 0; i < data_num; i++ )
            {
                // X should be marked as visited as second X is not correct
                if ( ( ::toupper(PosData[i].abs.name) == 'X' ) && ( i == 0 ) )
                {
                    _adapter->mXact.setValue(ComputeValue(PosData[i].abs.data, PosData[i].abs.dec) );
                }

                if ( ::toupper(PosData[i].abs.name) == 'Y' )
                {
                    _adapter->mYact.setValue(ComputeValue(PosData[i].abs.data, PosData[i].abs.dec) );
                }

                if ( ::toupper(PosData[i].abs.name) == 'Z' )
                {
                    _adapter->mZact.setValue(ComputeValue(PosData[i].abs.data, PosData[i].abs.dec) );
                }

                if ( ::toupper(PosData[i].abs.name) == 'A' )
                {
                    _adapter->mAact.setValue(ComputeValue(PosData[i].abs.data, PosData[i].abs.dec) );
                }

                if ( ::toupper(PosData[i].abs.name) == 'B' )
                {
                    _adapter->mBact.setValue(ComputeValue(PosData[i].abs.data, PosData[i].abs.dec) );
                }

                if ( ::toupper(PosData[i].abs.name) == 'C' )
                {
                    _adapter->mCact.setValue(ComputeValue(PosData[i].abs.data, PosData[i].abs.dec) );
                }
            }
        }
        catch ( ... )
        {
            Logger.LogErrorMessage("CFS15D::getPositions Exception\n", 3);
        }

        Logger.LogMessage(StrFormat("CFS15D::getPositions %.3f %.3f %.3f\n",
                _adapter->mXact.getValue( ), _adapter->mYact.getValue( ), _adapter->mZact.getValue( ) ), Logger.HEAVYDEBUG);
    }
    Logger.LogMessage("CFS15D::getPositions Done\n", Logger.HEAVYDEBUG);

#ifdef LOADS
    ODBSVLOAD sv[MAX_AXIS];
    short     num = _nAxes;
    Logger.LogMessage("CFS15D::get Loads Enter\n", Logger.HEAVYDEBUG);
    try
    {
        ret = cnc_rdsvmeter(mFlibhndl, &num, sv); // 15,16,18,21,0,powermate

        if ( ret )
        {
            LOGONCE Logger.Fatal(StrFormat("CFS15D::cnc_rdsvmeter Failed\n") );
            return;
        }

        if ( num > 0 )
        {
            mXload.setValue(ComputeValue(sv[0].svload.data, sv[0].svload.dec) );
        }

        if ( num > 1 )
        {
            mYload.setValue(ComputeValue(sv[1].svload.data, sv[1].svload.dec) );
        }

        if ( num > 2 )
        {
            mZload.setValue(ComputeValue(sv[2].svload.data, sv[2].svload.dec) );
        }

        for ( int i = 3; i < _nAxes; i++ )
        {
            if ( ::toupper(sv[i].svload.name) == 'A' )
            {
                mAload.setValue(ComputeValue(sv[i].svload.data, sv[i].svload.dec) );
            }

            if ( ::toupper(sv[i].svload.name) == 'B' )
            {
                mBload.setValue(ComputeValue(sv[i].svload.data, sv[i].svload.dec) );
            }

            if ( ::toupper(sv[i].svload.name) == 'C' )
            {
                mCload.setValue(ComputeValue(sv[i].svload.data, sv[i].svload.dec) );
            }
            Logger.LogMessage(StrFormat("CFS15D::getPositions cnc_rdsvmeter Axis %c Data=%d Dec=%d\n", sv[i].svload.name, sv[i].svload.data, sv[i].svload.dec), Logger.HEAVYDEBUG);
        }
    }
    catch ( ... )
    { }
    Logger.LogMessage("CFS15D::get Loads leave\n", Logger.HEAVYDEBUG);
#endif
}
int CFS15D::getSpeeds ( )
{
    Logger.LogMessage("CFS15D::getSpeeds enter\n", 3);

    if ( !_adapter->mConnected )
    {
        return -1;
    }

    ODBSPEED speed;
    /* Data type. 0 ( feed rate ), 1 ( spindle speed  ) , -1 (  all  ) */
    short ret = cnc_rdspeed(_adapter->mFlibhndl, -1, &speed);
#if 0
    ODBACT  buf;
    ODBACT2 buf2[4];
    try
    {
        cnc_acts(mFlibhndl, &buf);

        short sp_no = 4;
        short ret2;

        for ( int i = 0; i < 4; i++ )
        {
            ret2 = cnc_acts2(mFlibhndl, i, buf2);

            if ( EW_NUMBER != ret2 )
            {
                Logger.LogMessage(StrFormat("Spindle[%d]=%ld\n", buf2[0].data), 3);
            }
        }
    }
    catch ( ... )
    {
        Logger.LogMessage(StrFormat("cnc_acts problem\n"), -1);
    }
    ODBCMD buf;
    short  num_cmd = 1;
    double spindlespeed;
    try
    {
        // This function cannot be used for Series 16/18/21/0 and Power Mate
        short sret = cnc_rdcommand(_adapter->mFlibhndl, 18, 1, &num_cmd, &buf);

        if ( ( sret == EW_OK ) && ( num_cmd == 1 ) && ( buf.adrs == 'S' ) )
        {
            spindlespeed = buf.cmd_val;

            if ( buf.dec_val > 0 )
            {
                spindlespeed = spindlespeed / ( buf.dec_val * 10.0 );
            }
        }
        else
        {
            spindlespeed = speed.actf.data;
        }
    }
    catch ( ... )
    {
        Logger.Fatal("cnc_rdcommand speed problem\n");
    }
#endif

    if ( ret != EW_OK )
    {
        CiSeries("CFS15D::cnc_rdspeed", ret);
    }

    if ( ret == EW_OK )
    {
        _adapter->mPathFeedrate.setValue(speed.actf.data * pow(10.0, (double) speed.actf.dec) );
        _adapter->mSpindleSpeed.setValue(speed.acts.data * pow(10.0, (double) speed.acts.dec) );

        // The spindle speed is one of the main spindle that is displayed in the position screen of the CNC.
        // HACK just use actual feed.
        // _adapter->mSpindleSpeed.setValue(spindlespeed);
        // Reads the actual rotational speed of the spindle connected to CNC
        // mSpindleSpeed.setValue(buf.data);
    }
    Logger.LogMessage("CFS15D::getSpeeds done\n", 3);

#if 0
    Logger.LogMessage("CFS15D::getSpindleLoad\n", 5);
    try
    {
        short nspd;
        ret = cnc_rdnspdl(_adapter->mFlibhndl, &nspd);

        if ( ret != EW_OK )
        {
            LogErrorMessage("CFS15D::cnc_rdnspdl", ret);
            return;
        }

        ODBSPN *load = static_cast<ODBSPN *>( alloca(nspd * sizeof( ODBSPN ) ) );

        // ODBSPLOAD load[MAX_SPINDLES];   /* 4 = maximum spinlde number */
        ret = cnc_rdspload(_adapter->mFlibhndl, -1, load);

        if ( ret != EW_OK )
        {
            LogErrorMessage("CFS15D::cnc_rdspload", ret);
            return;
        }
        _adapter->mSpindleSpeedLoad.setValue(load[0].data[0]);
    }
    catch ( ... )
    {
        Logger.LogMessage("CFS15D::Spindle Load exception\n", 3);
    }
#endif
    Logger.LogMessage("CFS15D::getSpeeds Leave\n", 3);
    return EW_OK;
}
int CFS15D::getStatus ( )
{
    Logger.LogMessage("CFS15D::getStatus Enter\n", 3);

    if ( !_adapter->mConnected )
    {
        return -1;
    }

    short    ret;
    IODBSGNL sgnl;

    // / Difference 15i versus 16/18/.. i series
    if ( EW_OK == cnc_rdopnlsgnl(_adapter->mFlibhndl, 0x20, &sgnl) )
    {
        _adapter->mPathFeedrateOvr.setValue(100.0); // doesnt work for 15i - (sgnl.feed_ovrd * 10.0);
    }

    // this works for 15 i
    if ( EW_OK == cnc_rdopnlsgnl(_adapter->mFlibhndl, 0x40, &sgnl) )
    {
        _adapter->mSpindleSpeedOvr.setValue(100.0); // doesnt work for 15i - (sgnl.spdl_ovrd * 10.0);
    }
    ODBST status;
    ret = cnc_statinfo(_adapter->mFlibhndl, &status);

    if ( ret == EW_OK )
    {
        // Series 16/18/21/0/Power Mate
        if ( status.aut == 1 ) // MDI for aut
        {
            _adapter->mMode.setValue(ControllerMode::eMANUAL_DATA_INPUT);
        }
        else if ( status.aut != 0 ) // auto
        {
            _adapter->mMode.setValue(ControllerMode::eAUTOMATIC);
        }

        if ( status.manual != 0 ) // 0 is no manual selection
        {
            _adapter->mMode.setValue(ControllerMode::eMANUAL);
        }

        /** run
        Status of automatic operation 0  :  ****(reset)
        1  :  STOP
        2  :  HOLD
        3  :  STaRT
        4  :  MSTR(during retraction and re-positioning of tool retraction and recovery, and operation of JOG MDI)
        */

        if ( ( status.run == 0 ) || ( status.run == 1 ) )           //  STOP or HOLD is ePAUSED
        {
            _adapter->mExecution.setValue(Execution::eINTERRUPTED); // ePAUSED
        }
        else if ( status.run == 8 )                                 // reset
        {
            _adapter->mExecution.setValue(Execution::eREADY);       // /eIDLE
        }
        else  // STaRT, etc.
        {
            _adapter->mExecution.setValue(Execution::eACTIVE);  // eEXECUTING
        }

        // Alarms are removed
#if 0
        if ( status.emergency == 0 )
        {
            mAlarm.setValue(Alarm::eESTOP, "EMerGency", Alarm::eCRITICAL, Alarm::eCLEARED, "EMerGency Status Set");
        }
        else
        {
            mAlarm.setValue(Alarm::eESTOP, "EMerGency", Alarm::eCRITICAL, Alarm::eACTIVE, "EMerGency Status Set");
        }

        if ( status.alarm == 0 )
        {
            mAlarm.setValue(Alarm::eOTHER, "ALarM", Alarm::eERROR, Alarm::eCLEARED, "ALarM Status Set");
        }
        else
        {
            mAlarm.setValue(Alarm::eOTHER, "ALarM", Alarm::eERROR, Alarm::eACTIVE, "ALarM Status Set");
        }
#endif
    }

    // ////////????????????????
    // else if (status.warning == 1)
    //  mAlarm.setValue(Alarm::eOTHER, "WaRNing", Alarm::eWARNING, Alarm::eACTIVE, "WaRNing Status Set");

    Logger.LogMessage("CFS15D::getStatus Leave\n", 3);
    return EW_OK;
}
int CFS15D::getVersion ( )
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
    if ( !_adapter->mConnected )
    {
        return -1;
    }
    Logger.LogMessage("FanucMTConnectAdapter::getVersion Enter\n", Logger.HEAVYDEBUG);
    ODBSYS sysinfo;
    short  ret = cnc_sysinfo(_adapter->mFlibhndl, &sysinfo); // 15,16,18,21,0,powermate

    if ( ret == EW_OK )
    {
        std::string versioninfo = StdStringFormat("CNC=%c%c Series=%c%c%c%c Version=%c%c%c%c ", sysinfo.cnc_type[0], sysinfo.cnc_type[1],
            sysinfo.series[0], sysinfo.series[1], sysinfo.series[2], sysinfo.series[3],
            sysinfo.version[0], sysinfo.version[1], sysinfo.version[2], sysinfo.version[3]
            );
        Logger.LogMessage(versioninfo.c_str( ), Logger.FATAL);
#if 0
        if ( AutoVersionDetect )
        {
            if ( std::string(sysinfo.cnc_type) == "15" )
            {
                Globals.FanucVersion = "15i";
            }
            else
            {
                Globals.FanucVersion = "iSeries";
            }
        }
#endif
    }
    else
    {
        CiSeries("FanucMTConnectAdapter::cnc_sysinfo", ret);
    }
    return EW_OK;
}
void CFS15D::disconnect ( )
{
    if ( _adapter->mConnected )
    {
        _adapter->mPower.setValue(PowerState::eOFF);
        cnc_freelibhndl(_adapter->mFlibhndl);
        _adapter->mConnected = false;
    }
}
int CFS15D::connect ( )
{
    LOGONCE Logger.Status(StrFormat("FanucMTConnectAdapter::connect Using Protocol=%s\n", Globals.FanucProtocol.c_str( ) ) );

    short ret;
    _adapter->mFlibhndl = 0;

    if ( Globals.FanucProtocol == "HSSB" )
    {
        ret = ::cnc_allclibhndl2(Globals.HssbPort, &_adapter->mFlibhndl);
    }
    else
    {
        ret = ::cnc_allclibhndl3(_adapter->mDeviceIP, _adapter->mDevicePort, 10, &_adapter->mFlibhndl);
    }

    Logger.LogMessage(StrFormat("FanucMTConnectAdapter::connect cnc_allclibhndl3 Result: %d\n", ret), Logger.HEAVYDEBUG);

    if ( ret != EW_OK )
    {
        LogErrorMessage("FanucMTConnectAdapter::connect cnc_allclibhndl3", ret);
        _adapter->mPower.setValue(PowerState::eOFF);
        _adapter->mConnected = false;
        Sleep(500);
        LOGONCE Logger.Fatal(StrFormat("FanucMTConnectAdapter::connect FAILED Power = OFF\n", ret) );
        return -1;
    }

    Logger.LogMessage(StrFormat("FanucMTConnectAdapter::connect connected ON\n", ret), Logger.HEAVYDEBUG);
    _adapter->mConnected = true;
    _adapter->mPower.setValue(PowerState::eON);
    return 0;
}
int CF15B::LogErrorMessage (std::string msg, short errcode)
{
    if ( errcode == EW_OK )
    {
        return 0;
    }
    std::stringstream str;
    str << msg + " Error : ";

    switch ( errcode )
    {
    case EW_PROTOCOL:
        {
            str << "EW_PROTOCOL - protocol error";
        }
        break;

    case EW_SOCKET:
        {
            str << "EW_SOCKET - Windows socket error ";
        }
        break;

    case EW_HANDLE:
        {
            str << "EW_HANDLE  - DLL not exist error";
        }
        break;

    case EW_VERSION:
        {
            str << "EW_VERSION - CNC/PMC version missmatch";
        }
        break;

    case EW_UNEXP:
        {
            str << "EW_UNEXP - abnormal error";
        }
        break;

    case EW_NODLL:
        {
            str << "EW_NODLL";
        }
        break;

    case EW_BUS:
        {
            str << "EW_BUS - bus error";
        }
        break;

    case EW_SYSTEM2:
        {
            str << "EW_SYSTEM2 - system error";
        }
        break;

    case EW_HSSB:
        {
            str << "EW_HSSB - hssb communication error";
        }
        break;

    case EW_SYSTEM:
        {
            str << "EW_SYSTEM";
        }
        break;

    case EW_MMCSYS:
        {
            str << "EW_MMCSYS - emm386 or mmcsys install error";
        }
        break;

    case EW_RESET:
        {
            str << "EW_RESET - reset or stop occured error";
        }
        break;

    case EW_BUSY:
        {
            str << "EW_BUSY - busy error";
        }
        break;

    case EW_PARITY:
        {
            str << " EW_PARITY - shared RAM parity error ";
        }
        break;

    case EW_FUNC:
        {
            str << "EW_FUNC - command prepare error";         // 1
            /*case EW_NOPMC:*/ str << " or EW_NOPMC - pmc not exist";
        }
        break;                                                       // 1

    case EW_LENGTH:
        {
            str << "EW_LENGTH - data block length error";
        }
        break;                                                           // 2

    case EW_NUMBER:
        {
            str << "EW_NUMBER - data number error";           // 3
            /*case EW_RANGE:*/ str << " or EW_RANGE -address range error";
        }
        break;                                                             // 3

    case EW_ATTRIB:
        {
            str << "EW_ATTRIB - data attribute error";            // 4
            /*case EW_TYPE:*/ str << " or EW_TYPE -data type error";
        }
        break;                                                       // 4

    case EW_DATA:
        {
            str << "EW_DATA - data error";
        }
        break;                                          // 5

    case EW_NOOPT:
        {
            str << "EW_NOOPT - No option The extended driver/library function is necessary.";
        }
        break;                                                                                               // 6

    case EW_PROT:
        {
            str << "EW_PROT - write protect error";
        }
        break;                                                    // 7

    case EW_OVRFLOW:
        {
            str << "EW_OVRFLOW - memory overflow error";
        }
        break;                                                           // 8

    case EW_PARAM:
        {
            str << "EW_PARAM - cnc parameter not correct error";
        }
        break;                                                                 // 9

    case EW_BUFFER:
        {
            str << "EW_BUFFER - buffer error";
        }
        break;                                                 // 10

    case EW_PATH:
        {
            str << "EW_PATH - path error";
        }
        break;                                           // 11

    case EW_MODE:
        {
            str << "EW_MODE - cnc mode error";
        }
        break;                                              // 12

    case EW_REJECT:
        {
            str << "EW_REJECT - execution rejected error";
        }
        break;                                                             // 13

    case EW_DTSRVR:
        {
            str << "EW_DTSRVR - data server error";
        }
        break;                                                      // 14

    case EW_ALARM:
        {
            str << "EW_ALARM - alarm has been occurred";
        }
        break;                                                         // 15

    case EW_STOP:
        {
            str << "EW_STOP -CNC is not running";
        }
        break;                                                 // 16

    case EW_PASSWD:
        {
            str << "EW_PASSWD -  protection data error ";
        }
        break;                                                           // 17
    }

    str << std::endl;
    Logger.LogMessage(str.str( ).c_str( ), 3);
    return -1;
}
