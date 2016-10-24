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

#ifndef FANUC_ADAPTER_HPP
#define FANUC_ADAPTER_HPP

#include "adapter.hpp"
#include "device_datum.hpp"
#include "Service.hpp"
#include <map>


#ifdef F15i
class CFS15D;
#endif
#ifdef iSERIES
class CiSeries;
#endif
#ifdef F15M	
class CF15B;
#endif


/* 
 * Provides a connection to the data available from the FANUC Focus library.
 */
class FanucMTConnectAdapter : public Adapter , public MTConnectService
{
protected:

#ifdef F15i
	CFS15D * _FS15D;
	friend  CFS15D;
#endif

#ifdef F15M	
	CF15B * _F15M;
 	friend CF15B;
#endif

#ifdef iSERIES
	CiSeries * _iSeries;
	friend CiSeries;
#endif

 /* Define all the data values here */
  

  /* Events */
  //Alarm mAlarm;
  PowerState mPower;
  Execution mExecution;
  IntEvent mLine; 
  ControllerMode mMode;
  Event mProgram;

  enum { X=1, Y=2, Z=4, A=8, B=16, C=32 };
  //IntEvent mPartCount; // We have to figure this out

  /* Samples */
  /* Linear Axis */
  Sample mXact;
  Sample mYact;
  Sample mZact;
  Sample mAact;
  Sample mBact;
  Sample mCact;
  
  Sample mXload;
  Sample mYload;
  Sample mZload;
  Sample mAload;
  Sample mBload;
  Sample mCload;


  /* Spindle */
  Sample mSpindleSpeed;  
  Sample mSpindleSpeedOvr;  
  Sample mSpindleSpeedLoad;  
  
  /* Path Feedrate */
  Sample mPathFeedrate;
  Sample mPathFeedrateOvr;
 
  unsigned short mFlibhndl;
  bool mConnected;
  int mDevicePort;
  const char *mDeviceIP;

  int _nAxes;
  std::map<char,int> axisnum;


  virtual  void start();
  virtual  void stop();
  virtual  void initialize(int aArgc, const char *aArgv[]);
  static int LogErrorMessage(std::string msg, short errcode);
	void disconnect();

protected:
 void getVersion();
  void getSpeeds();
  void getLine();
  void getStatus();
  bool IsCNCRunning();

public:
  FanucMTConnectAdapter(int aServerPort, const char *aDeviceIP, int aDevicePort, int delay=100);
  ~FanucMTConnectAdapter();
 
  virtual void gatherDeviceData();
  static std::string getProgramName(char * buffer) ;
};

#endif