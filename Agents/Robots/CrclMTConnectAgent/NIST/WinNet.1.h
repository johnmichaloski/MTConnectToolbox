//
// WinNet.h
//

#pragma once

#include <WinNetWk.h>
#include <string>
#include "StdStringFcn.h"
// #pragma comment(lib. "mpr.lib")
//#pragma comment(lib, "Netapi32.lib")
//#pragma comment(lib, "Advapi32.lib")

#define UNICODE
#include <lm.h>

extern		HRESULT FailWithMsg(HRESULT hr, tstring errmsg);

class CWinNet
{
	std::string        sUNCPath ;
	std::string        sUser ;
	std::string        sPassword ;
	std::string        sDomain ;

	// https://msdn.microsoft.com/en-us/library/windows/desktop/aa385327(v=vs.85).aspx
	HRESULT CreateFolderShare(std::string user, std::string pw, std::string drive, std::string remotename)
	{
		NETRESOURCE nr;
		DWORD res;
		TCHAR szUserName[32] = user.c_str(); /// "MyUserName",
		szPassword[32] = pw.c_str(); // "MyPassword",
		szLocalName[32] = drive.c_str(); // "Q:",
		szRemoteName[MAX_PATH] = remotename.c_str(); // "\\\\products2\\relsys";

		//

		// Assign values to the NETRESOURCE structure.
		//
		nr.dwType = RESOURCETYPE_ANY;
		nr.lpLocalName = szLocalName;
		nr.lpRemoteName = szRemoteName;
		nr.lpProvider = NULL;

		//
		// Call the WNetAddConnection2 function to assign
		//   a drive letter to the share.
		//
		res = WNetAddConnection2(&nr, szPassword, szUserName, FALSE);

		//
		// If the call fails, inform the user; otherwise,
		//  return ok.
		//
		if(res != NO_ERROR)
			return FailWithMsg(E_FAIL, StdStringFormat("Error: %ld for connection %s\n",  res,szRemoteName));
		return S_OK;
	}
	//Usage: NetShareGetInfo sharename <servername>
	HRESULT  NetShareGetInfoFunction( LPTSTR   lpszServer , LPTSTR lpszShare)
	{
		PSHARE_INFO_502 BufPtr;
		NET_API_STATUS res;
		HRESULT hr=S_OK;
		//
		// Call the NetShareGetInfo function, specifying level 502.
		//
		if((res = NetShareGetInfo (_bstr_t(lpszServer),_bstr_t(lpszShare),502,(LPBYTE *) &BufPtr)) == ERROR_SUCCESS)
		{
			//
			// Print the retrieved data.
			//
			FailWithMsg(S_OK, StdStringFormat("%S\t%S\t%u\n",BufPtr->shi502_netname, BufPtr->shi502_path, BufPtr->shi502_current_uses));
			//
			// Validate the value of the 
			//  shi502_security_descriptor member.
			//
			if (IsValidSecurityDescriptor(BufPtr->shi502_security_descriptor))
				hr=FailWithMsg(S_OK, "It has a valid Security Descriptor.\n");
			else
				hr=FailWithMsg(E_FAIL, "It does not have a valid Security Descriptor.\n");
			//
			// Free the allocated memory.
			//
			NetApiBufferFree(BufPtr);
		}
		else 
			hr=FailWithMsg(E_FAIL, StdStringFormat("Error: %ld\n",res));
		return hr;
	}
	// Add local share https://msdn.microsoft.com/en-us/library/windows/desktop/bb525384(v=vs.85).aspx 
	HRESULT NetUseWithCredentials()
	{
		// set netuseadd and error checking variables
		DWORD rc, error=0;

		// set the structure (use_info_2)
		USE_INFO_2 useinfo ;
		memset( &useinfo, '\0', sizeof u);

		//fill in the USER_INFO_2 for connection infor
		useinfo.ui2_local =  NULL;
		useinfo.ui2_remote = _bstr_t(sUNCPath);
		useinfo.ui2_username = _bstr_t(sUser);
		useinfo.ui2_domainname = _bstr_t(sDomain);
		useinfo.ui2_password = _bstr_t(sPassword);
		useinfo.ui2_asg_type = USE_DISKDEV;
		useinfo.ui2_usecount = 1;
		uint paramErrorIndex;
		// make the connection
		rc = NetUseAdd(NULL, 2 /*level*/, (byte *) &useinfo, &error);

		// check for errors (see win32 api error file)
		if (rc != ERROR_SUCCESS)
			return FailWithMsg(E_FAIL, StdStringFormat("Netuseadd() returned %lu (arge# = %lu)\n", rc, error));
		return S_OK;
	}
	/// <summary>
	/// Connects to a UNC path using the credentials supplied.
	/// </summary>
	/// <param name="UNCPath">Fully qualified domain name UNC path</param>
	/// <param name="User">A user with sufficient rights to access the path.</param>
	/// <param name="Domain">Domain of User.</param>
	/// <param name="Password">Password of User</param>
	/// <returns>S_OK if mapping succeeds.</returns>
	HRESULT NetUseWithCredentials(std::string UNCPath, std::string User, std::string Domain, std::string Password)
	{
		sUNCPath = UNCPath;
		sUser = User;
		sPassword = Password;
		sDomain = Domain;
		return NetUseWithCredentials();
	}
};