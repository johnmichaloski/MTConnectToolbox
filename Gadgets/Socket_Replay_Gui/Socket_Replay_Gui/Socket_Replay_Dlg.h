
//
// Socket_Replay_Dlg.h
//

#pragma once
#include "SocketParser.h"
#include "SocketBackEnd.h"
#include <string>
#include "RCSMsgQueue.h"

/**
 * @brief The Socket_Replay_Dlg class is a simple win32 class that does
 * not need ATL/WTL or MFC to present a dialog for the user to replay a
 * saved stream file.  It only works on win32 PCs!
 * The user inputs a socket port number and a full path filename using
 * the win32 file control dialog, and when you click the Start button,
 * the file will be replaid, once there is at least one connection to
 * localhost and port number.  If the use wishes to stop the replay, the
 * STOP button is clicked. To exit the application the close button on
 * the top right window corner (i.e., [x]) is clicked.
 *
 */
class Socket_Replay_Dlg
{
public:

    /**
    * @brief Socket_Replay_Dlg constructor for replay dialog handler.
    * * Must create, center and then Uses DialogProc in combination with
    * GetObjectFromWindow to get appropriate this pointer to an instance
    * of this class.
    */
    Socket_Replay_Dlg(void);

    ~Socket_Replay_Dlg(void);

    /**
     * @brief CenterWindow centers window on screen, from MS and web.
     * @param hwndWindow handle of window to center.
     * @return true if done, false if failed.
     */
    static BOOL             CenterWindow (HWND hwndWindow);

    /**
     * @brief DialogProc
     * @param hDlg
     * @param uMsg
     * @param wParam
     * @param lParam
     * @return
     */
    static INT_PTR CALLBACK DialogProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    /**
     * @brief Create creates the replay dialog window using resource id.
     * @param hInst handle to the applicaiton instance.
     * @param nCmdShow show type  from command line
     * @return handle to dialog window
     */
    HWND                    Create (HINSTANCE hInst, int nCmdShow);

    /**
     * @brief SelectFile uses win32 control to select exiting file.
     * @param hwnd handle to parent window
     * @param folder starting folder to find replay file
     * @return full path of replay filename or empty
     */
    char *                  SelectFile (HWND hwnd, const char *folder);

    // Streaming related methods

    /**
     * @brief halt stops the streaming loop thread
     */
    void                    halt ( );

    /**
     * @brief fatal_msg logs a message to the text edit window.
     * Clears screen first.
     */
    void                    fatal_msg (std::string);

    /**
     * @brief screen_msg logs a message to the text edit window.
     */
    void                    screen_msg (std::string);

    // Public members
    bool bOptionWait;                                      /**< wait before replaying until connection */
    SocketBackEnd mBackend;                                /**< handles asio socket connections */
    SocketParser mParser;                                  /**< parses a replay file and calculates timing */
protected:

    /**
     * @brief stream_loop is thread that parses a playback file
     * and outputs the next packet to a socket.
	 *
     */
    void                    stream_loop ( );

    /**
     * @brief GetObjectFromWindow returns a pointer the window (stored as the WindowLong)
     * useful for threading to get the class instance associated with a handle to window.
     * From https://www.codeproject.com/articles/2556/a-simple-win-window-wrapper-class.
	 *
     * @param hWnd
     * @return
     */
    inline static Socket_Replay_Dlg *GetObjectFromWindow (HWND hWnd)
    {
#ifdef WIN64
        return (Socket_Replay_Dlg *) GetWindowLongPtr(hWnd, GWLP_USERDATA);
#else
        return (Socket_Replay_Dlg *) GetWindowLongPtr(hWnd, GWL_USERDATA);
#endif
    }

    // Streaming related varaibled
    RCS::CMessageQueue<std::string> buffer_queue;          /**< mutex queue of output */
    bool mStreamFlag;                                      /**< this is the streaming thread flag */
    int mCountdown;                                        /**< this is the countdown */
    double dTimeMultipler;                                 /**< replay timing multiplier (to slow down or speed up) */
    std::string mFilename;                                 /**< this is the replay filename */

    // Gui related variables
    char szFile[260];                                      /**<  win32 buffer for dlg file name */
    HWND hDlg;                                             /**<  win32 handle to  dlg window */
    HWND port_hwnd;                                        /**<  win32 handle to port edit window */
    HWND edit_hwnd;                                        /**<  win32 handle to status output edit window */
};
