
//
// UR_Adapter.h
//

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#pragma once
#include "UR_Agent.h"
#include "UrInterface.h"
#include "ur_communication.h"
#include <string>

#include "NIST/AppEventLog.h"
#include "NIST/Config.h"
#include "NIST/Config.h"
#include "NIST/RCSMsgQueue.h"
#include "NIST/StdStringFcn.h"

class AgentConfigurationEx;
class Device;

/**
 * @brief The AdapterT class contains variables and methods
 * expected in all adapters. Many of the variable are
 * pure virtual function (or abstract function) in C++,
 * which is a virtual function for which there is no implementation yet,
 * it is only declared. So technically, since not all methods
 * are an abstract function, the AdapterT is not interface in the
 * Microsoft MIDL sense. Instead, many common functions are
 * implemented that would be shared across adapters, yet all
 * are virtual so an implementing adapter could override if necessary.
 */
class AdapterT
{
public:

    /**
   * @brief AdapterT constructor that requires agent parent pointer, reference to
   * config file,
   * ip of machine, and device name
   * @param mtcagent parent agent pointer
   * @param config reference to config file used by parent agent.
   * @param device name of the device (must not have blanks in name)
   */
    AdapterT(AgentConfigurationEx *mtcagent,
             Nist::Config &        config,
             std::string           device);

    /**
   * @brief setMTCTagValue set the tag in the agent to a value
   * Uses parent agent and device name for pointers into agent.
   * @param tag tagname as string
   * @param value as string.
   */
    void         setMTCTagValue (std::string tag, std::string value);

    /**
  * @brief addUrdfAsset passes URDF xml as an MTConnect asset named robot
  * The asset name depends on the robot name which has to be fixed in the routine.
  */
    void         addUrdfAsset (std::string urdfstr);

    /**
   * @brief doDisconnect disconnects adatper from device
   */
    void doDisconnect ( ) { }

    /**
   * @brief doStop corresponds to MTC Institute agent stop method.Stops this
   * adapter.
   * Each different variety of adapter typically has a different method and length
   * of
   * time to stop.
   */
    virtual void doStop ( );

    /**
   * @brief doCycle abstract class all instance of adapter must implement
   */
    virtual void doCycle ( ) = 0;

    /**
   * @brief doConfig abstract class all instance of adapter must implement
   */
    virtual void doConfig ( ) = 0;

    /**
   * @brief gatherDeviceData abstract class all instance of adapter must implement
   * @return E_NOTIMPL/
   */
    virtual HRESULT gatherDeviceData ( ) { return E_NOTIMPL; }

    /**
   * @brief resetOff turns off all tag names.
   */
    virtual void resetOff ( );

    /**
   * @brief resetOn turns on avail and power.
   */
    virtual void resetOn ( );

    /**
   * @brief The Item struct contains a simple struct for data items
   */
    struct Item
    {
        /**
         * @brief mAliasd device tagname as opposed to MTConnect tagname
         * (e.g., MTConnect tagname, and OPC tagname, where OPC tagname is alias.)
         */
        std::string mAlias;
        std::string mTagname;                              // !< name in devices.xml to match
        std::string mType;                                 // !< type either event or sample
        std::string mSubtype;                              // !< should be actual or target
        std::string mValue;                                // !< value of tag
        std::string mLastvalue;                            // !< last value to see if it should be updated
    };

    /**
   * @brief The ItemsType struct is the list of all tag handled by this adapter.
   */
    struct ItemsType : public std::vector<Item *>
    {
        /**
     * @brief FindAlias find a matching item by alias name
     * @param name string containing alias
     * @return  item pointer or NULL
     */
        Item *findAlias (std::string name)
        {
            for ( size_t i = 0; i < this->size( ); i++ )
            {
                if ( at(i)->mAlias == name )
                {
                    return at(i);
                }
            }
            return NULL;
        }

        /**
         * @brief findTag find a matching item by tag name
         * @param name string containing tag name
         * @return  item pointer or NULL
         */
        Item *findTag (std::string name)
        {
            for ( size_t i = 0; i < this->size( ); i++ )
            {
                Item *item = at(i);

                if ( at(i)->mTagname == name )
                {
                    return item;
                }
            }
            return NULL;
        }

        /**
         * @brief setTag updates the local item copy, not the MTConnect agent copy.
         * Note if not found, ignored. No error.
         * @param name string containing tag name
         * @param value to assign to item given by tagname.
         */
        void setTag (std::string name, std::string value)
        {
            for ( size_t i = 0; i < this->size( ); i++ )
            {
                if ( at(i)->mTagname == name )
                {
                    at(i)->mValue = value;
                    return;
                }
            }
        }

        /**
         * @brief getTag return the local copy of the tagname value
         * @param name string containing tag name
         * @param defaultVal default value if tagname not found
         * @return string containing value
         */
        std::string getTag (std::string name, std::string defaultVal)
        {
            for ( size_t i = 0; i < this->size( ); i++ )
            {
                if ( at(i)->mTagname == name )
                {
                    return at(i)->mValue;
                }
            }
            return defaultVal;
        }

        /**
         * @brief getSymbolEnumValue returns a new enumerated value for a tag.value
         * pair.
         * For example: execution.INTERRUPTED becomes execution.PAUSE where
         * execution is
         * ths tag, and INTERRUPTED and PAUSED are the enumeration values.
         * @param szEnumTag enumerated tag: tagname.value concatenation
         * @param defaultVal default  if enumerated tagname not found
         * @return new tagname value or default if not found
         */
        std::string getSymbolEnumValue (std::string szEnumTag,
                                        std::string defaultVal)
        {
            for ( size_t i = 0; i < this->size( ); i++ )
            {
                if ( at(i)->mType != "Enum" )
                {
                    continue;
                }

                if ( at(i)->mAlias != szEnumTag )
                {
                    return at(i)->mValue;
                }
            }
            return defaultVal;
        }
    }
    items;

    // ///////////////////////////////////////////
    int mServerRate;                                       // !< normal rate of adapter (approximate)
    bool mRunning;                                         // !< adapter running thread flag
    int mQueryServerPeriod;                                // !< querying device for connection time wait
    Nist::Config & mConfig;                                // !< configuration file passed as reference
    std::string
        mDevice;                                           // !<  adapter name for device (should not have any spaces)
    AgentConfigurationEx *mAgentconfig;                    // !< pointer to parent agent
    std::vector<std::string> mTagnames;                    // !< list of tagnames for this adapter
};

class UR_Adapter : public AdapterT
{
public:
    UR_Adapter(AgentConfigurationEx *mtcagent,
               Nist::Config &        config,
               std::string           device)
        : AdapterT(mtcagent, config, device), mConfig(config) { }

    /**
     * @brief CreateItem creates item in tagname list.
     * @param tag name
     * @param type either sample or event (both handled same).
     */
    void            createItem (std::string tag, std::string type = "Event");

    /**
     * @brief doCycle is the thread routine
     */
    virtual void    doCycle ( );

    /**
     * @brief doConfig reads adapter specific configuration parameters
     */
    virtual void    doConfig ( );

    /**
     * @brief gatherDeviceData gets latest tag values
     * @return 0 if sucessful, fail if disconnected
     */
    virtual HRESULT gatherDeviceData ( );

    /**
     * @brief doStop is called when agent service (or app) stops.
     */
    virtual void    doStop ( );

    // Methods for CSV file generation based on tags
    std::string     dumpHeader ( );
    std::string     dumpDataItems ( );

    ////////////////////////////////////////////////////////////////////////////
    Nist::Config & mConfig;                                // !< refernece to configuration file
    std::string mIp;                                       // !< ip of the ur robot controller.
    size_t mCycleCnt;                                      // !< cycle count of messages. First is version.
    std::vector<std::string> mJointnames;                  // !< lists of my joint names
    std::string mUrdfFile;                                 // !< filename path of urdf

    // UR message handling - use mutex queue for communication
    typedef std::vector<uint8_t> msgbuffer_t;              // !< type of ur message vector
    RCS::CMessageQueue<msgbuffer_t> mUrQMsgs;              // !<  mutexed message queue
    ur_communication mUrComm;                              // !< ur communication handler
	ur::ur_robot_state mUrData;                             // !< ur message and data decoding handler

 
#ifdef _DEBUG
    std::ofstream tagfile;                                 // !< output file stream to save tagname for csv file
#endif
};
