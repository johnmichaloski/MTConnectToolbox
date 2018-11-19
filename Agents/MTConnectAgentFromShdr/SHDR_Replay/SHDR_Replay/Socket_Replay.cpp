
//
// ShdrEcho.cpp : Defines the entry point for the console application.
//

/*
 * DISCLAIMER:
 * This software was produced by the National Institute of Standards
 * and Technology (NIST), an agency of the U.S. government, and by statute is
 * not subject to copyright in the United States.  Recipients of this software
 * assume all responsibility associated with its operation, modification,
 * maintenance, and subsequent redistribution.
 *
 * See NIST Administration Manual 4.09.07 b and Appendix I.
 */

#include "stdafx.h"
#include "SocketParser.h"
#include "SocketBackEnd.h"
#include <iostream>
#include "Timing.h"
#include "Config.h"

// Doesn't seem to work - need to add library path for lib include
#define BOOSTLIBPATH(X)    "C:\\Program Files\\NIST\\src\\boost_1_54_0\\MSVC10\\stage\\lib64-msvc-10.0\\" ## X

#if defined( WIN64 ) && defined( _DEBUG )
#pragma message( "DEBUG x64" )
#pragma comment(lib, BOOSTLIBPATH("libboost_system-vc100-mt-sgd-1_54.lib"))
#pragma comment(lib,  BOOSTLIBPATH("libboost_thread-vc100-mt-sgd-1_54.lib"))

#elif !defined( _DEBUG ) && defined( WIN64 )
#pragma message( "RELEASE x64" )
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")

#elif defined( _DEBUG ) && defined( WIN32 )
#pragma message( "DEBUG x32" )
#pragma comment(lib, "libboost_thread-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-sgd-1_54.lib")
#elif !defined( _DEBUG ) && defined( WIN32 )
#pragma message( "RELEASE x32" )
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")
#endif

int _tmain(int argc, _TCHAR* argv[])
{
	std::string ExeDirectory=ExtractDirectory(argv[0]);
	SocketBackEnd backend;
	SocketParser parser(backend);
	bool bOptionWait;
	int nSleep = 100;

	// Option handling - hard coded for now
	// ExeDirectory+"CannedScript.txt", ExeDirectory+"out.txt";
	std::string filename  ;
	long portnum=7878;
	std::string ip = "127.0.0.1";
	double dTimeMultipler = 1.0;
	//parser.Repeat()=true;
	parser.Repeat()=false;
	bOptionWait=false;
	std::string cfgfile = ExeDirectory+"Config.ini";
	Nist::Config config;
	try {

		if(!config.load(cfgfile))
		{
			throw std::runtime_error(StrFormat("No such file %s", cfgfile.c_str()));
		}
		// simpler than using boost program options and requirement to link and load DLL :(
		parser.Repeat()   = config.GetSymbolValue("GLOBALS.Repeat", 0).toNumber<int>( );
		filename          = config.GetSymbolValue("GLOBALS.Filename", ExeDirectory+"out.txt").c_str( );
		bOptionWait       = config.GetSymbolValue("GLOBALS.Wait", 1).toNumber<int>( );
		portnum           = config.GetSymbolValue("GLOBALS.PortNum", portnum).toNumber<int>( );
		ip                = config.GetSymbolValue("GLOBALS.IP", ip).c_str( );
		dTimeMultipler    = config.GetSymbolValue("GLOBALS.TimeMultipler", dTimeMultipler).toNumber<double>( );
		
		parser.Init(filename);
		backend.Init(ip, portnum, "M1");
		double delay=0;

		while(true)
		{
			SocketBackEnd::_io_service.run_one();
			if(backend.Count()<1 && bOptionWait)
			{
				nSleep=100;
			}
			else
			{
				if(delay<0)
				{
					delay=(long) parser.ProcessStream()*dTimeMultipler;
					if(delay<0)
						break; // should only get here if no repeat
					OutputDebugString(parser.GetLatestBuffer().c_str());
					backend.StoreShdrString(parser.GetLatestBuffer()+"\n");
				}
				else
					delay-=100;
			}
			Timing::Sleep(nSleep);
		}
		backend.Quit();
		Timing::Sleep(2000);
	}
	catch(std::exception err)
	{
		std::cout << err.what();
		std::cout << StrFormat("Exception in file %s at line %d\n", filename.c_str(),  parser.LineNumber());
	}
	return 0;
}

