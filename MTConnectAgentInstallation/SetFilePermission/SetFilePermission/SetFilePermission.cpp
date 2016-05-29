//
// SetFilePermission.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include <AtlBase.h>
#include <AtlConv.h>
#include <shellapi.h>
#include <comdef.h>
#include <vector>
#include <string>

#include "SetFilePermission.h"
#include "FilePermission.h"
#include "File.h"


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	USES_CONVERSION;
	const char * cmdline = GetCommandLine();
#if 1
	LPWSTR *szArgList;
	int argCount;
	std::vector<std::string> args;
	szArgList = CommandLineToArgvW(GetCommandLineW(), &argCount);

	//::MessageBoxA(NULL, "SetFilePermission", "Hello", MB_OK);
	for(int i = 0; i < argCount; i++)
	{
		args.push_back((LPCSTR) bstr_t(szArgList[i]));
	}
	//DebugBreak();
	if(File.IsDirectory(args[1]))
	{
		std::vector<std::string> files = File.FileList(args[1]);
		std::vector<std::string> allfiles= File.RecursiveFindFiles(args[1]);
		for(int j=0; j< allfiles.size(); j++)
		{
			CFilePermission::SetFilePermission(allfiles[j]);
		}
	}
	else
	{
		for(int j=1; j< args.size(); j++)
		{
			CFilePermission::SetFilePermission(args[j]);
		}
	}
#endif
#if 0
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SETFILEPERMISSION, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SETFILEPERMISSION));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
#endif
	LocalFree(szArgList);
	return 0;
}


