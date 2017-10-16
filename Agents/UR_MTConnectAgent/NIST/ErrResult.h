#pragma once

#include <boost/filesystem.hpp>
#include <string>

//

/**
 * @brief The E class in conjunction with a C++ preprocessor macro
 * is a class that throws a run_time expection, if an HRESULT failed.
 * So, it checks every integer result and throws an error string if the result
 * is
 * not zero (or S_OK in MSVC).
 *
 * This is useful as it is often hard to check each line of code for an error.
 * This can be especially helful if you are in a debugger that can break on
 * expections.
 */
class E
{
public:

    /**
     * @brief E constructor that builds the basis of the error string given
     * programming line of code, the filename and the line number in the file.
     * @param error line of program with error.
     * @param file_name full path of file with error
     * @param line_number line number in file of error
     */
    E(char *error, char *file_name, int line_number)
    {
        // char buffer[128];
        error_str  = error;
        error_str += "@";
        error_str += ExtractFilename(file_name);
        error_str += strprintf<int>(":%d", line_number);
    }

    /**
     * @brief operator = will throw a full error string if the equals is not
     * S_OK(0).
     * @param hr the result to test.
     */
    void operator = (HRESULT hr)
    {
        if ( hr != S_OK )
        {
            std::string myerror = strprintf<HRESULT>("%x:", hr) + error_str;
#ifdef _DEBUG
            OutputDebugString(myerror.c_str( ));
            assert(0);
#else
            throw std::runtime_error(myerror_str.c_str( ));
#endif
        }
    }

protected:
    std::string error_str;                                 // !< full error string

    /**
     * @brief strprintf bogus way of printing out a number.
     * Note you can add extra chacters before and after the number format.
     * @param format the format with which to format the number
     * @param t instance of type t to convert into string
     * @return string containing converted number
     */
    template<typename T>
    std::string strprintf (char *format, T t)
    {
        char buffer[128];

        snprintf(buffer, 128, format, t);
        return buffer;
    }

    /**
     * @brief ExtractFilename parse a WINDOWS file path to find file.
     * No error checking.
     * @param path full path to a file
     * @return just the filename with extension as string
     */
    std::string ExtractFilename (const std::string & path)
    {
        return path.substr(
            path.find_last_of(boost::filesystem::path::preferred_separator) + 1);
    }
};
#define EF(X)    E(# X, __FILE__, __LINE__) = X
