
Unsure about UNC naming for local file system




1) You will need to install boost to get its include and libraries.

2) You will need to modify the include directive in

C:\Users\michalos\Documents\GitHub\AgfmMTConnectAgent\AgfmAgent\IncludeDirX32.txt

to point to the location of you AgfmAgent (now in C:\Users\michalos\Documents\GitHub\AgfmMTConnectAgen)
which contains:

-I"C:\Program Files\NIST\src\boost_1_54_0"
-I.
-I"C:\Users\michalos\Documents\GitHub\AgfmMTConnectAgent\MTConnectAgent\agent"
-I"C:\Users\michalos\Documents\GitHub\AgfmMTConnectAgent\MTConnectAgent\lib"
-I"C:\Users\michalos\Documents\GitHub\AgfmMTConnectAgent\MTConnectAgent\win32\libxml2-2.7.7\include"
-I"C:\Users\michalos\Documents\GitHub\AgfmMTConnectAgent\MTConnectAgent"
-I"C:\Users\michalos\Documents\GitHub\AgfmMTConnectAgent\NIST"

3) IN the AgfmAgent project setting for the linker, you will need to add the folders that point to your file location for:
a) C:\Users\michalos\Documents\GitHub\AgfmMTConnectAgent\MTConnectAgent\win32\libxml2-2.7.7\lib
b) C:\Program Files\NIST\src\boost_1_54_0\stage or wherever you have built the libraries for boost - Note 32 bit is different
that 64 bit!!


On Target Machine (Installation)
1) Create  install directory C:\Program Files\MTConnect\AgfmAgent
2) Copy files and exe to install directory
3) Change file permission via cmd and InstallShield in install directory to everyone full control.
4) Register Agent in Windows service as automatic with reference to AgfmAgent BUT NOT STARTED
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
				return E_FAIL;

		}
		fclose( stream );
#endif

		//BY_HANDLE_FILE_INFORMATION fInfo = {0};
		//GetFileInformationByHandle(hFile, &fInfo);
		//numread = (fInfo.nFileSizeHigh * (MAXDWORD+1)) + fInfo.nFileIndexLow;
		//void * readBuffer = malloc(numread);
		//data.resize(numread);

http://www.codeproject.com/Articles/35773/Subverting-Vista-UAC-in-Both-32-and-64-bit-Archite
http://stackoverflow.com/questions/4278373/how-to-start-a-process-from-windows-service-into-currently-logged-in-users-sess
https://msdn.microsoft.com/en-us/library/windows/desktop/ms682431(v=vs.85).aspx (sample w CreateProcessWithLogonW)
http://www.codeproject.com/Articles/36581/Interaction-between-services-and-applications-at-u (user tokens)
https://msdn.microsoft.com/en-us/library/aa378184(VS.85).aspx (Logon )

http://www.installsetupconfig.com/win32programming/windowsthreadsprocessapis7_20.html


----
https://msdn.microsoft.com/en-us/library/windows/desktop/aa385413(v=vs.85).aspx  (WNetAddConnection2 function example)
