
//
// Moto_Enums.h
//
#ifndef _Moto_Enums_h_
#define _Moto_Enums_h_

namespace moto
{
enum {JOINT_MAX = 10};		/* this is really 10 per the spec */
#define JOINT_PMIN_DEFAULT -1000.0
#define JOINT_PMAX_DEFAULT 1000.0
#define JOINT_VMAX_DEFAULT 1000.0

enum {
    MESSAGE_PORT_DEFAULT = 11000,
    STATE_PORT_DEFAULT = 11002
};
enum {
    ROBOT_MODE_MANUAL = 1,
    ROBOT_MODE_AUTO = 2
};
enum SpecialSeqValue
{
    // This is totally different from Gijs wireshark LUA dissector
    // never used, so it may not matter. But still.
    //    END_TRAJECTORY = -1,  // sequence number per the spec
    //    STOP_TRAJECTORY = -2
    START_TRAJECTORY_DOWNLOAD      = -1,
    START_TRAJECOTRY_STREAMING     = -2,
    END_TRAJECTORY                 = -3,
    STOP_TRAJECTORY                = -4
};
struct valid_field_types {
    static int const VALID_FIELD_TYPE_TIME          = 0x01;
    static int const VALID_FIELD_TYPE_POSITION      = 0x02;
    static int const VALID_FIELD_TYPE_VELOCITY      = 0x04;
    static int const VALID_FIELD_TYPE_ACCELERATION  = 0x08;
    std::string to_string(unsigned int bitmask)
    {
        std::string  strs ("Valid Fields=");
        if(bitmask &VALID_FIELD_TYPE_TIME) strs+="time|";
        if(bitmask &VALID_FIELD_TYPE_POSITION) strs+="pos|";
        if(bitmask &VALID_FIELD_TYPE_VELOCITY) strs+="vel|";
        if(bitmask &VALID_FIELD_TYPE_ACCELERATION) strs+="acc|";
        if(strs.size()>0)
            strs.pop_back(); // remove last |
        return strs;
    }
};


struct message_types {
    static const int MESSAGE_PING = 1;
    static const int MESSAGE_GET_VERSION = 2;
    static const int MESSAGE_JOINT_POSITION = 10;
    static const int MESSAGE_JOINT = 10;
    static const int MESSAGE_JOINT_TRAJ_PT = 11;	  /* one point */
    static const int MESSAGE_JOINT_TRAJ = 12;	  /* an array of points */
    static const int MESSAGE_ROBOT_STATUS = 13;
    static const int MESSAGE_JOINT_TRAJ_PT_FULL = 14;
    static const int MESSAGE_JOINT_FEEDBACK = 15;
    static const int MESSAGE_READ_INPUT = 20;
    static const int MESSAGE_WRITE_OUTPUT = 21;
    static const int MESSAGE_CART_TRAJ_PT = 31;
    static const int MESSAGE_CART_FEEDBACK = 35;
    static const int MESSAGE_OBJECT_STATE = 40;
    static std::string to_string(int i)
    {
        switch(i)
        {
        case MESSAGE_PING : return "message_ping";
        case MESSAGE_GET_VERSION : return "message_get_version";
            //case MESSAGE_JOINT:
        case MESSAGE_JOINT_POSITION : return "message_joint_position";
        case MESSAGE_JOINT_TRAJ_PT : return "message_joint_traj_pt";
        case MESSAGE_JOINT_TRAJ : return "message_joint_traj";
        case MESSAGE_ROBOT_STATUS : return "message_status";
        case MESSAGE_JOINT_TRAJ_PT_FULL : return "message_joint_traj_full";
        case MESSAGE_JOINT_FEEDBACK : return "message_joint_feedback";
        case MESSAGE_READ_INPUT : return "message_read_input";
        case MESSAGE_WRITE_OUTPUT : return "message_write_intput";
        case MESSAGE_CART_TRAJ_PT : return "message_cart_traj_pt";
        case MESSAGE_CART_FEEDBACK : return "message_cart_feedback";
        case MESSAGE_OBJECT_STATE : return "message_object_state";
		default: return Util::StrFormat("message_invalid %d", i);
        }
    }
} ;

struct simple_message_comm_types
{
    static const int COMM_INVALID=0;
    static const int COMM_TOPIC = 1;
    static const int COMM_REQUEST = 2;
    static const int COMM_REPLY = 3;
    // add is_valid method?
    static std::string to_string(int i)
    {
        switch(i)
        {
        case COMM_TOPIC : return "comm_topic";
        case COMM_REQUEST : return "comm_request";
        case COMM_REPLY : return "comm_reply";
        default: return "comm_invalid";
        }
    }
};

struct simple_message_reply_types
{
    static const int REPLY_NA = 0;
    static const int REPLY_SUCCESS = 1;
    static const int REPLY_FAILURE = 2;
    static const int REPLY_EXECUTING = 3	;	/* EXTENSION */
    // add is_valid method?
    static std::string to_string(int i)
    {
        switch(i)
        {
        case REPLY_SUCCESS : return "reply_success";
        case REPLY_FAILURE : return "reply_failure";
        case REPLY_EXECUTING : return "reply_executing";
        default: return "reply_na";
        }
    }
};
}
#endif
