
//
// Socket_Record_Dlg.h
//

#pragma once
#include "socket_communication.h"
#include <string>
#include "RCSMsgQueue.h"

class Socket_Record_Dlg
{
public:
    Socket_Record_Dlg(void);
    ~Socket_Record_Dlg(void);
    static BOOL             CenterWindow (HWND hwndWindow);
    static INT_PTR CALLBACK DialogProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // https://www.codeproject.com/articles/2556/a-simple-win-window-wrapper-class
    // returns a pointer the window (stored as the WindowLong)
    inline static Socket_Record_Dlg *GetObjectFromWindow (HWND hWnd)
    {
        return (Socket_Record_Dlg *) GetWindowLongPtr(hWnd, GWL_USERDATA);
    }

    HWND                    Create (HINSTANCE hInst, int nCmdShow);
    char *                  SelectFile (HWND hwnd, const char *folder);

    // Gui related variables
    char szFile[260];                                      // buffer for file name
    HWND hDlg;
    HWND port_hwnd;
    HWND edit_hwnd;
    HWND ip_hwnd;

    // Streaming related varaibled
    void                    halt ( );
    void                    fatal_msg (std::string);
    void                    screen_msg (std::string);
    RCS::CMessageQueue<std::string> buffer_queue;
    bool mStreamFlag;
    socket_communication recorder;
    double dTimeMultipler;
    std::string mFilename;
    std::string mPort;
    std::string mIP;
};
