// MtConnectDboardGui.cpp : main source file for MtConnectDboardGui.exe
//

#include "stdafx.h"

#include "resource.h"
#include "StdStringFcn.h"

// Note: Proxy/Stub Information
//		To build a separate proxy/stub DLL, 
//		run nmake -f MtConnectDboardGuips.mk in the project directory.
#include "initguid.h"
#include "MtConnectDboardGui.h"
#include "MtConnectDboardGui_i.c"

#include "aboutdlg.h"
#include "MainFrm.h"
#include "OdbcArchiver.h"

CServerAppModule _Module;
CMainFrame wndMain;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	AtlTrace("Run Enter");
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);


	//OdbcArchiver odbc;
	//odbc.open("CMSD","root","bingo");
	//odbc.createSchema("Factory");
	//int n = odbc.sizeTable("Factory", "alarms");
	//std::vector<std::string> fields=TrimmedTokenize("Severity,RaisedBy,Shift,Description,Program,RaisedAt,ResolvedAt,TBF, TTR", ",");
	//DataDictionary typedata;
	//typedata.Mixin(fields, "adVarWChar");
	//typedata["ResolvedAt"]=typedata["RaisedAt"]="adDate"; 
	//typedata["TBF"]=typedata["TTR"]="adTime";// 00:00:00

	//odbc.selectTable("Factory", "alarms",fields, typedata);

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(SW_HIDE);
#if 0
	if(dlgMain.Create(NULL) == NULL)
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}
	dlgMain.ShowWindow(SW_HIDE);
#endif
	_Module.Lock();
	//dlgMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	AtlTrace("_tWinMain Enter");
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(ObjectMap, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	AtlAxWinInit();

	// Parse command line, register or unregister or run the server
	int nRet = 0;
	TCHAR szTokens[] = _T("-/");
	bool bRun = true;
	bool bAutomation = false;

	LPCTSTR lpszToken = _Module.FindOneOf(::GetCommandLine(), szTokens);
	AtlTrace(CString(lpszToken));
	while(lpszToken != NULL)
	{
		if(lstrcmpi(lpszToken, _T("UnregServer")) == 0)
		{
			_Module.UpdateRegistryFromResource(IDR_MtConnectDboardGUI, FALSE);
			nRet = _Module.UnregisterServer(TRUE);
			bRun = false;
			break;
		}
		///  /RegServer
		else if(lstrcmpi(lpszToken, _T("RegServer")) == 0)
		{
			_Module.UpdateRegistryFromResource(IDR_MtConnectDboardGUI, TRUE);
			nRet = _Module.RegisterServer(TRUE);
			bRun = false;
			break;
		}
		//else if((lstrcmpi(lpszToken, _T("Automation")) == 0) ||
		//	(lstrcmpi(lpszToken, _T("Embedding")) == 0))
		//{
		//	bAutomation = true;
		//	break;
		//}
		lpszToken = _Module.FindOneOf(lpszToken, szTokens);
	}

	if(bRun)
	{
		AtlTrace("_tWinMain StartMonitor");
		_Module.StartMonitor();
		hRes = _Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE | REGCLS_SUSPENDED);
		ATLASSERT(SUCCEEDED(hRes));
		hRes = ::CoResumeClassObjects();
		ATLASSERT(SUCCEEDED(hRes));


		if(bAutomation)
		{
			CMessageLoop theLoop;
			nRet = theLoop.Run();
		}
		else
		{
			nRet = Run(lpstrCmdLine, nCmdShow);
		}

		_Module.RevokeClassObjects();
		::Sleep(_Module.m_dwPause);
	}

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
