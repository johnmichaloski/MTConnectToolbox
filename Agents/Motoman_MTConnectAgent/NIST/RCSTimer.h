//

// RCSTimer.h
//

/*
DISCLAIMER:
This software was produced by the National Institute of Standards
and Technology (NIST), an agency of the U.S. government, and by statute is
not subject to copyright in the United States.  Recipients of this software
assume all responsibility associated with its operation, modification,
maintenance, and subsequent redistribution.

See NIST Administration Manual 4.09.07 b and Appendix I.
*/

#pragma once
#ifndef #ifdef C11
#error Only C11 support for RCSTimer.h
#endif

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string.h>
#include <thread>

namespace RCS
{
    class Timer;
/* prototype for signal hander function */
    typedef int (*RCS_TIMERFUNC)(void *_arg);
/* sleeps # of seconds, to clock tick resolution */

// this will be a fraction of seconds
// Use: RCS::esleep(10.0);  // sleep ten seconds

    template<class Rep, class Period>
    double ToNanoseconds (std::chrono::duration<Rep, Period> d)
    {
        return static_cast<double>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(d).count( ) );
    }
    template<class Rep, class Period>
    double ToSeconds (std::chrono::duration<Rep, Period> d)
    {
        return static_cast<double>(
            std::chrono::duration_cast<std::chrono::seconds>(d).count( ) );
    }
/**
* \brief Timer is a general-purpose timer.
* The Timer is a general-purpose timer, which can be used for waiting until a
*synchronous time tick, sleep for any period at all, or to
* obtain a time in system clock ticks from creation of the timer.
*/
    class Timer
    {
public:

        /*!
        * \brief timeout is wait interval, rounded up to clock tick resolution;
        * function is external time base, if provided.
        * \param  timeout period.
        */
        Timer(double _timeout, RCS_TIMERFUNC _function = (RCS_TIMERFUNC) 0)
        {
            zero_timer( );

            if ( _timeout < clk_tck_val )
            {
                /* bump interval up to minimum system clock tick */
                timeout = clk_tck_val;
            }
            else
            {
                timeout = _timeout;
            }
            function = _function;
            idle     = 0.0;                                /* set accumulated idle time to 0.0 */
            counts   = 0;                                  /* set accumulated waits to 0 */

            last_time  = etime( );                         /* initialize start time and last time called	to
                             current time since epoch */
            start_time = etime( );                         /* set creation time to now */
            time_since_real_sleep
                = start_time;                              // boost::chrono::time_point_cast<boost::chrono::microseconds>(start_time);
                                                           // //  start_time;
            _bSuspend = false;
        }

        /*!
        * \brief return last sleep number of seconds to slept.
        * \return  last seconds (or fractions) last slept. -199.99 if unused.
        */
        static double & last_esleep_seconds_to_sleep ( )
        {
            static double last_esleep_seconds_to_sleep = -199.99;

            return last_esleep_seconds_to_sleep;
        }

        static int & etime_disabled ( )
        {
            static int etime_disabled = 0;

            return etime_disabled;
        }

        static double & etime_disable_time ( )
        {
            static double etime_disable_time = 0;

            return etime_disable_time;
        }

        /*!
        * \brief sleep number of seconds to sleep.
        * \param  seconds (or fractions) to sleep. Must be positive.
        */
        void esleep (double seconds_to_sleep)
        {
            Timer::last_esleep_seconds_to_sleep( ) = seconds_to_sleep;

            if ( seconds_to_sleep <= 0.0 )
            {
                return;
            }

            // convert seconds to nanoseconds
            double                   nanosec = seconds_to_sleep * 1E09;
            std::chrono::nanoseconds nanosleep((long long) nanosec);
            std::this_thread::sleep_for(nanosleep);
        }

        /*!
        * \brief number of seconds from some epoch, to clock tick resolution.
        * \return  high_resolution_clock now
        */
        std::chrono::high_resolution_clock::time_point etime ( )
        {
            return std::chrono::high_resolution_clock::now( );
        }

        /*!
        * \brief number of clock ticks per second using high resolution timer.
        * \return number of ticks per second.
        */
        double clk_tck ( )
        {
            return ( (double) std::chrono::high_resolution_clock::period::num )
                   / ( (double) std::chrono::high_resolution_clock::period::den );
        }

        /*!
        * \brief wait on synch; returns # of cycles missed.
        * \return # of cycles missed.
        */
        int wait ( )
        {
            // boost::chrono::high_resolution_clock::time_point interval;		/* interval
            // between this and last wakeup */
            double numcycles;                              /* interval, in units of timeout */
            int    missed    = 0;                          /* cycles missed */
            double remaining = 0.0;                        /* time remaining until timeout */

            std::chrono::high_resolution_clock::time_point
                time_in;                                   /* time wait() was entered */
            std::chrono::high_resolution_clock::time_point
                time_done;                                 /* time user function finished */

            if ( _bSuspend )
            {
                std::unique_lock<std::mutex> aLock(condMutex, std::try_to_lock);

                // std::mutex::scoped_lock aLock(condMutex);
                cWakeup.wait(aLock);
            }

            /* first call the user timing function, if any */
            if ( function != 0 )
            {
                /* set time in */
                time_in = etime( );

                if ( ( *function )(0) == -1 )
                {
                    return -1;                             /* fatal error in timing function */
                }
                time_done = etime( );
            }
            else
            {
                /* set time in, time done not used */
                time_in = etime( );
            }

            /* calculate the interval-- for user timing functions, this is how
            long between this wakeup and the last wakeup.  For internal timers,
            this is how long we need to sleep to make it to the next interval
            on time. */
            double interval = ToNanoseconds(time_in - last_time) / 1E9;
            numcycles = interval / timeout;

            /* synchronize and set last_time correctly; update idle time */
            counts++;

            if ( function != 0 )
            {
                last_time = time_done;
            }
            idle     += timeout > interval ? timeout - interval : 0;
            busy     += interval;
            missed    = (int) numcycles;
            remaining = timeout - interval;
            esleep(remaining);
            last_time = etime( );
            return missed;
        }

        /*!
        * \brief Returns % loading on timer, 0.0 means all waits, 1.0 means no time in
        * wait. This is average load.
        * \return double or -1 of time spent busy.
        */
        double load ( )
        {
            if ( counts * timeout > 1e-9 )
            {
                return busy / ( counts * timeout );
            }
            return -1.0;
        }

        /*!
        * \brief Compute free time over all cycles.
        */
        double free ( )
        {
            if ( counts * timeout > 1e-9 )
            {
                return idle / ( counts * timeout );
            }
            return -1.0;
        }

        /*!
        * \brief Synchronize the timing service.
        * Initialize start time and last time called to current time since epoch.
        */
        void sync ( )
        {
            last_time = etime( );                          /* initialize start time and last time called to
                            current time since epoch */
        }

        /*!
        * \brief Suspend the timing.
        */
        void suspend ( ) { _bSuspend = true; }

        /*!
        * \brief Resume the timing. Wakeup timer with boost conditional notify.
        */
        void resume ( )
        {
            _bSuspend = false;
            cWakeup.notify_all( );
        }

        std::string str_now_datetime ( )
        {
            using namespace std;
            using namespace std::chrono;
            system_clock::time_point tp = std::chrono::high_resolution_clock::now( );

            auto         ttime_t = system_clock::to_time_t(tp);
            auto         tp_sec  = system_clock::from_time_t(ttime_t);
            milliseconds ms      = duration_cast<milliseconds>(tp - tp_sec);

            std::tm *ttm = localtime(&ttime_t);

            char date_time_format[] = "%Y/%m/%d-%H:%M:%S";

            char time_str[] = "yyyy/mm/dd HH:MM:SS.fff";

            strftime(time_str, strlen(time_str), date_time_format, ttm);

            string result(time_str);
            result.append(".");
            result.append(to_string(ms.count( )));
            return result;
        }

        std::string str_now_time ( )
        {
            using namespace std;
            using namespace std::chrono;
            system_clock::time_point tp = std::chrono::high_resolution_clock::now( );

            auto         ttime_t = system_clock::to_time_t(tp);
            auto         tp_sec  = system_clock::from_time_t(ttime_t);
            milliseconds ms      = duration_cast<milliseconds>(tp - tp_sec);

            std::tm *ttm = localtime(&ttime_t);

            char date_time_format[] = "%H:%M:%S";

            char time_str[] = "HH:MM:SS.fff";

            strftime(time_str, strlen(time_str), date_time_format, ttm);

            string result(time_str);
            result.append(".");
            result.append(to_string(ms.count( )));
            return result;
        }

        static void setnow (struct timeval *tv)
        {
            auto now       = std::chrono::system_clock::now( );
            auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch( ));

            tv->tv_sec  = millisecs.count( ) / 1000;
            tv->tv_usec = ( millisecs.count( ) % 1000 ) * 1000;
        }

private:
        Timer & operator = (const Timer & _timer)
        {
            ( *this ) = _timer;
            return *this;
        }

        /*!
        * \brief zeroes out the class parameters.
        */
        void zero_timer ( )
        {
            function                = 0;
            busy                    = 0.0;
            idle                    = 0.0;                 /* set accumulated idle time to 0.0 */
            counts                  = 0;                   /* set accumulated waits to 0 */
            start_time              = etime( );            /* set creation time to now */
            time_since_real_sleep   = start_time;
            counts_per_real_sleep   = 0;
            counts_since_real_sleep = 0;
            clk_tck_val             = clk_tck( );
            timeout                 = clk_tck_val;
            _bSuspend               = false;
        }

        void set_timeout (double _timeout) { timeout = _timeout; }

        RCS_TIMERFUNC function;                            /**< copy of function */

        double timeout;                                    /**< copy of timeout value */
        std::chrono::high_resolution_clock::time_point start_time, last_time;
        std::chrono::high_resolution_clock::time_point time_since_real_sleep;
        std::condition_variable cWakeup;                   /**<  conditional value to wakeup timer */
        std::mutex condMutex;                              /**<  mutex on timer */

        bool _bSuspend;                                    /**< suspend timer wait flag */
        double clk_tck_val;                                /**< clock tick seconds*/
        double idle;                                       /**< accumulated idle time in seconds*/
        double busy;                                       /**< accumulated busy time in seconds*/
        int counts;                                        /**< accumulated wait cycles */
        int counts_since_real_sleep;                       /**< integer value of counts since last sleep*/
        int counts_per_real_sleep;                         /**< integer value of counts per sleep*/
    };
}
