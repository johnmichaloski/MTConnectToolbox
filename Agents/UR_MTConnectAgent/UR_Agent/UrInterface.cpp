
//
// UrInterface.cpp
//

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#include "UrInterface.h"
#include "stdafx.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "NIST/hexdump.h"

#ifdef __linux__
#include "LinuxGlobals.h"
#endif

using namespace ur;
int ur::numjoints = 6;

///////////////////////////////////////////////////
void ur_version_message::decode (char *buf, int len)
{
#ifdef _DEBUG
    assert(len > 0);
    std::cout << NIST::HexDump(buf, len);
#endif
    size_t offset = 0;

    offset += bscopy((char *) &messageSize, buf + offset, sizeof( messageSize ));
    offset += bscopy((char *) &messageType, buf + offset, sizeof( messageType ));

    //
    // messageType
    // ==
    // 20!
    offset += bscopy((char *) &timestamp, buf + offset, sizeof( timestamp ));
    offset += bscopy((char *) &source, buf + offset, sizeof( source ));
    offset += bscopy((char *) &robot_message_type, buf + offset,
                     sizeof( robot_message_type ));

    offset += bscopy((char *) &project_name_size, buf + offset,
                     sizeof( project_name_size ));

    memcpy(&project_name, &buf[offset], sizeof( char ) * project_name_size);
    offset += project_name_size;
    project_name[project_name_size] = '\0';

    offset += bscopy((char *) &major_version, buf + offset, sizeof( major_version ));
    offset += bscopy((char *) &minor_version, buf + offset, sizeof( minor_version ));

    /// offset+=bscopy((char
    // *)
    // &svn_revision,
    // buf+offset,
    // sizeof(svn_revision));
    offset += bscopy((char *) &bugfixVersion, buf + offset, sizeof( bugfixVersion ));
    offset += bscopy((char *) &buildNumber, buf + offset, sizeof( buildNumber ));
    memcpy(&build_date, buf + offset, sizeof( char ) * len - offset);
    build_date[len - offset] = '\0';

    //
    // if
    // (major_version
    // <
    // 2)
    // {
    //	robot_mode_running_
    // =
    // robotStateTypeV18::ROBOT_RUNNING_MODE;
    //
    // }
}
double ur_version_message::getVersion ( )
{
    double ver;

    //
    // val_lock_.lock();
    //
    // FIXME
    // need
    // a
    // locked
    // copy
    ver = this->major_version + 0.1 * this->minor_version;

    //
    // +
    // .0000001
    // *
    // this->svn_revision;

    //
    // val_lock_.unlock();
    return ver;
}
//////////////////////////////////////////////////////////////////////////////
#include "NIST/StdStringFcn.h"
size_t ur_configuration_data::msglength ( )
{
    return struct_decoder<ur_configuration_data>::msglength( )
           + sizeof( ur_joint_limits ) * numjoints
           + sizeof( ur_joint_maxs ) * numjoints;
}
void ur_configuration_data::read (char *buf)
{
    _joint_limits.resize(numjoints);
    _joint_maxes.resize(numjoints);

    //
    // DHa.resize(numjoints,0.0)	;
    //
    // DHd.resize(numjoints,0.0)	;
    //
    // DHalpha.resize(numjoints,0.0)	;
    //
    // DHtheta.resize(numjoints,0.0)	;
    //
    // motorType.resize(numjoints,0)	;
    size_t offset = 0;

    for ( size_t i = 0; i < numjoints; i++ )
    {
        offset += bscopy((char *) &_joint_limits[i].min_limit, buf + offset,
                         sizeof( _joint_limits[i].min_limit ));
        offset += bscopy((char *) &_joint_limits[i].max_limit, buf + offset,
                         sizeof( _joint_limits[i].max_limit ));
        offset += bscopy((char *) &_joint_maxes[i].max_speed, buf + offset,
                         sizeof( _joint_maxes[i].max_speed ));
        offset += bscopy((char *) &_joint_maxes[i].max_acceleration, buf + offset,
                         sizeof( _joint_maxes[i].max_acceleration ));
    }
    buf = buf + sizeof( ur_joint_limits ) * numjoints
          + sizeof( ur_joint_maxs ) * numjoints;
    struct_decoder<ur_configuration_data>::read(buf);
}
void ur_configuration_data::write (char *buf, int bAddHeader)
{
    //
    // Unclear
    // how
    // you
    // check
    // if
    // large
    // enough
    size_t offset = 0;

    if ( bAddHeader )
    {
        packageSize = msglength( );
        packageType = package_types::CONFIGURATION_DATA;

        offset += bscopy(buf + offset, (char *) &packageSize, sizeof( packageSize ));
        offset += bscopy(buf + offset, (char *) &packageType, sizeof( packageType ));
    }

    for ( size_t i = 0; i < numjoints; i++ )
    {
        offset += bscopy(buf + offset, (char *) &_joint_limits[i].min_limit,
                         sizeof( _joint_limits[i].min_limit ));
        offset += bscopy(buf + offset, (char *) &_joint_limits[i].max_limit,
                         sizeof( _joint_limits[i].max_limit ));
        offset += bscopy(buf + offset, (char *) &_joint_maxes[i].max_speed,
                         sizeof( _joint_maxes[i].max_speed ));
        offset += bscopy(buf + offset, (char *) &_joint_maxes[i].max_acceleration,
                         sizeof( _joint_maxes[i].max_acceleration ));
    }
    buf = buf + offset;
    struct_decoder<ur_configuration_data>::write(buf);
}
std::string ur_configuration_data::print ( )
{
    std::string tmp;

    tmp += "ur_configuration_data:\n";
    tmp += "\tjoint min,max,maxvel,maxacc\n";

    for ( size_t i = 0; i < numjoints; i++ )
    {
        tmp
            += StdStringFormat("\t %d = %f:%f:%f:%f \n", i, _joint_limits[i].min_limit,
                               _joint_limits[i].min_limit, _joint_maxes[i].max_speed,
                               _joint_maxes[i].max_acceleration);
    }
    tmp += StdStringFormat("\tvJointDefault=%f\n", vJointDefault);
    tmp += StdStringFormat("\taJointDefault=%f\n", aJointDefault);
    tmp += StdStringFormat("\tvToolDefault=%f\n", vToolDefault);
    tmp += StdStringFormat("\taToolDefault=%f\n", aToolDefault);
    tmp += StdStringFormat("\teqRadius=%f\n", eqRadius);

    tmp += StdStringFormat("\tDHa=");

    for ( size_t i = 0; i < numjoints; i++ )
    {
        tmp += StdStringFormat("%f:", DHa[i]);
    }
    tmp += StdStringFormat("\n\tDHd=");

    for ( size_t i = 0; i < numjoints; i++ )
    {
        tmp += StdStringFormat("%f:", DHd[i]);
    }
    tmp += StdStringFormat("\n\tDHalpha=");

    for ( size_t i = 0; i < numjoints; i++ )
    {
        tmp += StdStringFormat("%f:", DHalpha[i]);
    }
    tmp += StdStringFormat("\n\tDHtheta=");

    for ( size_t i = 0; i < numjoints; i++ )
    {
        tmp += StdStringFormat("%f:", DHtheta[i]);
    }
    tmp += StdStringFormat("\n\tmasterboardVersion=%d\n", masterboardVersion);
    tmp += StdStringFormat("\tcontrollerBoxType=%d\n", controllerBoxType);
    tmp += StdStringFormat("\trobotType=%d\n", robotType);
    tmp += StdStringFormat("\trobotSubType=%d\n", robotSubType);
    tmp += StdStringFormat("\tmotorType=");

    for ( size_t i = 0; i < numjoints; i++ )
    {
        tmp += StdStringFormat("%d:", motorType[i]);
    }
    tmp += "\n";

    return tmp;
}
//////////////////////////////////////////////////////////////////////////////////////////////
unsigned char test_msg[] =
{
    {   0 }, {   0 }, {   1 }, { 209 }, {  16 }, {   0 }, {   0 }, {   0 }, {  29 }, {   0 }, {   0 },
    {   0 }, {   0 }, {   0 }, {   0 }, {   5 }, {   7 }, { 246 }, {   1 }, {   1 }, {   1 }, {   0 },
    {   0 }, {   0 }, {   0 }, {   0 }, {  63 }, { 240 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 },
    {   0 }, {   0 }, {   0 }, {   0 }, { 251 }, {   1 }, {  64 }, {   1 }, {  78 }, { 244 }, {  77 },
    { 189 }, { 249 }, { 149 }, {  64 }, {   1 }, {  78 }, { 247 }, {  89 }, {  95 }, { 104 }, {  85 },
    {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, { 188 }, { 220 }, {  97 },
    {   3 }, {  66 }, {  62 }, {   0 }, {   0 }, {  66 }, {   0 }, { 102 }, { 103 }, {  66 }, {  99 },
    { 153 }, { 154 }, { 253 }, { 191 }, { 246 }, {  74 }, { 170 }, { 216 }, { 242 }, {  29 }, { 102 },
    { 191 }, { 246 }, {  74 }, { 178 }, {  44 }, {  92 }, {  72 }, { 137 }, {   0 }, {   0 }, {   0 },
    {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, { 192 }, {   1 }, { 106 }, {  78 }, {  66 }, {  63 },
    { 153 }, { 154 }, {  66 }, {   4 }, { 204 }, { 205 }, {  66 }, { 104 }, {   0 }, {   0 }, { 253 },
    {  63 }, { 253 }, {  49 }, { 202 }, {  91 }, { 202 }, {   8 }, {  64 }, {  63 }, { 253 }, {  49 },
    { 210 }, { 233 }, {  51 }, {  16 }, {  35 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 },
    {   0 }, {   0 }, { 191 }, { 155 }, { 135 }, {  34 }, {  66 }, {  62 }, {   0 }, {   0 }, {  65 },
    { 249 }, { 153 }, { 154 }, {  66 }, { 100 }, {   0 }, {   0 }, { 253 }, { 191 }, { 220 }, { 115 },
    { 204 }, { 104 }, { 205 }, { 239 }, { 254 }, { 191 }, { 220 }, { 118 }, {  68 }, { 109 }, {  49 },
    {  34 }, { 158 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, { 190 },
    {  60 }, { 245 }, { 109 }, {  66 }, {  63 }, { 153 }, { 154 }, {  66 }, {  25 }, {  51 }, {  51 },
    {  66 }, { 116 }, { 204 }, { 205 }, { 253 }, {  63 }, { 242 }, { 146 }, { 224 }, { 105 }, { 231 },
    {  66 }, { 209 }, {  63 }, { 242 }, { 146 }, {  65 }, {   3 }, { 193 }, { 196 }, { 156 }, {   0 },
    {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, { 190 }, { 115 }, { 157 }, { 190 },
    {  66 }, {  62 }, {   0 }, {   0 }, {  66 }, {  21 }, {  51 }, {  51 }, {  66 }, { 119 }, { 153 },
    { 154 }, { 253 }, { 191 }, { 231 }, { 207 }, {   8 }, { 215 }, {  85 }, {  22 }, {  88 }, { 191 },
    { 231 }, { 206 }, {  77 }, { 130 }, { 151 }, { 190 }, {  17 }, { 191 }, { 146 }, { 242 }, { 158 },
    { 148 }, { 114 }, { 240 }, {  57 }, { 188 }, { 224 }, { 224 }, {  96 }, {  66 }, {  68 }, { 102 },
    { 103 }, {  66 }, {  37 }, { 153 }, { 154 }, {  66 }, { 127 }, { 153 }, { 154 }, { 253 }, {   0 },
    {   0 }, {   0 }, {  53 }, {   4 }, {  63 }, { 217 }, { 153 }, {  52 }, { 224 }, {  36 }, { 238 },
    {  93 }, { 191 }, { 217 }, { 153 }, { 169 }, {  67 }, { 241 }, { 211 }, {  23 }, {  63 }, { 207 },
    { 255 }, { 137 }, {   8 }, {  22 }, { 253 }, { 198 }, {  63 }, { 240 }, {   0 }, { 170 }, { 111 },
    { 207 }, {  54 }, { 176 }, {  63 }, { 243 }, {  51 }, {  88 }, { 137 }, {  58 }, { 151 }, { 217 },
    {  63 }, { 201 }, { 148 }, { 119 }, { 151 }, {  70 }, { 237 }, { 237 }, {   0 }, {   0 }, {   0 },
    {  29 }, {   5 }, {  64 }, { 143 }, {  64 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {  64 },
    { 143 }, {  64 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {  64 }, { 143 }, {  64 }, {   0 },
    {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {  61 }, {   3 }, {   0 }, {  63 },
    {   0 }, {   0 }, {   0 }, {   0 }, {  63 }, { 123 }, { 129 }, { 184 }, {  27 }, { 129 }, { 184 },
    {  28 }, {  63 }, { 112 }, { 225 }, {  14 }, {  16 }, { 225 }, {  14 }, {  17 }, {   0 }, {   0 },
    {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {   0 },
    {   0 }, {   0 }, {   0 }, {   0 }, {   0 }, {  66 }, {  87 }, {  51 }, {  51 }, {  66 }, {  66 },
    {   0 }, {   0 }, {  62 }, {  35 }, { 215 }, {  11 }, {  61 }, { 241 }, { 169 }, { 253 }, {   0 },
    {   0 }, {   0 }, {  37 }, {   2 }, {   0 }, {   0 }, {  63 }, { 141 }, {  83 }, {  47 }, { 180 },
    { 171 }, { 196 }, { 232 }, {  63 }, { 137 }, {  46 }, {  99 }, { 102 }, {  69 }, { 149 }, { 155 },
    {  66 }, {  55 }, {  51 }, {  51 }, {   0 }, {  59 }, {  68 }, { 155 }, { 166 }, {  66 }, {  92 },
    {   0 }, {   0 }, { 253 }, {   0 }
};

void test1_robot_state ( )
{
    ur_message_header hdr;

    hdr.read((char *) &test_msg[0]);
    std::cout << hdr.dump( );
    std::cout << hdr.print( ).c_str( );
}
struct array_test : public struct_decoder<array_test>
{
    char     charArray[10];
    uint32_t intArray[3];

    BEGIN_SERIAL_PROP_MAP(array_test)

    //
    // PROP_SERIAL_ENTRY(charArray,
    //
    // "%s")
    PROP_SERIAL_ENTRY_ARRAY(charArray, sizeof( char ), "%c")
    PROP_SERIAL_ENTRY_ARRAY(intArray, sizeof( uint32_t ), "%d")
    END_SERIAL_PROP_MAP( )
};

void test3_robot_state ( )
{
    unsigned char array_msg[] = { { 48 }, {  49 }, { 50 }, { 51 }, { 52 }, {  53 }, { 54 }, { 55 },
                                  { 56 }, {  57 }, {  0 }, {  0 }, {  1 }, { 209 }, {  0 }, {  0 },
                                  {  1 }, { 208 }, {  0 }, {  0 }, {  0 }, {  24 } };
    array_test    _array;

    _array.read(array_msg);
    std::cout << _array.print( ).c_str( );
}
void test2_robot_state ( )
{
    ur_message_header hdr;

    std::cout << &hdr << "\n";
    hdr.read((char *) &test_msg[0]);

    //
    // std::cout
    // <<
    // hdr.dump();
    std::cout << NIST::HexDump(&hdr, 16);

    //
    // std::cout
    // <<
    // NIST::HexDump(&test_msg[0],
    // 16);
    ur_robot_state r;

    r.unpack(&test_msg[0], hdr.packageSize);
    std::cout << r.dump( ).c_str( );
}
void convert_bin_telnet_Cpp (std::string path)
{
    std::ofstream of(( path + "1" ).c_str( ));
    std::ifstream fin(path.c_str( ));
    std::string   msgs(( std::istreambuf_iterator<char>(fin) ),
                       std::istreambuf_iterator<char>( ));

    ur_message_header hdr;
    size_t            msgsize = msgs.size( );
    size_t            offset  = 0;
    size_t            n       = 0;

    while ( true )
    {
        hdr.read(&msgs[0]);

        if ( hdr.packageSize > msgsize )
        {
            break;                                         //
                                                           // unfinished
                                                           // message
        }
        of << StdStringFormat("char * msg%d = { \n", n++);

        //
        // convert
        // binary
        // to
        // {byte},
        // output
        for ( size_t j = 0; j < hdr.packageSize; j++ )
        {
            of << StdStringFormat("{%d},", msgs[j]);

            if ( !( j % 10 ) )
            {
                of << std::endl;
            }
        }
        of << "};\n";
        msgsize -= hdr.packageSize;
    }

    of.close( );
}
#define _USE_MATH_DEFINES
#include "NIST/File.h"
#include "math.h"

void write_ur_configuration_data (ur_configuration_data & config)
{
    //
    // Initialize
    // joint
    // limits
    // struct
    ur_joint_limits jl[6] = { { 1., 2. }, { 3.,  4. }, {  5.,  6. },
                              { 7., 8. }, { 9., 10. }, { 11., 12. } };

    std::vector<ur_joint_limits> vjl = std::vector<ur_joint_limits>(jl, jl + 6);

    //
    // Initialize
    // joint
    // max
    // vel/acc
    // struct
    ur_joint_maxs jm[6] = { { -1., 2. }, { -3.,  4. }, {  -5.,  6. },
                            { -7., 8. }, { -9., 10. }, { -11., 12. } };

    std::vector<ur_joint_maxs> vjm = std::vector<ur_joint_maxs>(jm, jm + 6);

    //
    // Initialize
    // config
    // test
    // joint
    // limits
    // and
    // joint
    // maxes
    config._joint_limits = vjl;
    config._joint_maxes  = vjm;

    //
    // config._joint_limits.resize(6,
    // joint_limits(-10.0,+10.0));
    //
    // config._joint_maxes.resize(6,
    // joint_maxes(1.0,10.0));

    config.vJointDefault = 1.0;
    config.aJointDefault = 10.;
    config.vToolDefault  = 1.;
    config.aToolDefault  = 5.;
    config.eqRadius      = 2.;

    double dDHa[6] = { 1., 2., 3., 4., 5., 6. };

    //
    // config.DHa=std::vector<double>(dDHa,
    // dDHa+6)	;
    memcpy(config.DHa, dDHa, sizeof( dDHa ));

    double dDHd[6] = { 6., 5., 4., 3., 2., 1. };

    //
    // config.DHd=std::vector<double>(dDHd,
    // dDHd+6)	;
    memcpy(config.DHd, dDHd, sizeof( dDHd ));

    double dDHalpha[6] = { 1., 10., 100., 100., 10., 1. };

    //
    // config.DHalpha=std::vector<double>(dDHalpha,
    // dDHalpha+6)	;
    memcpy(config.DHalpha, dDHalpha, sizeof( dDHalpha ));

    double dDHtheta[6] = { M_PI, M_PI / 2., M_PI / 4., M_PI / 4., M_PI / 2., M_PI };

    //
    // config.DHtheta=std::vector<double>(dDHtheta,
    // dDHtheta+6)	;
    memcpy(config.DHtheta, dDHtheta, sizeof( dDHtheta ));

    std::vector<double> DHtheta;
    config.masterboardVersion = 2;
    config.controllerBoxType  = 99;
    config.robotType          = 3;
    config.robotSubType       = 3;
    int iMotorType[6] = { 1, 2, 3, 4, 5, 6 };

    //
    // config.motorType=std::vector<int>(iMotorType,
    // iMotorType+6);
    memcpy(config.motorType, iMotorType, sizeof( iMotorType ));

    //
    // Now
    // write
    // the
    // ur
    // configuration
    // to
    // file
    char buffer[2048];

    config.write(buffer, 1);
    buffer[config.msglength( ) + 1] = 0;
    std::ofstream outFile;
    outFile.open(File.ExeDirectory( ) + "config.txt",
                 std::ios::out | std::ios::binary);
    outFile.write(buffer, config.msglength( ) + 5);
    outFile.close( );
}
void read_ur_configuration_data (ur_configuration_data & config)
{
    char buffer[2048];

    std::ifstream inFile;
    inFile.open(File.ExeDirectory( ) + "config.txt",
                std::ios::in | std::ios::binary);

    //
    // get
    // length
    // of
    // file:
    // annoying
    inFile.seekg(0, inFile.end);
    int length = inFile.tellg( );

    inFile.seekg(0, inFile.beg);

    inFile.read(buffer, length);
    inFile.close( );

    ur_message_header hdr;

    hdr.read(buffer);

    config.read(buffer + 5);
}
////////////////////////////////////////////////////////////////////
void ur_robot_state::unpack (uint8_t *buf, unsigned int buf_length)
{
    unsigned int      offset = 0;
    ur_message_header hdr;

    while ( buf_length > offset )
    {
        hdr.read((char *) buf);

        if ( hdr.packageSize + offset > buf_length )
        {
            /* Should returns missing bytes to unpack a message, or 0 if all data was
             * parsed */

            // Kind of a mess without a framing byte to understand when a message
            // ends.
            return;
        }

        switch ( hdr.packageType )
        {
        case message_types::ROBOT_MESSAGE:
            {
                unpackRobotMessage(buf, offset, hdr.packageSize);
            }
            break;

        case message_types::ROBOT_STATE:
            {
                unpackRobotState(buf, offset, hdr.packageSize);
            }
            break;

        case message_types::PROGRAM_STATE_MESSAGE:
            {
                logDebug("message_types::PROGRAM_STATE_MESSAGE not implemented\n");

                // Don't do anything atm...
            }
            break;

        default:
            { }
            break;
        }

        offset += hdr.packageSize;
    }
}
void ur_robot_state::unpackRobotState (uint8_t *buf, unsigned int offset,
                                   uint32_t len)
{
    // package_types
    offset += 5;

    while ( offset < len )
    {
        ur_message_header hdr;
        hdr.read((char *) &buf[offset]);

        switch ( hdr.packageType )
        {
        case package_types::JOINT_DATA:
            {
                _ur_joint_data.clear( );
                size_t n = ur_joint_data( ).msglength( );

                // fIXME: hardcoded subtraction of hdr legnth
                for ( size_t j = 0; j < hdr.packageSize - 5; j += n )
                {
                    _ur_joint_data.push_back(ur_joint_data( ));
                    _ur_joint_data.back( ).read((char *) &buf[j + offset + hdr.msglength( )]);
                }
            }
            break;

        case package_types::TOOL_DATA:
            {
                _ur_tool_data.read((char *) &buf[offset + hdr.msglength( )]);
            }
            break;

        case package_types::CARTESIAN_INFO:
            {
                _ur_cartesian_info.read((char *) &buf[offset + hdr.msglength( )]);
            }
            break;

        case package_types::KINEMATICS_INFO:
            {
                logDebug("package_types::KINEMATICS_INFO not implemented\n");

                // no real info, possibly checksum
            }
            break;

        case package_types::CONFIGURATION_DATA:
            {
                _ur_configuration_data.read((char *) &buf[offset + hdr.msglength( )]);

                // ErrorMessage ("package_types::CONFIGURATION_DATA not implemented\n");
            }
            break;

        case package_types::FORCE_MODE_DATA:
            {
                _ur_force_mode_data.read((char *) &buf[offset + hdr.msglength( )]);
            }
            break;

        case package_types::ADDITIONAL_INFO:
            {
                _ur_additional_info.read((char *) &buf[offset + hdr.msglength( )]);
            }
            break;

        case package_types::CALIBRATION_DATA:
            {
                LOG_ONCE(logDebug("package_types::CALIBRATION_DATA skipped\n"));
            }
            break;

        case package_types::ROBOT_MODE_DATA:
            {
                _ur_robot_mode_data.read((char *) &buf[offset + hdr.msglength( )]);
            }
            break;

        case package_types::MASTERBOARD_DATA:
            {
                _ur_masterboard_data.read((char *) &buf[offset + hdr.msglength( )]);
            }
            break;

        default:
            { }
            break;
        }

        offset += hdr.packageSize;
    }

    // new_data_available_ = true;
    // pMsg_cond_->notify_all();
}
void ur_robot_state::unpackRobotMessage (uint8_t *buf, unsigned int offset,
                                     uint32_t len)
{
    ur_robot_message rmsg;

    rmsg.read((char *) buf);

    // robot_message_type
    switch ( rmsg.robot_message_type )
    {
    case robot_message_type::ROBOT_MESSAGE_VERSION:
        {
            _ur_version_message.read((char *) buf + 5);
        }
        break;

    case robot_message_type::ROBOT_MESSAGE_TEXT:
        {                                                  /*0*/
            logDebug("robot_message_type::ROBOT_MESSAGE_TEXT not implemented\n");
        }
        break;

    case robot_message_type::ROBOT_MESSAGE_PROGRAM_LABEL:
        {                                                  /*1*/
            logDebug(
                "robot_message_type::ROBOT_MESSAGE_PROGRAM_LABEL not implemented\n");
        }
        break;

    case robot_message_type::PROGRAM_STATE_MESSAGE_VARIABLE_UPDATE:
        {                                                  /*2*/
            logDebug("robot_message_type::PROGRAM_STATE_MESSAGE_VARIABLE_UPDATE not "
                     "implemented\n");
        }
        break;

    case robot_message_type::ROBOT_MESSAGE_SAFETY_MODE:
        {                                                  /*5*/
            logDebug("robot_message_type::ROBOT_MESSAGE_SAFETY_MODE not implemented\n");
        }
        break;

    case robot_message_type::ROBOT_MESSAGE_ERROR_CODE:
        {                                                  /*6*/
            logDebug("robot_message_type::ROBOT_MESSAGE_ERROR_CODE not implemented\n");
        }
        break;

    case robot_message_type::ROBOT_MESSAGE_KEY:
        {
            logDebug("robot_message_type::ROBOT_MESSAGE_KEY not implemented\n");
        }
        break;

    case robot_message_type::ROBOT_MESSAGE_REQUEST_VALUE:
        {
            logDebug(
                "robot_message_type::ROBOT_MESSAGE_REQUEST_VALUE not implemented\n");
        }
        break;

    case robot_message_type::ROBOT_MESSAGE_RUNTIME_EXCEPTION:
        {                                                  /* 10 */
            logDebug("robot_message_type::ROBOT_MESSAGE_RUNTIME_EXCEPTION not "
                     "implemented\n");
        }
        break;

    default:
        {
            logDebug("robot_message_type::???????? default not implemented\n");
        }
        break;
    }
}
std::string ur_robot_state::dump ( )
{
    std::string tmp;
    tmp += "Mode\n";
    tmp += _ur_robot_mode_data.print( );
    tmp += "Cartesian\n";
    tmp += _ur_cartesian_info.print( );
    tmp += "Joint\n";

    if ( _ur_joint_data.size( ) == numjoints )
    {
        for ( size_t i = 0; i < _ur_joint_data.size( ); i++ )
        {
            tmp += _ur_joint_data[i].print( );
        }
    }
    tmp += "Tool\n";
    tmp += _ur_tool_data.print( );
    tmp += "Force\n";
    tmp += _ur_force_mode_data.print( );
    tmp += "Additional Info\n";
    tmp += _ur_additional_info.print( );
    return tmp;
}
bool ur_robot_state::isReady ( )
{
    if ( _ur_robot_mode_data.robotMode == robot_mode_running_ )
    {
        return true;
    }
    return false;
}
void ur_robot_state::setDisconnected ( )
{
    _ur_robot_mode_data.isRobotConnected   = false;
    _ur_robot_mode_data.isRealRobotEnabled = false;
    _ur_robot_mode_data.isPowerOnRobot     = false;

    // FIXME - should I change other MTConnect tags: power, all unavailable
}
////////////////////////////////////////////////////////////////////


