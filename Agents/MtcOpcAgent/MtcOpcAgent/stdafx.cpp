// stdafx.cpp : source file that includes just the standard includes
// MtcOpcAgent.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#include <tlhelp32.h>
#include <algorithm> 
static std::string MakeLower(std::string str)
{
	std::transform(str.begin(),str.end(),str.begin(),tolower);
	return str;
}
/*
	DWORD processID = FindProcessId(processName);

	if ( processID == 0 )
		std::wcout << "Could not find " << processName.c_str() << std::endl;
	else
		std::wcout << "Process ID is " << processID << std::endl;
*/

DWORD FindProcessId(std::string processName)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);
	
	processName=MakeLower(processName);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if ( processesSnapshot == INVALID_HANDLE_VALUE )
		return 0;

	Process32First(processesSnapshot, &processInfo);
	
	std::string nextprocessname = MakeLower(processInfo.szExeFile);

	if ( !processName.compare(nextprocessname) )
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while ( Process32Next(processesSnapshot, &processInfo) )
	{
		nextprocessname = MakeLower(processInfo.szExeFile);
		if ( !processName.compare(nextprocessname) )
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}
	
	CloseHandle(processesSnapshot);
	return 0;
}

std::string ErrorFormatMessage(HRESULT dwLastErr )
{
	TCHAR * pTemp;
	_bstr_t errmsg(L"");
	int nLen = ::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, 
		dwLastErr,
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR) &pTemp, 
		1, 
		NULL );

	errmsg=(LPCTSTR) pTemp;
	::LocalFree( pTemp );
	return (LPCSTR) errmsg;
}


//ptime GetDateTime(std::string s)
//{
//	// parse 2012-02-03T17:31:51.0968Z 
//	int Year, Month, Day, Hour, Minute, Second, Millisecond;
//	if(sscanf(s.c_str(), "%d-%d-%dT%d:%d:%d.%d", &Year, &Month, &Day, &Hour, &Minute,&Second, &Millisecond)==7){}
//		//12/2/2009 2:42:25 PM
//	else if(sscanf(s.c_str(), "%d/%d/%4d%d:%d:%d", &Month, &Day, &Year,  &Hour, &Minute,&Second)==6){}
//	else throw std::exception("Unrecognized date-time format\n");
//
//	 return ptime( date(Year,Month,Day),
//                  hours(Hour)  +
//                  minutes(Minute) +
//                  seconds(Second) +
//                  boost::posix_time::millisec(int(Millisecond)) );
//
//	//return COleDateTime( Year, Month, Day, Hour, Minute, Second ); 
//}
//
//std::string GetTimeStamp(TimeFormat format)
//{
//  SYSTEMTIME st;
//  char timestamp[64];
//  GetSystemTime(&st);
//  sprintf(timestamp, "%4d-%02d-%02dT%02d:%02d:%02d", st.wYear, st.wMonth,
//    st.wDay, st.wHour, st.wMinute, st.wSecond);
//  
//  if (format == GMT_UV_SEC)
//  {
//    sprintf(timestamp + strlen(timestamp), ".%04dZ", st.wMilliseconds);
//  }
//  else
//  {
//    strcat(timestamp, "Z");
//  }
//  
//  return timestamp;
//}