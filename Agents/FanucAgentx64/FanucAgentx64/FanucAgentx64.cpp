// FanucAgentx64.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma comment( lib, "msxml2.lib" )
#pragma comment(lib,"psapi") 
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")

#define MTCLIBPATH(X)    "C:\\Users\\michalos\\Documents\\Visual Studio 2015\\Projects\\FanucAgentx64\\Agent\\win32\\libxml2-2.7.7\\lib\\" ## X
#define FANUCLIBPATH(X) "C:\\Users\\michalos\\Documents\\Visual Studio 2015\\Projects\\FanucAgentx64\\Fwlib64\\" ## X

#if defined( WIN64 ) && defined( _DEBUG )
#pragma message( "DEBUG x64" )
#pragma comment(lib, MTCLIBPATH("libxml2_64d.lib"))
#pragma comment(lib, FANUCLIBPATH("fwlib64.lib"))

#elif !defined( _DEBUG ) && defined( WIN64 )
#pragma message( "RELEASE x64" )
#pragma comment(lib,  MTCLIBPATH("libxml2_64.lib"))
#pragma comment(lib, FANUCLIBPATH("fwlib64.lib"))
#endif

FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}


