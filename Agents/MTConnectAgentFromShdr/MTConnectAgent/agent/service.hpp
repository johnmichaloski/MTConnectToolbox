#ifndef SERVICE_HPP
#define SERVICE_HPP

#include <string>
#include "dlib/logger.h"

#define NAME_LEN 80

class MTConnectService {
public:
  MTConnectService();
  virtual int main(int aArgc, const char *aArgv[]);
  virtual void initialize(int aArgc, const char *aArgv[]) = 0;

  void setName(const std::string &aName) { mName = aName; }
  virtual void stop() = 0;
  virtual void start() = 0;
  const std::string &name() { return mName; }
  
  // jlm added
  std::string &configfile() { return mConfigFile; }
  void install();
  bool mIsService;
  bool mIsDebug;

protected:
  std::string mName;
  std::string mConfigFile;
  std::string mPidFile;

  
  void remove();
  
#ifndef _WINDOWS
  void daemonize();  
#endif

};

#endif
