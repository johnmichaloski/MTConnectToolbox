//
// AtlDebugFlags.h
//
#pragma once

//#define _CRTDBG_MAP_ALLOC  - won't work, not needed
#include <stdlib.h>
#include <crtdbg.h>

#ifdef DEBUGMEMORYINTERFACES
#define _ATL_DEBUG_INTERFACES
#define _ATL_DEBUG_QI 
#endif

// Add 	_CrtDumpMemoryLeaks();
// after program done