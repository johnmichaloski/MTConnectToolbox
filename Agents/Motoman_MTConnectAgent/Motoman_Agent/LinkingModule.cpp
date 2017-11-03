
//
// LinkingModule.cpp
//

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

/**
 This source file is a container for the major linking module in the application.
 The links are generally hard coded, but at least you know where problems occur  (personal preference).
*/
#include "stdafx.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Psapi.lib")

// Service api
#pragma comment(lib, "advapi32.lib")

// Wintricks:
#pragma comment(lib, "mpr.lib")
#pragma comment(lib, "Netapi32.lib")


#define PCNAME  1 // "moutaineer"
//#define PCNAME  2 // "agility"
//#define PCNAME  3 // "micfamily"

#if PCNAME == 3
#define MTCLIBPATH(X)                                                         \
    "D:\\michalos\\My Work\\MTConnect\\motoman_MTConnectAgent\\MTConnectAgent\\win32\\libxml2-2.7.7\\lib\\" ## X
#define BOOSTLIBPATH(X) \
    "D:\\Program Files\\NIST\\src\\boost_1_54_0\\stageX32\\lib\\" ## X
#elif PCNAME == 1
#define MTCLIBPATH(X)                                     \
    "C:\\Users\\michalos\\Documents\\GitHub\\Agents\\motoman_MTConnectAgent\\MTConnectAgent\\win32\\libxml2-2.7.7\\lib\\" ## X
#define BOOSTLIBPATH(X) \
    "C:\\Program Files\\NIST\\src\\boost_1_54_0\\vc10\\stagex64\\lib\\"##X
#elif PCNAME == 2
#define MTCLIBPATH(X)                                     \
    "C:\\Users\\michalos\\Documents\\Motoman_MTConnectAgent\\MTConnectAgent\\win32\\libxml2-2.7.7\\lib\\" ## X
#define BOOSTLIBPATH(X) \
    "C:\\Program Files\\NIST\\src\\boost_1_54_0\\vc10\\stagex64\\lib\\"##X
#else
#error no link library macros defined
#endif

#if defined( WIN64 ) && defined( _DEBUG )
#pragma message("DEBUG x64")
#pragma comment(lib, MTCLIBPATH("libxml2_64d.lib"))
#pragma comment(lib,  BOOSTLIBPATH("libboost_system-vc100-mt-sgd-1_54.lib"))
#pragma comment(lib,  BOOSTLIBPATH("libboost_thread-vc100-mt-sgd-1_54.lib"))
#pragma comment(lib, BOOSTLIBPATH("libboost_date_time-vc100-mt-sgd-1_54.lib"))
#pragma comment(lib, BOOSTLIBPATH("libboost_regex-vc100-mt-sgd-1_54.lib"))

#elif !defined( _DEBUG ) && defined( WIN64 )
#pragma message("RELEASE x64")
#pragma comment(lib, MTCLIBPATH("libxml2_64.lib"))
#pragma comment(lib, BOOSTLIBPATH("libboost_thread-vc100-mt-s-1_54.lib"))
#pragma comment(lib, BOOSTLIBPATH("libboost_system-vc100-mt-s-1_54.lib"))
#pragma comment(lib, BOOSTLIBPATH("libboost_date_time-vc100-mt-s-1_54.lib"))
#pragma comment(lib, BOOSTLIBPATH("libboost_regex-vc100-mt-s-1_54.lib"))
#pragma comment(lib, BOOSTLIBPATH("libboost_chrono-vc100-mt-s-1_54.lib"))

#elif defined( _DEBUG ) && defined( WIN32 )
#pragma message("DEBUG x32")

// #pragma message( MTCLIBPATH("libxml2d.lib") )
#pragma comment(lib, MTCLIBPATH("libxml2d.lib"))
#pragma comment(lib, BOOSTLIBPATH("libboost_thread-vc100-mt-sgd-1_54.lib"))
#pragma comment(lib, BOOSTLIBPATH("libboost_system-vc100-mt-sgd-1_54.lib"))
#elif !defined( _DEBUG ) && defined( WIN32 )
#pragma message("RELEASE x32")
#pragma comment(lib, MTCLIBPATH("libxml2d.lib"))
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")
#endif