//

// FS15D.cpp
//
// If you make an application for Series 15i, you must specify the compiling option, which is "FS15D".
#pragma message("Compile FS15D.cpp")
#pragma comment(lib, "./Distribution/15iHSSB/Fwlib32.lib")

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

// #include "fwlib32.h"
#include "MTCFanucAgent.h"
#include "CmdHandler.h"

// Michaloski
#include "GLogger.h"
#include "StdStringFcn.h"

// This is here so the header file include can be cpp file specific
static ODBAXISNAME axisname[MAX_AXIS];

static double ComputeValue (long data, short dec)
{
    return ( (long double) data ) / pow(10.0, (double) dec);
}
CFS15D::CFS15D(CCmdHandler *agent) : _adapter(agent)
{
    nLastBlockNum = HUGE;
}
CFS15D::~CFS15D(void)
{ }
int CFS15D::getAlarmInfo ( )
{
    // char *almmsg[] = {
    //	"P/S 100 ALARM","P/S 000 ALARM",
    //	"P/S 101 ALARM","P/S ALARM (1-255)",
    //	"OT ALARM",     "OH ALARM",
    //	"SERVO ALARM",  "unknown ALARM",
    //	"APC ALARM",    "SPINDLE ALARM",
    //	"P/S ALARM (5000-)"
    // } ;

    char *almmsg[] =
    {
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
        "Reverse control alarm"
    };

    short          ret = 0;
    ODBALM         buf;
    unsigned short idx;

    ret = cnc_alarm(_adapter->mFlibhndl, &buf);

    if ( ret != EW_OK )
    {
        return LogErrorMessage("15iSeries::getAlarmInfo() cnc_alarm FAILED\n", ret);
    }

    if ( buf.data == 0 )
    {
        _adapter->SetMTCTagValue("alarm", "");
    }
    else
    {
        std::string sAlarm;

        for ( idx = 0; idx < 11; idx++ )
        {
            if ( buf.data & 0x0001 )
            {
                sAlarm += almmsg[idx];
            }
            buf.data >>= 1;
        }
        _adapter->SetMTCTagValue("alarm", sAlarm);
    }
    return 0;
}
int CFS15D::getInfo ( )
{
    short  ret = 0;
    ODBSYS sysinfo;

    if ( EW_OK != ::cnc_sysinfo(_adapter->mFlibhndl, &sysinfo) )
    {
        return GLogger.Warning(StdStringFormat("Error: 15iSeries::connect cnc_sysinfo FAILED %d\n", ret) );
    }

    _adapter->_nAxes = atoi(sysinfo.axes);
    GLogger.Info(StdStringFormat("15iSeries::connect axes number %d\n", _adapter->_nAxes) );

    if ( _adapter->_nAxes >= MAX_AXIS )
    {
        GLogger.Warning(StdStringFormat("15iSeries::connect axes number %d > MAX_AXIS OF HEADER!!!\n", _adapter->_nAxes) );
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
        return GLogger.Warning(StdStringFormat("15iSeries::connect cnc_rdaxisname FAILED %d\n", ret) );
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

    GLogger.LogMessage(StdStringFormat("Axis # %d\n", _adapter->_nAxes), 3);

    for ( int i = 0; i < n; i++ )
    {
        GLogger.Info(StdStringFormat("\t%d : %c pos = %d\n", i, axisname[i].name, _adapter->axisnum[axisname[i].name]) );
    }
    return EW_OK;
}
int CFS15D::getLine ( )
{
    GLogger.LogMessage("15iSeries::getLine Enter\n", 3);
    short ret;

    if ( !_adapter->mConnected )
    {
        return -1;
    }

    GLogger.Info("15iSeries::getLine\n");
    ODBSEQ block;
    ret = cnc_rdseqnum(_adapter->mFlibhndl, &block);  // 15,16,18,21,0,powermate

    if ( ret == EW_OK )
    {
        _adapter->SetMTCTagValue("line", StdStringFormat("%d", block.data) );
    }
    else
    {
        GLogger.Info(StdStringFormat("Error 15iSeries::cnc_rdseqnum=%d\n", ret) );
    }
    std::string prognum, progname;
    ODBPRO      buf;
    ret = cnc_rdprgnum(_adapter->mFlibhndl, &buf);    // 15,16,18,21,0,powermate

    if ( ret == EW_OK )
    {
        prognum = StdStringFormat("O%04hd", buf.mdata); // mdata -  Main program number. data- Running program number.
        GLogger.Info(StdStringFormat("15iSeries::cnc_rdprgnum prognum = %d\n", prognum) );
    }
    else
    {
        GLogger.Info(StdStringFormat("Error CFS15D::cnc_rdprgnum=%d", ret) );
    }

#if 0
    if ( ret == EW_OK )
        _adapter->SetMTCTagValue("program", ( LPCTSTR ) ::StdStringFormat("%d", buf.mdata);

            // mProgram.setValue((LPCTSTR) ::StdStringFormat("%d", buf.mdata));
#else
#define MAX_PROG_SIZE    ( 1024 * 2 )

    // see if new program - using sequence number or change in program #
    // if(nLastBlockNum > mLine.getValue())  // DOES NOT WORK
    {
        unsigned short length = MAX_PROG_SIZE;
        short          blknum;
        static char    data[MAX_PROG_SIZE + 1];
        ret = cnc_rdexecprog( _adapter->mFlibhndl, &length, &blknum, data);  // 15,16,18,21,0,powermate

                              if ( ret != EW_OK )
                              {
                                  GLogger.Warning(StdStringFormat("Error: CFS15D::cnc_rdexecprog=%d\n", ret) );
                              }

                              if ( ret == EW_OK )
                              {
                                  GLogger.LogMessage(StdStringFormat("15iSeries::cnc_rdexecprog blck counter %d\n", (int) blknum), 3);
                                  GLogger.LogMessage(StdStringFormat("15iSeries::cnc_rdexecprog length %d\n", (int) length), 3);
                                  GLogger.LogMessage("15iSeries::cnc_rdexecprog Search for O# comment\n", 3);
                                  GLogger.Info(data);

                                  // issue here - main program versus subprogram
                                  progname = _adapter->getProgramName(data);
                                  GLogger.Info(StdStringFormat("15iSeries::cnc_rdexecprog getProgramName Found  %s\n", progname.c_str( ) ) );
                              }

                              if ( !progname.empty( ) )
                              {
                                  // _adapter->mProgram.setValue(progname.c_str());
                                  _adapter->SetMTCTagValue("program", progname);
                              }
                              else if ( !prognum.empty( ) )
                              {
                                  _adapter->SetMTCTagValue("program", prognum);

                                  // _adapter->mProgram.setValue(prognum.c_str());;
                              }
                              else
                              {
                                  _adapter->SetMTCTagValue("program", "O1");

                                  // _adapter->mProgram.setValue("O1");
                              }
    }

    // nLastBlockNum=_adapter->mLine.getValue();
#endif
            GLogger.LogMessage("15iSeries::getLine Leave\n", 3);
            return EW_OK;
            }

            int CFS15D::getPositions( )
            {
                if ( !_adapter->mConnected )
                {
                    return -1;
                }

                GLogger.LogMessage("15iSeries::getPositions Enter\n", HEAVYDEBUG);

                // ODBPOS *PosData = static_cast<ODBPOS *>(alloca(_nAxes * sizeof(ODBPOS)));

                /* type.
                0	:	absolute position
                1	:	machine position
                2	:	relative position
                3	:	distance to go
                -1	:	all type
                */
                short data_num = MAX_AXIS;
                short type     = -1;
                ODBPOS PosData[MAX_AXIS];                                                      // Position Data
                short ret = cnc_rdposition(_adapter->mFlibhndl, type, &data_num, &PosData[0]); // 15,16,18,21,0,powermate

                if ( ret == EW_OK )
                {
                    // Fanuc ABC units are in degrees
                    try
                    {
                        for ( int i = 0; i < data_num; i++ )
                        {
                            GLogger << HEAVYDEBUG << "Axis " << PosData[i].abs.name << " Data:" << PosData[i].abs.data << "Dec:" << PosData[i].abs.dec << std::endl;

                            if ( ( ::toupper(PosData[i].abs.name) == 'X' ) && ( i == 0 ) )
                            {
                                _adapter->SetMTCTagValue("Xabs", StdStringFormat("%8.4f", ComputeValue(PosData[i].abs.data, PosData[i].abs.dec) ) );
                            }

                            if ( ::toupper(PosData[i].abs.name) == 'Y' )
                            {
                                _adapter->SetMTCTagValue("Yabs", StdStringFormat("%8.4f", ComputeValue(PosData[i].abs.data, PosData[i].abs.dec) ) );
                            }

                            if ( ::toupper(PosData[i].abs.name) == 'Z' )
                            {
                                _adapter->SetMTCTagValue("Zabs", StdStringFormat("%8.4f", ComputeValue(PosData[i].abs.data, PosData[i].abs.dec) ) );
                            }

                            if ( ::toupper(PosData[i].abs.name) == 'A' )
                            {
                                _adapter->SetMTCTagValue("Aabs", StdStringFormat("%8.4f", ComputeValue(PosData[i].abs.data, PosData[i].abs.dec) ) );
                            }

                            if ( ::toupper(PosData[i].abs.name) == 'B' )
                            {
                                _adapter->SetMTCTagValue("Babs", StdStringFormat("%8.4f", ComputeValue(PosData[i].abs.data, PosData[i].abs.dec) ) );
                            }

                            if ( ::toupper(PosData[i].abs.name) == 'C' )
                            {
                                _adapter->SetMTCTagValue("Cabs", StdStringFormat("%8.4f", ComputeValue(PosData[i].abs.data, PosData[i].abs.dec) ) );
                            }
                        }
                    }
                    catch ( ... )
                    {
                        GLogger.LogMessage("CFS15D::getPositions Exception\n", 3);
                    }

                    // GLogger.LogMessage(StdStringFormat("CFS15D::getPositions %.3f %.3f %.3f\n",
                    //	_adapter->mXact.getValue(), _adapter->mYact.getValue(), _adapter->mZact.getValue()) , GLogger.HEAVYDEBUG);
                }
                else
                {
                    _adapter->disconnect( );
                }
                GLogger.LogMessage("15iSeries::getPositions Done\n", HEAVYDEBUG);

#ifdef LOADS
                ODBSVLOAD sv[MAX_AXIS];
                short num = _nAxes;
                GLogger.LogMessage("15iSeries::get Loads Enter\n", GLogger.HEAVYDEBUG);
                try
                {
                    ret = cnc_rdsvmeter(mFlibhndl, &num, sv); // 15,16,18,21,0,powermate

                    if ( ret )
                    {
                        LOGONCE GLogger.Fatal(StdStringFormat("CFS15D::cnc_rdsvmeter Failed\n") );
                        return;
                    }

                    if ( num > 0 )
                    {
                        _adapter->SetMTCTagValue("Xload", StdStringFormat("%8.4f", ComputeValue(sv[0].svload.data, sv[0].svload.dec) );

                            if ( num > 1 )
                            {
                                _adapter->SetMTCTagValue("Yload", StdStringFormat("%8.4f", ComputeValue(sv[1].svload.data, sv[1].svload.dec) );

                                    if ( num > 2 )
                                        _adapter->SetMTCTagValue ( "Zload", StdStringFormat("%8.4f", ComputeValue(sv[2].svload.data, sv[2].svload.dec) );

                                                                   // mXload.setValue(ComputeValue(sv[0].svload.data, sv[0].svload.dec));
                                                                   // if(num>1) mYload.setValue(ComputeValue(sv[1].svload.data, sv[1].svload.dec));
                                                                   // if(num>2) mZload.setValue(ComputeValue(sv[2].svload.data, sv[2].svload.dec));

                                                                   for ( int i = 3; i < _nAxes; i++ )
                                                                   {
                                                                       if ( ::toupper(sv[i].svload.name) == 'A' )
                                                                           _adapter->SetMTCTagValue ( "Aload", StdStringFormat("%8.4f", ComputeValue(sv[i].svload.data, sv[i].svload.dec) );

                                                                                                      // mAload.setValue(ComputeValue( sv[i].svload.data , sv[i].svload.dec));
                                                                                                      if ( ::toupper(sv[i].svload.name) == 'B' )
                                                                                                          _adapter->SetMTCTagValue ( "Bload", StdStringFormat("%8.4f", ComputeValue(sv[i].svload.data, sv[i].svload.dec) );

                                                                                                                                     // mBload.setValue(ComputeValue( sv[i].svload.data , sv[i].svload.dec));
                                                                                                                                     if ( ::toupper(sv[i].svload.name) == 'C' )
                                                                                                                                         _adapter->SetMTCTagValue ( "Cload", StdStringFormat("%8.4f", ComputeValue(sv[i].svload.data, sv[i].svload.dec) );

                                                                                                                                                                    // mCload.setValue(ComputeValue( sv[i].svload.data , sv[i].svload.dec));
                                                                                                                                                                    // GLogger.LogMessage(StdStringFormat("CFS15D::getPositions cnc_rdsvmeter Axis %c Data=%d Dec=%d\n", sv[i].svload.name, sv[i].svload.data, sv[i].svload.dec), GLogger.HEAVYDEBUG);
                                                                                                                                                                    }
                                                                                                                                                                    }
                                                                                                                                                                    catch ( ... )
                                                                                                                                                                    { }
#endif
                GLogger.Info("CFS15D::get Loads leave\n");
                return EW_OK;
            }

            int CFS15D::getStatus( )
            {
                GLogger.LogMessage("15iSeries::getStatus Enter\n", 3);

                if ( !_adapter->mConnected )
                {
                    return -1;
                }

                short ret;
                IODBSGNL sgnl;

                // / Difference 15i versus 16/18/.. i series
                if ( EW_OK == cnc_rdopnlsgnl(_adapter->mFlibhndl, 0x20, &sgnl) )
                {
                    _adapter->SetMTCTagValue("path_feedrateovr", StdStringFormat("%8.4f", sgnl.feed_ovrd * 10.0) );
                }

                // this one may fail
                if ( EW_OK == cnc_rdopnlsgnl(_adapter->mFlibhndl, 0x40, &sgnl) )
                {
                    _adapter->SetMTCTagValue("Sovr", StdStringFormat("%8.4f", sgnl.spdl_ovrd * 10.0) );
                }

                ODBST status;
                ret = cnc_statinfo(_adapter->mFlibhndl, &status);
                GLogger.Info(StdStringFormat("CFS15D::getStatus() - status.aut=%d status.run = %d\n", status.aut, status.run) );

                if ( ret == EW_OK )
                {
                                                                                                // Series 15i
                    if ( ( ( status.aut == 1) ) || ( status.aut == 4 ) || ( status.aut == 5 ) ) // MDI, EDIT, Teach In
                    {
                        _adapter->SetMTCTagValue("controllermode", "MANUAL");
                    }
                    else
                    {
                        _adapter->SetMTCTagValue("controllermode", "AUTOMATIC");
                    }

                    /** run
                    Status of automatic operation
                    0	:	STOP
                    1	:	HOLD
                    2	:	STaRT
                    3	:	MSTR(jog mdi)
                    4	:	ReSTaRt(not blinking)
                    Except under manual mode and under cutter radius compensation outside corner
                    5	:	PRSR(program restart)
                    6	:	NSRC(sequence number search)
                    7	:	ReSTaRt(blinking)
                    Under manual mode or under cutter radius compensation outside corner
                    8	:	ReSET
                    9	:	(not used)
                    10	:	(not used)
                    11	:	(not used)
                    12	:	(not used)
                    13	:	HPCC(during RISC operation)
                    */

                    if ( status.run == 0 )
                    {
                        _adapter->SetMTCTagValue("execution", "IDLE");      // eREADY eIDLE
                    }
                    else if ( status.run == 1 )                             //  STOP or HOLD is ePAUSED
                    {
                        _adapter->SetMTCTagValue("execution", "PAUSED");    // HOLD eINTERRUPTED
                    }
                    else if ( status.run > 1 )                              // STaRT
                    {
                        _adapter->SetMTCTagValue("execution", "EXECUTING"); // == eACTIVE
                    }

                    // Alarms are removed
#if 0
                    if ( status.CFS15D.emergency == 0 )
                    {
                        mAlarm.setValue(Alarm::eESTOP, "EMerGency", Alarm::eCRITICAL, Alarm::eCLEARED, "EMerGency Status Set");
                    }
                    else
                    {
                        mAlarm.setValue(Alarm::eESTOP, "EMerGency", Alarm::eCRITICAL, Alarm::eACTIVE, "EMerGency Status Set");
                    }

                    if ( status.CFS15D.alarm == 0 )
                    {
                        mAlarm.setValue(Alarm::eOTHER, "ALarM", Alarm::eERROR, Alarm::eCLEARED, "ALarM Status Set");
                    }
                    else
                    {
                        mAlarm.setValue(Alarm::eOTHER, "ALarM", Alarm::eERROR, Alarm::eACTIVE, "ALarM Status Set");
                    }
#endif

                    // ////////????????????????
                    // else if (status.warning == 1)
                    //  mAlarm.setValue(Alarm::eOTHER, "WaRNing", Alarm::eWARNING, Alarm::eACTIVE, "WaRNing Status Set");
                }
                else
                {
                    _adapter->disconnect( );
                }
                GLogger.LogMessage("15iSeries::getStatus Leave\n", 3);
                return EW_OK;
            }

            int CFS15D::getSpeeds( )
            {
                GLogger.LogMessage("15iSeries::getSpeeds enter\n", 3);

                if ( !_adapter->mConnected )
                {
                    return -1;
                }

                double spindlespeed = 0, feedrate = 0;

                ODBSPEED speed;
                /* Data type. 0 ( feed rate ), 1 ( spindle speed  ) , -1 (  all  ) */
                short ret = cnc_rdspeed(_adapter->mFlibhndl, -1, &speed);

                if ( ret == EW_OK )
                {
                    spindlespeed = speed.acts.data;

                    if ( speed.acts.dec > 0 )
                    {
                        spindlespeed = spindlespeed / pow(10.0, (double) speed.acts.dec);
                    }

                    feedrate = speed.actf.data;

                    if ( speed.actf.dec > 0 )
                    {
                        feedrate = feedrate / pow(10.0, (double) speed.actf.dec);
                    }
                }
            }

            _adapter->SetMTCTagValue("path_feedratefrt", StdStringFormat("%8.4f", feedrate) );
            _adapter->SetMTCTagValue("Srpm", StdStringFormat("%8.4f", spindlespeed) );

#if 0
            ODBACT buf;
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
                        GLogger.LogMessage(StdStringFormat("Spindle[%d]=%ld\n", buf2[0].data), 3);
                    }
                }
            }
            catch ( ... )
            {
                GLogger.LogMessage(StdStringFormat("cnc_acts problem\n"), -1);
            }
#endif

#if 0
            ODBCMD buf;
            short num_cmd = 1;
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
                        _adapter->SetMTCTagValue("Srpm", StdStringFormat("%8.4f", spindlespeed / ( buf.dec_val * 10.0) ) );
                    }

                    // spindlespeed=spindlespeed /(buf.dec_val*10.0);
                }
                else
                {
                    spindlespeed = speed.actf.data;
                }
            }
            catch ( ... )
            {
                GLogger.Fatal("cnc_rdcommand speed problem\n");
            }

            if ( ret == EW_OK )
            {
                // _adapter->mPathFeedrate.setValue(speed.actf.data);
                _adapter->SetMTCTagValue("path_feedratefrt", StdStringFormat("%8.4f", speed.actf.data) );

                // The spindle speed is one of the main spindle that is dispalyed in the position screen of the CNC.
                // HACK just use actual feed.
                _adapter->SetMTCTagValue("Srpm", StdStringFormat("%8.4f", spindlespeed) );

                // _adapter->mSpindleSpeed.setValue(spindlespeed);
                // Reads the actual rotational speed of the spindle connected to CNC
                // mSpindleSpeed.setValue(buf.data);
            }
            else
            {
                _adapter->disconnect( );
            }
#endif
            GLogger.LogMessage("15iSeries::getSpeeds done\n", 3);

            GLogger.LogMessage("15iSeries::getSpindleLoad\n", 5);
#if 1
            short nspd;
            ret = cnc_rdnspdl(_adapter->mFlibhndl, &nspd);

            if ( ret != EW_OK )
            {
                LOGONCE GLogger.Fatal("15iSeries::cnc_rdnspdl Failed\n");
                return -1;
            }

            ODBSPN *load = static_cast<ODBSPN *>( alloca(nspd * sizeof( ODBSPN ) ) );

            // ODBSPLOAD load[MAX_SPINDLES];   /* 4 = maximum spinlde number */
            ret = cnc_rdspload(_adapter->mFlibhndl, -1, load);

            if ( ret != EW_OK )
            {
                LOGONCE GLogger.Fatal(StdStringFormat("15iSeries::cnc_rdspload Failed\n") );
                return ret;
            }
            _adapter->SetMTCTagValue("Sload", StdStringFormat("%8.4f", load[0].data[0]) );

            // _adapter->mSpindleSpeedLoad.setValue(load[0].data[0]);
#endif
            GLogger.LogMessage("15iSeries::getLoads Leave\n", 3);
            return EW_OK;
            }

            int CFS15D::getVersion( )
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
                GLogger.LogMessage("FanucMTConnectAdapter::getVersion Enter\n", HEAVYDEBUG);
                ODBSYS sysinfo;
                short ret = cnc_sysinfo(_adapter->mFlibhndl, &sysinfo); // 15,16,18,21,0,powermate

                if ( ret == EW_OK )
                {
                    std::string versioninfo = StdStringFormat("CNC=%c%c Series=%c%c%c%c Version=%c%c%c%c ", sysinfo.cnc_type[0], sysinfo.cnc_type[1],
                        sysinfo.series[0], sysinfo.series[1], sysinfo.series[2], sysinfo.series[3],
                        sysinfo.version[0], sysinfo.version[1], sysinfo.version[2], sysinfo.version[3]
                        );
                    GLogger.Fatal(versioninfo);
#if 0
                    if ( AutoVersionDetect )
                    {
                        if ( std::string(sysinfo.cnc_type) == "15" )
                        {
                            Globals.FanucVersion = "15i";
                        }
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
                    GLogger.Warning(StdStringFormat("Error: FanucMTConnectAdapter::cnc_sysinfo=%d\n", ret) );
                }
                return EW_OK;
            }
            void CFS15D::disconnect( )
            {
                LOGONCE GLogger.Status(StdStringFormat("15iSeries::disconnect Using Protocol=%s\n", Globals.FanucProtocol.c_str( ) ) );

                if ( _adapter->mConnected )
                {
                    _adapter->SetMTCTagValue("power", "OFF");

                    // _adapter->mPower.setValue(PowerState::eOFF);
                    cnc_freelibhndl(_adapter->mFlibhndl);
                    _adapter->mConnected = false;
                }
            }

            int CFS15D::connect( )
            {
                LOGONCE GLogger.Status(StdStringFormat("15iSeries::connect Using Protocol=%s\n", Globals.FanucProtocol.c_str( ) ) );

                short ret;
                _adapter->mFlibhndl = 0;

                if ( Globals.FanucProtocol == "HSSB" )
                {
                    GLogger.LogMessage(StdStringFormat("iSeries::connect HSSB cnc_allclibhndl2 Port=%d\n", _adapter->mDevicePort), FATAL);

                    // GLogger << FATAL  << "THIS EXECUTABLE DOES NOT HANDLE HSSB - EXCLUSIVELY Focas w/ Processing library for TCP/IP\n";
#ifdef SERIESHSSB
                    ret = ::cnc_allclibhndl2(_adapter->mDevicePort, &_adapter->mFlibhndl);
#else
                    GLogger << FATAL << "THIS EXECUTABLE DOES NOT HANDLE LAN - EXCLUSIVELY Focas w/ Processing library for HSSB" << std::endl;
                    return -1;
#endif
                }
                else
                {
                    GLogger.LogMessage(StdStringFormat("iSeries::connect LAN cnc_allclibhndl3 IP=%s  Port=%d\n", _adapter->mDeviceIP.c_str( ), _adapter->mDevicePort), FATAL);
#ifdef SERIESLAN
                    ret = ::cnc_allclibhndl3(_adapter->mDeviceIP.c_str( ), _adapter->mDevicePort, 10, &_adapter->mFlibhndl);
#else
                    GLogger << FATAL << "THIS EXECUTABLE DOES NOT HANDLE LAN - EXCLUSIVELY Focas w/ Processing library for HSSB" << std::endl;
                    return -1;
#endif
                }

                /*if(Globals.FanucProtocol == "HSSB")
                {
                    ret =  ::cnc_allclibhndl2(_adapter->mDevicePort,&_adapter->mFlibhndl);
                }
                else
                {
                    GLogger.LogMessage(StdStringFormat("15iSeries::connect LAN cnc_allclibhndl3 IP=%s  Port=%d\n",_adapter->mDeviceIP.c_str(), _adapter->mDevicePort ), HEAVYDEBUG);
                    ret = ::cnc_allclibhndl3(_adapter->mDeviceIP.c_str(), _adapter->mDevicePort, 10, &_adapter->mFlibhndl);
                }*/

                GLogger.LogMessage(StdStringFormat("15iSeries::connect cnc_allclibhndl3 Result: %d\n", ret), HEAVYDEBUG);

                if ( ret != EW_OK )
                {
                    LogErrorMessage("15iSeries::connect cnc_allclibhndl3", ret);
                    _adapter->SetMTCTagValue("power", "OFF");
                    _adapter->SetMTCTagValue("avail", "UNAVAILABLE");
                    _adapter->mConnected = false;
                    LOGONCE GLogger.Fatal(StdStringFormat("15iSeries::connect FAILED Power = OFF\n", ret) );
                    return -1;
                }

                GLogger.LogMessage(StdStringFormat("15iSeries::connect connected ON\n", ret), HEAVYDEBUG);
                _adapter->mConnected = true;
                _adapter->SetMTCTagValue("power", "ON");
                _adapter->SetMTCTagValue("avail", "AVAILABLE");
                return 0;
            }
            int CFS15D::LogErrorMessage(std::string msg, short errcode)
            {
                if ( errcode == EW_OK )
                {
                    return 0;
                }
                std::stringstream str;
                str << msg + " Error : ";

                switch ( errcode )
                {
                // case  EW_PROTOCOL: str<< "EW_PROTOCOL - protocol error";break;
                // case EW_SOCKET: str<< "EW_SOCKET - Windows socket error ";break;
                case EW_HANDLE:
                    { str << "EW_HANDLE  - DLL not exist error";
                    }
                    break;

                case EW_VERSION:
                    { str << "EW_VERSION - CNC/PMC version missmatch";
                    }
                    break;

                case EW_UNEXP:
                    { str << "EW_UNEXP - abnormal error";
                    }
                    break;

                case EW_NODLL:
                    { str << "EW_NODLL";
                    }
                    break;

                case EW_BUS:
                    { str << "EW_BUS - bus error";
                    }
                    break;

                case EW_SYSTEM2:
                    { str << "EW_SYSTEM2 - system error";
                    }
                    break;

                case EW_HSSB:
                    { str << "EW_HSSB - hssb communication error";
                    }
                    break;

                case EW_SYSTEM:
                    { str << "EW_SYSTEM";
                    }
                    break;

                case EW_MMCSYS:
                    { str << "EW_MMCSYS - emm386 or mmcsys install error";
                    }
                    break;

                case EW_RESET:
                    { str << "EW_RESET - reset or stop occured error";
                    }
                    break;

                case EW_BUSY:
                    { str << "EW_BUSY - busy error";
                    }
                    break;

                case EW_PARITY:
                    { str << " EW_PARITY - shared RAM parity error ";
                    }
                    break;

                case EW_FUNC:
                    { str << "EW_FUNC - command prepare error"; // 1
                      /*case EW_NOPMC:*/ str << " or EW_NOPMC - pmc not exist";
                    }
                    break;                                           // 1

                case EW_LENGTH:
                    { str << "EW_LENGTH - data block length error";
                    }
                    break;                                               // 2

                case EW_NUMBER:
                    { str << "EW_NUMBER - data number error"; // 3
                      /*case EW_RANGE:*/ str << " or EW_RANGE -address range error";
                    }
                    break;                                                 // 3

                case EW_ATTRIB:
                    { str << "EW_ATTRIB - data attribute error";  // 4
                      /*case EW_TYPE:*/ str << " or EW_TYPE -data type error";
                    }
                    break;                                           // 4

                case EW_DATA:
                    { str << "EW_DATA - data error";
                    }
                    break;                              // 5

                case EW_NOOPT:
                    { str << "EW_NOOPT - No option The extended driver/library function is necessary.";
                    }
                    break;                                                                                   // 6

                case EW_PROT:
                    { str << "EW_PROT - write protect error";
                    }
                    break;                                        // 7

                case EW_OVRFLOW:
                    { str << "EW_OVRFLOW - memory overflow error";
                    }
                    break;                                               // 8

                case EW_PARAM:
                    { str << "EW_PARAM - cnc parameter not correct error";
                    }
                    break;                                                     // 9

                case EW_BUFFER:
                    { str << "EW_BUFFER - buffer error";
                    }
                    break;                                     // 10

                case EW_PATH:
                    { str << "EW_PATH - path error";
                    }
                    break;                               // 11

                case EW_MODE:
                    { str << "EW_MODE - cnc mode error";
                    }
                    break;                                  // 12

                case EW_REJECT:
                    { str << "EW_REJECT - execution rejected error";
                    }
                    break;                                                 // 13

                case EW_DTSRVR:
                    { str << "EW_DTSRVR - data server error";
                    }
                    break;                                          // 14

                case EW_ALARM:
                    { str << "EW_ALARM - alarm has been occurred";
                    }
                    break;                                             // 15
//	case EW_STOP: str<< "EW_STOP -CNC is not running";  break; // 16
//	case EW_PASSWD: str<< "EW_PASSWD -  protection data error ";  break; // 17
                }

                str << std::endl;
                GLogger.LogMessage(str.str( ).c_str( ), 3);
                return -1;
            }
