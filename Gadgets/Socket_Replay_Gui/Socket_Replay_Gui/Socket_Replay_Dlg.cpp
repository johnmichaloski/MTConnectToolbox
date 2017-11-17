
//
// Socket_Replay_Dlg.cpp
//

#include "StdAfx.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <Commdlg.h>
#include "Socket_Replay_Dlg.h"

#define TIMER1    1051
extern std::string             ExeDirectory;
extern boost::asio::io_service _io_service;

Socket_Replay_Dlg::Socket_Replay_Dlg(void) : mParser(mBackend)
{
    mCountdown     = 100;
    dTimeMultipler = 1.0;
    mStreamFlag    = false;
}
Socket_Replay_Dlg::~Socket_Replay_Dlg(void)
{ }
void Socket_Replay_Dlg::halt ( )
{
    mStreamFlag = false;
    mBackend.Quit( );
}
void Socket_Replay_Dlg::stream_loop ( )
{
    int    nSleep = mCountdown;
    double delay  = 0;

    try
    {
        mStreamFlag = true;

        while ( mStreamFlag )
        {
            if ( ( mBackend.Count( ) < 1 ) && bOptionWait )
            {
                nSleep = mCountdown;
            }
            else
            {
                if ( delay < 0 )
                {
                    delay = (long) mParser.ProcessStream( ) * dTimeMultipler;

                    if ( delay < 0 )
                    {
                        break;                             // should only get here if no repeat
                    }
                    mBackend.StoreSocketString(mParser.GetLatestMsg( ));

                    // this->screen_msg(mParser.GetLatestBuffer()+"\r\n"); // GUI slow down
                    buffer_queue.AddMsgQueue(mParser.GetLatestBuffer( ) + "\r\n");
                }
                else
                {
                    delay -= mCountdown;
                }
            }
            Timing::Sleep(nSleep);
        }

        mBackend.Quit( );
        Timing::Sleep(2000);
    }
    catch ( std::exception err )
    {
        std::string errmsg = StrFormat("Exception in file %s at line %d %s\n", 
			mFilename.c_str( ), 
			mParser.LineNumber( ), err.what( ));
        this->fatal_msg(errmsg);
    }
}
BOOL Socket_Replay_Dlg::CenterWindow (HWND hwndWindow)
{
    HWND hwndParent;
    RECT rectWindow, rectParent;

    // hwndParent = GetParent(hwndWindow);
    if ( ( hwndParent = GetParent(hwndWindow) ) == NULL )
    {
        hwndParent = GetDesktopWindow( );
    }

    // make the window relative to its parent
    // if ((hwndParent = GetParent(hwndWindow)) != NULL)
    {
        GetWindowRect(hwndWindow, &rectWindow);
        GetWindowRect(hwndParent, &rectParent);

        int nWidth  = rectWindow.right - rectWindow.left;
        int nHeight = rectWindow.bottom - rectWindow.top;

        int nX = ( ( rectParent.right - rectParent.left ) - nWidth ) / 2 + rectParent.left;
        int nY = ( ( rectParent.bottom - rectParent.top ) - nHeight ) / 2 + rectParent.top;

        int nScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
        int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

        // make sure that the dialog box never moves outside of the screen
        if ( nX < 0 )
        {
            nX = 0;
        }

        if ( nY < 0 )
        {
            nY = 0;
        }

        if ( nX + nWidth > nScreenWidth )
        {
            nX = nScreenWidth - nWidth;
        }

        if ( nY + nHeight > nScreenHeight )
        {
            nY = nScreenHeight - nHeight;
        }

        MoveWindow(hwndWindow, nX, nY, nWidth, nHeight, FALSE);

        return TRUE;
    }

    return FALSE;
}
char *Socket_Replay_Dlg::SelectFile (HWND hwnd, const char *folder)
{
    OPENFILENAME ofn;                                      // common dialog box structure

    szFile[0] = 0;

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof( ofn ));
    ofn.lStructSize = sizeof( ofn );
    ofn.hwndOwner   = hwnd;
    ofn.lpstrFile = szFile;

    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0]    = '\0';
    ofn.nMaxFile        = sizeof( szFile );
    ofn.lpstrFilter     = "All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex    = 1;
    ofn.lpstrFileTitle  = NULL;
    ofn.nMaxFileTitle   = 0;
    ofn.lpstrInitialDir = folder;
    ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box.

    if ( GetOpenFileName(&ofn) == TRUE )
    {
        strncpy(szFile, ofn.lpstrFile, strlen(ofn.lpstrFile));
    }
    return szFile;
}
HWND Socket_Replay_Dlg::Create (HINSTANCE hInst, int nCmdShow)
{
    hDlg      = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_DIALOG1), 0, Socket_Replay_Dlg::DialogProc, 0);
    port_hwnd = GetDlgItem(hDlg, IDC_PORT);
    edit_hwnd = GetDlgItem(hDlg, IDC_STATUSEDIT);
#ifdef WIN64
    SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG) this);
#else
    SetWindowLongPtr(hDlg, GWL_USERDATA, (LONG) this);
#endif
    ShowWindow(hDlg, nCmdShow);
    return hDlg;
}
void Socket_Replay_Dlg::fatal_msg (std::string str)
{
    ::PostMessage(edit_hwnd, WM_SETTEXT, NULL, (LPARAM) str.c_str( ));
}
void Socket_Replay_Dlg::screen_msg (std::string str)
{
    // Thanks to https://www.codeproject.com/Articles/2739/Quick-positioning-of-the-caret-at-the-end-of-the-t
    SendDlgItemMessage(hDlg, IDC_STATUSEDIT, EM_SETSEL, 0, -1);
    SendDlgItemMessage(hDlg, IDC_STATUSEDIT, EM_SETSEL, -1, -1);
    SendDlgItemMessage(hDlg, IDC_STATUSEDIT, EM_REPLACESEL, 0, (long) str.c_str( ));
}
INT_PTR CALLBACK Socket_Replay_Dlg::DialogProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Socket_Replay_Dlg *pThis = Socket_Replay_Dlg::GetObjectFromWindow(hDlg);

    switch ( uMsg )
    {
    case WM_TIMER:
        {
            if ( wParam == TIMER1 )
            {
                std::string msg;

                while ( pThis->buffer_queue.SizeMsgQueue( ) > 0 )
                {
                    msg += pThis->buffer_queue.PopFrontMsgQueue( );
                }

                if ( !msg.empty( ) )
                {
                    pThis->screen_msg(msg);
                }
            }
            return TRUE;
        }

    case WM_COMMAND:
        {
            switch ( LOWORD(wParam) )
            {
            case IDCANCEL:
                {
                    // STOP COMMAND
                    KillTimer(hDlg, TIMER1);
                    pThis->halt( );
                    return TRUE;
                }

            case IDOK:
                {
                    // GO COMMAND
                    if ( pThis->mStreamFlag )              // already running?
                    {
                        return TRUE;
                    }
                    BOOL  fError;
                    TCHAR filename[256];
                    int   port = GetDlgItemInt(hDlg, IDC_PORT, &fError, false /* no minus*/);
                    UINT  m    = GetDlgItemText(hDlg, IDC_FILENAMEEDIT, filename, 256);
                    pThis->mFilename = filename;
                    pThis->mBackend.Init("127.0.0.1", port);
                    pThis->mParser.Init(filename);
                    SetTimer(hDlg, TIMER1, 100, NULL);     // no timer callback
                    boost::thread t(boost::bind(&Socket_Replay_Dlg::stream_loop, pThis));
                }
                return TRUE;

            case IDC_FILESELECTBUTTON:
                {
                    char *filename = pThis->SelectFile(hDlg, ExeDirectory.c_str( ));
                    SetDlgItemText(hDlg, IDC_FILENAMEEDIT, filename);
                }
                break;
            }

            return TRUE;
        }

    case WM_CLOSE:
        {
            // stop
            PostQuitMessage(0);
            return TRUE;                                   /* just continue reading on... */
        }

    case WM_DESTROY:
        {
            DestroyWindow(hDlg);
            return TRUE;
        }

    case WM_INITDIALOG:
        {
            // center dialog
            Socket_Replay_Dlg::CenterWindow(hDlg);
            SetWindowText(hDlg, "Socket_replay");
            HWND port_hwnd = GetDlgItem(hDlg, IDC_PORT);
            SetWindowText(port_hwnd, "50241");
			return TRUE;
        }
        break;
    }

    return FALSE;
}
