/*
 * Copyright (c) 2008, AMT – The Association For Manufacturing Technology (“AMT”)
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

#include "targetver.h"

#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/thread.hpp>

#include "internal.hpp"
#include "fake_adapter.hpp"

#include "LinuxGlobals.h"

#include "CrclInterface.h"
static boost::asio::io_service ios;

FakeAdapter::FakeAdapter(int aPort)
: Adapter(aPort, 1000),
mAvailability("avail"),
mSystem("system"),
mExecution("exec"),
mPower("power"),
mMode("controllermode"),
mPosition("position"),
mXorient("xorient"),
mZorient("zorient"),
mHeartbeat("heartbeat"),
mlast_update("last_update"),
mProgram("program"),
mAlarm("alarm"),
xmlClient(&ios) {
    addDatum(mAvailability);
    addDatum(mSystem);
    addDatum(mExecution);
    addDatum(mPower);
    addDatum(mMode);
    addDatum(mPosition);
    addDatum(mXorient);
    addDatum(mZorient);
    addDatum(mHeartbeat);
    addDatum(mlast_update);
    addDatum(mProgram);
    addDatum(mAlarm);
    _heartbeat = 0;
   
}

FakeAdapter::~FakeAdapter() {
}

void FakeAdapter::SocketInit(std::string ip,std::string port)
{
	// xmlClient.Init("127.0.0.1", "64444");
	 _socketport=port;
	 _socketip=ip;	 
	 xmlClient.Init(ip, port);
}
boost::asio::io_service * FakeAdapter::GetIos() {
    return &ios;
}
void FakeAdapter::initialize(int aArgc, const char *aArgv[]) {
    MTConnectService::initialize(aArgc, aArgv);
    if (aArgc > 1) {
        mPort = atoi(aArgv[1]);
    }
}

void FakeAdapter::start() {
    startServer();
}

void FakeAdapter::stop() {
    stopServer();
}

void FakeAdapter::decode(std::string msg) {
    if (xmlClient.FindLeadingElement(msg) == "</CRCLStatus>") {
        CrclInterface crcl;
        crcl.ParseCRCLStatusString(msg);
        if (crcl._status.CommandState == "Done")
            mExecution.setValue(Execution::eREADY);
        else if (crcl._status.CommandState == "Working")
            mExecution.setValue(Execution::eACTIVE);
        else if (crcl._status.CommandState == "Error")
            mExecution.setValue(Execution::eREADY);
        mAlarm.setValue("");
        mProgram.setValue("");
        mPosition.setValue(crcl._status.Point[0], crcl._status.Point[1], crcl._status.Point[2]);
        mXorient.setValue(crcl._status.XAxis[0], crcl._status.XAxis[1], crcl._status.XAxis[2]);
        mZorient.setValue(crcl._status.ZAxis[0], crcl._status.ZAxis[1], crcl._status.ZAxis[2]);
    };
}

void FakeAdapter::disconnect() {
    Globals::ErrorMessage("Disconnected\n");
    mAvailability.unavailable();
}
void FakeAdapter::connect() {
    Globals::ErrorMessage("Connect\n");
    xmlClient.Connect(); // async connect
}
void FakeAdapter::SocketConnectReadDeviceData() {

    while (1) {
        Globals::Sleep(100);
         //ios.run();
        try {
            ios.poll_one();
            if (!xmlClient.IsConnected()) {
                //if (!xmlClient.IsWaitingToConnect())
                xmlClient.Connect();
                //if (!xmlClient.IsConnected()) Globals::Sleep(2000);
            } else {
                std::string statcmd = CrclInterface().CRCLGetStatusCmd();
                xmlClient.SyncWrite(statcmd);
                Globals::Sleep(100);
                for (int i = 0; i < 10 && (xmlClient.SizeMsgQueue() < 1); i++) {
                    //ios.run_one();
                    boost::this_thread::yield();
                    Globals::Sleep(100);
                }
                if (xmlClient.SizeMsgQueue() < 1) {
                    Globals::DebugMessage("No status read\n");
                }
                else
                {
                     Globals::DebugMessage("Read new status \n");
                }
            }
        } catch (std::exception e) {
            disconnect();
        } catch (...) {
            disconnect();
        }
    }
}


void FakeAdapter::gatherDeviceData() {
    mAvailability.available();
    //mHeartbeat.setValue(_heartbeat++);
    try {
        boost::this_thread::yield();
        // If nothing yet, give up
        if (xmlClient.SizeMsgQueue() < 1) {
            Globals::DebugMessage("No status returned for device\n");
            return;
        }
        //mlast_update.setValue(Globals::GetTimeStamp().c_str());
        std::string msg = xmlClient.LatestMsgQueue();
        decode(msg);

    } catch (std::exception e) {
        disconnect();
    } catch (...) {
        disconnect();
    }
}



