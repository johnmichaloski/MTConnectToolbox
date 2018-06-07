
//
// ResetAtMidnightThread.h
//

#pragma once
#include <string>
#include <atlbase.h>
#include "atlutil.h"
#include "ATLComTime.h"
#include "NIST/StdStringFcn.h"

template<typename T>
class CResetAtMidnightThread
{
public:
	CResetAtMidnightThread(void){bResetAtMidnight=false;bTestReset=false;}
	~CResetAtMidnightThread(void){}
	int bResetAtMidnight;
	int bTestReset;
	bool Start();
	void Stop()
	{
		if(bResetAtMidnight)
			_resetthread.Shutdown();
	}
	////////////////////////////////////////////////////////////////////
	void Warning(std::string err)
	{
		logWarning(err.c_str());
	}
	void Fatal(std::string err)
	{
		logFatal(err.c_str());
	}
protected:
	CWorkerThread<> _resetthread;
	struct CResetThread : public IWorkerThreadClient
	{
		HRESULT Execute(DWORD_PTR dwParam, HANDLE hObject);
		HRESULT CloseHandle(HANDLE){ ::CloseHandle(_hTimer); return S_OK; }
		HANDLE _hTimer;
	} _ResetThread;
};



template<typename T>
inline  HRESULT CResetAtMidnightThread<T>::CResetThread::Execute(DWORD_PTR dwParam, HANDLE hObject)
{
	static char name[] = "CResetAtMidnightThread::CResetThread::Execute";

	T * service = (T*) dwParam;
	CancelWaitableTimer(hObject);

	try {

		PROCESS_INFORMATION pi;
		ZeroMemory( &pi, sizeof(pi) );

		STARTUPINFO si;
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;	 // set the window display to HIDE	

		// SCM reset command of this service - BLOW LINE WORKS
		//std::string cmd = StdStringFormat("cmd /c net stop \"%s\" & net start \"%s\"", service->name(), service->name());        // Command line
		std::string cmd;
		if(!service->mIsDebug)
		{
			cmd = StdStringFormat("cmd /c net stop \"%s\" & net start \"%s\"",
				service->name( ).c_str(),
				service->name( ).c_str()); 
		}
		else
		{
			// If not service, it is an app exe run from command line,
			// it will restart in the current DOS console.
			TCHAR buf[1000];
			::GetModuleFileName(NULL, buf, 1000);
			std::string exepath = File.ExeDirectory( );
			std::string exe     = File.Filename(std::string(buf));
			cmd	= StdStringFormat("cmd /c taskkill /F  /IM \"%s\" & \"%s\" debug",	exe.c_str( ), buf);         
		}

		logDebug("Reset at midnight with command %s\n", cmd.c_str());


		if(!::CreateProcess( NULL,   // No module name (use command line)
			const_cast<char *>(cmd.c_str()),
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi ))           // Pointer to PROCESS_INFORMATION structure
			logFatal("CreateProcess FAIL ") ;

		::Sleep(5000); // make sure process has spawned before killing thread
	}
	catch(...)
	{
		service->Fatal("Exception  - CResetAtMidnightThread<T>::CResetThread::Execute(DWORD_PTR dwParam, HANDLE hObject)");
	}
	return S_OK;
}

template<typename T>
inline bool CResetAtMidnightThread<T>::Start()
{
	static char name[] = "CResetAtMidnightThread::Start";
	if(bResetAtMidnight)
	{
		logWarn("Agent will Reset At Midnight\n");

		COleDateTime now = COleDateTime::GetCurrentTime();
		COleDateTime date2;
		if(!bTestReset)
		{
			// reset at 12:00:01
			date2	= COleDateTime(now.GetYear( ), now.GetMonth( ), now.GetDay( ), 0, 0, 0)
				+ COleDateTimeSpan(1, 0, 0, 1);
		}
		else
		{
			// testing reset time - 2 minutes which feels like a long time
			date2	= now + COleDateTimeSpan(0, 0, 2, 0);             
		}
		//COleDateTime date2 =  COleDateTime(now.GetYear(), now.GetMonth(), now.GetDay(), 0, 0, 0) +  COleDateTimeSpan(1, 0, 0, 1);
		//COleDateTime date2 =  now +  COleDateTimeSpan(0, 0, 2, 0); // testing reset time - 2 minutes
		COleDateTimeSpan tilmidnight = date2-now;
		_resetthread.Initialize();
		_resetthread.AddTimer(
			(long) tilmidnight.GetTotalSeconds() * 1000,
			&_ResetThread,
			(DWORD_PTR) this,
			&_ResetThread._hTimer  // stored newly created timer handle
			) ;
		logStatus("Agent will Reset at  %s\n", date2.Format("%A, %B %d, %Y %H:%M:%S"));
		return true;
	}
	return false;
}