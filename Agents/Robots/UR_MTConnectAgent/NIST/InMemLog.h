#ifndef _IN_MEM_LOG_H
#define _IN_MEM_LOG_H

#include "time.h"
#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "NIST/RCSTimer.h"

#ifndef timersub

/**
  * timersub is a posix preprocessor definition.
  * Copied here since not available in windows.
  * Useful for subtracting high precision timer values.
  */
#define timersub(a, b, result)                                 \
    do {                                                       \
        ( result )->tv_sec  = ( a )->tv_sec - ( b )->tv_sec;   \
        ( result )->tv_usec = ( a )->tv_usec - ( b )->tv_usec; \
        if ( ( result )->tv_usec < 0 ) {                       \
            --( result )->tv_sec;                              \
            ( result )->tv_usec += 1000000;                    \
        }                                                      \
    } while ( 0 )
#endif

/**
 * @brief The in_mem_log struct is a class that acts as a real-time logging
 * mechanism.
 * The class provides a stream buffer so save the logging (max 64K) and then
 * later when timing
 * is not critical, a user can save the log to a file. THe class also provides a
 * high resolution
 * timer that can provide timing of each log entry. Thus, with the timer you can
 * determine when
 * a log entry was posted that can be used to understand the timing of the
 * calling process.
 * Note, there is a posix in-memory FILE facility in which to write, but was not
 * supported in
 * Windows. So this was developed instead.
 */
struct in_mem_log
{
    struct timeval
    {
        long tv_sec;                                       /* seconds */
        long tv_usec;                                      /* and microseconds */
    };

    /**
     * @brief in_mem_log constructor.
     */
    in_mem_log( )
    {
        bIncrementTiming = false;
        counter          = 0;
        bufsz            = 65536;
        numchar          = 0;
        bTiming          = true;
        bCounter         = false;
    }

    /*!
    * \brief StrFormat  accepts a traditional C format string and expects
    * parameter to follow on calling stack and will
    * produce a string from it.
    * \param fmt is the C format string.
    */
    std::string       StrFormat (const char *fmt, ...)
    {
        va_list argptr;

        va_start(argptr, fmt);
        int         m;
        int         n = (int) strlen(fmt) + 1028;
        std::string tmp(n, '0');

        while ( ( m = vsnprintf(&tmp[0], n - 1, fmt, argptr) ) < 0 )
        {
            n = n + 1028;
            tmp.resize(n, '0');
        }

        va_end(argptr);
        return tmp.substr(0, m);
    }

    /**
     * @brief open opens the logging facility.
     * @param path of the file in which to write the contents of the streambuffer.
     */
    void              open (std::string path) { robotfname = path; }

    /**
     * @brief log append a timestamped entry to log
     * @param info string to append to streambuffer
     */
    void              log (std::string info)
    {
        RCS::Timer::setnow(&currentts);

        if ( counter == 0 )
        {
            lastts = currentts;
        }
        timersub(&currentts, &lastts, &diffts);

        if ( bIncrementTiming )
        {
            lastts = currentts;
        }
        std::string ts;

        if ( bCounter )
        {
            ts += StrFormat("%5d ", counter);
        }

        if ( bTiming )
        {
            ts += StrFormat("%d.%06d ", diffts.tv_sec, diffts.tv_usec);
        }
        buf << ts << info;                                 // << std::endl;
        counter++;
    }

    /**
     * @brief time_quantum return a float numer of the current difference
     * timestamp.
     * @return the float value combining seconds and microseconds.
     */
    float             time_quantum ( )
    {
        // std::string ts = StrFormat("%d.%06d", diffts.tv_sec, diffts.tv_usec);
        // return atof(ts.c_str());
        return (float) diffts.tv_sec + ( (float) diffts.tv_usec ) / 1.0E6;
    }

    /**
     * @brief close will write the stream buffer to file.
     */
    void              close ( )
    {
        FILE *fp = fopen(robotfname.c_str( ), "w");

        fwrite(buf.str( ).c_str( ), sizeof( char ), buf.str( ).size( ), fp);
        fclose(fp);
        buf.seekp(0);
    }

    //////////////////////////////////////////////

    /// Flags to control logging

    bool              bTiming;                             /**< boolean in which time increments are prepended to log
                   entries */
    bool              bIncrementTiming;                    /**< boolean in which incremental times are logged */
    bool              bCounter;                            /**< boolean in which counter are prepended first to log
                    entries  */
protected:
    struct timeval    lastts;
    struct timeval    currentts;
    struct timeval    diffts;
    int               counter;                             /**< used if counter is required */
    size_t            bufsz;                               /**< size of the buffer stream */
    size_t            numchar;
    std::string       robotfname;                          /**< full pathname of the log file */
    std::stringstream buf;                                 /**<  stream buffer of "inmemory" buffer **/
};

#endif                                                     // _IN_MEM_LOG_H
