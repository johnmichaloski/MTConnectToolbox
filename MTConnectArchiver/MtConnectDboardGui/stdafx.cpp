// stdafx.cpp : source file that includes just the standard includes
//	MtConnectDboardGui.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "stdStringFcn.h"

#if (_ATL_VER < 0x0700)
#include <atlimpl.cpp>
#endif //(_ATL_VER < 0x0700)

#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#if (_ATL_VER < 0x0700)
#include <statreg.cpp>
#endif //(_ATL_VER < 0x0700)
#endif //_ATL_STATIC_REGISTRY
_bstr_t ErrorFormatMessage(HRESULT dwLastErr )
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
	return errmsg;
}
std::string GetTimeStamp(int format)
{
  SYSTEMTIME st;
  char timestamp[64];
  GetSystemTime(&st);
  if(format & LOCAL)
	 GetLocalTime(&st);
  
  if(format & MIDNITE)
	st.wHour= st.wMinute= st.wSecond=0;

  if(format & DATEONLY)
	  sprintf(timestamp, "%4d-%02d-%02d", st.wYear, st.wMonth,st.wDay);
  else if(format &  TIMEONLY)
	  sprintf(timestamp, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
  else
	  sprintf(timestamp, "%4d-%02d-%02dT%02d:%02d:%02d", st.wYear, st.wMonth,
	  st.wDay, st.wHour, st.wMinute, st.wSecond);
  
  std::string tstamp=timestamp;
  
  if (format & GMT_UV_SEC)
  {
    sprintf(timestamp + strlen(timestamp), ".%04dZ", st.wMilliseconds);
	tstamp=timestamp;
  }
  else if(format & HUM_READ)
  {
	tstamp=timestamp;
	ReplaceAll(tstamp,"T", " ");
  }
  else
  {
	  tstamp=timestamp;
	  tstamp+="Z";  
  }
  
  return tstamp;
}

COleDateTime GetDateTime(std::string s)
{
	// parse 2012-02-03T17:31:51.0968Z 
	int Year, Month, Day, Hour, Minute, Second, Millisecond=0;
	if(sscanf(s.c_str(), "%d-%d-%dT%d:%d:%d.%d", &Year, &Month, &Day, &Hour, &Minute,&Second, &Millisecond)==7){}
		//12/2/2009 2:42:25 PM
	else if(sscanf(s.c_str(), "%d-%d-%dT%d:%d:%d", &Year, &Month, &Day, &Hour, &Minute,&Second)==6){}
	else if(sscanf(s.c_str(), "%d/%d/%4d%d:%d:%d", &Month, &Day, &Year,  &Hour, &Minute,&Second)==6){}
	else throw std::exception("Unrecognized date-time format\n");

	 //return ptime( date(Year,Month,Day),
  //                hours(Hour)  +
  //                minutes(Minute) +
  //                seconds(Second) +
  //                boost::posix_time::millisec(int(Millisecond)) );

	return COleDateTime( Year, Month, Day, Hour, Minute, Second ); 
}

