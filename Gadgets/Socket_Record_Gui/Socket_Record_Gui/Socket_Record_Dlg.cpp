
//
// Socket_Record_Dlg.cpp
//

#include "StdAfx.h"

#include <Commdlg.h>
#include "Socket_Record_Dlg.h"
#include <StrSafe.h>

#define TIMER1    1051
extern std::string ExeDirectory;

Socket_Record_Dlg::Socket_Record_Dlg(void)  : recorder(this)
{
    dTimeMultipler = 1.0;
    mStreamFlag    = false;
}
Socket_Record_Dlg::~Socket_Record_Dlg(void)
{ }
void Socket_Record_Dlg::halt ( )
{
    recorder.halt( );
}
BOOL Socket_Record_Dlg::CenterWindow (HWND hwndWindow)
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
char *Socket_Record_Dlg::SelectFile (HWND hwnd, const char *folder)
{
    OPENFILENAME ofn;                                      // common dialog box structure
    char         szFileName[MAX_PATH] = "";

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof( ofn ));
    ofn.lStructSize = sizeof( ofn );
    ofn.hwndOwner   = hwnd;
    ofn.lpstrFile   = szFile;

    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0]   = '\0';
    ofn.nMaxFile       = MAX_PATH;
    ofn.lpstrFilter    = "All\0*.*\0Text\0*.txt\0";
    ofn.nFilterIndex   = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle  = 0;

    ofn.lpstrInitialDir = folder;
    ofn.Flags           = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt     = "txt";

    // Display the Open dialog box.

    if ( GetSaveFileName(&ofn) == TRUE )
    {
        // hf = CreateFile(ofn.lpstrFile,
        // GENERIC_READ,
        // 0,
        // (LPSECURITY_ATTRIBUTES) NULL,
        // OPEN_EXISTING,
        // FILE_ATTRIBUTE_NORMAL,
        // (HANDLE) NULL);

        strncpy(szFile, ofn.lpstrFile, strlen(ofn.lpstrFile));
    }
    return szFile;
}
HWND Socket_Record_Dlg::Create (HINSTANCE hInst, int nCmdShow)
{
    hDlg      = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_DIALOG1), 0, Socket_Record_Dlg::DialogProc, 0);
    port_hwnd = GetDlgItem(hDlg, IDC_PORT);
    edit_hwnd = GetDlgItem(hDlg, IDC_STATUSEDIT);
    SetWindowLongPtr(hDlg, GWL_USERDATA, (LONG) this);
    ShowWindow(hDlg, nCmdShow);
    return hDlg;
}
void Socket_Record_Dlg::fatal_msg (std::string str)
{
    ::PostMessage(edit_hwnd, WM_SETTEXT, NULL, (LPARAM) str.c_str( ));
}
void Socket_Record_Dlg::screen_msg (std::string str)
{
    // Thanks to https://www.codeproject.com/Articles/2739/Quick-positioning-of-the-caret-at-the-end-of-the-t
    SendDlgItemMessage(hDlg, IDC_STATUSEDIT, EM_SETSEL, 0, -1);
    SendDlgItemMessage(hDlg, IDC_STATUSEDIT, EM_SETSEL, -1, -1);
    SendDlgItemMessage(hDlg, IDC_STATUSEDIT, EM_REPLACESEL, 0, (long) str.c_str( ));
}
INT_PTR CALLBACK Socket_Record_Dlg::DialogProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Socket_Record_Dlg *pThis = Socket_Record_Dlg::GetObjectFromWindow(hDlg);

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
                    pThis->mStreamFlag = false;
                    KillTimer(hDlg, TIMER1);
                    pThis->recorder.halt( );

                    // STOP COMMAND
                    // SendMessage(hDlg, WM_CLOSE, 0, 0);
                    return TRUE;
                }

            case IDOK:
                {
                    // GO COMMAND
                    if ( pThis->mStreamFlag )              // already running?
                    {
                        return TRUE;
                    }
                    pThis->mStreamFlag = true;

                    BOOL  fError;
                    TCHAR buffer[256];

                    // Filename
                    UINT m = GetDlgItemText(hDlg, IDC_FILENAMEEDIT, buffer, 256);
                    pThis->mFilename = buffer;

                    // Port
                    m            = GetDlgItemText(hDlg, IDC_PORT, buffer, 256);
                    pThis->mPort = buffer;

                    // IP
                    HWND  hWndIPAddress = GetDlgItem(hDlg, IDC_IPADDRESS1);
                    DWORD dwAddr;
                    int   iCount = (int) SendMessage(hWndIPAddress, IPM_GETADDRESS, 0, (LPARAM) &dwAddr);
                    StringCchPrintf(buffer, _countof(buffer), "%ld.%ld.%ld.%ld",
                                    FIRST_IPADDRESS(dwAddr),
                                    SECOND_IPADDRESS(dwAddr),
                                    THIRD_IPADDRESS(dwAddr),
                                    FOURTH_IPADDRESS(dwAddr));
                    pThis->mIP = buffer;

                    SetTimer(hDlg, TIMER1, 100, NULL);     // no timer callback

                    pThis->recorder.init(pThis->mIP, pThis->mPort, pThis->mFilename);
                    pThis->recorder.start( );
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
        {                                                  /* there are more things to go here, */
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
            Socket_Record_Dlg::CenterWindow(hDlg);
            SetWindowText(hDlg, "Socket_replay");

            HWND port_hwnd = GetDlgItem(hDlg, IDC_PORT);
            SetWindowText(port_hwnd, "7878");

            HWND hWndIPAddress = GetDlgItem(hDlg, IDC_IPADDRESS1);
            SendMessage(hWndIPAddress, IPM_SETADDRESS, 0, MAKEIPADDRESS(127, 0, 0, 1));

            HWND hWndFile = GetDlgItem(hDlg, IDC_FILENAMEEDIT);
            SetWindowText(hWndFile, ( ExeDirectory + "log.txt" ).c_str( ));
        }
        break;
    }

    return FALSE;
}
