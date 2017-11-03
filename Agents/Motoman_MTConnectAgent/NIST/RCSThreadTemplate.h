//

// RCSThreadTemplate.h
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
#include "RCSTimer.h"
#include <boost/thread.hpp>

namespace RCS
{
/**
 * \brief Thread is an RCS ulapi equivalent for a timed thread.
 * Given a cycle time, the thread provides a wait function to sleep to exactly
 * the
 * amount of the thread cycle time. It keeps track of busy/idle time for
 * diagnostic
 * purposes.
 * <BR> Notes:
 * https://www.quantnet.com/threads/c-multithreading-in-boost.10028/
 */
    class Thread
    {
public:

        /*!
         * \brief Constructor of thread, that takes cycle time as input.
         */
        Thread(double cycletime) : _timer(cycletime)
        {
            _bThread    = true;
            _cycletime  = cycletime;
            _debugLevel = 99;
            _bDone      = false;
        }

        /*!
         * \brief Destructor of thread, makes sure thread has stopped.
         */
        ~Thread( ) { Stop( ); }

        /*!
         * \brief ThreadGroup is a static definition of boost thread group.
         */
        static boost::thread_group & ThreadGroup ( )
        {
            static boost::thread_group _group;

            return _group;
        }

        /*!
         * \brief Threads is a static definition of all the threads that have been
         * created.
         */
        static std::vector<Thread *> & Threads ( )
        {
            static std::vector<Thread *> _group;

            return _group;
        }

        /*!
         * \brief Name returns name of thread.
         */
        std::string & Name ( ) { return _name; }

        /*!
         * \brief Uses boost thread join routine.
         */
        void Join ( ) { return m_thread.join( ); }

        /*!
         * \brief Init function called before Action() loop.
         */
        virtual void Init ( ) { }

        /*!
         * \brief Cleanup function called after Action() loop done.
         */
        virtual void Cleanup ( ) { }

        /*!
         * \brief Action override function called every cycle.
         */
        virtual int Action ( ) { }

        /*!
         * \brief Start starts the thread which call Init(), and then does Action()
         * loop.
         */
        void Start ( )
        {
            _bThread = true;
            Threads( ).push_back(this);

            // m_thread = boost::thread(boost::bind(&Thread::Cycle, this));
            ThreadGroup( ).create_thread(boost::bind(&Thread::Cycle, this));
        }

        /*!
         * \brief Static StopAll which stops all the threads created in the boost
         * thread group.
         */
        static void StopAll ( )
        {
            for ( int i = 0; i < Threads( ).size( ); i++ )
            {
                std::cout << "Stop Thread = " << Threads( ).at(i)->Name( ).c_str( )
                          << std::endl;
                Threads( ).at(i)->Stop( );
            }

            ThreadGroup( ).join_all( );
        }

        static void SuspendAll ( )
        {
            for ( int i = 0; i < Threads( ).size( ); i++ )
            {
                std::cout << "Suspend Thread = " << Threads( ).at(i)->Name( ).c_str( )
                          << std::endl;
                Threads( ).at(i)->Suspend( );
            }
        }

        static void ResumeAll ( )
        {
            for ( int i = 0; i < Threads( ).size( ); i++ )
            {
                std::cout << "Suspend Thread = " << Threads( ).at(i)->Name( ).c_str( )
                          << std::endl;
                Threads( ).at(i)->Resume( );
            }
        }

        /*!
         * \brief Stop stops the thread loop.
         * \param bWait indicates whether to wait until thread has finished.
         */
        void Stop (bool bWait = false)
        {
            _bThread = false;

            // wait until done
            //            while (bWait && !_bDone)
            //                boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        }

        /*!
         * \brief Suspend stops the thread loop until restarted with Resume().
         */
        void Suspend ( ) { _timer.suspend( ); }

        /*!
         * \brief Resume resume execution of the thread loop stopped with Suspend().
         */
        void Resume ( ) { _timer.resume( ); }

        /*!
         * \brief Load returns the load of the thread cycle.
         */
        double Load ( ) { return _timer.load( ); }

        /*!
         * \brief CycleTime returns the cycle time of the thread cycle in seconds.
         * \return double       returns cycle time of thread in seconds.
         */
        double & CycleTime ( ) { return _cycletime; }

        /*!
         * \brief SetDebugLevel sets the debugging level of the thread.
         * \param int       specified debug level, as an integer.
         */
        void SetDebugLevel (int n) { _debugLevel = n; }

        /*!
         * \brief DebugLevel returns the debugging level of the thread.
         * \return int       returns debug dlvel of thread.
         */
        int & DebugLevel ( ) { return _debugLevel; }

        /*!
         * \brief Cycle is the thread main function. It calls init, action, and
         * cleanup.
         * After each cycle waits exa_newccctly amount given by cycle time.
         */
        void Cycle ( )
        {
            _bDone = false;
            Init( );
            _timer.sync( );

            try
            {
                while ( _bThread )
                {
                    _bThread = (bool) Action( );
                    _timer.wait( );                        // is this polling waiting or sleeping/yielding?
                }
            }
            catch ( ... )
            {
                std::cout << "Unhandled exception - " << Name( ).c_str( ) << std::endl;
            }
            Cleanup( );
            _bDone = true;
        }

protected:
        std::string _name;                                 /**< name of thread */
        double _cycletime;                                 /**< cycletime of thread in seconds */
        int _debugLevel;                                   /**< debug level of thread */
        bool _bThread;                                     /**< boolean loop thread */
        bool _bDone;                                       /**< boolean indicating whether thread has finished */
        RCS::Timer
            _timer;                                        /**< RCS timer for coordinating wait and duration of thread */
        boost::thread m_thread;                            /**< boost thread*/
    };
}
