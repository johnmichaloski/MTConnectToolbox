
ShdrEcho
===========

The ShdrEcho program accepts a MTConnect SHDR archival file, and echo's it to any listening Agents.
The  ShdrEcho program replaces the first and every subsequent archival timestamp with
the current time and duration offset. The ShdrEcho program  waits between updates by
the amount of time between timestamps in the MTConnect SHDR archival file. 
A sample line from a SHDR would look like:
	
	12/2/2009 2:42:25 PM|power|ON|controllermode|AUTOMATIC|execution|EXECUTING|program|117Z2716-54_1-1|line|0|Srpm|1031|Tool|50

where there is a leading timestamp, and then either samples, events, conditions, information, or multi-line assets are
given. This program just echos whatever follows the timestamp until the line feed. Information SHDR begin with a * instead
of a timestamp, and this is detected. Multiline asset device information is not handled.

This program  was developed in  Microsoft Windows Visual Studio C++ 10.0.
Most but not all MSVC dependencies have been removed. 
The inclusion of precompiled headers (which really speeds things up) are included,
and no easy way to mitigate, such #define way to add "stdafx.h", was found.

The ShdrEcho program reads a Config.ini file that is in the same folder as the ShdrEcho.exe
executable. The Conifg.ini file is a  Below is a sample config.ini file. 
There  is only one section [GLOBALS]. 
Within the section are 4 variables, Filename, Repeat, Wait, PortNum and IP to 

	[GLOBALS]
	Filename= C:\Users\michalos\Documents\GitHub\MTConnectSolutions\ShdrEcho\ShdrEcho\x64\Debug\out.txt
	Repeat=1
	Wait=0
	PortNum=7878


The GLOBALS section variables have the following meaning:

- The filename variable specifies the filename (including path) of the SHDR archival file.
- Repeat variable determines whether the echo will restart when the archival file is finished.
- The Wait variable is used to tell the program to wait for a connection before reading the SHDR archival file
- The PortNum is the TCP/IP socket port of the MTConnect SHDR connection that the program will listen to. 
- Note, the IP of the  ShdrEcho program must always be localhost or 127.0.0.1.
