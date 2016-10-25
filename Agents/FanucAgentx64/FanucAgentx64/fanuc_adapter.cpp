/*
* Copyright (c) 2008, AMT  The Association For Manufacturing Technology (AMT)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the AMT nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* DISCLAIMER OF WARRANTY. ALL MTCONNECT MATERIALS AND SPECIFICATIONS PROVIDED
* BY AMT, MTCONNECT OR ANY PARTICIPANT TO YOU OR ANY PARTY ARE PROVIDED "AS IS"
* AND WITHOUT ANY WARRANTY OF ANY KIND. AMT, MTCONNECT, AND EACH OF THEIR
* RESPECTIVE MEMBERS, OFFICERS, DIRECTORS, AFFILIATES, SPONSORS, AND AGENTS
* (COLLECTIVELY, THE "AMT PARTIES") AND PARTICIPANTS MAKE NO REPRESENTATION OR
* WARRANTY OF ANY KIND WHATSOEVER RELATING TO THESE MATERIALS, INCLUDING, WITHOUT
* LIMITATION, ANY EXPRESS OR IMPLIED WARRANTY OF NONINFRINGEMENT,
* MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.

* LIMITATION OF LIABILITY. IN NO EVENT SHALL AMT, MTCONNECT, ANY OTHER AMT
* PARTY, OR ANY PARTICIPANT BE LIABLE FOR THE COST OF PROCURING SUBSTITUTE GOODS
* OR SERVICES, LOST PROFITS, LOSS OF USE, LOSS OF DATA OR ANY INCIDENTAL,
* CONSEQUENTIAL, INDIRECT, SPECIAL OR PUNITIVE DAMAGES OR OTHER DIRECT DAMAGES,
* WHETHER UNDER CONTRACT, TORT, WARRANTY OR OTHERWISE, ARISING IN ANY WAY OUT OF
* THIS AGREEMENT, USE OR INABILITY TO USE MTCONNECT MATERIALS, WHETHER OR NOT
* SUCH PARTY HAD ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES.
*/

#include "stdafx.h"
#include <math.h>

// #include "internal.hpp"
#include "fanuc_adapter.h"
#include <sstream>

// Michaloski
#include "Logger.h"
#include "EnumProc.h"
#include "Globals.h"
#include "StdStringFcn.h"

#ifdef F15i
#include "FS15D.h"
#endif
#ifdef iSERIES
#include "iSeries.h"
#endif
#ifdef F15M
#include "F15B.h"
#endif

#define LOADS

static void trans_func (unsigned int u, EXCEPTION_POINTERS *pExp)
{
    std::string errmsg = StdStringFormat("Fanuc Shdr trans_func - Code = 0x%x\n",
        pExp->ExceptionRecord->ExceptionCode);

    Logger.LogMessage(std::string("FanucMTConnectAdapter::gatherDeviceData Exception Raised\n" + errmsg).c_str( ), Logger.LERROR);
    throw std::exception(errmsg.c_str( ) );
}
FanucMTConnectAdapter::FanucMTConnectAdapter(int aPort, const char *aDeviceIP, int aDevicePort, int delay) :
    Adapter(aPort, delay),

    mMode("controllermode"),
    mPathFeedrate("path_feedratefrt"),
    mPathFeedrateOvr("path_feedrateovr"),

    mPower("power"),
    mExecution("execution"),
    mProgram("program"),
    mLine("line"),

    mXact("Xabs"),
    mYact("Yabs"),
    mZact("Zabs"),
    mAact("Adeg"),
    mBact("Bdeg"),
    mCact("Cdeg"),

    mXload("Xlod_percent"),
    mYload("Ylod_percent"),
    mZload("Zlod_percent"),
    mAload("Alod_percent"),
    mBload("Blod_percent"),
    mCload("Clod_percent"),

    mSpindleSpeed("Srpm"),
    mSpindleSpeedLoad("Slod_percent"),
    mSpindleSpeedOvr("Sovr")
{
// mAlarm("alarm"), // removed in version 1.2
    Logger.LogMessage("FanucMTConnectAdapter::FanucMTConnectAdapter\n", Logger.LDEBUG);
    mConnected = false;

    // addDatum(mAlarm);
    addDatum(mPower);
    addDatum(mExecution);
    addDatum(mLine);
    addDatum(mProgram);

    addDatum(mXact);
    addDatum(mYact);
    addDatum(mZact);
    addDatum(mAact);
    addDatum(mBact);
    addDatum(mCact);

    addDatum(mXload);
    addDatum(mYload);
    addDatum(mZload);
    addDatum(mAload);
    addDatum(mBload);
    addDatum(mCload);

    addDatum(mSpindleSpeed);
    addDatum(mPathFeedrate);
    addDatum(mMode);
    addDatum(mSpindleSpeedLoad);

    addDatum(mPathFeedrateOvr);
    addDatum(mSpindleSpeedOvr);

    mDevicePort = aDevicePort;
    mDeviceIP   = aDeviceIP;

    _nAxes = 3;

#ifdef F15M
    _F15M = new CF15B(this);
#endif
#ifdef F15i
    _FS15D = new CFS15D(this);
#endif
#ifdef iSERIES
    _iSeries = new CiSeries(this);
#endif
}
void FanucMTConnectAdapter::initialize (int aArgc, const char *aArgv[])
{
    OutputDebugString("FanucMTConnectAdapter::initialize()\n");
    MTConnectService::initialize(aArgc, aArgv);

    if ( aArgc > 1 )
    { }
}
void FanucMTConnectAdapter::start ( )
{
    OutputDebugString("FanucMTConnectAdapter::start()\n");
    startServer( );
}
void FanucMTConnectAdapter::stop ( )
{
    OutputDebugString("FanucMTConnectAdapter::stop()\n");
    disconnect( );
    stopServer( );
}
void FanucMTConnectAdapter::disconnect ( )
{
#ifdef F15i
    _FS15D->disconnect( );
#endif
#ifdef F15M
    _F15M->disconnect( );
#endif
#ifdef iSeries
    _iSeries->disconnect( );
#endif
}
FanucMTConnectAdapter::~FanucMTConnectAdapter( )
{
    disconnect( );
}
#pragma comment(lib,"psapi")
bool FanucMTConnectAdapter::IsCNCRunning ( )
{
    // Check for local machine cnc pid running.
    if ( Globals.szFanucExe.size( ) > 0 )
    {
        if ( !CProcessIterator::FindProcess( (LPCSTR) Globals.szFanucExe.c_str( ) ) )
        {
            return 0;
        }
    }

    // Check for remote machine cnc pid running - ouch!
    return 1;
}
void FanucMTConnectAdapter::gatherDeviceData ( )
{
    Logger.LogMessage("FanucMTConnectAdapter::gatherDeviceData\n", Logger.HEAVYDEBUG);
    _set_se_translator(trans_func);    // correct thread?
    try
    {
#if 0
        if ( !this->IsCNCRunning( ) )
        {
            ::Sleep(2000);
            return;
        }
#endif
        ::Sleep(1000);

#ifdef F15i
        if ( !mConnected )
        {
            _FS15D->connect( );
        }

        if ( mConnected )
        {
            // if(Globals.FanucVersion == "15i")
            {
                _FS15D->getVersion( );
                _FS15D->getInfo( );
                _FS15D->getLine( );
                _FS15D->getPositions( );
                _FS15D->getStatus( );
                _FS15D->getSpeeds( );
            }
        }
#else
#ifdef F15M
        if ( !mConnected )
        {
            _F15M->connect( );
        }

        if ( mConnected )
        {
            // if(Globals.FanucVersion == "15M")
            {
                _F15M->saveStateInfo( );
                _F15M->getVersion( );
                _F15M->getInfo( );
                _F15M->getLine( );
                _F15M->getPositions( );
                _F15M->getStatus( );

                // hack in getSpeeds
                _F15M->getSpeeds( );
            }
        }
#else
        if ( !mConnected )
        {
            _iSeries->connect( );
        }

        if ( mConnected )
        {
            // else
            {
                _iSeries->getVersion( );
                _iSeries->getInfo( );
                _iSeries->getLine( );
                _iSeries->getPositions( );
                _iSeries->getStatus( );
                _iSeries->getSpeeds( );
            }
        }
#endif
#endif
    }
    catch ( ... )
    {
        Logger.LogMessage("FanucMTConnectAdapter::gatherDeviceData Exception Raised\n", Logger.WARNING);
        disconnect( );
    }
}
// static std::string DumpODBPOS(ODBPOS & pos)
// {
//	std::string tmp;
//	tmp+= StrFormat("Abs  Data=%d Dec=%d Unit=%d Display=%d Axis=%c Suff=%c\n",
//		pos.abs.data, pos.abs.dec, pos.abs.unit, pos.abs.disp, pos.abs.name, pos.abs.suff);
//	tmp+= StrFormat("Mach Data=%d Dec=%d Unit=%d Display=%d Axis=%c Suff=%c\n",
//		pos.abs.data, pos.mach.dec, pos.mach.unit, pos.mach.disp, pos.mach.name, pos.mach.suff);
//	tmp+= StrFormat("Rel  Data=%d Dec=%d Unit=%d Display=%d Axis=%c Suff=%c\n",
//		pos.rel.data, pos.rel.dec, pos.rel.unit, pos.rel.disp, pos.rel.name, pos.rel.suff);
//	tmp+= StrFormat("Dist Data=%d Dec=%d Unit=%d Display=%d Axis=%c Suff=%c\n",
//		pos.dist.data, pos.dist.dec, pos.dist.unit, pos.dist.disp, pos.dist.name, pos.dist.suff);
//	return tmp;
// }

std::string FanucMTConnectAdapter::getProgramName (char *buf)
{
    Logger.LogMessage("FanucMTConnectAdapter::getProgramName Enter\n", 3);

    int               prognum;
    std::string       progname;
    std::stringstream ss(buf);
    std::string       sLine;

    while ( std::getline(ss, sLine, '\n') )
    {
        //  look for O # which must be there
        if ( 1 != sscanf(sLine.c_str( ), "O%d", &prognum) )
        {
            continue;
        }

        progname = StdStringFormat("O%d", prognum);
#if 0

        // if find comment inside (...) return as program name
        int n = sLine.find("(");
        int m = sLine.find(")");

        if ( ( std::string::npos != n ) && ( std::string::npos != m ) )
        {
            return sLine.substr(n + 1, m - n - 1);
        }

        // else could be standalone  on next line
        if ( std::getline(ss, sLine, '\n') )
        {
            n = sLine.find("(");
            m = sLine.find(")");

            if ( ( std::string::npos != n ) && ( std::string::npos != m ) )
            {
                return sLine.substr(n + 1, m - n - 1);
            }
        }
#endif
        return progname;
    }

    Logger.LogMessage("FanucMTConnectAdapter::getProgramName Leave\n", 3);

    return "";
}
