// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200

#include <atlbase.h>
//#define BOOST_NO_EXCEPTIONS
#define BOOST_EXCEPTION_DISABLE

#if _ATL_VER >= 0x0700
	#include <atlcoll.h>
	#include <atlstr.h>
	#include <atltypes.h>
	#define _WTL_NO_CSTRING
	#define _WTL_NO_WTYPES
//#define _CSTRING_NS ATL
#else
	#define _WTL_USE_CSTRING
#endif

#include <atlapp.h>

extern CAppModule _Module;

#include <atlcom.h>
#include <atlhost.h>


#include <atlwin.h>
#include <atlctl.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlcrack.h>

#pragma warning(disable: 4800) //warning C4800: forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable: 4018) //warning C4018: '<' : signed/unsigned mismatch
#pragma warning(disable: 4244) //warning C4244: 'argument' : conversion from 'double' to 'LPARAM', possible loss of data
#pragma warning(disable: 4267) //warning C4244: 'argument' : conversion from 'double' to 'LPARAM', possible loss of data
#pragma warning(disable: 4312) //warning C4244: conversion from 'OPCHANDLE' to 'OPCItem *' of greater size
#pragma warning(disable: 4800) //warning C4800: forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable: 4018) //warning C4018: '<' : signed/unsigned mismatch
#pragma warning(disable: 4996) 
#pragma warning(disable: 4311)
#pragma warning(disable: 4996)
#pragma warning(disable: 4541)
#pragma warning(disable: 4996)

//#include <atlmisc.h>

#define END_MSG_MAP_EX	END_MSG_MAP
