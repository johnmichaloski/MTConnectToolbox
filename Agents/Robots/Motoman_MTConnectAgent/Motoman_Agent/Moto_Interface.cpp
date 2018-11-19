
//
// Moto_Interface.cpp
//

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#include "Moto_Interface.h"
#include "stdafx.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "NIST/hexdump.h"
#include "NIST/Logger.h"
#include "Globals.h"

#ifdef __linux__
#include "LinuxGlobals.h"
#endif

using namespace moto;
int moto::numjoints = JOINT_MAX;


////////////////////////////////////////////////////////////////////

moto_robot_state::moto_robot_state()
{
	setVersion(3.0);
}
void  moto_robot_state::setVersion(float ver)
{
	header.mVersion=ver;
	robot_status.mVersion=ver;
	jtp_state.mVersion=ver;
	jnt_feedback.mVersion=ver;
}


size_t moto_robot_state::unpack (uint8_t *buf, unsigned int buf_length)
{
	//logDebug(Nist::HexDump(buf, buf_length).c_str());
	//header.skip_swap()=!is_bigendian();  // override default as simple msg is little endian!
	byteorder_decoder::bIgnoreSwapFlag=is_bigendian();  // override default as simple msg is little endian!
	header.read(buf);
	size_t bytesRead=0;

	try {
		while(bytesRead<buf_length)
		{

			logDebug("interpret message %s\n", message_types::to_string(header.message_type).c_str());

			// switch on the message type and handle it
			switch (header.message_type) {
			case message_types::MESSAGE_ROBOT_STATUS:
				logDebug("MESSAGE_ROBOT_STATUS: buffer size=%d msg size = %d bytesRead =%d\n", buf_length, robot_status.msglength(), bytesRead);
				if((buf_length-bytesRead) < robot_status.msglength())
				{
					return bytesRead;
				}
				//bytesRead+=sizeof(int32_t);  // skip length

				robot_status.read(buf+bytesRead+sizeof(int32_t));

				// Update robot status
				//motoman_comm::moto_robot->read(&robot_status);

				if(Globals.bHexdump)
					logDebug("Robot status = \n%s\n", Nist::HexDump(&robot_status, sizeof(robot_status), 16).c_str());
				break;
			case message_types::MESSAGE_JOINT_FEEDBACK :
				logDebug("MESSAGE_JOINT_FEEDBACK: buffer size=%d msg size = %d bytesRead =%d\n", buf_length, jnt_feedback.msglength(), bytesRead);

				if((buf_length-bytesRead) < jnt_feedback.msglength())
				{
					logDebug("Decode MESSAGE_JOINT_FEEDBACK  - inusfficient message size\n");
					return bytesRead; 
				}
				//bytesRead+=sizeof(int32_t);  // skip length

				if (Globals.bDebugMask & DEBUG_FEEDBACK)
					logDebug("Decode MESSAGE_JOINT_FEEDBACK\n");

				jnt_feedback.read(buf+bytesRead+sizeof(int32_t));


				// Update robot joints
				//motoman_comm::moto_robot->read(&jtp_state);
				// Update screen - upon command

				if (Globals.bDebugMask & DEBUG_FEEDBACK)
					logDebug(jnt_feedback.print().c_str());
				if(Globals.bHexdump)
					logDebug("joint Status=\n%s\n", Nist::HexDump(&jnt_feedback, jnt_feedback.msglength(), 16).c_str());
				break;


				/* NOTE: JOINT_POSITION is deprecated, but JOINT_TRAJ_PT gives
				an error in the industrial robot client. */
			case message_types::MESSAGE_JOINT_POSITION:
				logDebug("MESSAGE_JOINT_POSITION: buffer size=%d msg size = %d bytesRead =%d\n", buf_length, jtp_state.msglength(), bytesRead);
				if( (buf_length-bytesRead) < jtp_state.msglength())
				{
					logDebug("Decode MESSAGE_JOINT_POSITION  - inusfficient message size\n");
					return bytesRead; // FIXME: should all of message come at once seems so.
				}
				//bytesRead+=sizeof(int32_t);  // skip length

				logDebug("Decode MESSAGE_JOINT_POSITION\n");
				jtp_state.read(buf+bytesRead+sizeof(int32_t));

				// Update robot joints
				//motoman_comm::moto_robot->read(&jtp_state);

				if (Globals.bDebugMask & DEBUG_FEEDBACK)
					logDebug(jtp_state.print().c_str());
				if(Globals.bHexdump)
					logDebug(Nist::HexDump(&jtp_state, jtp_state.msglength(), 16).c_str());
				break;
			default:
				// unknown message
				logError("unknown status type: %d\n", header.message_type);
#ifdef _DEBUG
				assert(0);
#endif
				break;
			} // switch (message type)
			bytesRead+= header.length+sizeof(int32_t);  // does the length include the header size?
			header.read(buf+bytesRead);
		} /* while */
	}
	catch(...)
	{
		assert(0);
	}

	return bytesRead; // header.length+sizeof(header.length);
}

std::string moto_robot_state::dump ( )
{
	std::string tmp;
	tmp += "Mode\n";
	tmp += robot_status.print( );
	tmp += "Joint\n";
	tmp +=jnt_feedback.print();
	return tmp;
}
bool moto_robot_state::isReady ( )
{
	assert(0);
	return false;
}
void moto_robot_state::setDisconnected ( )
{

}
////////////////////////////////////////////////////////////////////


