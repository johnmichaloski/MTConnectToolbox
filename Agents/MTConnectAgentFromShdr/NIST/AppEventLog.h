//
// AppEventLog.h
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#pragma once
#include <string>
#include "Atlstr.h"

class EventLogging
{

#define PROVIDER_NAME "840DNcDdeAdapter"
public:
	// Hardcoded insert string for the event messages.
	LPWSTR pBadCommand ;
	LPWSTR pFilename ;
	LPWSTR pNumberOfRetries;
	LPWSTR pSuccessfulRetries ;

	HANDLE hEventLog ;

	EventLogging()
	{
		pBadCommand = L"The command that was not valid";
		pFilename = L"c:\\folder\\file.ext";
		pNumberOfRetries = L"3";
		pSuccessfulRetries = L"0";


		hEventLog=NULL;
	}
	~EventLogging()
	{		
		if (hEventLog)
			DeregisterEventSource(hEventLog);
	}
	void Register()
	{
		hEventLog = RegisterEventSource(NULL, PROVIDER_NAME);
		if (NULL == hEventLog)
		{
			AtlTrace(L"RegisterEventSource failed with 0x%x.\n", GetLastError());

		}

	}
	HRESULT LogEvent(const TCHAR* format, ...)
	{
		CString str;
		va_list ptr; 
		va_start(ptr, format);
		str.FormatV(format, ptr);
		va_end(ptr);
		OutputDebugString(str);
		LogEvent( str);
		return E_FAIL;
	}

	#define GENERIC_CATEGORY                 ((WORD)0x00000001L)
	#define GENERIC_INVALID                 ((DWORD)0xC0020100L)

	void LogEvent(std::string errmsg, 
		WORD wType=EVENTLOG_ERROR_TYPE) //EVENTLOG_SUCCESS
		//WORD wCategory=GENERIC_CATEGORY,
		//DWORD dwEventID=GENERIC_INVALID )// EVENTLOG_WARNING_TYPE EVENTLOG_INFORMATION_TYPE
	{
		USES_CONVERSION;
		LPWSTR pInsertStrings[1]; //;= {errmsg.c_str()};
		LPCSTR pAInsertStrings[1]; //;= {errmsg.c_str()};
		DWORD dwEventDataSize = 0;
		pInsertStrings[0]=A2W(errmsg.c_str());
		pAInsertStrings[0]=errmsg.c_str();
		// The source name (provider) must exist as a subkey of Application.
				// This event includes user-defined data as part of the event. The event message
		// does not use insert strings.
		dwEventDataSize = ((DWORD)wcslen(pInsertStrings[0]) + 1) * sizeof(WCHAR);
		if(!::ReportEvent(hEventLog, EVENTLOG_SUCCESS, 0, 0, NULL, 1, 0,  &pAInsertStrings[0], NULL))

		//if (!ReportEvent(hEventLog, wType, wCategory, 0, NULL, 0, dwEventDataSize, NULL, pInsertStrings[0]))

		//if (!ReportEvent(hEventLog, wType, wCategory, dwEventID, NULL, 0, dwEventDataSize, NULL, pInsertStrings[0]))
		{
			AtlTrace(L"ReportEvent failed with 0x%x.\n", GetLastError());
			return;
		}
#if 0
		// This event uses insert strings.
		pInsertStrings[0] = pFilename;
		if (!ReportEvent(hEventLog, wType, DataBASE_CATEGORY, MSG_BAD_FILE_CONTENTS, NULL, 1, 0, (LPCWSTR*)pInsertStrings, NULL))
		{
			AtlTrace(L"ReportEvent failed with 0x%x for event 0x%x.\n", GetLastError(), MSG_BAD_FILE_CONTENTS);
			goto cleanup;
		}

		// This event uses insert strings.
		pInsertStrings[0] = pNumberOfRetries;
		pInsertStrings[1] = pSuccessfulRetries;
		if (!ReportEvent(hEventLog, wType, NETWORK_CATEGORY, MSG_RETRIES, NULL, 2, 0, (LPCWSTR*)pInsertStrings, NULL))
		{
			AtlTrace(L"ReportEvent failed with 0x%x for event 0x%x.\n", GetLastError(), MSG_RETRIES);
		}

		// This event uses insert strings.
		pInsertStrings[0] = pQuarts;
		pInsertStrings[1] = pGallons;
		if (!ReportEvent(hEventLog, wType , UI_CATEGORY, MSG_COMPUTE_CONVERSION, NULL, 2, 0, (LPCWSTR*)pInsertStrings, NULL))
		{
			AtlTrace(L"ReportEvent failed with 0x%x for event 0x%x.\n", GetLastError(), MSG_COMPUTE_CONVERSION);
		}
#endif
	}
};
__declspec(selectany)  EventLogging EventLogger;
