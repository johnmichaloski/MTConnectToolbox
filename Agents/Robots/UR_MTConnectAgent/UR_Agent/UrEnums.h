
//
// UrEnums.h
//
#ifndef _UrEnums_h_
#define _UrEnums_h_

namespace ur
{
    struct message_types
    {
        static const int   ROBOT_STATE           = 16;
        static const int   ROBOT_MESSAGE         = 20;
        static const int   PROGRAM_STATE_MESSAGE = 25;
        static std::string to_string (int i)
        {
            switch ( i )
            {
            case ROBOT_STATE:
                {
                    return "ROBOT_STATE";
                }

            case ROBOT_MESSAGE:
                {
                    return "ROBOT_MESSAGE";
                }

            case PROGRAM_STATE_MESSAGE:
                {
                    return "PROGRAM_STATE_MESSAGE";
                }

            default:
                return "no message type match";
            }
        }
    };

    struct package_types
    {
        static const int   ROBOT_MODE_DATA    = 0;
        static const int   JOINT_DATA         = 1;
        static const int   TOOL_DATA          = 2;
        static const int   MASTERBOARD_DATA   = 3;
        static const int   CARTESIAN_INFO     = 4;
        static const int   KINEMATICS_INFO    = 5;
        static const int   CONFIGURATION_DATA = 6;
        static const int   FORCE_MODE_DATA    = 7;
        static const int   ADDITIONAL_INFO    = 8;
        static const int   CALIBRATION_DATA   = 9;
        static std::string to_string (int i)
        {
            switch ( i )
            {
            case ROBOT_MODE_DATA:
                {
                    return "ROBOT_MODE_DATA";
                }

            case JOINT_DATA:
                {
                    return "JOINT_DATA";
                }

            case TOOL_DATA:
                {
                    return "TOOL_DATA";
                }

            case MASTERBOARD_DATA:
                {
                    return "MASTERBOARD_DATA";
                }

            case CARTESIAN_INFO:
                {
                    return "CARTESIAN_INFO";
                }

            case KINEMATICS_INFO:
                {
                    return "KINEMATICS_INFO";
                }

            case CONFIGURATION_DATA:
                {
                    return "CONFIGURATION_DATA";
                }

            case FORCE_MODE_DATA:
                {
                    return "FORCE_MODE_DATA";
                }

            case ADDITIONAL_INFO:
                {
                    return "ADDITIONAL_INFO";
                }

            case CALIBRATION_DATA:
                {
                    return "CALIBRATION_DATA";
                }

            default:
                return "no package type match";
            }
        }
    };

    struct robot_message_type
    {
        static const int   ROBOT_MESSAGE_TEXT                    = 0;
        static const int   ROBOT_MESSAGE_PROGRAM_LABEL           = 1;
        static const int   PROGRAM_STATE_MESSAGE_VARIABLE_UPDATE = 2;
        static const int   ROBOT_MESSAGE_VERSION                 = 3;
        static const int   ROBOT_MESSAGE_SAFETY_MODE             = 5;
        static const int   ROBOT_MESSAGE_ERROR_CODE              = 6;
        static const int   ROBOT_MESSAGE_KEY               = 7;
        static const int   ROBOT_MESSAGE_REQUEST_VALUE     = 9;
        static const int   ROBOT_MESSAGE_RUNTIME_EXCEPTION = 10;
        static std::string to_string (int i)
        {
            switch ( i )
            {
            case ROBOT_MESSAGE_TEXT:
                {
                    return "ROBOT_MESSAGE_TEXT";
                }

            case ROBOT_MESSAGE_PROGRAM_LABEL:
                {
                    return "ROBOT_MESSAGE_PROGRAM_LABEL";
                }

            case PROGRAM_STATE_MESSAGE_VARIABLE_UPDATE:
                {
                    return "PROGRAM_STATE_MESSAGE_VARIABLE_UPDATE";
                }

            case ROBOT_MESSAGE_VERSION:
                {
                    return "ROBOT_MESSAGE_VERSION";
                }

            case ROBOT_MESSAGE_SAFETY_MODE:
                {
                    return "ROBOT_MESSAGE_SAFETY_MODE";
                }

            case ROBOT_MESSAGE_ERROR_CODE:
                {
                    return "ROBOT_MESSAGE_ERROR_CODE";
                }

            case ROBOT_MESSAGE_KEY:
                {
                    return "ROBOT_MESSAGE_KEY";
                }

            case ROBOT_MESSAGE_REQUEST_VALUE:
                {
                    return "ROBOT_MESSAGE_REQUEST_VALUE";
                }

            case ROBOT_MESSAGE_RUNTIME_EXCEPTION:
                {
                    return "ROBOT_MESSAGE_RUNTIME_EXCEPTION";
                }

            default:
                return "no robot mode match";
            }
        }
    };
    struct robot_mode_type
    {
        static const int   ROBOT_RUNNING_MODE           = 0;
        static const int   ROBOT_FREEDRIVE_MODE         = 1;
        static const int   ROBOT_READY_MODE             = 2;
        static const int   ROBOT_INITIALIZING_MODE      = 3;
        static const int   ROBOT_SECURITY_STOPPED_MODE  = 4;
        static const int   ROBOT_EMERGENCY_STOPPED_MODE = 5;
        static const int   ROBOT_FATAL_ERROR_MODE       = 6;
        static const int   ROBOT_NO_POWER_MODE          = 7;
        static const int   ROBOT_NOT_CONNECTED_MODE     = 8;
        static const int   ROBOT_SHUTDOWN_MODE          = 9;
        static const int   ROBOT_SAFEGUARD_STOP_MODE    = 10;
        static std::string to_string (int i)
        {
            switch ( i )
            {
            case ROBOT_RUNNING_MODE:
                {
                    return "ROBOT_RUNNING_MODE";
                }

            case ROBOT_FREEDRIVE_MODE:
                {
                    return "ROBOT_FREEDRIVE_MODE";
                }

            case ROBOT_READY_MODE:
                {
                    return "ROBOT_READY_MODE";
                }

            case ROBOT_INITIALIZING_MODE:
                {
                    return "ROBOT_INITIALIZING_MODE";
                }

            case ROBOT_SECURITY_STOPPED_MODE:
                {
                    return "ROBOT_SECURITY_STOPPED_MODE";
                }

            case ROBOT_EMERGENCY_STOPPED_MODE:
                {
                    return "ROBOT_EMERGENCY_STOPPED_MODE";
                }

            case ROBOT_FATAL_ERROR_MODE:
                {
                    return "ROBOT_FATAL_ERROR_MODE";
                }

            case ROBOT_NO_POWER_MODE:
                {
                    return "ROBOT_NO_POWER_MODE";
                }

            case ROBOT_NOT_CONNECTED_MODE:
                {
                    return "ROBOT_NOT_CONNECTED_MODE";
                }

            case ROBOT_SHUTDOWN_MODE:
                {
                    return "ROBOT_SHUTDOWN_MODE";
                }

            case ROBOT_SAFEGUARD_STOP_MODE:
                {
                    return "ROBOT_SAFEGUARD_STOP_MODE";
                }

            default:
                return "no robot mode match";
            }
        }
    };

    struct robot_state_type
    {
        static const int   ROBOT_MODE_DISCONNECTED      = 0;
        static const int   ROBOT_MODE_CONFIRM_SAFETY    = 1;
        static const int   ROBOT_MODE_BOOTING           = 2;
        static const int   ROBOT_MODE_POWER_OFF         = 3;
        static const int   ROBOT_MODE_POWER_ON          = 4;
        static const int   ROBOT_MODE_IDLE              = 5;
        static const int   ROBOT_MODE_BACKDRIVE         = 6;
        static const int   ROBOT_MODE_RUNNING           = 7;
        static const int   ROBOT_MODE_UPDATING_FIRMWARE = 8;
        static std::string to_string (int i)
        {
            switch ( i )
            {
            case ROBOT_MODE_DISCONNECTED:
                {
                    return "ROBOT_MODE_DISCONNECTED";
                }

            case ROBOT_MODE_CONFIRM_SAFETY:
                {
                    return "ROBOT_MODE_CONFIRM_SAFETY";
                }

            case ROBOT_MODE_BOOTING:
                {
                    return "ROBOT_MODE_BOOTING";
                }

            case ROBOT_MODE_POWER_OFF:
                {
                    return "ROBOT_MODE_POWER_OFF";
                }

            case ROBOT_MODE_POWER_ON:
                {
                    return "ROBOT_MODE_POWER_ON";
                }

            case ROBOT_MODE_IDLE:
                {
                    return "ROBOT_MODE_IDLE";
                }

            case ROBOT_MODE_BACKDRIVE:
                {
                    return "ROBOT_MODE_BACKDRIVE";
                }

            case ROBOT_MODE_RUNNING:
                {
                    return "ROBOT_MODE_RUNNING";
                }

            case ROBOT_MODE_UPDATING_FIRMWARE:
                {
                    return "ROBOT_MODE_UPDATING_FIRMWARE";
                }

            default:
                return "no robot mode match";
            }
        }
    };
    struct control_mode_type
    {
        static const int   CONTROL_MODE_POSITION = 0;
        static const int   CONTROL_MODE_TEACH    = 1;
        static const int   CONTROL_MODE_FORCE    = 2;
        static const int   CONTROL_MODE_TORQUE   = 3;
        static std::string to_string (int i)
        {
            switch ( i )
            {
            case CONTROL_MODE_POSITION:
                {
                    return "CONTROL_MODE_POSITION";
                }

            case CONTROL_MODE_TEACH:
                {
                    return "CONTROL_MODE_TEACH";
                }

            case CONTROL_MODE_FORCE:
                {
                    return "CONTROL_MODE_FORCE";
                }

            case CONTROL_MODE_TORQUE:
                {
                    return "CONTROL_MODE_TORQUE";
                }

            default:
                return "no control mode match";
            }
        }
    };
    struct joint_mode_type
    {
        static const int   JOINT_SHUTTING_DOWN_MODE        = 236;
        static const int   JOINT_PART_D_CALIBRATION_MODE   = 237;
        static const int   JOINT_BACKDRIVE_MODE            = 238;
        static const int   JOINT_POWER_OFF_MODE            = 239;
        static const int   JOINT_NOT_RESPONDING_MODE       = 245;
        static const int   JOINT_MOTOR_INITIALISATION_MODE = 246;
        static const int   JOINT_BOOTING_MODE = 247;
        static const int   JOINT_PART_D_CALIBRATION_ERROR_MODE = 248;
        static const int   JOINT_BOOTLOADER_MODE  = 249;
        static const int   JOINT_CALIBRATION_MODE = 250;
        static const int   JOINT_FAULT_MODE       = 252;
        static const int   JOINT_RUNNING_MODE     = 253;
        static const int   JOINT_IDLE_MODE        = 255;
        static std::string to_string (int i)
        {
            switch ( i )
            {
            case JOINT_SHUTTING_DOWN_MODE:
                {
                    return "JOINT_SHUTTING_DOWN_MODE";
                }

            case JOINT_PART_D_CALIBRATION_MODE:
                {
                    return "JOINT_PART_D_CALIBRATION_MODE";
                }

            case JOINT_BACKDRIVE_MODE:
                {
                    return "JOINT_BACKDRIVE_MODE";
                }

            case JOINT_POWER_OFF_MODE:
                {
                    return "JOINT_POWER_OFF_MODE";
                }

            case JOINT_NOT_RESPONDING_MODE:
                {
                    return "JOINT_NOT_RESPONDING_MODE";
                }

            case JOINT_MOTOR_INITIALISATION_MODE:
                {
                    return "JOINT_MOTOR_INITIALISATION_MODE";
                }

            case JOINT_BOOTING_MODE:
                {
                    return "JOINT_BOOTING_MODE";
                }

            case JOINT_PART_D_CALIBRATION_ERROR_MODE:
                {
                    return "JOINT_PART_D_CALIBRATION_ERROR_MODE";
                }

            case JOINT_BOOTLOADER_MODE:
                {
                    return "JOINT_BOOTLOADER_MODE";
                }

            case JOINT_CALIBRATION_MODE:
                {
                    return "JOINT_CALIBRATION_MODE";
                }

            case JOINT_FAULT_MODE:
                {
                    return "JOINT_FAULT_MODE";
                }

            case JOINT_RUNNING_MODE:
                {
                    return "JOINT_RUNNING_MODE";
                }

            case JOINT_IDLE_MODE:
                {
                    return "JOINT_IDLE_MODE";
                }

            default:
                return "no control mode match";
            }
        }
    };
    struct safety_mode_type
    {
        static const int   SAFETY_MODE_NORMAL                = 1;
        static const int   SAFETY_MODE_REDUCED               = 2;
        static const int   SAFETY_MODE_PROTECTIVE_STOP       = 3;
        static const int   SAFETY_MODE_RECOVERY              = 4;
        static const int   SAFETY_MODE_SAFEGUARD_STOP        = 5;
        static const int   SAFETY_MODE_SYSTEM_EMERGENCY_STOP = 6;
        static const int   SAFETY_MODE_ROBOT_EMERGENCY_STOP  = 7;
        static const int   SAFETY_MODE_VIOLATION             = 8;
        static const int   SAFETY_MODE_FAULT = 9;
        static std::string to_string (int i)
        {
            switch ( i )
            {
            case SAFETY_MODE_NORMAL:
                {
                    return "SAFETY_MODE_NORMAL";
                }

            case SAFETY_MODE_REDUCED:
                {
                    return "SAFETY_MODE_REDUCED";
                }

            case SAFETY_MODE_PROTECTIVE_STOP:
                {
                    return "SAFETY_MODE_PROTECTIVE_STOP";
                }

            case SAFETY_MODE_RECOVERY:
                {
                    return "SAFETY_MODE_RECOVERY";
                }

            case SAFETY_MODE_SAFEGUARD_STOP:
                {
                    return "SAFETY_MODE_SAFEGUARD_STOP";
                }

            case SAFETY_MODE_SYSTEM_EMERGENCY_STOP:
                {
                    return "SAFETY_MODE_SYSTEM_EMERGENCY_STOP";
                }

            case SAFETY_MODE_ROBOT_EMERGENCY_STOP:
                {
                    return "SAFETY_MODE_ROBOT_EMERGENCY_STOP";
                }

            case SAFETY_MODE_VIOLATION:
                {
                    return "SAFETY_MODE_VIOLATION";
                }

            case SAFETY_MODE_FAULT:
                {
                    return "SAFETY_MODE_FAULT";
                }

            default:
                return "no control mode match";
            }
        }
    };
}
#endif
