//

// NikonAgent.h
//
#pragma once
#include <vector>
#include <map>
#include "StdStringFcn.h"
#include "Globals.h"
#include "atlutil.h"

class NikonAdapter;

#include "AppEventLog.h"
#include "wtypes.h"
#include "Config.h"
using namespace crp;
#include <boost\thread.hpp>
#include "agent.hpp"
#include "config.hpp"

// extern  char * monitorHtml;
//
// class AgentEx : public Agent
// {
// public:
//	AgentEx()
//	{
//	}
//	virtual std::string handleExtensionCall(const std::string& call, const std::string& device)
//	{
//		//	::ReadFile( ::ExeDirectory() + "jsmtconnect.html", contents);
//		return monitorHtml ;
//	}
//
// };

class AgentConfigurationT : public AgentConfiguration
{
public:
  std::string         contents;
  EventLogging        eventlog;
  std::string         tagdefs;
  crp::Config         config;
  boost::thread_group _group;

  std::vector<std::string> _devices;

  AgentConfigurationT( )
  { }

  struct Item
  {
    std::string _alias;
    std::string _tagname;
    std::string _type;
    std::string _subtype;
    std::string _value;
    std::string _lastvalue;
    VARENUM     vt;

    // Event * _event;
  };

  struct ItemsType : public std::vector<Item *>
  {
    Item *FindAlias (std::string name)
    {
      for ( int i = 0; i < this->size( ); i++ )
      {
        if ( at(i)->_alias == name )
        {
          return at(i);
        }
      }
      return NULL;
    }

    Item *FindDDETag (std::string name)
    {
      for ( int i = 0; i < this->size( ); i++ )
      {
        Item *item = at(i);

        if ( at(i)->_tagname == name )
        {
          return item;
        }
      }
      return NULL;
    }

    std::string GetSymbolValue (std::string szEnumTag, std::string defaultVal)
    {
      for ( int i = 0; i < this->size( ); i++ )
      {
        if ( at(i)->_type != "Enum" )
        {
          continue;
        }

        if ( at(i)->_alias != szEnumTag )
        {
          return at(i)->_value;
        }
      }
      return defaultVal;
    }
  }
  items;

  void WarningMsg (std::string err)
  {
    OutputDebugString(err.c_str( ) );

    if ( Globals.Debug > 3 )
    {
//			EventLogger.LogEvent(err);
    }
  }

  void ErrMsg (std::string err)
  {
    OutputDebugString(err.c_str( ) );

    if ( Globals.Debug > 0 )
    {
//			EventLogger.LogEvent(err);
    }
  }

  void AbortMsg (std::string err)
  {
    OutputDebugString(err.c_str( ) );

//		EventLogger.LogEvent(err);
//	    ExitProcess(-1);
    //  std::string cmd = StdStringFormat("cmd /c net stop \"%s\" ",  adapter->name().c_str());
    // taskmgr.Launch(cmd, 000);
  }

  // Cannot throw from here - can be called from within catch
  virtual void clear ( )
  {
    for ( int i = 0; i < items.size( ); i++ )
    {
      if ( ( items[i]->_type == _T("Event") ) || ( items[i]->_type == _T("Sample") ) )
      {
        // items[i]->_event->setValue("UNKNOWN");
      }
    }
  }

  virtual void initialize (int aArgc, const char *aArgv[]) { AgentConfiguration::initialize(aArgc, aArgv); }
  virtual int  main (int aArgc, const char *aArgv[]);
  virtual void start ( ) { AgentConfiguration::start( ); }
  virtual void stop ( ) { AgentConfiguration::stop( ); }

  virtual HRESULT ParseOptions ( ) { return S_OK; }
};

class AgentConfigurationEx : public AgentConfigurationT
{
public:
  std::vector<NikonAdapter *> _cncHandlers;

  virtual void start ( );
  virtual void stop ( );
  virtual void initialize (int aArgc, const char *aArgv[]);
  virtual int main (int argc, const char *argv[]) { AgentConfigurationT::main(argc, argv); return 0; }

  // //////////////////////////////////////////////////////////////////
  bool         ResetAtMidnite ( );
  CWorkerThread<>              _resetthread;
  struct CResetThread : public IWorkerThreadClient
  {
    HRESULT    Execute (DWORD_PTR dwParam, HANDLE hObject);
    HRESULT CloseHandle (HANDLE) { ::CloseHandle(_hTimer); return S_OK; }
    HANDLE _hTimer;
  }
  _ResetThread;
};
