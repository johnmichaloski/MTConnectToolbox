//

// NikonAdapter.h
//

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#pragma once
#include <string>
#include "Config.h"
#include "NikonAgent.h"

#include "StdStringFcn.h"
#include "Config.h"
#include "AppEventLog.h"

using namespace crp;

class AgentConfigurationEx;
class Device;

class AdapterT
{
public:
  AdapterT(AgentConfigurationEx *mtcagent, // mtconnect agent
    crp::Config &                config,
    std::string                  machine,  // ip address or pc name
    std::string                  device);
  ~AdapterT(void);
  void            SetMTCTagValue (std::string tag, std::string value);
  void Disconnect ( ) { }
  void Stop ( ) { _mRunning = false; }
  virtual void Cycle ( ) { }
  virtual void    Config ( ) = 0;
  virtual HRESULT GatherDeviceData ( ) { return E_NOTIMPL; }

  virtual void    Off ( );
  virtual void    On ( );

  virtual HRESULT ErrorMessage (std::string errmsg);
  virtual HRESULT DebugMessage (std::string errmsg);

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

    Item *FindTag (std::string name)
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

    // This only updates the local copy, not the MTConnect agent copy
    void SetTag (std::string name, std::string value)
    {
      for ( int i = 0; i < this->size( ); i++ )
      {
        if ( at(i)->_tagname == name )
        {
          at(i)->_value = value;
          return;
        }
      }
    }

    std::string GetTag (std::string name, std::string defaultVal)
    {
      for ( int i = 0; i < this->size( ); i++ )
      {
        if ( at(i)->_tagname == name )
        {
          return at(i)->_value;
        }
      }
      return defaultVal;
    }

    std::string GetSymbolEnumValue (std::string szEnumTag, std::string defaultVal)
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

  // ///////////////////////////////////////////
  static int            mDebug;
  int                   _nServerRate;
  bool                  _mRunning;
  int                   _nQueryServerPeriod;
  crp::Config &         _config;
  std::string           _device;
  AgentConfigurationEx *_agentconfig;
  std::string           _status;
  EventLogging          eventlog;
  std::string           inifile;
};

class NikonAdapter : public AdapterT
{
public:
  NikonAdapter(AgentConfigurationEx *mtcagent,        // mtconnect agent
    crp::Config &                    _config,

    // std::string machine,  // ip address or pc name
    std::string                      device) : AdapterT(mtcagent, config, "127.0.0.1", device), config(_config)
  {
    _lastfilesize = 0;
    unc           = File.ExeDirectory( ) + "ProductionLog.csv";
    lastmodtime   = COleDateTime(0, 0, 0, 0, 0, 0);
  }

  void            CreateItem (std::string tag, std::string type = "Event");
  virtual void    Cycle ( );
  virtual void    Config ( );
  virtual HRESULT GatherDeviceData ( );
  virtual void    Dump ( );
  HRESULT         FailWithMsg (HRESULT hr, std::string errmsg);
  HRESULT         WarnWithMsg (HRESULT hr, std::string errmsg);

  // //////////////////////////////////////////////////////////////////////////
  crp::Config & config;
  size_t        _lastfilesize;
  std::string   unc;
  int           _simulation;
  COleDateTime  LastLogDate;
  COleDateTime  lastmodtime;
  std::string   _User;
  std::string   _Password;
  std::string   _LocalShare;
  std::string   _NetworkShare;
  bool			_bNotProductionMode;
};
