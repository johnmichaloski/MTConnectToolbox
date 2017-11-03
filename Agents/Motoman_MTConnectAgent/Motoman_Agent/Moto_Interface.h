
//
// Moto_Interface.h
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#ifndef _Moto_Interface_h
#define _Moto_Interface_h

// C++ std libraries - C98
#include <cstdint>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <boost/thread/mutex.hpp>

#include "NIST/struct_decoder.h"
#include "Moto_Enums.h"

/** Simple message format:

PREFIX (not considered part of the message)
	int LENGTH (HEADER + DATA) in bytes
HEADER
	int MSG_TYPE identifies type of message (standard and robot specific values)
	int COMM_TYPE identified communications type
	int REPLY CODE reply code (only valid in service replies)
BODY
	ByteArray DATA variable length data determined by message type and and communications type
*/

namespace moto
{
	struct simple_message_header ;
	struct joint_traj_pt_state_message ;
	struct robot_status_message ;
	struct joint_feedback_message;


	// Used to determine how many joint values to read per loop
	extern int numjoints;

	struct simple_message_header : public struct_decoder<simple_message_header>
	{
		float mVersion;
#pragma pack(push, 1)
		int32_t length;		  /* 4 bytes, constant value should be 3x4 = 12 */
		int32_t message_type;	  /* 4 bytes, constant value 1, PING */
		int32_t comm_type;	  /* 4 bytes, constant value 2, REQUEST */
		int32_t reply_type;	  /* 4 bytes, N/A */
#pragma pack(pop)

		BEGIN_SERIAL_PROP_MAP(simple_message_header)
			PROP_SERIAL_ENTRY(length,"%d")
			PROP_SERIAL_ENTRY(message_type,"%d")
			PROP_SERIAL_ENTRY(comm_type,"%d")
			PROP_SERIAL_ENTRY(reply_type,"%d")
		END_SERIAL_PROP_MAP()
	} ;

	struct joint_traj_pt_state_message :  public struct_decoder<joint_traj_pt_state_message>
	{
#pragma pack(push, 1)
		int32_t length;		  /* 4 bytes */
		int32_t message_type;	  /* 4 bytes, constant value 1, PING */
		int32_t comm_type;	  /* 4 bytes, constant value 2, REQUEST */
		int32_t reply_type;	  /* 4 bytes, N/A */

		int32_t unused_1;		  /* 4 bytes, N/A */
		float joints[JOINT_MAX];	/* 10 4-byte floats, N/A */
#pragma pack(pop)

		joint_traj_pt_state_message() {
			length = msglength();  // -4?
			/* NOTE: JOINT_POSITION is deprecated, but JOINT_TRAJ_PT gives
			an error in the industrial robot client. */
			message_type = message_types::MESSAGE_JOINT_POSITION;
			comm_type = simple_message_comm_types::COMM_TOPIC;
			reply_type = simple_message_reply_types::REPLY_NA;
			/* caller will need to set joints with method below */
		}


		BEGIN_SERIAL_PROP_MAP(joint_traj_pt_state_message)
			//PROP_SERIAL_ENTRY(length,"%d")
			PROP_SERIAL_ENTRY(message_type,"%d")
			PROP_SERIAL_ENTRY(comm_type,"%d")
			PROP_SERIAL_ENTRY(reply_type,"%d")
			PROP_SERIAL_ENTRY(unused_1,"%d")
			PROP_SERIAL_ENTRY_VARARRAY(joints, sizeof(float), "%f", &numjoints)
		END_SERIAL_PROP_MAP()

		int set_pos(float pos, int index) {
			if (index < 0 || index >= JOINT_MAX) return 0;
			joints[index] = pos;
			return 1;
		}

		int get_pos(float *pos, int index) {
			if (index < 0 || index >= JOINT_MAX) return 0;
			*pos = joints[index];
			return 1;
		}

	};
	struct robot_status_message : public struct_decoder<robot_status_message>
	{
		float mVersion;
#pragma pack(push, 1)
		int32_t length;		  /* 4 bytes */
		int32_t message_type;	  /* 4 bytes, constant value  */
		int32_t comm_type;	  /* 4 bytes, constant value  */
		int32_t reply_type;	  /* 4 bytes, N/A */

		int32_t drives_powered;
		int32_t e_stopped;
		int32_t error_code;
		int32_t in_error;
		int32_t in_motion;
		int32_t mode;
		int32_t motion_possible;
#pragma pack(pop)

		robot_status_message() {
			length = msglength();  // -4?
			message_type = message_types::MESSAGE_ROBOT_STATUS;
			comm_type = simple_message_comm_types::COMM_TOPIC;
			reply_type = simple_message_reply_types::REPLY_NA;
		}
		BEGIN_SERIAL_PROP_MAP(robot_status_message)
			//PROP_SERIAL_ENTRY(length,"%d")
			PROP_SERIAL_ENTRY(message_type,"%d")
			PROP_SERIAL_ENTRY(comm_type,"%d")
			PROP_SERIAL_ENTRY(reply_type,"%d")
			PROP_SERIAL_ENTRY(drives_powered,"%d")
			PROP_SERIAL_ENTRY(e_stopped,"%d")
			PROP_SERIAL_ENTRY(error_code,"%d")
			PROP_SERIAL_ENTRY(in_error,"%d")
			PROP_SERIAL_ENTRY(in_motion,"%d")
			PROP_SERIAL_ENTRY(mode,"%d")
			PROP_SERIAL_ENTRY(motion_possible,"%d")
		END_SERIAL_PROP_MAP()

	};
	struct joint_feedback_message : public struct_decoder<joint_feedback_message>		// ROS_MSG_JOINT_FEEDBACK = 15
	{
		float mVersion;
#pragma pack(push, 1)
		int32_t length;		  /* 4 bytes, constant value  */
		int32_t message_type;	  /* 4 bytes, constant value */
		int32_t comm_type;	  /* 4 bytes, constant value  */
		int32_t reply_type;	  /* 4 bytes, N/A */

		int32_t group_num;  				// Robot/group ID;  0 = 1st robot
		int32_t valid_fields;			// Bit-mask indicating which “optional” fields are filled with data. 1=time, 2=position, 4=velocity, 8=acceleration
		float time;					// Timestamp associated with this trajectory point; Units: in seconds
		float pos[JOINT_MAX];	// Desired joint positions in radian.  Base to Tool joint order
		float vel[JOINT_MAX];	// Desired joint velocities in radian/sec.
		float acc[JOINT_MAX];	// Desired joint accelerations in radian/sec^2.
#pragma pack(pop)
		joint_feedback_message() {
			length = msglength();  // -4?
			message_type = message_types::MESSAGE_JOINT_FEEDBACK;
			comm_type = simple_message_comm_types::COMM_TOPIC;
			reply_type = simple_message_reply_types::REPLY_NA;
		}
		BEGIN_SERIAL_PROP_MAP(joint_feedback_message)
			//PROP_SERIAL_ENTRY(length,"%d")
			PROP_SERIAL_ENTRY(message_type,"%d")
			PROP_SERIAL_ENTRY(comm_type,"%d")
			PROP_SERIAL_ENTRY(reply_type,"%d")
			PROP_SERIAL_ENTRY(group_num,"%d")
			PROP_SERIAL_ENTRY(valid_fields,"%x")
			PROP_SERIAL_ENTRY(time,"%f")
			PROP_SERIAL_ENTRY_VARARRAY(pos, sizeof(float), "%f", &numjoints)
			PROP_SERIAL_ENTRY_VARARRAY(vel, sizeof(float), "%f", &numjoints)
			PROP_SERIAL_ENTRY_VARARRAY(acc, sizeof(float), "%f", &numjoints)
		END_SERIAL_PROP_MAP()

	};


	struct moto_robot_state
	{
		boost::mutex mutex;
		moto_robot_state();
		/**
		* @brief unpack decodes one and potential submessage in buffer.
		* @param buf pointer to char buffer string to decode
		* @param buf_length size of buffer
		*/
		size_t          unpack (uint8_t *buf, unsigned int buf_length);

		/**
		* @brief dump generates string of all ur interface buffers
		* @return string containing dump
		*/
		std::string     dump ( );
		double      getVersion ( );
		void        setVersion(float ver);

		////////////////////////////////////
		bool                       new_data_available_;    // to avoid spurious wakes
		unsigned char              robot_mode_running_;

		// UR message decoded data
		moto::simple_message_header header;
		moto::robot_status_message robot_status;
		moto::joint_traj_pt_state_message jtp_state;
		moto::joint_feedback_message jnt_feedback;

		/**
		* @brief setDisconnected ur method to signal robot communication disconnected
		*/
		void            setDisconnected ( );

		/**
		* @brief isReady ur method to determine if robot running
		* @return true if running
		*/
		bool            isReady ( );    
	};
}

extern void         test1_robot_state ( );
extern void         test2_robot_state ( );
extern void         test3_robot_state ( );
extern void         convert_bin_telnet_Cpp (std::string);
#endif
