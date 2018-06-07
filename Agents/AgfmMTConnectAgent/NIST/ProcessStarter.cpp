#include "ProcessStarter.h"

#include "userenv.h"
#include "wtsapi32.h"
#include "winnt.h"

ProcessStarter::ProcessStarter(const std::string& processPath, const std::string& arguments)
: processPath_(processPath), arguments_(arguments)
{

}

PHANDLE ProcessStarter::GetCurrentUserToken()
{
    PHANDLE currentToken = 0;
    PHANDLE primaryToken = 0;

    int dwSessionId = 0;
    PHANDLE hUserToken = 0;
    PHANDLE hTokenDup = 0;

    PWTS_SESSION_INFO pSessionInfo = 0;
    DWORD dwCount = 0;

    WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &dwCount);

    int dataSize = sizeof(WTS_SESSION_INFO);

    for (DWORD i = 0; i < dwCount; ++i)
    {
        WTS_SESSION_INFO si = pSessionInfo[i];
        if (WTSActive == si.State)
        {
            dwSessionId = si.SessionId;
            break;
        }
    }

    WTSFreeMemory(pSessionInfo);

    BOOL bRet = WTSQueryUserToken(dwSessionId, currentToken);
    int errorcode = GetLastError();
    if (bRet == false)
    {
        return 0;
    }

    bRet = DuplicateTokenEx(currentToken, TOKEN_ASSIGN_PRIMARY | TOKEN_ALL_ACCESS, 0, SecurityImpersonation, TokenPrimary, primaryToken);
    errorcode = GetLastError();
    if (bRet == false)
    {
        return 0;
    }

    return primaryToken;
}

BOOL ProcessStarter::Run()
{
    PHANDLE primaryToken = GetCurrentUserToken();
    if (primaryToken == 0)
    {
        return FALSE;
    }
    STARTUPINFO StartupInfo;
    PROCESS_INFORMATION processInfo;
    StartupInfo.cb = sizeof(STARTUPINFO);

    SECURITY_ATTRIBUTES Security1;
    SECURITY_ATTRIBUTES Security2;

    std::string command = "\"" + processPath_ + "\"";
    if (arguments_.length() != 0)
    {
        command += " " + arguments_;
    }

    void* lpEnvironment = NULL;
    BOOL resultEnv = CreateEnvironmentBlock(&lpEnvironment, primaryToken, FALSE);
    if (resultEnv == 0)
    {                                
        long nError = GetLastError();                                
    }

    BOOL result = CreateProcessAsUser(primaryToken, 
		0, (LPSTR)(command.c_str()), 
		&Security1, &Security2, FALSE, 
		CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT, 
		lpEnvironment, 0, 
		&StartupInfo, 
		&processInfo);

    DestroyEnvironmentBlock(lpEnvironment);
    CloseHandle(primaryToken);
    return result;
}