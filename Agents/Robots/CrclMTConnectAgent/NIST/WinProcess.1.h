//
// WinProcess.h
//

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#include <comutil.h>
#include <userenv.h>
#include <wtsapi32.h>

// #define tstring
#include "StdStringFcn.h"


extern		HRESULT WarnWithMsg(HRESULT hr, tstring errmsg);
extern		HRESULT FailWithMsg(HRESULT hr, tstring errmsg);
/*** 
Sample:
	CWinProcess p;
	p.RunAsynchronousProcess("C:\\Program Files\\Windows NT\\Accessories\\wordpad.exe", "\"D:\\michalos\\My Work\\CrclMTConnectAgent\\CrclAgent\\x64\\Debug\\Config.ini\"");
	:Sleep(5000);
	p.IsRunning();
	p.TerminateProcess();
	*/
class CWinProcess
{

	LPVOID    lpvEnv;
	PROCESS_INFORMATION piProcessInfo; 
	DWORD _hProcessId;
	tstring FormatErrorMessage(size_t  iReturnVal)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			iReturnVal,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );
		tstring sData(static_cast<const TCHAR *>(lpMsgBuf), lstrlen((LPCTSTR)lpMsgBuf));
		return sData;
	}
public:
	CWinProcess() 
	{
		_hProcessId=0;
		lpvEnv=NULL;

	}
	~CWinProcess()
	{
		//if(lpvEnv != NULL)
		//	DestroyEnvironmentBlock(lpvEnv);

		/* Release handles */ 
		CloseHandle(piProcessInfo.hProcess); 
		CloseHandle(piProcessInfo.hThread); 
	}

	size_t RunAsynchronousProcess(tstring FullPathToExe, tstring Parameters) 
	{ 
		size_t iMyCounter = 0, iReturnVal = 0, iPos = 0; 
		DWORD dwExitCode = 0; 
		std::string sTempStr = ""; 

		/* - NOTE - You should check here to see if the exe even exists */ 

		/* Add a space to the beginning of the Parameters */ 
		if (Parameters.size() != 0) 
		{ 
			if (Parameters[0] != ' ') 
			{ 
				Parameters.insert(0,_T(" ")); 
			} 
		} 
 

		/* CreateProcess API initialization */ 
		STARTUPINFO siStartupInfo; 
		memset(&siStartupInfo, 0, sizeof(siStartupInfo)); 
		memset(&piProcessInfo, 0, sizeof(piProcessInfo)); 
		siStartupInfo.cb = sizeof(siStartupInfo); 

		if (CreateProcess(const_cast<LPCTSTR>(FullPathToExe.c_str()), 
			(LPTSTR) Parameters.c_str(),
			0, 
			0, 
			false, 
			CREATE_DEFAULT_ERROR_MODE, 
			0,
			0, 
			&siStartupInfo, 
			&piProcessInfo) != false) 
		{ 
			/* Watch the process. */ 
			//dwExitCode = WaitForSingleObject(piProcessInfo.hProcess, (SecondsToWait * 1000)); 
			_hProcessId=piProcessInfo.dwProcessId;
			iReturnVal=S_OK;
		} 
		else 
		{ 
			/* CreateProcess failed */ 
			iReturnVal = GetLastError(); 
			FailWithMsg(E_FAIL, FormatErrorMessage(iReturnVal));
			_hProcessId=0;
		} 

		return iReturnVal; 
	}
	void TerminateProcess()
	{
		if(piProcessInfo.hProcess==0)
			return;
		// The handle must have the PROCESS_TERMINATE access right.
		BOOL bTerm= ::TerminateProcess(piProcessInfo.hProcess,0);
		memset(&piProcessInfo, 0, sizeof(piProcessInfo));
	}
	void RunSynchronousProcess(tstring cmd, tstring args, int timeout=4000)
	{
		STARTUPINFO         siStartupInfo;
		PROCESS_INFORMATION piProcessInfo;

		memset(&siStartupInfo, 0, sizeof(siStartupInfo));
		memset(&piProcessInfo, 0, sizeof(piProcessInfo));

		siStartupInfo.cb = sizeof(siStartupInfo);

		if(CreateProcess(cmd.c_str(),     // Application name
			(LPTSTR) args.c_str(),                 // Application arguments
			0,
			0,
			FALSE,
			CREATE_DEFAULT_ERROR_MODE,
			0,
			0,                              // Working directory
			&siStartupInfo,
			&piProcessInfo) != 0)
		{
			WaitForSingleObject (piProcessInfo.hProcess, timeout);
			memset(&piProcessInfo, 0, sizeof(piProcessInfo));
		}
	}

	bool IsRunning()
	{
		if(piProcessInfo.hProcess==0)
			return false;
		DWORD dwExitCode;
		if(GetExitCodeProcess(piProcessInfo.hProcess,&dwExitCode))
		{
			if(dwExitCode==STILL_ACTIVE)
			{
				return true;
			}
			else {
				return false;
			}
		}
		else{
			//query failed, handle probably doesn't have the PROCESS_QUERY_INFORMATION access
		}
		return false;
	}

	/**
	BOOL WINAPI CreateProcessWithLogonW(
	_In_        LPCWSTR               lpUsername,
	_In_opt_    LPCWSTR               lpDomain,
	_In_        LPCWSTR               lpPassword,
	_In_        DWORD                 dwLogonFlags,
	_In_opt_    LPCWSTR               lpApplicationName,
	_Inout_opt_ LPWSTR                lpCommandLine,
	_In_        DWORD                 dwCreationFlags,
	_In_opt_    LPVOID                lpEnvironment,
	_In_opt_    LPCWSTR               lpCurrentDirectory,
	_In_        LPSTARTUPINFOW        lpStartupInfo,
	_Out_       LPPROCESS_INFORMATION lpProcessInfo
	);
	*/
	// logon as user "Administrator" as a local account with password // "12345678"
	// cmd full path to exde
	//
	HRESULT RunAsynchronousProcessAsUserW(tstring cmd, tstring Parameters, tstring user, tstring domain, tstring pw,int timeout=4000) 
	{
		HANDLE    hToken;
		PROCESS_INFORMATION pi = {0};
		STARTUPINFO         si = {0};
		TCHAR               szUserProfile[256] = _T("");
		WCHAR               wszUserProfile[256] =L"";

		if (!LogonUser(_bstr_t(user.c_str()), _bstr_t(domain.c_str()), _bstr_t(pw.c_str()), 
			LOGON32_LOGON_BATCH, LOGON32_PROVIDER_DEFAULT, &hToken))
			return FailWithMsg(E_FAIL,"RunAsynchronousProcessAsUserW LogonUser FAIL\n");
		FailWithMsg(E_FAIL,StdStringFormat("RunAsynchronousProcessAsUserW LogonUser Token 0x%x\n",hToken));

		if (!CreateEnvironmentBlock(&lpvEnv, hToken, FALSE))
			return FailWithMsg(E_FAIL,"RunAsynchronousProcessAsUserW CreateEnvironmentBlock FAIL\n");
		FailWithMsg(E_FAIL,StdStringFormat("RunAsynchronousProcessAsUserW CreateEnvironmentBlock %S\n", lpvEnv));

		DWORD dwSize = sizeof(szUserProfile)/sizeof(WCHAR);

		if (!GetUserProfileDirectoryW(hToken, wszUserProfile, &dwSize))
			return FailWithMsg(E_FAIL,"RunAsynchronousProcessAsUserW GetUserProfileDirectory FAIL\n");
		
		FailWithMsg(E_FAIL,StdStringFormat("RunAsynchronousProcessAsUserW GetUserProfileDirectory %S\n", wszUserProfile));

		STARTUPINFOW         siw = {0};
		siw.cb = sizeof(STARTUPINFOW);
		memset(&piProcessInfo, 0, sizeof(piProcessInfo)); 
		if(!CreateProcessWithLogonW(
			_bstr_t( user.c_str() ), //_bstr_t( "michalos" ),
			_bstr_t( domain.c_str() ), // _bstr_t( "NIST"), // _bstr_t( "\\\\.\\ " ),_bstr_t( "localhbost" ) 
			_bstr_t( pw.c_str() ), // _bstr_t( "jlm@#@#cbm1984"),
			LOGON_WITH_PROFILE,
			NULL,
			_bstr_t( cmd.c_str() ),
			 CREATE_UNICODE_ENVIRONMENT, 
			 lpvEnv,		  
			wszUserProfile, 
			&siw,
			&piProcessInfo
			))
		{
			DWORD dw = GetLastError();
			return FailWithMsg(E_FAIL, StdStringFormat(_T("CreateProcessWithLogonW: 0x%x User:%s Domain:%s Pw:%s FAIL\n"), dw, user.c_str(), 
				domain.c_str(),  pw.c_str()));

		}
		return S_OK;
	}

	void RunAsynchronousProcessAsUser(tstring cmd, tstring Parameters, tstring user, tstring pw,int timeout=4000) 
	{ 
		HANDLE hToken = NULL;


		if ( LogonUser( user.c_str(), NULL, pw.c_str(), 
			LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &hToken ) )
		{
			PROCESS_INFORMATION processInfo;
			ZeroMemory(&processInfo,sizeof(processInfo));

			STARTUPINFO startInfo;
			ZeroMemory(&startInfo,sizeof(startInfo));
			startInfo.wShowWindow = SW_SHOW;
			startInfo.lpDesktop = _T("winsta0\\default");
			/* Add a space to the beginning of the Parameters */ 
			if (Parameters.size() != 0) 
			{ 
				if (Parameters[0] != ' ') 
				{ 
					Parameters.insert(0, _T(" ")); 
				} 
			} 
			//if ( CreateProcessAsUser( hToken, 
			//	NULL, 
			//	const_cast<LPSTR>((cmd+Parameters).c_str()), 
			//	NULL, 
			//	NULL,
			//	TRUE, 
			//	CREATE_DEFAULT_ERROR_MODE, 
			//	NULL, 
			//	NULL, 
			//	&info, 
			//	&processInfo) )
			if ( 	CreateProcessAsUser( hToken, NULL, 
				const_cast<LPSTR>(cmd.c_str()), 
				NULL, NULL, TRUE, 
				CREATE_NEW_CONSOLE, NULL, NULL, &startInfo, 
				&processInfo))
			{
				// wait for the application to terminate
				WaitForSingleObject( processInfo.hProcess, 25000 );
			}
			else
			{
				// 0x522 ERROR_PRIVILEGE_NOT_HELD
				FailWithMsg(E_FAIL, StdStringFormat(_T("CreateProcessAsUser: 0x%x\n"), GetLastError()));
			}
			CloseHandle(hToken);
			memset(&piProcessInfo, 0, sizeof(piProcessInfo));
		}

	}
	PHANDLE GetCurrentUserToken()
	{
		PHANDLE currentToken = 0;
		PHANDLE primaryToken = 0;

		int dwSessionId = 0;
		PHANDLE hUserToken = 0;
		PHANDLE hTokenDup = 0;

		PWTS_SESSION_INFO pSessionInfo = 0;
		DWORD dwCount = 0;

		// Get the list of all terminal sessions 
		WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, 
			&pSessionInfo, &dwCount);

		int dataSize = sizeof(WTS_SESSION_INFO);

		// look over obtained list in search of the active session
		for (DWORD i = 0; i < dwCount; ++i)
		{
			WTS_SESSION_INFO si = pSessionInfo[i];
			if (WTSActive == si.State)
			{ 
				// If the current session is active – store its ID
				dwSessionId = si.SessionId;
				break;
			}
		} 	

		WTSFreeMemory(pSessionInfo);

		// Get token of the logged in user by the active session ID
		BOOL bRet = WTSQueryUserToken(dwSessionId, currentToken);
		if (bRet == false)
		{
			return 0;
		}

		bRet = DuplicateTokenEx(currentToken, 
			TOKEN_ASSIGN_PRIMARY | TOKEN_ALL_ACCESS,
			0, SecurityImpersonation, TokenPrimary, primaryToken);
		if (bRet == false)
		{
			return 0;
		}

		return primaryToken;
	}
};

#if 0
#define DESKTOP_ALL (DESKTOP_READOBJECTS | DESKTOP_CREATEWINDOW | \
DESKTOP_CREATEMENU | DESKTOP_HOOKCONTROL | DESKTOP_JOURNALRECORD | \
DESKTOP_JOURNALPLAYBACK | DESKTOP_ENUMERATE | DESKTOP_WRITEOBJECTS | \
DESKTOP_SWITCHDESKTOP | STANDARD_RIGHTS_REQUIRED)

#define WINSTA_ALL (WINSTA_ENUMDESKTOPS | WINSTA_READATTRIBUTES | \
WINSTA_ACCESSCLIPBOARD | WINSTA_CREATEDESKTOP | \
WINSTA_WRITEATTRIBUTES | WINSTA_ACCESSGLOBALATOMS | \
WINSTA_EXITWINDOWS | WINSTA_ENUMERATE | WINSTA_READSCREEN | \
STANDARD_RIGHTS_REQUIRED)

#define GENERIC_ACCESS (GENERIC_READ | GENERIC_WRITE | \
GENERIC_EXECUTE | GENERIC_ALL)

BOOL AddAceToWindowStation(HWINSTA hwinsta, PSID psid);

BOOL AddAceToDesktop(HDESK hdesk, PSID psid);

BOOL GetLogonSID (HANDLE hToken, PSID *ppsid);

VOID FreeLogonSID (PSID *ppsid);

BOOL StartInteractiveClientProcess (
    LPTSTR lpszUsername,    // client to log on
    LPTSTR lpszDomain,      // domain of client's account
    LPTSTR lpszPassword,    // client's password
    LPTSTR lpCommandLine    // command line to execute
) 
{
   HANDLE      hToken;
   HDESK       hdesk = NULL;
   HWINSTA     hwinsta = NULL, hwinstaSave = NULL;
   PROCESS_INFORMATION pi;
   PSID pSid = NULL;
   STARTUPINFO si;
   BOOL bResult = FALSE;

// Log the client on to the local computer.

   if (!LogonUser(
           lpszUsername,
           lpszDomain,
           lpszPassword,
           LOGON32_LOGON_INTERACTIVE,
           LOGON32_PROVIDER_DEFAULT,
           &hToken) ) 
   {
      goto Cleanup;
   }

// Save a handle to the caller's current window station.

   if ( (hwinstaSave = GetProcessWindowStation() ) == NULL)
      goto Cleanup;

// Get a handle to the interactive window station.

   hwinsta = OpenWindowStation(
       _T("winsta0"),                   // the interactive window station 
       FALSE,                       // handle is not inheritable
       READ_CONTROL | WRITE_DAC);   // rights to read/write the DACL

   if (hwinsta == NULL) 
      goto Cleanup;

// To get the correct default desktop, set the caller's 
// window station to the interactive window station.

   if (!SetProcessWindowStation(hwinsta))
      goto Cleanup;

// Get a handle to the interactive desktop.

   hdesk = OpenDesktop(
      _T("default"),     // the interactive window station 
      0,             // no interaction with other desktop processes
      FALSE,         // handle is not inheritable
      READ_CONTROL | // request the rights to read and write the DACL
      WRITE_DAC | 
      DESKTOP_WRITEOBJECTS | 
      DESKTOP_READOBJECTS);

// Restore the caller's window station.

   if (!SetProcessWindowStation(hwinstaSave)) 
      goto Cleanup;

   if (hdesk == NULL) 
      goto Cleanup;

// Get the SID for the client's logon session.

   if (!GetLogonSID(hToken, &pSid)) 
      goto Cleanup;

// Allow logon SID full access to interactive window station.

   if (! AddAceToWindowStation(hwinsta, pSid) ) 
      goto Cleanup;

// Allow logon SID full access to interactive desktop.

   if (! AddAceToDesktop(hdesk, pSid) ) 
      goto Cleanup;

// Impersonate client to ensure access to executable file.

   if (! ImpersonateLoggedOnUser(hToken) ) 
      goto Cleanup;

// Initialize the STARTUPINFO structure.
// Specify that the process runs in the interactive desktop.

   ZeroMemory(&si, sizeof(STARTUPINFO));
   si.cb= sizeof(STARTUPINFO);
   si.lpDesktop = TEXT("winsta0\\default");

// Launch the process in the client's logon session.

   bResult = CreateProcessAsUser(
      hToken,            // client's access token
      NULL,              // file to execute
      lpCommandLine,     // command line
      NULL,              // pointer to process SECURITY_ATTRIBUTES
      NULL,              // pointer to thread SECURITY_ATTRIBUTES
      FALSE,             // handles are not inheritable
      NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,   // creation flags
      NULL,              // pointer to new environment block 
      NULL,              // name of current directory 
      &si,               // pointer to STARTUPINFO structure
      &pi                // receives information about new process
   ); 

// End impersonation of client.

   RevertToSelf();

   if (bResult && pi.hProcess != INVALID_HANDLE_VALUE) 
   { 
      WaitForSingleObject(pi.hProcess, INFINITE); 
      CloseHandle(pi.hProcess); 
   } 

   if (pi.hThread != INVALID_HANDLE_VALUE)
      CloseHandle(pi.hThread);  

Cleanup: 

   if (hwinstaSave != NULL)
      SetProcessWindowStation (hwinstaSave);

// Free the buffer for the logon SID.

   if (pSid)
      FreeLogonSID(&pSid);

// Close the handles to the interactive window station and desktop.

   if (hwinsta)
      CloseWindowStation(hwinsta);

   if (hdesk)
      CloseDesktop(hdesk);

// Close the handle to the client's access token.

   if (hToken != INVALID_HANDLE_VALUE)
      CloseHandle(hToken);  

   return bResult;
}

BOOL AddAceToWindowStation(HWINSTA hwinsta, PSID psid)
{
   ACCESS_ALLOWED_ACE   *pace = NULL;
   ACL_SIZE_INFORMATION aclSizeInfo;
   BOOL                 bDaclExist;
   BOOL                 bDaclPresent;
   BOOL                 bSuccess = FALSE;
   DWORD                dwNewAclSize;
   DWORD                dwSidSize = 0;
   DWORD                dwSdSizeNeeded;
   PACL                 pacl;
   PACL                 pNewAcl = NULL;
   PSECURITY_DESCRIPTOR psd = NULL;
   PSECURITY_DESCRIPTOR psdNew = NULL;
   PVOID                pTempAce;
   SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
   unsigned int         i;

   __try
   {
      // Obtain the DACL for the window station.

      if (!GetUserObjectSecurity(
             hwinsta,
             &si,
             psd,
             dwSidSize,
             &dwSdSizeNeeded)
      )
      if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
      {
         psd = (PSECURITY_DESCRIPTOR)HeapAlloc(
               GetProcessHeap(),
               HEAP_ZERO_MEMORY,
               dwSdSizeNeeded);

         if (psd == NULL)
            __leave;

         psdNew = (PSECURITY_DESCRIPTOR)HeapAlloc(
               GetProcessHeap(),
               HEAP_ZERO_MEMORY,
               dwSdSizeNeeded);

         if (psdNew == NULL)
            __leave;

         dwSidSize = dwSdSizeNeeded;

         if (!GetUserObjectSecurity(
               hwinsta,
               &si,
               psd,
               dwSidSize,
               &dwSdSizeNeeded)
         )
            __leave;
      }
      else
         __leave;

      // Create a new DACL.

      if (!InitializeSecurityDescriptor(
            psdNew,
            SECURITY_DESCRIPTOR_REVISION)
      )
         __leave;

      // Get the DACL from the security descriptor.

      if (!GetSecurityDescriptorDacl(
            psd,
            &bDaclPresent,
            &pacl,
            &bDaclExist)
      )
         __leave;

      // Initialize the ACL.

      ZeroMemory(&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION));
      aclSizeInfo.AclBytesInUse = sizeof(ACL);

      // Call only if the DACL is not NULL.

      if (pacl != NULL)
      {
         // get the file ACL size info
         if (!GetAclInformation(
               pacl,
               (LPVOID)&aclSizeInfo,
               sizeof(ACL_SIZE_INFORMATION),
               AclSizeInformation)
         )
            __leave;
      }

      // Compute the size of the new ACL.

      dwNewAclSize = aclSizeInfo.AclBytesInUse +
            (2*sizeof(ACCESS_ALLOWED_ACE)) + (2*GetLengthSid(psid)) -
            (2*sizeof(DWORD));

      // Allocate memory for the new ACL.

      pNewAcl = (PACL)HeapAlloc(
            GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            dwNewAclSize);

      if (pNewAcl == NULL)
         __leave;

      // Initialize the new DACL.

      if (!InitializeAcl(pNewAcl, dwNewAclSize, ACL_REVISION))
         __leave;

      // If DACL is present, copy it to a new DACL.

      if (bDaclPresent)
      {
         // Copy the ACEs to the new ACL.
         if (aclSizeInfo.AceCount)
         {
            for (i=0; i < aclSizeInfo.AceCount; i++)
            {
               // Get an ACE.
               if (!GetAce(pacl, i, &pTempAce))
                  __leave;

               // Add the ACE to the new ACL.
               if (!AddAce(
                     pNewAcl,
                     ACL_REVISION,
                     MAXDWORD,
                     pTempAce,
                    ((PACE_HEADER)pTempAce)->AceSize)
               )
                  __leave;
            }
         }
      }

      // Add the first ACE to the window station.

      pace = (ACCESS_ALLOWED_ACE *)HeapAlloc(
            GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(psid) -
                  sizeof(DWORD));

      if (pace == NULL)
         __leave;

      pace->Header.AceType  = ACCESS_ALLOWED_ACE_TYPE;
      pace->Header.AceFlags = CONTAINER_INHERIT_ACE |
                   INHERIT_ONLY_ACE | OBJECT_INHERIT_ACE;
      pace->Header.AceSize  = LOWORD(sizeof(ACCESS_ALLOWED_ACE) +
                   GetLengthSid(psid) - sizeof(DWORD));
      pace->Mask            = GENERIC_ACCESS;

      if (!CopySid(GetLengthSid(psid), &pace->SidStart, psid))
         __leave;

      if (!AddAce(
            pNewAcl,
            ACL_REVISION,
            MAXDWORD,
            (LPVOID)pace,
            pace->Header.AceSize)
      )
         __leave;

      // Add the second ACE to the window station.

      pace->Header.AceFlags = NO_PROPAGATE_INHERIT_ACE;
      pace->Mask            = WINSTA_ALL;

      if (!AddAce(
            pNewAcl,
            ACL_REVISION,
            MAXDWORD,
            (LPVOID)pace,
            pace->Header.AceSize)
      )
         __leave;

      // Set a new DACL for the security descriptor.

      if (!SetSecurityDescriptorDacl(
            psdNew,
            TRUE,
            pNewAcl,
            FALSE)
      )
         __leave;

      // Set the new security descriptor for the window station.

      if (!SetUserObjectSecurity(hwinsta, &si, psdNew))
         __leave;

      // Indicate success.

      bSuccess = TRUE;
   }
   __finally
   {
      // Free the allocated buffers.

      if (pace != NULL)
         HeapFree(GetProcessHeap(), 0, (LPVOID)pace);

      if (pNewAcl != NULL)
         HeapFree(GetProcessHeap(), 0, (LPVOID)pNewAcl);

      if (psd != NULL)
         HeapFree(GetProcessHeap(), 0, (LPVOID)psd);

      if (psdNew != NULL)
         HeapFree(GetProcessHeap(), 0, (LPVOID)psdNew);
   }

   return bSuccess;

}

BOOL AddAceToDesktop(HDESK hdesk, PSID psid)
{
   ACL_SIZE_INFORMATION aclSizeInfo;
   BOOL                 bDaclExist;
   BOOL                 bDaclPresent;
   BOOL                 bSuccess = FALSE;
   DWORD                dwNewAclSize;
   DWORD                dwSidSize = 0;
   DWORD                dwSdSizeNeeded;
   PACL                 pacl;
   PACL                 pNewAcl = NULL;
   PSECURITY_DESCRIPTOR psd = NULL;
   PSECURITY_DESCRIPTOR psdNew = NULL;
   PVOID                pTempAce;
   SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
   unsigned int         i;

   __try
   {
      // Obtain the security descriptor for the desktop object.

      if (!GetUserObjectSecurity(
            hdesk,
            &si,
            psd,
            dwSidSize,
            &dwSdSizeNeeded))
      {
         if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
         {
            psd = (PSECURITY_DESCRIPTOR)HeapAlloc(
                  GetProcessHeap(),
                  HEAP_ZERO_MEMORY,
                  dwSdSizeNeeded );

            if (psd == NULL)
               __leave;

            psdNew = (PSECURITY_DESCRIPTOR)HeapAlloc(
                  GetProcessHeap(),
                  HEAP_ZERO_MEMORY,
                  dwSdSizeNeeded);

            if (psdNew == NULL)
               __leave;

            dwSidSize = dwSdSizeNeeded;

            if (!GetUserObjectSecurity(
                  hdesk,
                  &si,
                  psd,
                  dwSidSize,
                  &dwSdSizeNeeded)
            )
               __leave;
         }
         else
            __leave;
      }

      // Create a new security descriptor.

      if (!InitializeSecurityDescriptor(
            psdNew,
            SECURITY_DESCRIPTOR_REVISION)
      )
         __leave;

      // Obtain the DACL from the security descriptor.

      if (!GetSecurityDescriptorDacl(
            psd,
            &bDaclPresent,
            &pacl,
            &bDaclExist)
      )
         __leave;

      // Initialize.

      ZeroMemory(&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION));
      aclSizeInfo.AclBytesInUse = sizeof(ACL);

      // Call only if NULL DACL.

      if (pacl != NULL)
      {
         // Determine the size of the ACL information.

         if (!GetAclInformation(
               pacl,
               (LPVOID)&aclSizeInfo,
               sizeof(ACL_SIZE_INFORMATION),
               AclSizeInformation)
         )
            __leave;
      }

      // Compute the size of the new ACL.

      dwNewAclSize = aclSizeInfo.AclBytesInUse +
            sizeof(ACCESS_ALLOWED_ACE) +
            GetLengthSid(psid) - sizeof(DWORD);

      // Allocate buffer for the new ACL.

      pNewAcl = (PACL)HeapAlloc(
            GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            dwNewAclSize);

      if (pNewAcl == NULL)
         __leave;

      // Initialize the new ACL.

      if (!InitializeAcl(pNewAcl, dwNewAclSize, ACL_REVISION))
         __leave;

      // If DACL is present, copy it to a new DACL.

      if (bDaclPresent)
      {
         // Copy the ACEs to the new ACL.
         if (aclSizeInfo.AceCount)
         {
            for (i=0; i < aclSizeInfo.AceCount; i++)
            {
               // Get an ACE.
               if (!GetAce(pacl, i, &pTempAce))
                  __leave;

               // Add the ACE to the new ACL.
               if (!AddAce(
                  pNewAcl,
                  ACL_REVISION,
                  MAXDWORD,
                  pTempAce,
                  ((PACE_HEADER)pTempAce)->AceSize)
               )
                  __leave;
            }
         }
      }

      // Add ACE to the DACL.

      if (!AddAccessAllowedAce(
            pNewAcl,
            ACL_REVISION,
            DESKTOP_ALL,
            psid)
      )
         __leave;

      // Set new DACL to the new security descriptor.

      if (!SetSecurityDescriptorDacl(
            psdNew,
            TRUE,
            pNewAcl,
            FALSE)
      )
         __leave;

      // Set the new security descriptor for the desktop object.

      if (!SetUserObjectSecurity(hdesk, &si, psdNew))
         __leave;

      // Indicate success.

      bSuccess = TRUE;
   }
   __finally
   {
      // Free buffers.

      if (pNewAcl != NULL)
         HeapFree(GetProcessHeap(), 0, (LPVOID)pNewAcl);

      if (psd != NULL)
         HeapFree(GetProcessHeap(), 0, (LPVOID)psd);

      if (psdNew != NULL)
         HeapFree(GetProcessHeap(), 0, (LPVOID)psdNew);
   }

   return bSuccess;
}


#endif
