
ShdrEcho
===========

This program that was developed in  Microsoft Windows Visual Studio C++ 10.0.
Most but not all MSVC dependencies have been removed. 
The inclusion of precompiled headers (which really speeds things up) are included,
and no easy way to mitigate, such #define way to add "stdafx.h", was found.


The ShdrEcho program accepts a captured MTConnect SHDR archival file, and echo's it to Agents.
The   ShdrEcho program replaces the first and every subsequent archival timestamp with
the current time and duration offset. The ShdrEcho program  waits between updates by
the amount of time between timestamps in the MTConnect SHDR archival file.

The ShdrEcho program reads a Config.ini file that is in the same folder as the ShdrEcho.exe
executable. The Conifg.ini file is a  Below is a sample config.ini file. 
There  is only one section [GLOBALS]. 
Within the section are 4 variables, Filename, Repeat, Wait, PortNum and IP to 

[GLOBALS]
Filename= C:\Users\michalos\Documents\GitHub\MTConnectSolutions\ShdrEcho\ShdrEcho\x64\Debug\out.txt
Repeat=1
Wait=0
PortNum=7878


The filename variable specifies the filename (including path) of the SHDR archival file.
Repeat variable determines whether the echo will restart when the archival file is finished.
The Wait variable is used to tell the program to wait for a connection before reading the SHDR 
archival file
The PortNum is the TCP/IP socket port of the MTConnect SHDR connection that the program will listen to.

Note, the IP of the  ShdrEcho program must always be localhost or 127.0.0.1.