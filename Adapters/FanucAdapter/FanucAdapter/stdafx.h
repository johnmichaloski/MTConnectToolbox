// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
//#include <windows.h>
#define iSERIES
//#define iSERIESHSSB
//#define SERIESHSSB
#define iSERIESLAN
//#define i160

//#define F15i
//#define F15M
//#define FOi

//#define ALARM
//#define LOADS
#define TOOLING

#define BUFSIZE 256

#ifndef LOGONCE
#define LOGONCE  static long nLog##__LINE__=0; if( 0 == nLog##__LINE__++) 
#endif

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


extern void ReportError(char * lpszFunction);

#define RUNONCE  static long nLog##__LINE__=0; if( 0 == nLog##__LINE__++) 
