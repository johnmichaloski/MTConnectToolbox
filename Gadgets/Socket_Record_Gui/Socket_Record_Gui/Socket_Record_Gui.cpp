
#include "stdafx.h"
#include <iostream>
#include <boost/asio.hpp>

#include "Config.h"
#include "Strsafe.h"
#include "Socket_Record_Dlg.h"

#include "Logger.h"

extern boost::asio::io_service io_service;

std::string ExeDirectory;
inline std::string ExtractDirectory (const std::string & path)
{
    return path.substr(0, path.find_last_of('\\') + 1);
}
int WINAPI _tWinMain (HINSTANCE hInst, HINSTANCE h0, LPTSTR lpCmdLine, int nCmdShow)
{
    HWND hDlg;
    MSG  msg;
    BOOL ret;

    ExeDirectory             = ExtractDirectory(__argv[0]);
    GLogger.Timestamping( )  = true;
    GLogger.DebugString( )   = "Socket_Record";
    GLogger.OutputConsole( ) = true;
    GLogger.Open(ExeDirectory + "Debug.txt");
    GLogger.DebugLevel( ) = 0;

    InitCommonControls( );
    Socket_Record_Dlg dlg;

    hDlg = dlg.Create(hInst, nCmdShow);

    while ( true )
    {
        if ( ret = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
        {
            // if(ret == -1)
            //	return -1;
            if ( msg.message == WM_QUIT )
            {
                return -1;
            }

            if ( !IsDialogMessage(hDlg, &msg) )
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            io_service.run_one( );
        }
    }

    return 0;
}
