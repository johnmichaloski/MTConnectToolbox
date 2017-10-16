
//
// UrInterface.h
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#ifndef _UrInterface_h
#define _UrInterface_h

// C++ std libraries - C98
#include <cstdint>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "NIST/struct_decoder.h"
#include "UrEnums.h"

namespace ur
{
	// Additional interface insight:
	// https://github.com/ros-industrial/universal_robot/blob/kinetic-devel/ur_driver/src/ur_driver/deserialize.py

	struct ur_additional_info;
	struct ur_cartesian_info;
	struct ur_configuration_data;
	struct ur_force_mode_data;
	struct ur_joint_data ;
	struct ur_joint_limits;
	struct ur_joint_maxs;
	struct ur_key_message;
	struct ur_label_message;
	struct ur_masterboard_data;
	struct ur_message_header;
	struct ur_popup_message;
	struct ur_request_value_message;
	struct ur_robot_comm_message;
	struct ur_robot_message;
	struct ur_robot_mode_data ;
	struct ur_robot_state;
	struct ur_runtime_exception_message;
	struct ur_security_message;
	struct ur_text_message;
	struct ur_tool_data;
	struct ur_var_message;
	struct ur_version_message;

	// Used to determine how many joint values to read per loop
	extern int numjoints;

	struct ur_message_header : public struct_decoder<ur_message_header>
	{
		uint32_t packageSize;
		uint8_t  packageType;
		BEGIN_SERIAL_PROP_MAP(ur_message_header)
			PROP_SERIAL_ENTRY(packageSize, "%d")
			PROP_SERIAL_ENTRY(packageType, "%c")
		END_SERIAL_PROP_MAP( )
	};

	struct ur_robot_mode_data : public struct_decoder<ur_robot_mode_data>
	{
		uint32_t      packageSize;
		int8_t        packageType;                         // 0 robot mode data subtype
		uint64_t      timestamp;
		bool          isRobotConnected;
		bool          isRealRobotEnabled;
		bool          isPowerOnRobot;
		bool          isEmergencyStopped;
		bool          isSecurityStopped;
		bool          isProgramRunning;
		bool          isProgramPaused;
		unsigned char robotMode;
		unsigned char controlMode;
		double        targetSpeedFraction;
		double        speedScaling;
		double        targetSpeedFractionLimit;
		double        speedFraction;
		bool          isProectiveStopped;

		BEGIN_SERIAL_PROP_MAP(ur_robot_mode_data)
			PROP_SERIAL_ENTRY(timestamp, "%llu")
			PROP_SERIAL_ENTRY(isRobotConnected, "%c")
			PROP_SERIAL_ENTRY(isRealRobotEnabled, "%c")
			PROP_SERIAL_ENTRY(isPowerOnRobot, "%c")
			PROP_SERIAL_ENTRY(isEmergencyStopped, "%c")
			PROP_SERIAL_ENTRY_VERSION(isSecurityStopped, "%c",1.6,1.8)
			PROP_SERIAL_ENTRY_VERSION(isProectiveStopped, "%c",3.0,VMAX)
			PROP_SERIAL_ENTRY(isProgramRunning, "%c")
			PROP_SERIAL_ENTRY(isProgramPaused, "%c")
			PROP_SERIAL_ENTRY(robotMode, "%c")
			PROP_SERIAL_ENTRY_VERSION(speedFraction, "%f",1.6,1.8)
			PROP_SERIAL_ENTRY_VERSION(controlMode, "%c",3.0,VMAX)
			PROP_SERIAL_ENTRY_VERSION(targetSpeedFraction, "%lf",3.0,VMAX)
			PROP_SERIAL_ENTRY_VERSION(speedScaling, "%lf",3.0,VMAX)
			PROP_SERIAL_ENTRY_VERSION(targetSpeedFractionLimit, "%lf",3.2,VMAX)
		END_SERIAL_PROP_MAP( )
	};

	struct ur_joint_data : public struct_decoder<ur_joint_data>
	{
		double        q_actual;
		double        q_target;
		double        qd_actual;
		float         I_actual;
		float         V_actual;
		float         T_motor;
		float         T_micro;
		unsigned char jointMode;                           ///< The "jointMode" field is a code for the joint
		///status (shown on the initialisation screen):
		BEGIN_SERIAL_PROP_MAP(ur_joint_data)
			PROP_SERIAL_ENTRY(q_actual, "%lf")
			PROP_SERIAL_ENTRY(q_target, "%lf")
			PROP_SERIAL_ENTRY(qd_actual, "%lf")
			PROP_SERIAL_ENTRY(I_actual, "%f")
			PROP_SERIAL_ENTRY(V_actual, "%f")
			PROP_SERIAL_ENTRY(T_motor, "%f")
			PROP_SERIAL_ENTRY(T_micro, "%f")
			PROP_SERIAL_ENTRY(jointMode, "%c")
		END_SERIAL_PROP_MAP( )
	};


	struct ur_cartesian_info : public struct_decoder<ur_cartesian_info>
	{
		uint32_t packageSize;
		int8_t   packageType;                              // = CARTESIAN_INFO = 4
		double   X;
		double   Y;
		double   Z;
		double   Rx;
		double   Ry;
		double   Rz;
		double TCPOffsetX;
		double TCPOffsetY;
		double TCPOffsetZ;
		double TCPOffsetRx;
		double TCPOffsetRy;
		double TCPOffsetRz;	

		BEGIN_SERIAL_PROP_MAP(ur_cartesian_info)
			PROP_SERIAL_ENTRY(X, "%lf")
			PROP_SERIAL_ENTRY(Y, "%lf")
			PROP_SERIAL_ENTRY(Z, "%lf")
			PROP_SERIAL_ENTRY(Rx, "%lf")
			PROP_SERIAL_ENTRY(Ry, "%lf")
			PROP_SERIAL_ENTRY(Rz, "%lf")
			PROP_SERIAL_ENTRY_VERSION(TCPOffsetX, "%lf",3.1,VMAX)
			PROP_SERIAL_ENTRY_VERSION(TCPOffsetY, "%lf",3.1,VMAX)
			PROP_SERIAL_ENTRY_VERSION(TCPOffsetZ, "%lf",3.1,VMAX)
			PROP_SERIAL_ENTRY_VERSION(TCPOffsetRx, "%lf",3.1,VMAX)
			PROP_SERIAL_ENTRY_VERSION(TCPOffsetRy, "%lf",3.1,VMAX)
			PROP_SERIAL_ENTRY_VERSION(TCPOffsetRz, "%lf",3.1,VMAX)
		END_SERIAL_PROP_MAP( )
		std::string pose_str ( )
		{
			std::stringstream ss;
			ss << X << " " << Y << " " << Z << " " << Rx << " " << Ry << " " << Rz;
			return ss.str( );
		}
	};

	struct ur_masterboard_data : public struct_decoder<ur_masterboard_data>
	{
		uint32_t      digitalInputBits;
		uint32_t      digitalOutputBits;
		int8_t        analogInputRange0;
		int8_t        analogInputRange1;
		double        analogInput0;
		double        analogInput1;
		int8_t        analogOutputDomain0;
		int8_t        analogOutputDomain1;
		double        analogOutput0;
		double        analogOutput1;
		float         masterBoardTemperature;
		float         robotVoltage48V;
		float         robotCurrent;
		float         masterIOCurrent;
		unsigned char safetyMode;
		unsigned char masterOnOffState;
		char          euromap67InterfaceInstalled;
		uint32_t      euromapInputBits;
		uint32_t      euromapOutputBits;
		float         euromapVoltage;
		float         euromapCurrent;
		uint32_t  ur_internal;
		uint8_t operationalModeSelectorInput;
		uint8_t threePositionEnablingDeviceInput;

		BEGIN_SERIAL_PROP_MAP(ur_masterboard_data)
			PROP_SERIAL_ENTRY(digitalInputBits, "%d")
			PROP_SERIAL_ENTRY(digitalOutputBits, "%d")
			PROP_SERIAL_ENTRY(analogInputRange0, "%c")
			PROP_SERIAL_ENTRY(analogInputRange1, "%c")
			PROP_SERIAL_ENTRY(analogInput0, "%lf")
			PROP_SERIAL_ENTRY(analogInput1, "%lf")
			PROP_SERIAL_ENTRY(analogOutputDomain0, "%c")
			PROP_SERIAL_ENTRY(analogOutputDomain1, "%c")
			PROP_SERIAL_ENTRY(analogOutput0, "%lf")
			PROP_SERIAL_ENTRY(analogOutput1, "%lf")

			PROP_SERIAL_ENTRY(masterBoardTemperature, "%f")
			PROP_SERIAL_ENTRY(robotVoltage48V, "%f")
			PROP_SERIAL_ENTRY(robotCurrent, "%f")
			PROP_SERIAL_ENTRY(masterIOCurrent, "%f")
			PROP_SERIAL_ENTRY(safetyMode, "%c")
			PROP_SERIAL_ENTRY(masterOnOffState, "%c")
			PROP_SERIAL_ENTRY(euromap67InterfaceInstalled, "%c")
			PROP_SERIAL_ENTRY(euromapInputBits, "%d")
			PROP_SERIAL_ENTRY(euromapOutputBits, "%d")
			PROP_SERIAL_ENTRY(euromapVoltage, "%f")
			PROP_SERIAL_ENTRY(euromapCurrent, "%f")
			PROP_SERIAL_ENTRY_VERSION(ur_internal, "%d",3.0,VMAX)
			PROP_SERIAL_ENTRY_VERSION(operationalModeSelectorInput, "%c",3.2,VMAX)
			PROP_SERIAL_ENTRY_VERSION(threePositionEnablingDeviceInput, "%lf",3.2,VMAX)
		END_SERIAL_PROP_MAP( )
	};

	struct ur_tool_data : public struct_decoder<ur_tool_data>
	{
		uint32_t      packageSize;
		unsigned char packageType;                         // = TOOL_DATA = 2
		int8_t        analogInputRange2;
		int8_t        analogInputRange3;
		double        analogInput2;
		double        analogInput3;
		float         toolVoltage48V;
		int8_t        toolOutputVoltage;
		float         toolCurrent;
		float         toolTemperature;
		int8_t        toolMode;
		BEGIN_SERIAL_PROP_MAP(ur_tool_data)
			PROP_SERIAL_ENTRY(analogInputRange2, "%c")
			PROP_SERIAL_ENTRY(analogInputRange3, "%c")
			PROP_SERIAL_ENTRY(analogInput2, "%lf")
			PROP_SERIAL_ENTRY(analogInput3, "%lf")
			PROP_SERIAL_ENTRY(toolVoltage48V, "%f")
			PROP_SERIAL_ENTRY(toolOutputVoltage, "%c")
			PROP_SERIAL_ENTRY(toolCurrent, "%f")
			PROP_SERIAL_ENTRY(toolTemperature, "%f")
			PROP_SERIAL_ENTRY(toolMode, "%c")
		END_SERIAL_PROP_MAP( )
	};

	struct ur_robot_message : public struct_decoder<ur_robot_message>
	{
		uint32_t packageSize;
		int8_t   packageType;                              // = TOOL_DATA = 2
		uint64_t timestamp;
		int8_t   source;
		int8_t   robot_message_type;
		BEGIN_SERIAL_PROP_MAP(ur_robot_message)
			PROP_SERIAL_ENTRY(packageSize, "%d")
			PROP_SERIAL_ENTRY(packageType, "%c")
			PROP_SERIAL_ENTRY(timestamp, "%ld")
			PROP_SERIAL_ENTRY(source, "%c")
			PROP_SERIAL_ENTRY(robot_message_type, "%c")
		END_SERIAL_PROP_MAP( )
	};

	// This is the first package sent on both the primary and secondary client
	// interfaces. This package it not part of the robot state message.
	struct ur_version_message : public struct_decoder<ur_version_message>
	{
		int           messageSize;
		unsigned char messageType;                         // = ROBOT_MESSAGE = 20
		uint64_t      timestamp;
		int8_t        source;
		int8_t        robot_message_type;                  // 3
		int8_t        project_name_size;
		char          project_name[15];
		uint8_t       major_version;
		uint8_t       minor_version;
		int svnRevision;
		// uint32_t svn_revision;
		int           bugfixVersion;
		int           buildNumber;
		char          build_date[25];
		BEGIN_SERIAL_PROP_MAP(ur_version_message)
			PROP_SERIAL_ENTRY(messageSize, "%d")
			PROP_SERIAL_ENTRY(messageType, "%c")
			PROP_SERIAL_ENTRY(timestamp, "%llu")
			PROP_SERIAL_ENTRY(source, "%c")
			PROP_SERIAL_ENTRY(robot_message_type, "%c")
			PROP_SERIAL_ENTRY(project_name_size, "%c")
			PROP_SERIAL_ENTRY(project_name, "%s")
			PROP_SERIAL_ENTRY(major_version, "%c")
			PROP_SERIAL_ENTRY(minor_version, "%c")
			PROP_SERIAL_ENTRY_VERSION(svnRevision, "%d",VMIN,3.2)
			PROP_SERIAL_ENTRY_VERSION(bugfixVersion, "%d",3.3,VMAX)
			PROP_SERIAL_ENTRY_VERSION(buildNumber, "%d",3.3,VMAX)
			PROP_SERIAL_ENTRY(build_date, "%s")
		END_SERIAL_PROP_MAP( )
		void   decode (char *buf, int len);
		double getVersion ( );
	};

	struct ur_security_message : public struct_decoder<ur_security_message>
	{
		uint32_t messageSize;
		uint8_t  messageType;                              // = ROBOT_MESSAGE = 20
		uint64_t timestamp;
		char     source;
		char     robotMessageType;                         // = ROBOT_MESSAGE_SECURITY = 5
		int      robotMessageCode;
		int      robotMessageArgument;
		char     textMessage[256];                         // ?????????
		BEGIN_SERIAL_PROP_MAP(ur_security_message)
			PROP_SERIAL_ENTRY(timestamp, "%llu")
			PROP_SERIAL_ENTRY(source, "%c")
			PROP_SERIAL_ENTRY(robotMessageType, "%c")
			PROP_SERIAL_ENTRY(robotMessageCode, "%d")
			PROP_SERIAL_ENTRY(robotMessageArgument, "%d")
			PROP_SERIAL_ENTRY_ARRAY(textMessage, 1, "%c")
		END_SERIAL_PROP_MAP( )
	};

	struct ur_robot_comm_message : public struct_decoder<ur_robot_comm_message>
	{
		uint32_t messageSize;
		uint8_t  messageType;                              // = ROBOT_MESSAGE = 20
		uint64_t timestamp;
		uint8_t  source;
		uint8_t  robotMessageType;                         // = ROBOT_MESSAGE_ERROR_CODE = 6
		uint32_t robotMessageCode;
		uint32_t robotMessageArgument;
		char     textMessage[256];                         // FIXME!!!!!!
		BEGIN_SERIAL_PROP_MAP(ur_robot_comm_message)
			PROP_SERIAL_ENTRY(timestamp, "%llu")
			PROP_SERIAL_ENTRY(source, "%c")
			PROP_SERIAL_ENTRY(robotMessageType, "%c")
			PROP_SERIAL_ENTRY(robotMessageCode, "%d")
			PROP_SERIAL_ENTRY(robotMessageArgument, "%d")
			PROP_SERIAL_ENTRY_ARRAY(textMessage, 1, "%c")
		END_SERIAL_PROP_MAP( )
	};

	struct ur_key_message : public struct_decoder<ur_key_message>
	{
		int           messageSize;
		unsigned char messageType;                         // = ROBOT_MESSAGE = 20
		uint64_t      timestamp;
		char          source;
		char          robotMessageType;                    // = ROBOT_MESSAGE_KEY = 7
		int           robotMessageCode;
		int           robotMessageArgument;
		unsigned char titleSize;
		char          messageTitle[256];
		char          textMessage[256];
		BEGIN_SERIAL_PROP_MAP(ur_key_message)
			PROP_SERIAL_ENTRY(messageSize, "%d")
			PROP_SERIAL_ENTRY(messageType, "%c")
			PROP_SERIAL_ENTRY(timestamp, "%llu")
			PROP_SERIAL_ENTRY(source, "%c")
			PROP_SERIAL_ENTRY(robotMessageType, "%c")
			PROP_SERIAL_ENTRY(robotMessageCode, "%d")
			PROP_SERIAL_ENTRY(robotMessageArgument, "%d")
			PROP_SERIAL_ENTRY(titleSize, "%c")
			PROP_SERIAL_ENTRY_ARRAY(messageTitle, 1, "%c")
			PROP_SERIAL_ENTRY_ARRAY(textMessage, 1, "%c")
		END_SERIAL_PROP_MAP( )
	};

	struct ur_label_message : public struct_decoder<ur_label_message>
	{
		int           messageSize;
		unsigned char messageType;                         // = ROBOT_MESSAGE = 20
		uint64_t      timestamp;
		char          source;
		char          robotMessageType;                    // = ROBOT_MESSAGE_PROGRAM_LABEL = 1
		int           id;
		char *        textMessage;
		BEGIN_SERIAL_PROP_MAP(ur_label_message)

			// PROP_SERIAL_ENTRY(messageSize,"%d")
			// PROP_SERIAL_ENTRY(messageType,"%c")
			PROP_SERIAL_ENTRY(timestamp, "%llu")
			PROP_SERIAL_ENTRY(source, "%c")
			PROP_SERIAL_ENTRY(robotMessageType, "%c")
			PROP_SERIAL_ENTRY(id, "%d")
			PROP_SERIAL_ENTRY(textMessage, "%c")
		END_SERIAL_PROP_MAP( )
	};

	struct ur_popup_message : public struct_decoder<ur_label_message>
	{
		int           messageSize;
		unsigned char messageType;                         // = ROBOT_MESSAGE = 20
		uint64_t      timestamp;
		char          source;
		char          robotMessageType;                    // = ROBOT_MESSAGE_POPUP = 2
		bool          warning;
		bool          error;
		unsigned char titleSize;
		char *        messageTitle;
		char *        textMessage;
	};

	struct ur_request_value_message
		: public struct_decoder<ur_request_value_message>
	{
		int           messageSize;
		unsigned char messageType;                         // = ROBOT_MESSAGE = 20
		uint64_t      timestamp;
		char          source;
		char          robotMessageType;                    // = ROBOT_MESSAGE_REQUEST_* = 0-8
		unsigned int  requestId;
		char *        textMessage;
	};

	struct ur_text_message : public struct_decoder<ur_text_message>
	{
		int           messageSize;
		unsigned char messageType;                         // = ROBOT_MESSAGE = 20
		uint64_t      timestamp;
		char          source;
		char          robotMessageType;                    // = ROBOT_MESSAGE_TEXT = 0
		char *        textMessage;
		BEGIN_SERIAL_PROP_MAP(ur_text_message)

			// PROP_SERIAL_ENTRY(messageSize,"%d")
			// PROP_SERIAL_ENTRY(messageType,"%c")
			PROP_SERIAL_ENTRY(timestamp, "%llu")
			PROP_SERIAL_ENTRY(source, "%c")
			PROP_SERIAL_ENTRY(robotMessageType, "%c")
			PROP_SERIAL_ENTRY(textMessage, "%c")
		END_SERIAL_PROP_MAP( )
	};
	struct ur_var_message : public struct_decoder<ur_var_message>
	{
		int           messageSize;
		unsigned char messageType;                         // = ROBOT_MESSAGE = 20
		uint64_t      timestamp;
		char          source;
		char          robotMessageType;                    // = ROBOT_MESSAGE_VARIABLE = 8
		int           code;
		int           argument;
		unsigned char titleSize;
		char *        messageTitle;
		char *        messageText;
	};

	struct ur_runtime_exception_message
		: public struct_decoder<ur_runtime_exception_message>
	{
		int           messageSize;
		unsigned char messageType;                         // = ROBOT_MESSAGE = 20
		uint64_t      timestamp;
		char          source;
		char          robotMessageType;                    // = ROBOT_MESSAGE_RUNTIME_EXCEPTION = 10
		uint32_t      lineNumber;
		uint32_t      columnNumber;
		char          textMessage;                         // this is an array ?
		BEGIN_SERIAL_PROP_MAP(ur_runtime_exception_message)

			// PROP_SERIAL_ENTRY(messageSize,"%d")
			// PROP_SERIAL_ENTRY(messageType,"%c")
			PROP_SERIAL_ENTRY(timestamp, "%llu")
			PROP_SERIAL_ENTRY(source, "%c")
			PROP_SERIAL_ENTRY(robotMessageType, "%c")
			PROP_SERIAL_ENTRY(lineNumber, "%d")
			PROP_SERIAL_ENTRY(columnNumber, "%d")
			PROP_SERIAL_ENTRY(textMessage, "%c")
		END_SERIAL_PROP_MAP( )
	};
	struct ur_additional_info : public struct_decoder<ur_additional_info>
	{
		int           messageSize;
		unsigned char messageType;                         // = DDITIONAL_INFO = 8
		bool          teachButtonPressed;
		bool          teachButtonEnabled;
		BEGIN_SERIAL_PROP_MAP(ur_additional_info)

			// PROP_SERIAL_ENTRY(messageSize,"%d")
			// PROP_SERIAL_ENTRY(messageType,"%c")
			PROP_SERIAL_ENTRY(teachButtonPressed, "%c")
			PROP_SERIAL_ENTRY(teachButtonEnabled, "%c")
		END_SERIAL_PROP_MAP( )
	};

	struct ur_joint_limits
	{
		double min_limit;
		double max_limit;

		// ur_joint_limits(double min=0.0, double max=0.0)
		// {
		//	 min_limit=min;
		//	 max_limit=max;
		// }
	};
	struct ur_joint_maxs
	{
		double max_speed;
		double max_acceleration;

		// ur_joint_maxs(double vel=0.0, double acc=0.0)
		// {
		//	 max_speed=vel;
		//	 max_acceleration=acc;
		// }
	};
#if 1
	struct ur_configuration_data : public struct_decoder<ur_configuration_data>
	{
		int                          packageSize;
		unsigned char                packageType;          // = CONFIGURATION_DATA = 6

		std::vector<ur_joint_limits> _joint_limits;
		std::vector<ur_joint_maxs>   _joint_maxes;
		double                       vJointDefault;
		double                       aJointDefault;
		double                       vToolDefault;
		double                       aToolDefault;
		double                       eqRadius;
		double                       DHa[8];
		double                       DHd[8];
		double                       DHalpha[8];
		double                       DHtheta[8];
		int                          masterboardVersion;
		int                          controllerBoxType;
		int                          robotType;
		int                          robotSubType;
		int                          motorType[8];

		// class struct as type - not handled so have to build special case
		size_t      msglength ( );
		void        read (char *buf);
		void        write (char *buf, int bAddHeader);
		std::string print ( );
		BEGIN_SERIAL_PROP_MAP(ur_configuration_data)

			// Have to skip these or extensively redo all macros
			// Have base class that is close to this
			// PROP_SERIAL_ENTRY_VARARRAY(_joint_limits, sizeof(ur_joint_limits), "%-")
			// PROP_SERIAL_ENTRY_VARARRAY(_joint_maxes, sizeof(ur_joint_maxs), "%-")
			PROP_SERIAL_ENTRY(vJointDefault, "%lf")
			PROP_SERIAL_ENTRY(aJointDefault, "%lf")
			PROP_SERIAL_ENTRY(vToolDefault, "%lf")
			PROP_SERIAL_ENTRY(aToolDefault, "%lf")
			PROP_SERIAL_ENTRY(eqRadius, "%lf")

			PROP_SERIAL_ENTRY_VARARRAY(DHa, sizeof( double ), "%lf", &numjoints)
			PROP_SERIAL_ENTRY_VARARRAY(DHd, sizeof( double ), "%lf", &numjoints)
			PROP_SERIAL_ENTRY_VARARRAY(DHalpha, sizeof( double ), "%lf", &numjoints)
			PROP_SERIAL_ENTRY_VARARRAY(DHtheta, sizeof( double ), "%lf", &numjoints)
			PROP_SERIAL_ENTRY(masterboardVersion, "%d")
			PROP_SERIAL_ENTRY(controllerBoxType, "%d")
			PROP_SERIAL_ENTRY(robotType, "%d")
			PROP_SERIAL_ENTRY(robotSubType, "%d")
			PROP_SERIAL_ENTRY_VARARRAY(motorType, sizeof( int ), "%d", &numjoints)
		END_SERIAL_PROP_MAP( )
	};
#endif
	struct ur_force_mode_data : public struct_decoder<ur_force_mode_data>
	{
		int           packageSize;
		unsigned char packageType;                         // = FORCE_MODE_DATA = 7

		double        X;
		double        Y;
		double        Z;
		double        Rx;
		double        Ry;
		double        Rz;
		double        robotDexterity;
		BEGIN_SERIAL_PROP_MAP(ur_force_mode_data)

			// PROP_SERIAL_ENTRY(messageSize,"%d")
			// PROP_SERIAL_ENTRY(messageType,"%c")
			PROP_SERIAL_ENTRY(X, "%lf")
			PROP_SERIAL_ENTRY(Y, "%lf")
			PROP_SERIAL_ENTRY(Z, "%lf")
			PROP_SERIAL_ENTRY(Rx, "%lf")
			PROP_SERIAL_ENTRY(Ry, "%lf")
			PROP_SERIAL_ENTRY(Rz, "%lf")
			PROP_SERIAL_ENTRY(robotDexterity, "%lf")
		END_SERIAL_PROP_MAP( )
	};

	struct ur_robot_state
	{
		/**
		* @brief unpack decodes one and potential submessage in buffer.
		* @param buf pointer to char buffer string to decode
		* @param buf_length size of buffer
		*/
		void            unpack (uint8_t *buf, unsigned int buf_length);

		/**
		* @brief unpackRobotState unpacks ur robot state message
		* @param buf pointer to char buffer string to decode
		* @param offset offset into buffer
		* @param len size of buffer
		*/
		void            unpackRobotState (uint8_t *buf, unsigned int offset, uint32_t len);

		/**
		* @brief unpackRobotMessage unpacks ur robot mode message
		* @param buf pointer to char buffer string to decode
		* @param offset offset into buffer
		* @param len size of buffer
		*/
		void            unpackRobotMessage (uint8_t *buf, unsigned int offset, uint32_t len);

		/**
		* @brief dump generates string of all ur interface buffers
		* @return string containing dump
		*/
		std::string     dump ( );
		double      getVersion ( );

		////////////////////////////////////
		bool                       new_data_available_;    // to avoid spurious wakes
		unsigned char              robot_mode_running_;

		// UR message decoded data
		ur::ur_message_header _ur_message_header;              // !< ur message hdr (size/type)
		ur::ur_robot_mode_data _ur_robot_mode_data;            // !< mode data
		ur::ur_tool_data _ur_tool_data;                        // !< tooling data
		ur::ur_version_message _ur_version_message;            // !< verions data of controller
		ur::ur_masterboard_data _ur_masterboard_data;          // !< motherboard information
		ur::ur_cartesian_info _ur_cartesian_info;              // !< cartesian pose
		std::vector<ur::ur_joint_data>
			_ur_joint_data;                                        // !< data for each joint as vector
		ur::ur_force_mode_data _ur_force_mode_data;            // !< force data
		ur::ur_additional_info _ur_additional_info;
		ur::ur_configuration_data _ur_configuration_data;      // !< configuration of robot

		ur_robot_state( )
		{
		}
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
extern void         write_ur_configuration_data (ur::ur_configuration_data & config);
extern void         read_ur_configuration_data (ur::ur_configuration_data & config);
#endif
