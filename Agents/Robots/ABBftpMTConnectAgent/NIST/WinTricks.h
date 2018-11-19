//
// WinTricks.h
//

#pragma once
#include "StdStringFcn.h"


/**
Map a network share directory you would use this command:

net use \\Server\ShareName

Then... if you want to access the network share through a location on your local C: drive, you'll want to set up a symbolic link:

mklink /d  C:\Folder\NewShareName \\Server\ShareName
 
Now when you navigate to C:\Folder\NewShareName you'll see the contents of \\Server\ShareName ...
*/
namespace WinTricks
{

	bool LogonAddNetworkResource(tstring remotename, tstring localname, tstring pw, tstring user);
	bool AddNetworkResource(tstring remotename, tstring localname, tstring pw, tstring user);
	bool NetworkResourceExists(tstring remotename, tstring localname, tstring pw, tstring user);
	DWORD MapNetworkDrive(tstring strRemoteName, tstring strLocalDrive, tstring strUseName, tstring strPassword);
	void SetFilePermission(tstring szFilename);
	bool  DriveExists(tstring drive);
	std::vector<tstring> GetLogicalDrives();
};