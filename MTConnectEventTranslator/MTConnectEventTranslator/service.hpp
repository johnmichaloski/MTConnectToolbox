#ifndef SERVICE_HPP
#define SERVICE_HPP


#define SHUT_RDWR SD_BOTH
typedef int socklen_t;
#define snprintf _snprintf
#define strdup _strdup
#define stricmp _stricmp
#define strdup _strdup

/* Internal types */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#define SERVICE_NAME_LEN 80

class MTConnectService {
public:
  MTConnectService();
  virtual int main(int aArgc, const char *aArgv[]);
  virtual void initialize(int aArgc, const char *aArgv[]) = 0;

  void setName(const char *aName);
  virtual void stop() = 0;
  virtual void start() = 0;
  virtual void logoff() {}
  virtual void logon() {}

  const char *name() { return mName; }
  
protected:
  char mName[SERVICE_NAME_LEN];
  bool mIsService;
  bool mDebug;
  
  void install(int argc, const char *argv[]);
  void remove();
};
//#if 1
//#ifdef WIN32
//class ServiceLogger : public Logger {
//public:
//  virtual void error(const char *aFormat, ...);
//  virtual void warning(const char *aFormat, ...);
//  virtual void info(const char *aFormat, ...);  
//  virtual void debug(const char *aFormat, ...);  
//
//protected:
//};
//#else
//class ServiceLogger : public Logger {};
//#endif
//#endif
#endif
