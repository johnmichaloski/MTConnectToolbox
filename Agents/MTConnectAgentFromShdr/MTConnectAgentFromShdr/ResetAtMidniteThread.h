//
// ResetAtMidniteThread.h
//

#pragma once
#include <string>
#include <atlbase.h>
#include "atlutil.h"
#include "ATLComTime.h"
#include "NIST/StdStringFcn.h"

template<typename T>
class CResetAtMidniteThread
{
public:
	CResetAtMidniteThread(void){_bResetAtMidnight=false;}
	~CResetAtMidniteThread(void){}
	int _bResetAtMidnight;
	bool Start();
	void Stop()
	{
		if(_bResetAtMidnight)
			_resetthread.Shutdown();
	}
	////////////////////////////////////////////////////////////////////
	void Warning(std::string err)
	{
		AtlTrace(err.c_str());
	}
	void Fatal(std::string err)
	{
		AtlTrace(err.c_str());
	}
protected:
//	bool ResetAtMidnite();
	CWorkerThread<> _resetthread;
	struct CResetThread : public IWorkerThreadClient
	{
		HRESULT Execute(DWORD_PTR dwParam, HANDLE hObject);
		HRESULT CloseHandle(HANDLE){ ::CloseHandle(_hTimer); return S_OK; }
		HANDLE _hTimer;
	} _ResetThread;
};

//template<typename T>
//inline bool CResetAtMidniteThread<T>::ResetAtMidnite()
//{
//	static char name[] = "CResetAtMidniteThread::ResetAtMidnite";
//	COleDateTime now = COleDateTime::GetCurrentTime();
//	COleDateTime date2 =  COleDateTime(now.GetYear(), now.GetMonth(), now.GetDay(), 0, 0, 0) +  COleDateTimeSpan(1, 0, 0, 1);
//	//COleDateTime date2 =  now +  COleDateTimeSpan(0, 0, 2, 0); // testing reset time - 2 minutes
//	COleDateTimeSpan tilmidnight = date2-now;
//	_ResetThread.Initialize();
//	_ResetThread.AddTimer(
//		(long) tilmidnight.GetTotalSeconds() * 1000,
//		&_ResetThread,
//		(DWORD_PTR) this,
//		&_ResetThread._hTimer  // stored newly created timer handle
//		) ;
//	return true;
//}

template<typename T>
inline  HRESULT CResetAtMidniteThread<T>::CResetThread::Execute(DWORD_PTR dwParam, HANDLE hObject)
{
	static char name[] = "CResetAtMidniteThread::CResetThread::Execute";

	T * service = (T*) dwParam;
	CancelWaitableTimer(hObject);
	//DebugBreak();

	try {

		PROCESS_INFORMATION pi;
		ZeroMemory( &pi, sizeof(pi) );

		STARTUPINFO si;
		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;	 // set the window display to HIDE	

		// SCM reset command of this service 
		std::string cmd = StdStringFormat("cmd /c net stop \"%s\" & net start \"%s\"", service->name(), service->name());        // Command line

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
			AtlTrace("CreateProcess FAIL ") ;

		::Sleep(5000); // make sure process has spawned before killing thread
	}
	catch(...)
	{
		service->Fatal("Exception  - ResetAtMidnightThread(void *oObject");
	}
	return S_OK;
}

template<typename T>
inline bool CResetAtMidniteThread<T>::Start()
{
	static char name[] = "CResetAtMidniteThread::Start";
	//_bResetAtMidnight = ConvertString<bool>(reader.block("OPCSERVER")["ResetAtMidnight"], true);
	if(_bResetAtMidnight)
	{
		GLogger.Fatal("Agent will Reset At Midnight\n");

		COleDateTime now = COleDateTime::GetCurrentTime();
		COleDateTime date2 =  COleDateTime(now.GetYear(), now.GetMonth(), now.GetDay(), 0, 0, 0) +  COleDateTimeSpan(1, 0, 0, 1);
		//COleDateTime date2 =  now +  COleDateTimeSpan(0, 0, 2, 0); // testing reset time - 2 minutes
		COleDateTimeSpan tilmidnight = date2-now;
		_resetthread.Initialize();
		_resetthread.AddTimer(
			(long) tilmidnight.GetTotalSeconds() * 1000,
			&_ResetThread,
			(DWORD_PTR) this,
			&_ResetThread._hTimer  // stored newly created timer handle
			) ;
	return true;
	}
	return false;
}