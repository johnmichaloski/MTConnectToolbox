// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#pragma warning ( disable : 4996 )	// 'sprintf': This function or variable may be unsafe. Consider using sprintf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
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