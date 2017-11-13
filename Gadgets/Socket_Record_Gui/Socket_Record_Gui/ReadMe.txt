
Sun 11/12/17 05:57:28 PM
Removed append status text
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
