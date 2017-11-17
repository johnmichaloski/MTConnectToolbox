#ifndef GLOBALS_H
#define GLOBALS_H
#include <QThread>
#include <QSettings>
#include <QTimer>
#include <iostream>



namespace Globals
{

inline std::string & ExeDirectory()
{
    static std::string MyExeDirectory;
    return MyExeDirectory;
}
/**
* \brief Format a std string like a printf statement.
    * \param format is the printf like format string. Uses stdargs.h.
* \return amount of time to delay in seconds given shdr.
*/

inline std::string StrFormat(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);

    int m;
    size_t n= strlen(format) + 1028;
    std::string tmp(n,'0');


    // Kind of a bogus way to insure that we don't
    // exceed the limit of our buffer
    while((m=_vsnprintf(&tmp[0], n-1, format, ap))<0)
    {
        n=n+1028;
        tmp.resize(n,'0');
    }
    va_end(ap);
    return tmp.substr(0,m);

}
/**
* \brief Trim whitespace from left side of string.
* \param str is the string to trim.
* \return trimmed string.
*/
inline std::string &LeftTrim(std::string &str)
{

    size_t startpos = str.find_first_not_of(" \t\r\n");
    if( std::string::npos != startpos )
        str = str.substr( startpos );
    return str;
}
/**
* \brief Trim whitespace from right side of string.
* \param str is the string to trim.
* \return trimmed string.
*/
inline std::string &RightTrim(std::string &str, std::string trim=" \t\r\n")
{
    size_t endpos = str.find_last_not_of(trim);
    if(std::string::npos != endpos )
        str = str.substr( 0, endpos+1 );
    return str;
}
/**
* \brief Trim whitespace from both sides of string.
* \param str is the string to trim.
* \return trimmed string.
*/
inline std::string &Trim(std::string &s)
{
    return LeftTrim(RightTrim(s));
}
};
#endif // GLOBALS_H
