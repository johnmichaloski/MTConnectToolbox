//
// WinTricks.cpp
//

#include "stdafx.h"
#include "WinTricks.h"

#include <Winnetwk.h>
#pragma comment(lib, "mpr.lib")
#pragma comment(lib, "Netapi32.lib")

//#include "Logger.h" 
#define DbgOut OutputDebugString
#include "Aclapi.h"
namespace WinTricks
{
	void LogErrorMsg(tstring errmsg)
	{
		OutputDebugString(errmsg.c_str());
		//GLogger.Fatal(errmsg);
	}
	tstring FormatError(TCHAR * pMessage, DWORD eNum )
	{
		TCHAR sysMsg[256] = {0};
		TCHAR* p;

		//DWORD eNum = GetLastError( );
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, eNum,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			sysMsg, 256, NULL );
		// Trim the end of the line and terminate it with a null
		p = sysMsg;
		while( ( *p > 31 ) || ( *p == 9 ) )
			++p;
		do { *p-- = 0; } while( ( p >= sysMsg ) &&
			( ( *p == '.' ) || ( *p < 33 ) ) );
		// return the message
		return StdStringFormat( "\n  WARNING: %s failed with error %d (%s)", pMessage, eNum, sysMsg );
	}
	/**
	Impersonation woks only for current thread.
	with local resources it will work as LocalSystem, with the added share it will work as user on remote computer specified in WNetAddConenction2 (in this case - Administrator on SomeComputer).
	You can omit using drive letter in NETRESOURCE and access files via "\server\share\filename.ext" notation
	*/
	bool LogonAddNetworkResource(tstring remotename, tstring localname, tstring pw, tstring user)
	{
		HANDLE hToken;
		LogonUser("NETWORK SERVICE", "NT AUTHORITY", NULL, LOGON32_LOGON_NEW_CREDENTIALS, LOGON32_PROVIDER_WINNT50, &hToken );
		ImpersonateLoggedOnUser(hToken);
		NETRESOURCE nr;  // https://msdn.microsoft.com/en-us/library/windows/desktop/aa385353(v=vs.85).aspx
		memset(&nr, 0, sizeof(nr)) ;
		nr.dwScope = RESOURCE_GLOBALNET;
		nr.dwType = RESOURCETYPE_DISK;
		nr.dwUsage = RESOURCEUSAGE_CONNECTABLE;
		nr.dwDisplayType = RESOURCEDISPLAYTYPE_SHARE;
		nr.lpRemoteName = (LPTSTR) remotename.c_str(); // "\\\\SomeCopmuter\\C$";
		nr.lpLocalName = (LPTSTR) localname.c_str(); // "Z:";
		DWORD dwError = WNetAddConnection2(&nr, pw.c_str(), user.c_str(), 0);
		return dwError==NO_ERROR;
	}
	// http://stackoverflow.com/questions/2968426/wnetaddconnection2-from-a-windows-service
	bool AddNetworkResource(tstring remotename, tstring localname, tstring pw, tstring user)
	{		
		NETRESOURCE nr;
		DWORD dwFlags = CONNECT_UPDATE_PROFILE;
		memset(&nr, 0, sizeof(nr)) ;
		nr.dwScope = RESOURCE_GLOBALNET;
		nr.dwType = RESOURCETYPE_DISK;
		nr.dwUsage = RESOURCEUSAGE_CONNECTABLE;
		nr.dwDisplayType = RESOURCEDISPLAYTYPE_SHARE;
		nr.lpRemoteName = (LPTSTR) remotename.c_str(); // "\\\\SomeCopmuter\\C$";
		nr.lpLocalName = (LPTSTR) localname.c_str(); // "Z:";	
		DWORD dwError = WNetAddConnection2(&nr, pw.c_str(), user.c_str(), 0); //  "SomeComputer\\Username"
		if(dwError!=NO_ERROR)
		{
			LogErrorMsg(FormatError(_T("AddNetworkResource"), dwError ));
		}
		return dwError==NO_ERROR;
	}
	bool NetworkResourceExists(tstring remotename, tstring localname, tstring pw, tstring user)
	{		
		NETRESOURCE nr;
		memset(&nr, 0, sizeof(nr)) ;
		nr.dwScope = RESOURCE_GLOBALNET;
		nr.dwType = RESOURCETYPE_DISK;
		nr.dwUsage = RESOURCEUSAGE_CONNECTABLE;
		nr.dwDisplayType = RESOURCEDISPLAYTYPE_SHARE;
		nr.lpRemoteName = (LPTSTR) remotename.c_str(); // "\\\\SomeCopmuter\\C$";
		nr.lpLocalName = (LPTSTR) localname.c_str(); // "Z:";	
		DWORD dwError = WNetAddConnection2(&nr, pw.c_str(),user.c_str(), 0); //  "SomeComputer\\Username"
		return dwError==ERROR_ALREADY_ASSIGNED;
	}
	DWORD MapNetworkDrive(tstring strRemoteName, tstring strLocalDrive, tstring strUseName, tstring strPassword)
	{
		DWORD dwLastError;
		NETRESOURCE nr ;
		memset(&nr, 0, sizeof(nr)) ;
		nr.dwType			= RESOURCETYPE_DISK;
		nr.dwScope = RESOURCE_GLOBALNET;
		nr.dwDisplayType = RESOURCEDISPLAYTYPE_SHARE;
		nr.dwUsage = RESOURCEUSAGE_CONNECTABLE;

		if (strLocalDrive.empty())
			return E_INVALIDARG;
		if (strRemoteName.empty())
			return E_INVALIDARG;

		nr.lpLocalName = (LPTSTR) strLocalDrive.c_str() ;
		nr.lpRemoteName = (LPTSTR) strRemoteName.c_str() ;

		if (!strPassword.empty() && !strUseName.empty())
		{
			dwLastError = WNetAddConnection2(&nr, strPassword.c_str(), strUseName.c_str(), 0) ;
			if(NO_ERROR != dwLastError)
				LogErrorMsg(FormatError(_T("MapNetworkDrive"), dwLastError ));
		}
		else
		{
			dwLastError = WNetAddConnection2(&nr, NULL, NULL, 0) ;
		}
		return dwLastError;
	}
	//////////////////////////////////////////////////////////////////////////////////////
	/*bool GetLogicalDriveStrings Lib "kernel32" Alias _
	"GetLogicalDriveStringsA" (ByVal nBufferLength As Long, _
	ByVal lpBuffer As String) As Long

	' Check whether a given drive exist
	' Note that this returns True even if the drive isn't currently ready
	' (e.g. a diskette isn't in drive A:)*/
	std::vector<tstring> GetLogicalDrives()
	{
		DWORD dwSize = MAX_PATH;
		char szLogicalDrives[MAX_PATH] = {0};
		DWORD dwResult = GetLogicalDriveStrings(dwSize,szLogicalDrives);
		std::vector<tstring> drives;

		if (dwResult > 0 && dwResult <= MAX_PATH)
		{
			TCHAR * szSingleDrive = szLogicalDrives;
			while(*szSingleDrive)
			{
				drives.push_back(szSingleDrive);

				// get the next drive
				szSingleDrive += strlen(szSingleDrive) + 1;
			}
		}
		return drives;
	}
	bool DriveExists(tstring drive)
	{
		std::vector<tstring> drives;
		if(drive.empty()) return false;
		//get the string that contains all drives
		drives=GetLogicalDrives();
		// check that the letter we're looking for is there
		return  (std::find(drives.begin(), drives.end(), drive) != drives.end());
	}

	void SetFilePermission(tstring szFilename) 
	{ 
		LPCTSTR FileName= szFilename.c_str();
		PSID pEveryoneSID = NULL; 
		BOOL a= TRUE;
		BOOL b= TRUE;
		BOOL c= TRUE;
		DWORD d= 0;
		DWORD ret = 0;
		PACL pACL = NULL; 
		EXPLICIT_ACCESS ea[1]; 
		SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY; 

		// Create a well-known SID for the Everyone group. 
		a = AllocateAndInitializeSid(&SIDAuthWorld, 1, 
			SECURITY_WORLD_RID, 
			0, 0, 0, 0, 0, 0, 0, 
			&pEveryoneSID); 
		if(!a)
			DbgOut("SID initialization failed");
		// Initialize an EXPLICIT_ACCESS structure for an ACE. 
		// The ACE will allow Everyone read access to the key. 
		ZeroMemory(&ea, 1 * sizeof(EXPLICIT_ACCESS)); 
		ea[0].grfAccessPermissions = 0xFFFFFFFF; 
		ea[0].grfAccessMode = GRANT_ACCESS; 
		ea[0].grfInheritance= NO_INHERITANCE; 
		ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID; 
		ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP; 
		ea[0].Trustee.ptstrName = (LPTSTR) pEveryoneSID; 

		// Create a new ACL that contains the new ACEs. 
		ret = SetEntriesInAcl(1, ea, NULL, &pACL); 

		if(ret != ERROR_SUCCESS)
			DbgOut("ACL entry failed");

		// Initialize a security descriptor.  
		PSECURITY_DESCRIPTOR pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, 
			SECURITY_DESCRIPTOR_MIN_LENGTH); 

		b = InitializeSecurityDescriptor(pSD,SECURITY_DESCRIPTOR_REVISION); 
		if(!b)
			DbgOut("Security Descriptor initialization failed");

		// Add the ACL to the security descriptor. 
		c = SetSecurityDescriptorDacl(pSD, 
			TRUE,   // bDaclPresent flag  
			pACL, 
			FALSE);  // not a default DACL 
		if(!c)
			DbgOut("SetSecurityDescriptorDacl failed");

		//Change the security attributes 
		d = SetFileSecurity(FileName, DACL_SECURITY_INFORMATION, pSD); 
		//       SetNamedSecurityInfo("C:\\Program Files\\Mydir\\My.log",SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,pSD,NULL,NULL,NULL);    
		if(d == 0)
			DbgOut("SetFileSecurity failed");
		if (pEveryoneSID) 
			FreeSid(pEveryoneSID); 
		if (pACL) 
			LocalFree(pACL); 
		if (pSD) 
			LocalFree(pSD); 
	}

	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa379620(v=vs.85).aspx
	//  Taking Object Ownership in C++
	// 

	// https://code.msdn.microsoft.com/windowsdesktop/CppCreateLowIntegrityProces-8dd7731d
	BOOL GetProcessIntegrityLevel(PDWORD pdwIntegrityLevel) 
	{ 
		DWORD dwError = ERROR_SUCCESS; 
		HANDLE hToken = NULL; 
		DWORD cbTokenIL = 0; 
		PTOKEN_MANDATORY_LABEL pTokenIL = NULL; 


		if (pdwIntegrityLevel == NULL) 
		{ 
			dwError = ERROR_INVALID_PARAMETER; 
			goto Cleanup; 
		} 


		// Open the primary access token of the process with TOKEN_QUERY. 
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) 
		{ 
			dwError = GetLastError(); 
			goto Cleanup; 
		} 


		// Query the size of the token integrity level information. Note that  
		// we expect a FALSE result and the last error ERROR_INSUFFICIENT_BUFFER 
		// from GetTokenInformation because we have given it a NULL buffer. On  
		// exit cbTokenIL will tell the size of the integrity level information. 
		if (!GetTokenInformation(hToken, TokenIntegrityLevel, NULL, 0, &cbTokenIL)) 
		{ 
			if (ERROR_INSUFFICIENT_BUFFER != GetLastError()) 
			{ 
				// When the process is run on operating systems prior to Windows  
				// Vista, GetTokenInformation returns FALSE with the  
				// ERROR_INVALID_PARAMETER error code because TokenElevation  
				// is not supported on those operating systems. 
				dwError = GetLastError(); 
				goto Cleanup; 
			} 
		} 


		// Now we allocate a buffer for the integrity level information. 
		pTokenIL = (TOKEN_MANDATORY_LABEL *)LocalAlloc(LPTR, cbTokenIL); 
		if (pTokenIL == NULL) 
		{ 
			dwError = GetLastError(); 
			goto Cleanup; 
		} 


		// Retrieve token integrity level information. 
		if (!GetTokenInformation(hToken, TokenIntegrityLevel, pTokenIL,  
			cbTokenIL, &cbTokenIL)) 
		{ 
			dwError = GetLastError(); 
			goto Cleanup; 
		} 


		// Integrity Level SIDs are in the form of S-1-16-0xXXXX. (e.g.  
		// S-1-16-0x1000 stands for low integrity level SID). There is one and  
		// only one subauthority. 
		*pdwIntegrityLevel = *GetSidSubAuthority(pTokenIL->Label.Sid, 0); 


Cleanup: 
		// Centralized cleanup for all allocated resources. 
		if (hToken) 
		{ 
			CloseHandle(hToken); 
			hToken = NULL; 
		} 
		if (pTokenIL) 
		{ 
			LocalFree(pTokenIL); 
			pTokenIL = NULL; 
			cbTokenIL = 0; 
		} 


		if (ERROR_SUCCESS != dwError) 
		{ 
			// Make sure that the error code is set for failure. 
			SetLastError(dwError); 
			return FALSE; 
		} 
		else 
		{ 
			return TRUE; 
		} 
	} 
	/**
	Usage:
	Code:
	SetPrivilege(SE_DEBUG_NAME, true);
	*/
	bool SetPrivilege(LPCTSTR privilege, bool enablePriv) 
	{
		LUID luid = {0};
		if (LookupPrivilegeValue(NULL, privilege, &luid) == FALSE)
			return false;

		HANDLE hToken = NULL;
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken) == FALSE)
			return false;

		TOKEN_PRIVILEGES tokenPriv = {0};
		tokenPriv.PrivilegeCount = 1;
		tokenPriv.Privileges[0].Luid = luid;
		tokenPriv.Privileges[0].Attributes = enablePriv ? SE_PRIVILEGE_ENABLED : 0;
		if (AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, NULL, NULL, NULL) != FALSE) {
			CloseHandle(hToken);
			return GetLastError() == ERROR_SUCCESS;
		}

		CloseHandle(hToken);
		return false;
	}
	//http://www.rohitab.com/discuss/topic/37154-c-dump-wireless-passwords/
	BOOL IsElevated()
	{
		DWORD dwSize = 0;
		HANDLE hToken = NULL;
		BOOL bReturn = FALSE;

		TOKEN_ELEVATION tokenInformation;

		if(!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
			return FALSE;

		if(GetTokenInformation(hToken, TokenElevation, &tokenInformation, sizeof(TOKEN_ELEVATION), &dwSize))
		{
			bReturn = (BOOL)tokenInformation.TokenIsElevated;
		}

		CloseHandle(hToken);
		return bReturn;
	}

	bool IsVistaOrHigher()
	{
		OSVERSIONINFO osVersion; 
		ZeroMemory(&osVersion, sizeof(OSVERSIONINFO));
		osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

		if(!GetVersionEx(&osVersion))
			return false;

		if(osVersion.dwMajorVersion >= 6)
			return true;
		return false;
	}
};