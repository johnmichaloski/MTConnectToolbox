//
// ProcessStarter.h
//

#ifndef _PROCESS_STARTER_H_
#define _PROCESS_STARTER_H_

#include "windows.h"
#include "winbase.h"


#include <string>

class ProcessStarter
{
public:
    ProcessStarter(const std::string& processPath, const std::string& arguments = "");
    PHANDLE GetCurrentUserToken();
    BOOL Run();
    
private:
    std::string processPath_;
    std::string arguments_;
};

#endif //_PROCESS_STARTER_H_