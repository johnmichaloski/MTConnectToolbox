//
//
//

#pragma once
#include "math.h"


/*
95 	1 	4 	0 	950
95 SP1 	1 	4 	0 	> 950 && <= 1080
95 OSR2 	1 	4 	< 10 	> 1080
98 	1 	4 	10 	1998
98 SP1 	1 	4 	10 	>1998 && < 2183
98 SE 	1 	4 	10 	>= 2183
Me 	1 	4 	90 	3000
NT 3.51 	2 	3 	51 	1057
NT 4 	2 	4 	0 	1381
2000 	2 	5 	0 	2195
XP 	2 	5 	1 	 
Windows Vista 	6.0.6000
Windows 7 	6.1.7600

CE 1.0 	3 	1 	0 	 
CE 2.0 	3 	2 	0 	 
CE 2.1 	3 	2 	1 	 
CE 3.0 	3 	3 	0

Use:

	OutputDebugString(StdStringFormat("OS Version %f  Major %x\n", OS.GetOSMajorMinorVersion(), OS.GetOSMajorVersion()).c_str());

*/
class COSVersion
{
	OSVERSIONINFOEX ver_info;
	DWORD dwVersion;
	DWORD  dwMajorVersion ;
	DWORD dwMinorVersion ;
	DWORD dwBuild ;

public:
	COSVersion()
	{
		memset(&ver_info, 0, sizeof(ver_info));
		ver_info.dwOSVersionInfoSize =  sizeof(ver_info);
		GetVersionEx((OSVERSIONINFO*)&ver_info);
		dwVersion=ver_info.dwMajorVersion;
		dwMajorVersion=ver_info.dwMajorVersion ;
		dwMinorVersion =ver_info.dwMinorVersion;
		dwBuild =ver_info.dwBuildNumber;

#if 0
		dwVersion = GetVersion();

		dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
		dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
		dwBuild = 0;
		if (dwVersion < 0x80000000)              
			dwBuild = (DWORD)(HIWORD(dwVersion));
#endif

	}
	double GetOSMajorMinorVersion()
	{
		double dVersion;
		dVersion=dwMajorVersion;
		if(dwMinorVersion>0)
			dVersion+= 1.0/((double) dwMinorVersion);
		return dVersion;
	}
	long GetOSMajorVersion()
	{
		return dwMajorVersion;
	}
};

__declspec(selectany)  COSVersion OS;