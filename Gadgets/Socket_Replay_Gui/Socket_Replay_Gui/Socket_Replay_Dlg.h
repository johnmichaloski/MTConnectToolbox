

//
//
//

#pragma once
#include "SocketParser.h"
#include "SocketBackEnd.h"
#include <string>

class Socket_Replay_Dlg
{
public:
	Socket_Replay_Dlg(void);
	~Socket_Replay_Dlg(void);
	static BOOL CenterWindow(HWND hwndWindow);
	static INT_PTR CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	//https://www.codeproject.com/articles/2556/a-simple-win-window-wrapper-class
	// returns a pointer the window (stored as the WindowLong)
	inline static Socket_Replay_Dlg *GetObjectFromWindow(HWND hWnd)
	{
		return (Socket_Replay_Dlg *)GetWindowLongPtr(hWnd, GWL_USERDATA);
	}
	HWND Create(HINSTANCE hInst,int nCmdShow);
	char * SelectFile(HWND hwnd,  const char * folder);
	// Gui related variables
	char szFile[260];       // buffer for file name
	HWND hDlg;
	HWND port_hwnd ;
	HWND edit_hwnd ;

	// Streaming related varaibled
	void halt();
	void stream_loop();
	void fatal_msg(std::string);
	void screen_msg(std::string);
	bool mStreamFlag;
	bool bOptionWait;
	int mCountdown;  // this is the minimum time slice
	SocketBackEnd mBackend;
	SocketParser mParser;
	double dTimeMultipler;
	std::string mFilename  ;
};

