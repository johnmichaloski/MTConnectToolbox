
//
// UR_Agent.h
//

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#pragma once
#include <map>
#include <vector>

// MS includes
#include "atlutil.h"
#include "wtypes.h"

// Boost includes
#include <boost\thread.hpp>

// MTConnect includes
#include "agent.hpp"
#include "config.hpp"

// NIST includes
#include "Globals.h"
#include "NIST/AppEventLog.h"
#include "NIST/Config.h"
#include "NIST/StdStringFcn.h"
using namespace Nist;

class UR_Adapter;

// extern  char * monitorHtml;
//
// class AgentEx : public Agent
// {
// public:
//	AgentEx()
//	{
//	}
//	virtual std::string handleExtensionCall(const std::string& call, const
// std::string& device)
//	{
//		//	::ReadFile( ::ExeDirectory() + "jsmtconnect.html",
// contents);
//		return monitorHtml ;
//	}
//
// };

/**
 * @brief The AgentConfigurationT class that all extension agents override.
 */
class AgentConfigurationT : public AgentConfiguration
{
public:
    std::map<std::string, std::vector<std::string> >
    mTagnames;                                             // !< map of tagnames per device
    std::map<std::string, std::vector<std::string> >
    mConditionTags;                                        // !< map of condition ids per device
    Nist::Config mConfig;                                  // !< ini configuration handler.
    boost::thread_group
        mGroup;                                            // !< thread pool to manage all the adapter threads.
    std::vector<std::string> mDevices;                     // !< names of the adapter devices

    AgentConfigurationT( ) { }

    /**
     * @brief initialize override of MTC Institute agent method initialize
     * @param aArgc command line argc
     * @param aArgv command line argv
     */
    virtual void initialize (int aArgc, const char *aArgv[])
    {
        AgentConfiguration::initialize(aArgc, aArgv);
    }

    /**
     * @brief main override of MTC Institute agent method main
     * @param aArgc command line argc
     * @param aArgv command line argv
     * @return
     */
    virtual int  main (int aArgc, const char *aArgv[]);

    /**
     * @brief start overridce of MTC Institute agent method start
     */
    virtual void start ( ) { AgentConfiguration::start( ); }

    /**
     * @brief stop overridce of MTC Institute agent method  stop
     */
    virtual void stop ( ) { AgentConfiguration::stop( ); }
};

class AgentConfigurationEx : public AgentConfigurationT
{
public:
    std::vector<UR_Adapter *>
    mAdapterHandlers;                                      // !< list of adapter instances (to start,stop, etc).

    /**
     * @brief start sets up all adapter, starts their threads, and
     * then calls the MTC Institute agent method start
     */
    virtual void start ( );

    /**
     * @brief stop  stops all adapters, waits until their threads are done, and
     * then calls the MTC Institute agent method stop
     */
    virtual void stop ( );
    virtual void initialize (int aArgc, const char *aArgv[]);
    virtual int main (int argc, const char *argv[])
    {
        AgentConfigurationT::main(argc, argv);
        return 0;
    }

    // //////////////////////////////////////////////////////////////////
    bool         ResetAtMidnite ( );
    CWorkerThread<> _resetthread;
    struct CResetThread : public IWorkerThreadClient
    {
        HRESULT  Execute (DWORD_PTR dwParam, HANDLE hObject);
        HRESULT CloseHandle (HANDLE)
        {
            ::CloseHandle(_hTimer);
            return S_OK;
        }

        HANDLE _hTimer;
    }
    _ResetThread;
};
