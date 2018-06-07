

// Note 32 bit agent will probably not work, due to some lower level construct within
// the main mtconnect institute code. So only 64 bit linking is supported

#include "stdafx.h"

#pragma comment(lib, "Ws2_32.lib")


#define MTCLIBPATH(X)    "C:\\Users\\michalos\\Documents\\GitHub\\Agents\\MTConnectAgentFromShdr\\MTConnectAgent\\win32\\libxml2-2.7.7\\lib\\" ## X
#define BOOSTLIBPATH64(X) \
    "C:\\Program Files\\NIST\\src\\boost_1_54_0\\vc10\\stagex64\\lib\\"##X
#define BOOSTLIBPATH32(X) \
    "C:\\Program Files\\NIST\\src\\boost_1_54_0\\vc10\\stagex32\\lib\\"##X


#if defined(WIN64) && defined( _DEBUG) 
#pragma message( "DEBUG x64" )
//#pragma comment(lib, "C:\\Users\\michalos\\Documents\\GitHub\\MTConnectSolutions\\MTConnectAgentFromShdr\\MTConnectAgentFromShdr\\x64\\Debug\\libiconv_a_debug.lib")
#pragma comment(lib, MTCLIBPATH("libxml2_64d.lib"))
#pragma comment(lib, BOOSTLIBPATH64("libboost_system-vc100-mt-sgd-1_54.lib"))
#pragma comment(lib, BOOSTLIBPATH64("libboost_thread-vc100-mt-sgd-1_54.lib"))

#elif !defined( _DEBUG) && defined(WIN64)
#pragma message( "RELEASE x64" )
#pragma comment(lib,  MTCLIBPATH("libxml2_64.lib"))
//#pragma comment(lib, "C:\\Users\\michalos\\Documents\\GitHub\\MTConnectSolutions\\MTConnectAgentFromShdr\\MTConnectAgentFromShdr\\x64\\Release\\libiconv_a.lib")
#pragma comment(lib, BOOSTLIBPATH64("libboost_thread-vc100-mt-s-1_54.lib"))
#pragma comment(lib, BOOSTLIBPATH64("libboost_system-vc100-mt-s-1_54.lib"))
#else
#error  32 bit agents are not supported in MSVC, since they don't work properly. They compile, link and don't work.
#endif