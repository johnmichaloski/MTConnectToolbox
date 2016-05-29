// MSI_Logging.cpp
//

#include "stdafx.h" 
#include <stdio.h>


void LogString(MSIHANDLE hInstall, TCHAR* szString)
{
	// if you are curious what the PMSIHANDLE is, look it up in the msi.chm.  It is actually a good idea to use it
	// rather than MSIHANDLE.  Basically it will free itself when it goes out of scope.  In the past I helped
	// track a bug down to the fact that they wen't using it...
	PMSIHANDLE newHandle = ::MsiCreateRecord(2);
	TCHAR szTemp[MAX_PATH * 2];
	sprintf(szTemp, "-- MSI_LOGGING --   %s", szString); 
	MsiRecordSetString(newHandle, 0, szTemp);
	MsiProcessMessage(hInstall, INSTALLMESSAGE(INSTALLMESSAGE_INFO), newHandle);
	// if you get one of those:  cannot convert parameter 2 from 'enum tagINSTALLMESSAGE' to 'enum tagMSIMESSAGE'
	// errors on the line above, then chances are you:
	//    1) havent installed the latest MSI SDK (now apart of the Platform SDK)
	// or 2) havent added the \include\ and \lib\ directories to the Visual Studio path (tools | options, directory tab).
}




int MsiMessageBox(MSIHANDLE hInstall, TCHAR* szString, DWORD dwDlgFlags)
{
	// I am not aware of anyway to programmatically set the dialog title through this code.
	// I think you may just have to author the Error Dialog in your .msi to use the text that you want.
	// chances are you will want to use [ProgramName] because the Error Dialog can get called from the MSI
	// it self, if it encounters an error, and it would be wierd if you hardcoded the title bar to 
	// "Desktop Shortcut?" or something like that.
	PMSIHANDLE newHandle = ::MsiCreateRecord(2);
	MsiRecordSetString(newHandle, 0, szString);
	return (MsiProcessMessage(hInstall, INSTALLMESSAGE(INSTALLMESSAGE_USER + dwDlgFlags), newHandle));
}
