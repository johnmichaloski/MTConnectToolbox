// MTConnectAgent1_3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <assert.h>
#define MTCLIBPATH(X)    "C:\\Users\\michalos\\Documents\\Visual Studio 2015\\Projects\\MTConnectAgent1_3\\MTConnectAgent\\win32\\libxml2-2.9\\lib\\" ## X

#pragma comment(lib, "legacy_stdio_definitions.lib")

#if defined( WIN64 ) && defined( _DEBUG )
#pragma message( "DEBUG x64" )
#pragma comment(lib, MTCLIBPATH("libxml2d_a_v120_64.lib"))
#elif !defined( _DEBUG ) && defined( WIN64 )
#pragma message( "RELEASE x64" )
#pragma comment(lib,  MTCLIBPATH("libxml2_a_v120_64.lib"))
#endif

FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}

 