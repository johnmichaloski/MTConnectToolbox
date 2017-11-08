
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
#include "socket_communication.h"
#include <iostream>
#include "Timing.h"
#include "Config.h"
#include "Logger.h"

int _tmain(int argc, _TCHAR* argv[])
{
	std::string ExeDirectory=ExtractDirectory(argv[0]);

	// Option handling - hard coded for now
	// ExeDirectory+"CannedScript.txt", ExeDirectory+"out.txt";
	socket_communication recorder;
	std::string ip = "127.0.0.1";
	std::string port = "30002";
	std::string filename ;


	// Set up local logging to debug.txt. 
	GLogger.Open(ExeDirectory + "debug.txt");
	GLogger.DebugLevel()=5;
	GLogger.Timestamping()=true;


	std::string cfgfile = ExeDirectory+"Config.ini";
	Nist::Config config;
	try {

		if(!config.load(cfgfile))
		{
			throw std::runtime_error(StrFormat("No such config file %s", cfgfile.c_str()));
		}
		// simpler than using boost program options and requirement to link and load DLL :(
		filename          = config.GetSymbolValue("GLOBALS.Filename", ExeDirectory+"out.txt").c_str( );
		port              = config.GetSymbolValue("GLOBALS.PortNum", "7878").c_str( );
		ip                = config.GetSymbolValue("GLOBALS.IP", ip).c_str( );
		
		if(FAILED(recorder.init(ip, port, filename)))
			throw std::runtime_error("Couldn't init - throw exception\n");
		recorder.start();

		getchar(); // wait for ^C or other character from console
		recorder.halt();
		Timing::Sleep(2000);
	}
	catch(std::exception err)
	{
		logFatal("Fatal exception %s\n", err.what());
	}
	return 0;
}

