#define VERSION_MAJOR 1
#define VERSION_MINOR 3
#define VERSION_REVISION 0
#define VERSION_BUILD 57
#ifdef _UNICODE
#define _T(x)      L ## x
#else
#define _T(x)      x
#endif 
#define STRINGIZE2(s) _T(#s)
#define STRINGIZE(s) STRINGIZE2(s)
#define PRODUCTNAME     "MTConnect Agent\0"
#define PRODUCTVER      VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILD
#define FILEVER         PRODUCTVER
#define STRPRODUCTVER        STRINGIZE(VERSION_MAJOR)            \
                      _T(".") STRINGIZE(VERSION_MINOR)    \
                       _T(".") STRINGIZE(VERSION_REVISION) \
                        _T(".") STRINGIZE(VERSION_BUILD)    
#define STRFILEVER      STRPRODUCTVER
