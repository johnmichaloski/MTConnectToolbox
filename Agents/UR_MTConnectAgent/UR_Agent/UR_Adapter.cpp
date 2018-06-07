
//
// UR_Adapter.cpp
//

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#include "stdafx.h"
#define BOOST_ALL_NO_LIB

#include "UR_Adapter.h"
#include "UR_Agent.h"

#include "ATLComTime.h"
#include "Agent.hpp"
#include "Device.hpp"
#include "config.hpp"
#include <algorithm>
#include <fstream>                                         // ifstream

#include <share.h>
#include <stdio.h>
#include <stdlib.h>

#include "Globals.h"
#include "NIST/File.h"
#include "NIST/HexDump.h"
#include "NIST/Logger.h"
#include "NIST/Logger.h"
#include "NIST/StdStringFcn.h"
#include "NIST/WinTricks.h"

using namespace Nist;
using namespace ur;

/////////////////////////////////////////////////////////////////////
static void trans_func (unsigned int u, EXCEPTION_POINTERS *pExp)
{
    std::string errmsg
        = StdStringFormat("UR_Adapter In trans_func - Code = 0x%x\n",
                          pExp->ExceptionRecord->ExceptionCode);

    OutputDebugString(errmsg.c_str( ));
    throw std::exception(errmsg.c_str( ), pExp->ExceptionRecord->ExceptionCode);
}

static const std::string CondLevels[] =
{
    "normal",
    "warning",
    "fault",
    "unavailable"
};


/////////////////////////////////////////////////////////////////////

AdapterT::AdapterT(AgentConfigurationEx *mtcagent,         // mtconnect agent
                   Nist::Config & config, std::string device)
    :

    mAgentconfig(mtcagent),
    mDevice(device), mConfig(config)
{
    mServerRate        = 1000;
    mQueryServerPeriod = 10000;
}
void AdapterT::doStop ( ) { mRunning = false; }
bool AdapterT::isValidCondition(std::string val)
{
	std::transform(val.begin( ), val.end( ), val.begin( ), tolower);

	for(size_t i=0; i < sizeof(CondLevels) / sizeof(CondLevels[0]); i++)
	{
		if(val == CondLevels[i])
			return true;
	}
	return false;
}

// NORMAL, WARNING, FAULT, or UNAVAILABLE
void AdapterT::setMTCConditionValue (std::string tag, std::string value)
{
	items.setTag(tag, value);

	Agent *agent = mAgentconfig->getAgent( );

	if ( agent == NULL )
	{
		logError("AdapterT::setMTCTagValue for %s NULL Agent Pointer\n",
			mDevice.c_str( ));
		return;
	}

	Device *pDev = agent->getDeviceByName(mDevice);

	if ( pDev == NULL )
	{
		logError("AdapterT::setMTCTagValue for %s NULL Device Pointer\n",
			mDevice.c_str( ));
		return;
	}
	DataItem *di = pDev->getDeviceDataItem(tag);

	if ( di != NULL )
	{
		std::string time = getCurrentTime(GMT_UV_SEC);
		agent->addToBuffer(di, value, time);
	}
	else
	{
		logError(" (%s) Could not find condition data item: %s  \n", mDevice.c_str( ),
			tag.c_str( ));
	}
}
void AdapterT::setMTCTagValue (std::string tag, std::string value)
{
    items.setTag(tag, value);

    Agent *agent = mAgentconfig->getAgent( );

    if ( agent == NULL )
    {
        logError("AdapterT::setMTCTagValue for %s NULL Agent Pointer\n",
                 mDevice.c_str( ));
        return;
    }

    Device *pDev = agent->getDeviceByName(mDevice);

    if ( pDev == NULL )
    {
        logError("AdapterT::setMTCTagValue for %s NULL Device Pointer\n",
                 mDevice.c_str( ));
        return;
    }
    DataItem *di = pDev->getDeviceDataItem(tag);

    if ( di != NULL )
    {
        std::string time = getCurrentTime(GMT_UV_SEC);
        agent->addToBuffer(di, value, time);
    }
    else
    {
        logError(" (%s) Could not find data item: %s  \n", mDevice.c_str( ),
                 tag.c_str( ));
    }
}
void AdapterT::addUrdfAsset (std::string urdfstr)
{
    std::string aId = this->mDevice + "_urdf_asset";

    // remove the <?xml version="1.0" ?> as it messes up web browsers
    size_t n = urdfstr.find("?>");

    if ( n != std::string::npos )
    {
        urdfstr = urdfstr.substr(n + 2);
    }

    // <DataItem id="m1_urdf_asset" type="ASSET_CHANGED" category="EVENT"/>
    std::string aBody = urdfstr;
    std::string type  = "Robot";

    ::Device *device = mAgentconfig->getAgent( )->getDeviceByName(mDevice);

    mAgentconfig->getAgent( )->addAsset(device, aId, aBody, type);
}
void AdapterT::resetOff ( )
{
    for ( int i = 0; i < items.size( ); i++ )
    {
        items[i]->mValue     = "UNAVAILABLE";
        items[i]->mLastvalue = "UNAVAILABLE";
        setMTCTagValue(items[i]->mTagname, "UNAVAILABLE");
    }

    setMTCTagValue("avail", "AVAILABLE");
    setMTCTagValue("power", "OFF");
}
void AdapterT::resetOn ( )
{
    setMTCTagValue("avail", "AVAILABLE");
    setMTCTagValue("power", "ON");
}
UR_Adapter::Item * UR_Adapter::createItem (std::string tag, std::string type)
{
    Item *item = new Item( );

    item->mType    = _T("Event");
    item->mTagname = tag;
    items.push_back(item);
	return item;
}
std::string UR_Adapter::dumpHeader ( )
{
    std::string tmp;

    for ( size_t i = 0; i < items.size( ); i++ )
    {
        tmp += StdStringFormat("%s,", items[i]->mTagname.c_str( ));
    }
    tmp += "\n";
    return tmp;
}
std::string UR_Adapter::dumpDataItems ( )
{
    std::string tmp;

    for ( size_t i = 0; i < items.size( ); i++ )
    {
        tmp += StdStringFormat("%s,", items[i]->mValue.c_str( ));
    }
    tmp += "\n";
    return tmp;
}
void UR_Adapter::doConfig ( )
{
    // GLogger.LogMessage(StdStringFormat("UR_Adapter::Config() for IP =
    // %s\n",mDevice.c_str()));
    std::string cfgfile = Globals.mInifile;
    try
    {
         mConfig.load(cfgfile);
         mServerRate
            = mConfig.GetSymbolValue<int>(mDevice + ".ServerRate", Globals.mServerRate);
        mQueryServerPeriod
            = mConfig.GetSymbolValue<int>(mDevice + ".QueryServer", Globals.mQueryServer);
        mIp         = mConfig.GetSymbolValue<std::string>(mDevice + ".ip", "127.0.0.1");
		mPort       = mConfig.GetSymbolValue<std::string>(mDevice + ".port", "30002");
        mJointnames = mConfig.GetTokens(mDevice + ".jointnames", ",");
        mUrdfFile
            = mConfig.GetSymbolValue<std::string>(mDevice + ".urdf", "ur3.urdf");
		mUrVersion
            = mConfig.GetSymbolValue<float>(mDevice + ".Version", "3.2");
		mUrData.setVersion(mUrVersion);

		logDebug("Device %s\n", mDevice.c_str());
		logDebug("\tAdapter %s Server Rate=%d\n", mDevice.c_str(), mServerRate);
		logDebug("\tAdapter %s Reconnect Rate=%d\n", mDevice.c_str(), mQueryServerPeriod);
		logDebug("\tAdapter %s IP=%s\n", mDevice.c_str(), mIp.c_str());
		logDebug("\tAdapter %s Urdf=%s\n", mDevice.c_str(), mUrdfFile.c_str());
		logDebug("\tAdapter %s Version=%f\n", mDevice.c_str(), mUrVersion);
	}
    catch ( std::exception errmsg )
    {
        logAbort("Could not find ini file for device %s\n", mDevice.c_str( ));
    }
    catch ( ... )
    {
        logAbort("Could not find ini file for device %s\n", mDevice.c_str( ));
    }
}
void UR_Adapter::doStop ( )
{
    mUrComm.halt( );                                       // this might take too long for service, maybe just  end loop
    // mUrComm.mKeepalive=false;
    AdapterT::doStop( );
}
void UR_Adapter::doCycle ( )
{
    doConfig( );
    mCycleCnt = 0;

    _set_se_translator(trans_func);                        // correct thread?
 
    // The universal robot communication connection to this adapter is inited.
    mUrComm.init(this, mIp, mPort);

    // There was a query to the primay socket, but was removed.
    // Only read version info, which is supplied in the second socket.


	// Create tagnames to communicate with agent (only sample and event)
	mTagnames = mAgentconfig->mTagnames[mDevice];
	mConditionTags= mAgentconfig->mConditionTags[mDevice];

    for ( size_t i = 0; i < mTagnames.size( ); i++ )
        createItem(mTagnames[i]);

   for ( size_t i = 0; i < mConditionTags.size( ); i++ )
   {
            Item *item = createItem(mConditionTags[i]);
			item->mType="Condition";
   }
    
    // Send this robot URDF to agent to cache as  asset
    // It is read from a file, not retrieved directly from ROS
    std::ifstream fin(( File.ExeDirectory( )  + "URDF\\" + mUrdfFile ).c_str( ));

    if ( fin.is_open( ) )
    {
        std::string contents(( std::istreambuf_iterator<char>(fin) ),
                             std::istreambuf_iterator<char>( ));
        addUrdfAsset(contents);
    }

    // Heartbeat indicates that the adapter process is cycling and not dead.
    int nHeartbeat = 0;

//#ifdef _DEBUG
//
//    // If in debug mode save all tag updates to csv file. This sets up header.
//    tagfile.open(File.ExeDirectory( ) + "tags.csv", std::ofstream::out);
//
//    if ( !tagfile.is_open( ) )
//    {
//        logWarn("tagfile.open failed");
//
//        // This should prevent any exceptions or other stuff.
//        tagfile.setstate(std::ios_base::badbit);
//    }
//
//    // Unclear what happens if tagfile creation fails, and we try to write to it.
//    tagfile << dumpHeader( ).c_str( );
//#endif

    mRunning = true;
    resetOff( );

    // Start the universal robot communication connection to this adapter.
    mUrComm.start( );
	::Sleep(100);

    while ( mRunning )
    {
        HRESULT hr;
        try
        {
            setMTCTagValue("heartbeat", StdStringFormat("%d", nHeartbeat++));
            setMTCTagValue("avail", "AVAILABLE");
            hr = gatherDeviceData( );

            if ( FAILED(hr) )
            {
                resetOff( );
                ::Sleep(this->mQueryServerPeriod);
                continue;
            }
            ::Sleep(mServerRate);
        }
        catch ( std::exception e )
        {
            resetOff( );
        }
        catch ( ... )
        {
            resetOff( );
        }
    }
}
HRESULT UR_Adapter::gatherDeviceData ( )
{
    USES_CONVERSION;
    HRESULT           hr = S_OK;
    ur_message_header hdr;
    COleDateTime      today;
    try
    {
        //if ( mUrComm.mConnected.get() == 0)
        if ( mUrComm.mConnected == false)
        {
             mUrQMsgs.ClearMsgQueue();
            LOG_ONCE(
                logError("%s UR device: socket not connected\n", mDevice.c_str( ))
				);
            return E_FAIL;
        }

        while ( mUrQMsgs.SizeMsgQueue( ) > 0 )
        {
			std::vector<uint8_t> msg = mUrQMsgs.PopFrontMsgQueue( );
			hdr.read(&msg[0]);
 
			if ( mCycleCnt == 0 )
			{
				mCycleCnt++;
				char *pMsg = (char *) &msg[0];
				mUrData._ur_version_message.decode(pMsg, msg.size( ));

				logStatus("Version = %f\n", mUrData._ur_version_message.getVersion( ));
				logStatus(mUrData._ur_version_message.print( ).c_str( ));
				// Revise version info for this adapter...
				//mUrData.setVersion(mUrData._ur_version_message.getVersion( ));
            }
            else
            {
				//logDebug(Nist::HexDump (&msg[0], 32).c_str());
                mUrData.unpack(&msg[0], msg.size( ));
                //logDebug(mUrData.dump( ).c_str( ));
				 decodeUrState ( );
            }
		}
	}
	catch ( std::exception e )
	{
		logError("Exception in %s - UR_Adapter::GatherDeviceData() %s\n",
			mDevice.c_str( ), (LPCSTR) e.what( ));
		resetOff( );
		doDisconnect( );
		hr = E_FAIL;
	}
	catch ( ... )
	{
		logError("Exception in %s - UR_Adapter::GatherDeviceData()\n",
			mDevice.c_str( ));
		resetOff( );
		doDisconnect( );
		hr = E_FAIL;
	}
	return hr;
}
HRESULT UR_Adapter::decodeUrState ( )
{
    HRESULT           hr = S_OK;
	try {
		// Now you need to store the values into the tags

		// First do system condition - name attribute only id
		items.setTag(this->mDevice+"system", "NORMAL");

        // Power
        if ( mUrData._ur_robot_mode_data.isPowerOnRobot )
        {
            items.setTag("power", "ON");
        }
        else
        {
            items.setTag("power", "OFF");
        }

        // Estop event tag
        if ( mUrData._ur_robot_mode_data.isEmergencyStopped )
        {
            items.setTag("estop", "TRIGGERED");
        }
        else
        {
            items.setTag("estop", "ARMED");
        }

        // Execution mode
        if ( mUrData._ur_robot_mode_data.isProgramRunning )
        {
            items.setTag("execution", "EXECUTING");
        }
        else if ( mUrData._ur_robot_mode_data.isProgramPaused )
        {
            items.setTag("execution", "PAUSED");
        }
        else
        {
            items.setTag("execution", "IDLE");
        }

        items.setTag("version",
                     StdStringFormat("%f", mUrData._ur_version_message.getVersion( )));
        items.setTag("program", mUrData._ur_version_message.project_name);
        items.setTag("pose", mUrData._ur_cartesian_info.pose_str( ));
        items.setTag("robotmode",
                     robot_state_type::to_string(mUrData._ur_robot_mode_data.robotMode));
        items.setTag("controlmode",
                     robot_state_type::to_string(mUrData._ur_robot_mode_data.controlMode));
        items.setTag(
            "fovr", StdStringFormat("%f", mUrData._ur_robot_mode_data.targetSpeedFraction));
        items.setTag(
            "motherboard_temp",
            StdStringFormat("%f", mUrData._ur_masterboard_data.masterBoardTemperature));

        // Joint tag values
        if ( mUrData._ur_joint_data.size( ) == mJointnames.size( ) )
        {
            for ( size_t i = 0; i < mJointnames.size( ); i++ )
            {
                items.setTag(mJointnames[i] + "_actpos",
                             StdStringFormat("%f", mUrData._ur_joint_data[i].q_actual));
                items.setTag(
                    mJointnames[i] + "_mode",
                    joint_mode_type::to_string(mUrData._ur_joint_data[i].jointMode).c_str( ));
                items.setTag(mJointnames[i] + "_I",
                             StdStringFormat("%f", mUrData._ur_joint_data[i].I_actual));
                items.setTag(mJointnames[i] + "_V",
                             StdStringFormat("%f", mUrData._ur_joint_data[i].V_actual));
                items.setTag(mJointnames[i] + "_T",
                             StdStringFormat("%f", mUrData._ur_joint_data[i].T_motor));
                items.setTag(mJointnames[i] + "_actvel",
                             StdStringFormat("%f", mUrData._ur_joint_data[i].qd_actual));

            }
        }
//#ifdef _DEBUG
//        tagfile << dumpDataItems( );
//        tagfile.flush( );
//#endif

        for ( int i = 0; i < items.size( ); i++ )
        {
            if ( ( items[i]->mType == _T("Event") ) ||
                 ( items[i]->mType == _T("Sample") ) )
            {
                if ( items[i]->mValue != items[i]->mLastvalue )
                {
                    this->setMTCTagValue(items[i]->mTagname, items[i]->mValue);
                    items[i]->mLastvalue = items[i]->mValue;
                }
            }
			else if (  items[i]->mType == _T("Condition") )
			{
                if ( items[i]->mValue != items[i]->mLastvalue )
                {
                    this->setMTCConditionValue(items[i]->mTagname, items[i]->mValue);
                    items[i]->mLastvalue = items[i]->mValue;
                }

			}
        }
    }
    catch ( std::exception e )
    {
        logError("Exception in %s - UR_Adapter::GatherDeviceData() %s\n",
                 mDevice.c_str( ), (LPCSTR) e.what( ));
        resetOff( );
        doDisconnect( );
        hr = E_FAIL;
    }
    catch ( ... )
    {
        logError("Exception in %s - UR_Adapter::GatherDeviceData()\n",
                 mDevice.c_str( ));
        resetOff( );
        doDisconnect( );
        hr = E_FAIL;
    }
    return hr;
}

