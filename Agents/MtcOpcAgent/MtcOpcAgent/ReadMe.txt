

/LIBPATH:"C:\Users\michalos\Documents\GitHub\MTConnect\Agent\libxml" 
/LIBPATH:"C:\Users\michalos\Documents\GitHub\MTConnect\lib\win64" 






	  <ClCompile>
      <Optimization>Disabled</Optimization>
      <AdditionalIncludeDirectories>..\..\src;..\opc;.;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <PreprocessorDefinitions>WIN32;_WINDOWS;_DEBUG;_CONSOLE;WINDOWS;%(PreprocessorDefinitions)</PreprocessorDefinitions>
   

   <IncludePath Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">C:\Program Files\NIST\src\wtl81\Include;.;C:\Users\michalos\Documents\GitHub\MTConnectGadgets\src\Agent\agent;C:\Users\michalos\Documents\GitHub\MTConnectGadgets\src\Agent\lib;C:\Users\michalos\Documents\GitHub\MTConnectGadgets\src\Agent\win32\libxml2-2.7.7\include;C:\Program Files\NIST\src\boost_1_54_0;$(IncludePath)</IncludePath>
    <IncludePath Condition="'$(Configuration)|$(Platform)'=='Release|x64'">C:\Program Files\NIST\src\wtl81\Include;.;C:\Users\michalos\Documents\GitHub\MTConnectGadgets\src\Agent\agent;C:\Users\michalos\Documents\GitHub\MTConnectGadgets\src\Agent\lib;C:\Users\michalos\Documents\GitHub\MTConnectGadgets\src\Agent\win32\libxml2-2.7.7\include;C:\Program Files\NIST\src\boost_1_54_0;.\x64;$(IncludePath)</IncludePath>
    <IncludePath Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">C:\Program Files\NIST\src\wtl81\Include;C:\Program Files\NIST\proj\MTConnect\Nist\MTConnectGadgets\src\Agent\agent;C:\Program Files\NIST\proj\MTConnect\Nist\MTConnectGadgets\src\Agent\lib;C:\Program Files\NIST\proj\MTConnect\Nist\MTConnectGadgets\src\Agent\win32\libxml2-2.7.7\include;C:\Program Files\NIST\proj\MTConnect\Nist\MTConnectGadgets\src\NIST;C:\Program Files\NIST\src\boost_1_44_0;$(IncludePath)</IncludePath>
    <IncludePath Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">C:\Program Files\NIST\src\wtl81\Include;C:\Program Files\NIST\proj\MTConnect\Nist\MTConnectGadgets\src\Agent\agent;C:\Program Files\NIST\proj\MTConnect\Nist\MTConnectGadgets\src\Agent\lib;C:\Program Files\NIST\proj\MTConnect\Nist\MTConnectGadgets\src\Agent\win32\libxml2-2.7.7\include;C:\Program Files\NIST\proj\MTConnect\Nist\MTConnectGadgets\src\NIST;C:\Program Files\NIST\src\boost_1_44_0;$(IncludePath)</IncludePath>



set INCLUDE=C:\Libraries\LibA\Include;%INCLUDE%
set LIB=C:\Libraries\LibA\Lib\x86;%LIB%



/p:"VCBuildAdditionalOptions= /useenv"



you can create a txt file (ex. IncludeDirs.txt). Inside that file you can add the include folders:

/I "."
/I ".."
/I ".\OtherFolder"
then in the properties->configuration properties->C/C++-> Command line add the following string:

@includedirs.txt
You can create a different file for each profile (Debug, Release, etc.)


    OPCHANDLE hClient;
    FILETIME ftTimeStamp;
    WORD wQuality;
    WORD wReserved;
    VARIANT vDataValue;

http://msdn.microsoft.com/en-us/library/ms220948(v=vs.80).aspx
http://vld.codeplex.com/downloads/get/342350
http://stackoverflow.com/questions/395123/raii-and-smart-pointers-in-c
http://www.microsoft.com/en-us/download/details.aspx?id=20028 
http://www.cplusplus.com/forum/general/8070/ 


#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h> 

//LEAKS
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

$IpValue=[EDITA1]$Devices=[EDITA2]$Target=[TARGETDIR]


1) Create AgentConfigurationEx, then call "thread" 
2) thread reads all the configurations, spawns OPC clients per configuration
3) call Agent.main()



	# ResetAtMidnight determines whether service is reset at midnite
	ResetAtMidnight=false
	
	#User=.,auduser,SUNRISE

	Language=English

	# Synchronous read of data, if so how ofen (ServerRate) in ms 
	ServerRate = 1000
	QueryServerPeriod=5000
	AutoReconnectMaxAttempts=0 



	for(int i=0; i< _machines.size(); i++)
	{
		if(_machines[i]->IsConnected() )
		{
			try
			{
				// static	boost::mutex _alarm_mutex; - declare as static in shared class...
				//boost::mutex::scoped_lock  lock(_alarm_mutex);
				_machines[i]->_CriticalSection.Lock();
				_machines[i]->ExportValues();
				_machines[i]->_CriticalSection.Unlock();

			}
			catch(...)
			{
				GLogger << INFO << "MtcOpcAdapter::gatherDeviceData() exception "  << std::endl;
			}
		}
	}
	/*
	 Threading to allow service to behave properly at startup/shutdown
	*/
	CWorkerThread<> _workerthread;
	CWorkerThread<> _enderthread;
	struct CStartThread : public IWorkerThreadClient
	{
		CStartThread()
		{ 
			_hEvent.Attach(CreateEvent(NULL, TRUE, FALSE, NULL)); 
		}
		HRESULT Execute(DWORD_PTR dwParam, HANDLE hObject);
		HRESULT CloseHandle(HANDLE){ return S_OK; }
		CHandle _hEvent;
	} _StartThread;

	struct CEndThread : public IWorkerThreadClient
	{
		CEndThread()
		{ 
			_hEvent.Attach(CreateEvent(NULL, TRUE, FALSE, NULL)); 
		}
		HRESULT Execute(DWORD_PTR dwParam, HANDLE hObject);
		HRESULT CloseHandle(HANDLE){ return S_OK; }
		CHandle _hEvent;
	} _EndThread;
	
	// OPC Specific threading to allow bad opc servers to reset
	CWorkerThread<> _resetthread;
	struct CResetThread : public IWorkerThreadClient
	{
		HRESULT Execute(DWORD_PTR dwParam, HANDLE hObject);
		HRESULT CloseHandle(HANDLE){ ::CloseHandle(_hTimer); return S_OK; }
		HANDLE _hTimer;
	} _ResetThread;
TODO:

1) SHDR Tags to finish:

1.	uuid
2.	manufacturer
3.	station
4.	serialNumber

0x80041008 WBEM_E_INVALID_PARAMETER

his is what I did in my vbs script to open a firewall exception for the service. I couldn't use the standard interactive pop-up for a service (that asks for permission to open the firewall), since it doesn't have a UI.

set oShell  = CreateObject("WScript.shell")
oShell.run "cmd /C netsh advfirewall firewall add rule program=""C:\Program Files (x86)\foo\bar\prog.exe"" name=""my-service"" dir=in action=allow"  
I added this vbs script to the "Commit" CustomAction of the Setup&Deployment Project, leaving the properties as defaults.

To debug problems with the vbs stage, I ran the msi from DOS using

msiexec /i mysetup.msi /L* install.log
Note that I originally used "Wscript.CreateObject" but that failed. This worked.

UINT result = 0;
TCHAR szQuery[] = "SELECT DefaultDir FROM Directory";
PMSIHANDLE hDB = NULL;
PMSIHANDLE hView = NULL;
PMSIHANDLE hRecord = NULL;
hDB = MsiGetActiveDatabase( hModule );
result = MsiDatabaseOpenView( hDB, szQuery, &hView );
result = MsiViewExecute( hView, hRecord );
while (MsiViewFetch( hView, &hRecord ) == ERROR_SUCCESS)
{
TCHAR szCurDir[MAX_PATH] = {0};
DWORD dwDirLen = MAX_PATH;
if (MsiRecordGetString( hRecord, 1, szCurDir, &dwDirLen) != ERROR_SUCCESS )
break; // fail. break out of the while loop.
// Do something. This sample code just pops up a message box. 
MsiMessageBox(hModule, szCurDir, MB_OK);
} 


2) Connecting to the Siemens 840D to receive shutdown notifications.
#import "IregieSvr.dll"

	CComPtr<IREGIESVRLib::IRegieSvr> server;
	
	OutputDebugString("Connecting to Siemens 840D Regie Server");
	CComVariant v1=hwndMainFrame;
	CComVariant v2;
	::ShowWindow(hwndMainFrame, SW_MINIMIZE);
	::ShowWindow(hwndMainFrame, SW_SHOW);
	if(FAILED(hr=server.CoCreateInstance(__uuidof(RegieSvr), NULL, CLSCTX_SERVER)))
		throw CString (_T("CoCreateInstance Siemens 830D RegieSvr FAILED"));
	if(FAILED(hr=server->InitSvr(v1,v2)))
		throw CString (_T("InitSvr(v1,v2) FAILED"));
	if(FAILED(hr=server->InitCompleteEx()))
		throw CString (_T("InitCompleteEx FAILED"));

		std::string ipaddr="127.0.0.1,127.0.0.2";
		std::string devices="M1,M3";

		std::string contents; 
		ReadFile(::ExeDirectory()+"MtcOpcAgent.ini", contents);

		ReplacePattern(contents, "ServerMachineName", "\n", "ServerMachineName=" + ipaddr + "\n");
		ReplacePattern(contents, "MTConnectDevice", "\n", "MTConnectDevice=" + devices + "\n");

		std::vector<std::string> ips=TrimmedTokenize(ipaddr,",");
		std::vector<std::string> devs=TrimmedTokenize(devices,",");
		if(ips.size() != devs.size())
			::MessageBox(NULL, "Mismatched # ips and devices", "Error", MB_OK);
		std::string tagsection="SIEMENS";
		for(int i=1; i< ips.size(); i++)
		{
			tagsection+=",SIEMENS";
		}
		ReplacePattern(contents, "OpcTags", "\n", "OpcTags=" + tagsection + "\n");
		WriteFile(::ExeDirectory()+"MtcOpcAgent1.ini",contents);
