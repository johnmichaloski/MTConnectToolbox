
	ReportSvcStatus( SERVICE_START_PENDING, NO_ERROR, 10000 );

AtlTrace("MTConnectService::main() Service\n");
	mIsService = true;
	SERVICE_TABLE_ENTRY DispatchTable[] = 
	{ 
		{  mName, (LPSERVICE_MAIN_FUNCTION) SvcMain }, 
		{ NULL, NULL } 
	}; 

	gService = this;

	if (!StartServiceCtrlDispatcher( DispatchTable )) 
	{ 
		SvcReportEvent("StartServiceCtrlDispatcher"); 
	} 

1)	WINDOWS FIREWALL: will unblock port 7878. Assuming administrator
		std::string msg = StdStringFormat("%s Firewall ON Port %x MADE UNBLOCKED\n",PROVIDER_NAME , mPort);
					EventLogger.LogEvent(msg);
	
	
	
	
	spawndde=F:\mmc2\ncdde.exe


Find EventListSvr.tlb in Siemens HMI Advanced CD or on 

============================================================================
2013-12-05T21:59:28.0593Z|power|UNAVAILABLE|avail|UNAVAILABLE|X|UNAVAILABLE|Y|UN
AVAILABLE|Z|UNAVAILABLE|controllermode|UNAVAILABLE|execution|UNAVAILABLE|program
|UNAVAILABLE|block|UNAVAILABLE
                              2013-12-05T22:02:59.0500Z|power|ON|avail|AVAILABLE
|X|71.755000|Y|-31.750000|Z|31.500000|controllermode|AUTOMATIC|execution|EXECUTI
NG|program|_N_WING_TIP_MPF
                          2013-12-05T22:03:00.0937Z|X|2.029000|Y|-10.160000|Z|48
.275000
       2013-12-05T22:03:02.0015Z|X|-5.266000|Y|10.160000|Z|35.945000
                                                                    2013-12-05T2
2:03:03.0109Z|X|-70.932000|Y|33.020000|Z|46.114000
                                                  2013-12-05T22:03:04.0203Z|X|-8
5.777000|Y|2.988000|Z|15.951000
                               2013-12-05T22:03:39.0734Z|X|-55.474000|Y|12.70000
0|Z|45.704000
             2013-12-05T22:03:40.0781Z|X|67.170000|Y|31.750000|Z|28.609000
                                                                          2013-1
2-05T22:03:41.0859Z|power|ON|avail|AVAILABLE|X|-34.933000|Y|49.530000|Z|42.49400
0|controllermode|AUTOMATIC|execution|EXECUTING|program|_N_WING_TIP_MPF|block|UNA
VAILABLE
        2013-12-05T22:03:41.0906Z|X|43.011000|Y|52.070000|Z|28.905000
                                                                     2013-12-05T
22:03:43.0000Z|X|48.920000|Y|73.660000|Z|20.310000
                                                  2013-12-05T22:03:44.0093Z|X|-3
2.266000|Y|96.495000|Z|27.965000
                                2013-12-05T22:03:45.0140Z|X|-27.102000|Y|26.5180
00|Z|47.092000
              2013-12-05T22:03:46.0250Z|X|-32.690000|Y|18.136000|Z|26.264000
                                                                            2013
-12-05T22:03:47.0609Z|X|21.229000|Y|-24.838000|Z|38.456000
                                                          2013-12-05T22:03:48.06
87Z|X|30.150000|Y|-2.565000|Z|12.929000
                                       2013-12-05T22:03:49.0796Z|X|-16.048000|Y|
-47.363000|Z|42.748000
                      2013-12-05T22:03:50.0875Z|X|-52.323000|Y|-75.476000|Z|19.4
82000
     2013-12-05T22:03:51.0968Z|X|28.147000|Y|52.624000|Z|8.661000
                                                                 2013-12-05T22:0
3:53.0062Z|X|96.037000|Y|94.677000|Z|71.120000
                                              2013-12-05T22:03:54.0156Z|X|67.026
000|Y|-72.873000|Z|63.500000
                            2013-12-05T22:03:55.0250Z|X|-73.028000|Y|24.282000|Z
|58.420000
          2013-12-05T22:03:56.0343Z|X|-136.677000|Y|121.437000|Z|1037.980000|pro
gram|_N_L6_SPF
              2013-12-05T22:03:57.0437Z|X|85.725000|Y|31.531000|Z|40.869000|prog
ram|_N_WING_TIP_MPF
                   2013-12-05T22:03:58.0531Z|X|-95.250000|Y|-9.525000|Z|40.35400
0
 2013-12-05T22:03:59.0625Z|X|85.725000|Y|70.569000|Z|12.827000
                                                              2013-12-05T22:04:0
0.0703Z|X|-14.013000|Y|-62.128000|Z|51.384000
                                             2013-12-05T22:04:01.0750Z|X|25.2480
00|Y|-81.991000|Z|48.997000
                           2013-12-05T22:04:02.0796Z|X|-41.108000|Y|96.774000
                                                                             201
3-12-05T22:04:03.0890Z|X|23.061000|Y|-91.923000|Z|44.221000
                                                           2013-12-05T22:04:05.0
Problem not connecting mNumDeviceData = 0 although there is  a connection



#include <atlbase.h>
extern CComModule _Module;		
#include <atlcom.h>
#include "eventlistsvr.tlh"

class CMcEventCreationSink :	public CComObjectRoot,
								public IDispatchImpl< IMcEventCreationSink, &IID_IMcEventCreationSink, &LIBID_EVENTLISTSVRLib>  
{
// COM-Map for QueryInterface
public:
	BEGIN_COM_MAP (CMcEventCreationSink)
		COM_INTERFACE_ENTRY (IMcEventCreationSink)
	END_COM_MAP ()

// Construction/Destruction
public:
	CMcEventCreationSink();
	virtual ~CMcEventCreationSink();

// IMcEventCreationSink methods
public:
	STDMETHOD(Refresh)();
	STDMETHOD(Acknowledge)( long Handle, VARIANT EventIDs, VARIANT TimeStamps, VARIANT Cookies);
	STDMETHOD(ShutDownRequest)( long Time );


};


STDMETHODIMP CMcEventCreationSink::Refresh()
{
	return S_OK;
}


STDMETHODIMP CMcEventCreationSink::Acknowledge( long Handle, VARIANT EventIDs, VARIANT TimeStamps, VARIANT Cookies)
{
	return S_OK;
}


STDMETHODIMP CMcEventCreationSink::ShutDownRequest( long Time )
{
	return S_OK;
}


if (g_pEventCreation!=NULL )
	{
		CComObject<CMcEventCreationSink>::CreateInstance(&g_pMcEventCreationSink);

		hr = g_pEventCreation->Register( g_pMcEventCreationSink, &g_hServerHandle );
		if (SUCCEEDED(hr))
		{
			SetWindowText( g_hServerHandleEdit, _ltot( g_hServerHandle, szHandle, 10 ));
			EnableWindow( g_hRegisterButton, FALSE );
			EnableWindow( g_hUnregisterButton, TRUE );
			EnableWindow( g_hSetEventButton, TRUE );
		}
		else
		{
			DisplayLastError( _T("IMcEventCreation::Register()"), hr, TRUE );
		}
	}

	unregister our client from the IMcEventCreation server
void Unregister()
{
	HRESULT	hr			= S_OK;
	
	if (g_pEventCreation!=NULL )
	{
		hr = g_pEventCreation->Unregister( g_hServerHandle );
		if (SUCCEEDED(hr))
		{
			SetWindowText( g_hServerHandleEdit, _T(""));
			EnableWindow( g_hRegisterButton, TRUE );
			EnableWindow( g_hUnregisterButton, FALSE );
			EnableWindow( g_hSetEventButton, FALSE );
		}
		else
		{
			DisplayLastError( _T("IMcEventCreation::Unregister()"), hr, TRUE );
		}
	}
}
Tag.DDE.ReadyActive=/Bag/State/ReadyActive
Tag.DDE.ResetActive=/Bag/State/ResetActive


1) Modify last linen of to point to adapter and then add @IncludeDirX32.txt  to C++ command line properies

-I"C:\Program Files\NIST\src\wtl81\Include" 
-I"C:\Program Files\NIST\src\boost_1_54_0"
-I.
-I”C:\Users\michalos\Documents\GitHub\840DNcDdeAdapter\MTConnectAdapter"

1a) make ascii not unicode project (should be handled by vxproj)
2) C++ Preprocess define: AFX, WINDOWS
3) Add pragmas to stdafx.h and remove excess warnings:

#pragma warning(disable: 4800) //warning C4800: forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable: 4018) //warning C4018: '<' : signed/unsigned mismatch
#pragma warning(disable: 4244) //warning C4244: 'argument' : conversion from 'double' to 'LPARAM', possible loss of data
#pragma warning(disable: 4267) //warning C4244: 'argument' : conversion from 'double' to 'LPARAM', possible loss of data
#pragma warning(disable: 4312) //warning C4244: conversion from 'OPCHANDLE' to 'OPCItem *' of greater size
#pragma warning(disable: 4800) //warning C4800: forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable: 4018) //warning C4018: '<' : signed/unsigned mismatch
#pragma warning(disable: 4996) 
#pragma warning(disable: 4311)
#pragma warning(disable: 4996)
#pragma warning(disable: 4541)
#pragma warning(disable: 4996)

4) Excluded from build: condition.cpp serial.cpp 


5) 
