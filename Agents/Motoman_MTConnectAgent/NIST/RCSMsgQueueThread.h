//

// MsgQueueThread.h
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
#include <RCSMsgQueue.h>
#include <boost/bind.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
namespace RCS
{
/**
 * \brief CMsgQueueThread is a message queued thread. It is activated when a
 * message is received.
 * <BR> Notes:
 * http://stackoverflow.com/questions/768351/complete-example-using-boostsignals-for-c-eventing
 */
    template<typename T>
    class CMsgQueueThread : public CMessageQueue<T>
    {
public:

        /*!
         * \brief Constructor of thread, that takes cycle time as input.
         */
        CMsgQueueThread( ) { }

        /*!
         * \brief Destructor of thread, makes sure thread has stopped.
         */
        ~CMsgQueueThread( ) { }

        virtual void AddMsgQueue (T t)
        {
            {
                boost::mutex::scoped_lock lock(m);
                CMessageQueue<T>::xml_msgs.push_back(t);
            }
            SigAction( );
        }

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
        static std::vector<CMsgQueueThread *> & Threads ( )
        {
            static std::vector<CMsgQueueThread *> _group;

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
        virtual int Action ( ) { return 0; }

        /*!
         * \brief Start starts the thread which call Init(), and then does Action()
         * loop.
         */
        void Start ( )
        {
            Threads( ).push_back(this);
            ThreadGroup( ).create_thread(boost::bind(&CMsgQueueThread<T>::Cycle, this));
            SigAction.connect(bind(&CMsgQueueThread<T>::Action, this));
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

        /*!
         * \brief Stop stops the thread, by notifying the wait condition.
         * \param bWait indicates whether to wait until thread has finished.
         */
        void Stop (bool bWait = false)
        {
            stopCond.notify_all( );
        }

        /*!
         * \brief Suspend stops the thread loop until restarted with Resume().
         */
        void Suspend ( ) { }

        /*!
         * \brief Resume resume execution of the thread loop stopped with Suspend().
         */
        void Resume ( ) { }

        /*!
         * \brief DebugLevel returns the debugging level of the thread.
         * \return int       returns debug dlvel of thread.
         */
        int & DebugLevel ( ) { return _debugLevel; }

        /*!
         * \brief Cycle is the thread main function. It calls init, action, and
         * cleanup.
         * After each cycle waits exactly amount given by cycle time.
         */
        void Cycle ( )
        {
            _bDone = false;
            Init( );
            boost::mutex::scoped_lock lock(stopMutex);
            try
            {
                stopCond.wait(lock);
                { }
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
        int _debugLevel;                                   /**< debug level of thread */
        bool _bDone;                                       /**< boolean indicating whether thread has finished */
        boost::thread m_thread;                            /**< boost thread*/
        boost::signals2::signal<int( )> SigAction;         /**< signals action method*/
        boost::mutex m;                                    /**< mutex for signal new message */
        boost::mutex stopMutex;                            /**< mutex for stopping */
        boost::condition stopCond;                         /**< condition for stopping */
    };
}
