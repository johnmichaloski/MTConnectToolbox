
#include "stdafx.h"
#include "SocketParser.h"
#include "SocketBackEnd.h"
#include <iostream>
#include "Timing.h"
#include "Config.h"
#include "Strsafe.h"
#include "Socket_Replay_Dlg.h"





std::string ExeDirectory;
int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE h0, LPTSTR lpCmdLine, int nCmdShow)
{
	HWND hDlg;
	MSG msg;
	BOOL ret;

	ExeDirectory=ExtractDirectory(__argv[0]);
	InitCommonControls();
	Socket_Replay_Dlg dlg;

	dlg.mParser.Repeat()=true;
	dlg.bOptionWait=true;

	hDlg=dlg.Create(hInst,nCmdShow);

	while((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
		if(ret == -1)
			return -1;

		if(!IsDialogMessage(hDlg, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}
