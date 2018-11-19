
STL Goodies!
http://www.bogotobogo.com/cplusplus/stl5_function_objects.php

Unsure about UNC naming for local file system




1) You will need to install boost to get its include and libraries.

2) You will need to modify the include directive in

C:\Users\michalos\Documents\GitHub\WiringMTConnectAgent\WiringAgent\IncludeDirX32.txt

to point to the location of you WiringAgent (now in C:\Users\michalos\Documents\GitHub\WiringMTConnectAgen)
which contains:

-I"C:\Program Files\NIST\src\boost_1_54_0"
-I.
-I"C:\Users\michalos\Documents\GitHub\WiringMTConnectAgent\MTConnectAgent\agent"
-I"C:\Users\michalos\Documents\GitHub\WiringMTConnectAgent\MTConnectAgent\lib"
-I"C:\Users\michalos\Documents\GitHub\WiringMTConnectAgent\MTConnectAgent\win32\libxml2-2.7.7\include"
-I"C:\Users\michalos\Documents\GitHub\WiringMTConnectAgent\MTConnectAgent"
-I"C:\Users\michalos\Documents\GitHub\WiringMTConnectAgent\NIST"

3) IN the WiringAgent project setting for the linker, you will need to add the folders that point to your file location for:
a) C:\Users\michalos\Documents\GitHub\WiringMTConnectAgent\MTConnectAgent\win32\libxml2-2.7.7\lib
b) C:\Program Files\NIST\src\boost_1_54_0\stage or wherever you have built the libraries for boost - Note 32 bit is different
that 64 bit!!


On Target Machine (Installation)
1) Create  install directory C:\Program Files\MTConnect\WiringAgent
2) Copy files and exe to install directory
3) Change file permission via cmd and InstallShield in install directory to everyone full control.
4) Register Agent in Windows service as automatic with reference to WiringAgent BUT NOT STARTED
5) User will need to configure  Config.ini for 
	MTConnectDevice=M1 
or whatever device name anb then configure ini file section:
	[M1]
	IPAddress=127.0.0.1
	ProductionLog=C:\Users\michalos\AppData\Local\MTConnect\ProductionLog.csv
	logging_level=ERROR
	Simulation=1
 Most important for now is UNC location for ProductionLog. IPAddress is ignored.

Prerequisite assumptions:

1) Starting midday service - otherwise may have to go back a day to look for Tags
2) Install changes permissions of Program Files (agent might write some logs to install directory) not temp directory.
cd /d %~dp0

echo y|  cacls .  /t  /c  /GRANT Everyone:F
NOTE changed already with addition of echo y and obsoleted cacls recently.
3) Fatal debug stoppage will go to Application Event Log (hopefullly)
4) Used sc.exe to install agent (and to stop and delete)
5) Agent is not configured! You will need to setup config.ini and then start agent.

Potential improvements:
1) check modification time stamp before copy


TESTS:
UNC remote fetch


SCRAPS:
#if 0
		FILE *stream;
		if( (stream = _fsopen( filename.c_str(), "rb", _SH_DENYNO )) == NULL )
		{
			DebugMessage(StdStringFormat("Couldn't open File \"%s\" for device %s\n", filename.c_str(), _device.c_str()));
			return E_FAIL;
		}

//		fseek(stream,0,SEEK_END);
//		long ulFileSize = ftell(stream);
		if(filesize>_lastfilesize && _lastfilesize != 0 )
		{
			fseek(stream,_lastfilesize,SEEK_SET);
			ulFileSize=filesize-_lastfilesize;
		}
		void  * pAddr = &data[0];
		if((numread= fread( pAddr, 1, ulFileSize, stream )) == 0)
		{
			DebugMessage(StdStringFormat("numread File %s is zero for device %s\n", filename.c_str(), _device.c_str()));
				re
				
				turn E_FAIL;

		}
		fclose( stream );
#endif

#if 0
		if(filesize>_lastfilesize && _lastfilesize != 0 )
		{	
			// Skip file until new lines
			//data=data.substr(_lastfilesize);			
		}
		else
		{
			size_t n=0;
			std::string date = StdStringFormat("%4d-%02d-%02d", today.GetYear(), today.GetMonth(), today.GetDay()); 
			// FIXME: maybe substract a day then search
			if((n=data.find(date)) == std::string::npos)
			{
				// Go back extra day - since could be 12:05
				COleDateTime newdate = today - COleDateTimeSpan(1,0,0,0);
				date = StdStringFormat("%4d-%02d-%02d", newdate.GetYear(), newdate.GetMonth(), newdate.GetDay()); 
				if((n=data.find(date)) == std::string::npos)
					return WarnWithMsg(E_FAIL, StdStringFormat("Cant find date \"%s\" for device %s\n", date.c_str(),  _device.c_str())) ;
			}

			data=data.substr(n); //return E_FAIL;


		}
#endif