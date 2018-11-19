//
// RunProcess.h
/

#pragma once


inline void RunSynchronousProcess(std::string cmd, std::string args, int timeout=4000)
{
	STARTUPINFO         siStartupInfo;
	PROCESS_INFORMATION piProcessInfo;

	memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	memset(&piProcessInfo, 0, sizeof(piProcessInfo));

	siStartupInfo.cb = sizeof(siStartupInfo);

	if(CreateProcess(cmd.c_str(),     // Application name
		(LPSTR) args.c_str(),                 // Application arguments
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
	}
}
