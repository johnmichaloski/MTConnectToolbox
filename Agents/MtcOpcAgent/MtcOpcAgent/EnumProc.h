//
// EnumProc.h
//


////////////////////////////////////////////////////////////////
// MSDN Magazine -- July 2002
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio 6.0 and Visual Studio .NET
// Runs in Windows XP and probably Windows 2000 too.
//
#pragma once
#include "stdafx.h"
#include <psapi.h>							 // PSAPI for EnumProcesses
class CWindowIterator;
class CMainWindowIterator;
class CProcessModuleIterator;
class CProcessIterator;

//////////////////
// Iterate the top-level windows. Encapsulates ::EnumWindows.
//
class CWindowIterator {
protected:
	HWND*	m_hwnds;				// array of hwnds for this PID
	DWORD m_nAlloc;			// size of array
	DWORD m_count;				// number of HWNDs found
	DWORD m_current;			// current HWND

public:
	// override to filter different kinds of windows
	virtual BOOL OnWindow(HWND hwnd) {
		return TRUE;
	}

	CWindowIterator(DWORD nAlloc=1024)
	{
		m_current = m_count = 0;
		m_hwnds = new HWND [nAlloc];
		m_nAlloc = nAlloc;
	}

	~CWindowIterator()
	{
		delete [] m_hwnds;
	}

	
	// Static enumerator passes to virtual fn.
	//////////////////
	static BOOL CALLBACK EnumProc(HWND hwnd, LPARAM lp)
	{
		return ((CWindowIterator*)lp)->OnEnumProc(hwnd);
	}

	DWORD GetCount() { return m_count; }

	// Move to first top-level window.
	// Stuff entire array and return the first HWND.
	//////////////////
	HWND First()
	{
		::EnumWindows(EnumProc, (LPARAM)this);
		m_current = 0;
		return Next();
	}
	HWND Next() {
		return m_hwnds && m_current < m_count ? m_hwnds[m_current++] : NULL;
	}
protected:
	// Virtual enumerator proc: add HWND to array if OnWindow is TRUE.
	//////////////////
	virtual BOOL OnEnumProc(HWND hwnd)
	{
		if (OnWindow(hwnd)) {
			if (m_count < m_nAlloc)
				m_hwnds[m_count++] = hwnd;
		}
		return TRUE; // keep looking
	}
};

//////////////////
// Iterate the top-level windows in a process.
//
class CMainWindowIterator : public CWindowIterator  {
protected:
	DWORD m_pid;							// process id
public:
	CMainWindowIterator(DWORD pid, DWORD nAlloc=1024)
		: CWindowIterator(nAlloc)
	{
		m_pid = pid;
	}

	~CMainWindowIterator()
	{
	}
protected:

	// virtual override: is this window a main window of my process?
	//////////////////
	BOOL OnWindow(HWND hwnd)
	{
		if (GetWindowLong(hwnd,GWL_STYLE) & WS_VISIBLE) {
			DWORD pidwin;
			GetWindowThreadProcessId(hwnd, &pidwin);
			if (pidwin==m_pid)
				return TRUE;
		}
		return FALSE;
	}
};

//////////////////
// Iterate the modules in a process. Note that the first module is the
// main EXE that started the process.
//
class CProcessModuleIterator {
protected:
	HANDLE	m_hProcess;			// process handle
	HMODULE*	m_hModules;			// array of module handles
	DWORD		m_count;				// size of array
	DWORD		m_current;			// next module handle
public:
	CProcessModuleIterator(DWORD pid)
	{
		// open the process
		m_hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
			FALSE, pid);
	}

	~CProcessModuleIterator()
	{
		CloseHandle(m_hProcess);
		delete [] m_hModules;
	}


	HANDLE GetProcessHandle()	{ return m_hProcess; }
	DWORD GetCount()				{ return m_count; }
	HMODULE Next() {
		return m_hProcess && m_current < m_count ? m_hModules[m_current++] : 0;
	}

	// Get first module in process. Call EnumProcesseModules to
	// initialize entire array, then return first module.
	//////////////////
	HMODULE First()
	{
		m_count = 0;
		m_current = (DWORD)-1; 
		m_hModules = NULL;
		if (m_hProcess) {
			DWORD nalloc = 1024;
			do {
				delete [] m_hModules;
				m_hModules = new HMODULE [nalloc];
				if (EnumProcessModules(m_hProcess, m_hModules,
					nalloc*sizeof(DWORD), &m_count)) {
						m_count /= sizeof(HMODULE);
						m_current = 0;
				}
			} while (nalloc <= m_count);
		}
		return Next();
	}
};


//////////////////
// Process iterator -- iterator over all system processes
// Always skips the first (IDLE) process with PID=0.
//
class CProcessIterator {
protected:
	DWORD*	m_pids;			// array of procssor IDs
	DWORD		m_count;			// size of array
	DWORD		m_current;		// next array item
public:
	CProcessIterator()
	{
		m_pids = NULL;
	}

	~CProcessIterator()
	{
		delete [] m_pids;
	}

	DWORD GetCount() { return m_count; }
	DWORD Next() 
	{
		return m_pids && m_current < m_count ? m_pids[m_current++] : 0;
	}
	static bool CProcessIterator::FindProcess(CString szProcessName)
	{
		USES_CONVERSION;
		// global switches
		BOOL bClassName=FALSE;		// show window classname
		BOOL bTitle=FALSE;			// show window title
		BOOL bBare=true;				// no header
		// Iterate over all processes
		int count=0;
		BOOL bFirstModule = TRUE;
		CProcessIterator itp;
		for (DWORD pid=itp.First(); pid; pid=itp.Next()) {
			// Note: first module in CProcessModuleIterator is EXE for this 
			// process
			TCHAR modname[_MAX_PATH];
			CProcessModuleIterator itm(pid);
			HMODULE hModule = itm.First(); // .EXE
			if (hModule) {
				GetModuleBaseName(itm.GetProcessHandle(),hModule,modname,_MAX_PATH);
				BOOL bFirstWindow = TRUE;
				CMainWindowIterator itw(pid);
				for (HWND hwnd = itw.First(); hwnd; hwnd=itw.Next()) {
					if (bFirstModule) {
						if (!bBare) {
							OutputDebugString(StrFormat(_T("PID  %-13s%-8s %s%s%s\n"),_T("Module"),_T("HWND"),_T("class name"),
								_T("/"),_T("title")) );
							OutputDebugString(_T("---- ------------ -------- --------------\n"));
						}
						bFirstModule = FALSE;
					}
					if (bFirstWindow) {
						if (!bBare) OutputDebugString(StrFormat(_T("%04x %-13S"), pid, modname));
						bFirstWindow = FALSE;
					} else {
						if (!bBare) OutputDebugString(StrFormat(_T("%18s")," "));
					}
					TCHAR classname[256],title[256];
					GetClassName(hwnd,classname,sizeof(classname));
					GetWindowText(hwnd,title,sizeof(title));
					if (!bBare)
						OutputDebugString(StrFormat(_T("%08x %s %s\n"), hwnd,classname,title));
					// Actual test
					if(szProcessName.CompareNoCase(modname)==0)
						return 1;
				}
				bFirstWindow || count++;
			}
		}
		if (!bBare) 
		{
			OutputDebugString(StrFormat(_T("----\n")));
			OutputDebugString(StrFormat(_T("%d processes found.\n"),count));
		}
		return 0;

	}
	// Get first process.
	// Call EnumProcesses to initialize entire array, then return first one.
	//////////////////
	DWORD First()
	{
		m_current = (DWORD)-1;
		m_count = 0;
		DWORD nalloc = 1024;
		do {
			delete [] m_pids;
			m_pids = new DWORD [nalloc];
			if (EnumProcesses(m_pids, nalloc*sizeof(DWORD), &m_count)) {
				m_count /= sizeof(DWORD);
				m_current = 1; // important: skip IDLE process with pid=0.
			}
		} while (nalloc <= m_count);

		return Next();
	}
};
