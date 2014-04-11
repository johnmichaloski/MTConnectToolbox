//
//
//
#pragma once


#include <string>
#include <ATLComTime.h>

#include "StdStringFcn.h"

enum TimeFormat
{
	HUM_READ=1,
	GMT=2,
	GMT_UV_SEC=4,
	LOCAL=8,
	MIDNITE=16,
	DATEONLY=32,
	TIMEONLY=64
};
class CElTime
{
public:
	CElTime()
	{

	}
	std::string HrMinSecFormat(int nsec)
	{
		return StdStringFormat("%02d:%02d:%02d", nsec/3600,  (nsec%3600)/60,  nsec%60);
	}

	std::string HrMinSecFormat(std::string sec)
	{
		int nsec=ConvertString<double>(sec,0.0);
		return StdStringFormat("%02d:%02d:%02d", nsec/3600,  (nsec%3600)/60,  nsec%60);
	}
	std::string HrMinSecFormat(COleDateTime now, COleDateTime start)
	{
		COleDateTimeSpan duration = now - start;
		OutputDebugString(StdStringFormat("Now = %s startTime = %s \n", now.Format("%Y/%m/%d %H:%M:%S"),start.Format("%Y/%m/%d %H:%M:%S")).c_str());
		int nsec = duration.GetTotalSeconds();
		if(nsec<0) nsec = 0;
		return StdStringFormat("%02d:%02d:%02d", nsec/3600,  (nsec%3600)/60,  nsec%60);
	}

	double Time2Seconds(std::string sec)
	{	
		int Hour=0,Minute=0,Seconds=0;
		if(sscanf(sec.c_str(), "%d:%d:%d",  &Hour, &Minute,&Seconds)!=3){ throw;}
		return 3600*Hour+60*Minute+Seconds;
	}

	COleDateTime GetDateTime(std::string s)
	{
		// parse 2012-02-03T17:31:51.0968Z 
		int Year, Month, Day, Hour, Minute, Second, Millisecond=0;
		if(sscanf(s.c_str(), "%d-%d-%dT%d:%d:%d.%d", &Year, &Month, &Day, &Hour, &Minute,&Second, &Millisecond)==7){}
		//12/2/2009 2:42:25 PM
		else if(sscanf(s.c_str(), "%d-%d-%dT%d:%d:%d", &Year, &Month, &Day, &Hour, &Minute,&Second)==6){}
		else if(sscanf(s.c_str(), "%d-%d-%d %d:%d:%d", &Year, &Month, &Day, &Hour, &Minute,&Second)==6){}
		else if(sscanf(s.c_str(), "%d/%d/%4d%d:%d:%d", &Month, &Day, &Year,  &Hour, &Minute,&Second)==6){}
		else throw std::exception("Unrecognized date-time format\n");

		//return ptime( date(Year,Month,Day),
		//                hours(Hour)  +
		//                minutes(Minute) +
		//                seconds(Second) +
		//                boost::posix_time::millisec(int(Millisecond)) );

		return COleDateTime( Year, Month, Day, Hour, Minute, Second ); 
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
	std::string GetTimeStamp(COleDateTime now= COleDateTime::GetCurrentTime())
	{
		return StdStringFormat("%s", now.Format("%Y-%m-%d %H:%M:%S"));
	}


}
;
__declspec(selectany)  CElTime Time;

class CShift
{
public:
	CShift()
	{
		LoadShifts();
	}
	std::vector<int> _shiftchanges; // in minutes
	// Input hr::min
	static int GetShiftTime(std::string s)
	{
		int Hour, Minute;
		if(sscanf(s.c_str(), "%d:%d", &Hour, &Minute)==2){}
		else return 0;//throw std::exception("Bad Shift time format -  hh:mm\n");
		return Hour * 60 + Minute;
	}
	// 	std::string shiftchanges =  config.GetSymbolValue("CONFIG.SHIFTCHANGES", L"00:00,08:00,16:00").c_str();
	void LoadShifts(std::string shiftchanges="00:00,08:00,16:00")
	{
		// Shift information
		std::vector<std::string> shifttimes =TrimmedTokenize(shiftchanges, ",");
		for(int i=0; i< shifttimes.size(); i++)
		{
			_shiftchanges.push_back(GetShiftTime(shifttimes[i]));
		}
	}
	//	COleDateTime now = COleDateTime::GetCurrentTime();
	// Shift 0 is an error
	int GetCurrentShift(COleDateTime now= COleDateTime::GetCurrentTime())
	{
		int minute = now.GetHour() * 60 + now.GetMinute();
		if(minute < _shiftchanges[0])
			return _shiftchanges.size();
		for(int i=1; i<= _shiftchanges.size(); i++)
		{
			int high = (i<_shiftchanges.size())? (_shiftchanges[i]-1) : _shiftchanges[i-1]+ _shiftchanges[0];
			if(minute >= _shiftchanges[i-1]  && minute <= high )
			{
				return i;
			}
		}
		return 1;
	}
	std::string GetCurrentShiftAsString(COleDateTime now= COleDateTime::GetCurrentTime())
	{
		return ConvertToString<int>(GetCurrentShift(now));
	}

	COleDateTime GetCurrentShiftStart(COleDateTime now= COleDateTime::GetCurrentTime())
	{
		int shift = GetCurrentShift(now); 
		COleDateTime start = COleDateTime(now.GetYear(), now.GetMonth(), now.GetDay(), 0 , 0 , 0) 
			+ COleDateTimeSpan(0,0,0,_shiftchanges[shift-1] * 60);
		//OutputDebugString(StdStringFormat("GetCurrentShiftStart Now Time = %s \n", now.Format("%Y/%m/%d %H:%M:%S")).c_str());
		//OutputDebugString(StdStringFormat("GetCurrentShiftStart Start Shift Time = %s \n", start.Format("%Y/%m/%d %H:%M:%S")).c_str());
		return start;
	}
	COleDateTime GetCurrentShiftEnd(COleDateTime now= COleDateTime::GetCurrentTime())
	{
		COleDateTime end;
		int shift = GetCurrentShift(now); 
		if(shift>= _shiftchanges.size())
		{
			shift=0;
			end = COleDateTime(now.GetYear(), now.GetMonth(), now.GetDay()+1, 0 , 0 , 0) 
				+ COleDateTimeSpan(0,0,0,_shiftchanges[shift] * 60);		}
		else
		{
			end = COleDateTime(now.GetYear(), now.GetMonth(), now.GetDay(), 0 , 0 , 0) 
				+ COleDateTimeSpan(0,0,0,_shiftchanges[shift] * 60);
		}
		//OutputDebugString(StdStringFormat("GetCurrentShiftStart Now Time = %s \n", now.Format("%Y/%m/%d %H:%M:%S")).c_str());
		OutputDebugString(StdStringFormat("GetCurrentShiftStart End Shift Time = %s \n", end.Format("%Y/%m/%d %H:%M:%S")).c_str());


		return end;
	}
	bool IsShiftChange(COleDateTime now= COleDateTime::GetCurrentTime())
	{
		 int n = GetCurrentShiftEnd(now);
		 int m = GetCurrentShiftEnd(now-COleDateTimeSpan(0,0,0, 30));
		 return n!=m;
	}
};
__declspec(selectany)  CShift Shift;