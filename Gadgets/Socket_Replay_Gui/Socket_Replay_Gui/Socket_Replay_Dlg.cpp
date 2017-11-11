#include "StdAfx.h"

#ifndef WIN64
#define BOOST_BIND_ENABLE_STDCALL
#endif
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <Commdlg.h>
#include "Socket_Replay_Dlg.h"


Socket_Replay_Dlg::Socket_Replay_Dlg(void) : mParser(mBackend)
{
	mCountdown=100;
	dTimeMultipler = 1.0;
}


Socket_Replay_Dlg::~Socket_Replay_Dlg(void)
{
}


void Socket_Replay_Dlg::halt()
{
	mStreamFlag=false;
}
void Socket_Replay_Dlg::stream_loop()
{
	int nSleep = mCountdown;
	double delay=0;
	try {
		mStreamFlag=true;
		while(mStreamFlag)
		{
			SocketBackEnd::_io_service.run_one();
			if(mBackend.Count()<1 && bOptionWait)
			{
				nSleep=mCountdown;
			}
			else
			{
				if(delay<0)
				{
					delay=(long) mParser.ProcessStream()*dTimeMultipler;
					if(delay<0)
						break; // should only get here if no repeat
					mBackend.StoreSocketString(mParser.GetLatestMsg());
					this->screen_msg(mParser.GetLatestBuffer()+"\r\n");
				}
				else
					delay-=mCountdown;
			}
			Timing::Sleep(nSleep);
		}
		mBackend.Quit();
		Timing::Sleep(2000);
	}
	catch(std::exception err)
	{
		std::string errmsg = StrFormat("Exception in file %s at line %d %s\n", mFilename.c_str(),  mParser.LineNumber(), err.what());
		this->fatal_msg(errmsg);
	}
}

BOOL Socket_Replay_Dlg::CenterWindow(HWND hwndWindow)
{
     HWND hwndParent;
     RECT rectWindow, rectParent;
	// hwndParent = GetParent(hwndWindow);
	 if ((hwndParent = GetParent(hwndWindow)) == NULL) 
	 {
		 hwndParent = GetDesktopWindow(); 
	 }

     // make the window relative to its parent
    // if ((hwndParent = GetParent(hwndWindow)) != NULL)
     {
         GetWindowRect(hwndWindow, &rectWindow);
         GetWindowRect(hwndParent, &rectParent);
 
         int nWidth = rectWindow.right - rectWindow.left;
         int nHeight = rectWindow.bottom - rectWindow.top;
 
         int nX = ((rectParent.right - rectParent.left) - nWidth) / 2 + rectParent.left;
         int nY = ((rectParent.bottom - rectParent.top) - nHeight) / 2 + rectParent.top;
 
         int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
         int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
 
         // make sure that the dialog box never moves outside of the screen
         if (nX < 0) nX = 0;
         if (nY < 0) nY = 0;
         if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
         if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;
 
         MoveWindow(hwndWindow, nX, nY, nWidth, nHeight, FALSE);
 
         return TRUE;
     }
 
     return FALSE;
}
char * Socket_Replay_Dlg::SelectFile(HWND hwnd, const char * folder)
{
	OPENFILENAME ofn;       // common dialog box structure

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	strncpy(szFile, folder, strlen(folder));
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = folder;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn)==TRUE) 
	{
		//hf = CreateFile(ofn.lpstrFile, 
		//GENERIC_READ,
		//0,
		//(LPSECURITY_ATTRIBUTES) NULL,
		//OPEN_EXISTING,
		//FILE_ATTRIBUTE_NORMAL,
		//(HANDLE) NULL);

		strncpy(szFile, ofn.lpstrFile, strlen(ofn.lpstrFile));
	}
	return szFile;

}

HWND Socket_Replay_Dlg::Create(HINSTANCE hInst,int nCmdShow)
{

  hDlg = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_DIALOG1), 0, Socket_Replay_Dlg::DialogProc, 0);
  port_hwnd = GetDlgItem(hDlg, IDC_PORT);
  edit_hwnd = GetDlgItem(hDlg, IDC_STATUSEDIT);
  SetWindowLongPtr(hDlg, GWL_USERDATA, (LONG) this);
  ShowWindow(hDlg, nCmdShow);
  return hDlg;
}

void  Socket_Replay_Dlg::fatal_msg(std::string str)
{
	::PostMessage(edit_hwnd,  WM_SETTEXT,  NULL, (LPARAM)  str.c_str());
}
void  Socket_Replay_Dlg::screen_msg(std::string str)
{
	// get new length to determine buffer size
	int outLength = GetWindowTextLength( edit_hwnd ) + str.size() + 1;

	// create buffer to hold current and new text
	std::vector<TCHAR> buf( outLength );
	TCHAR *pbuf = &buf[0];

	// get existing text from edit control and put into buffer
	GetWindowText( edit_hwnd, pbuf, outLength );

	// append the newText to the buffer
	_tcscat_s( pbuf, outLength, str.c_str() );

	// Set the text in the edit control
	SetWindowText( edit_hwnd, pbuf );
	//::PostMessage(edit_hwnd,  WM_SETTEXT,  NULL, (LPARAM)  pbuf);
}

INT_PTR CALLBACK Socket_Replay_Dlg::DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Socket_Replay_Dlg * pThis = Socket_Replay_Dlg::GetObjectFromWindow(hDlg);
	switch(uMsg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			pThis->halt();
			// STOP COMMAND
			//SendMessage(hDlg, WM_CLOSE, 0, 0);
			return TRUE;
		case IDOK:
			{
				// GO COMMAND
				BOOL fError;
				TCHAR filename[256];
				int port  = GetDlgItemInt(hDlg, IDC_PORT, &fError, false /* no minus*/); 
				UINT m = GetDlgItemText(hDlg, IDC_FILENAMEEDIT, filename, 256);
				pThis->mFilename=filename;
				//SendMessage(hDlg, WM_CLOSE, 0, 0);

				pThis->mBackend.Init("127.0.0.1", port);
				pThis->mParser.Init(filename);
				boost::thread t(boost::bind(&Socket_Replay_Dlg::stream_loop, pThis));
			}
			return TRUE;
		case IDC_FILESELECTBUTTON:
			{
				char * filename = pThis->SelectFile(hDlg, ExeDirectory.c_str());
				SetDlgItemText(hDlg, IDC_FILENAMEEDIT, filename);
			}
			break;
		}
		break;

	case WM_CLOSE: /* there are more things to go here, */
		// stop
		PostQuitMessage(0);
		return TRUE; /* just continue reading on... */

	case WM_DESTROY:
		DestroyWindow(hDlg);
		return TRUE;
	case WM_INITDIALOG:
		// center dialog
		Socket_Replay_Dlg::CenterWindow(hDlg);
		SetWindowText(hDlg, "Socket_replay");
		HWND port_hwnd = GetDlgItem(hDlg, IDC_PORT);
		SetWindowText(port_hwnd, "50241");

		break;
	}

  return FALSE;
}