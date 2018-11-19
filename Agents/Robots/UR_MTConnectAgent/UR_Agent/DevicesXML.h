//

// DevicesXML.h
//

#pragma once
#include <string>
#include <vector>

/**
 * @brief The CDevicesXML class handles generation of devices.xml and agent.cfg
 * files. The device configuration file is very kludgy.
 */
class CDevicesXML
{
public:
    CDevicesXML(void);
    ~CDevicesXML(void);

    /**
     * @brief WriteDevicesFile
     * @param devicenames list devices
     * @param xml  is the
     * @param xmlFile name of device file
     * @param destFolder folder to store devices file
     * @return 0 if sucess.
     */
    static HRESULT     WriteDevicesFile (std::vector<std::string> devicenames,
                                         std::string              xml,
                                         std::string              xmlFile,
                                         std::string              destFolder);

    static LRESULT     WriteAgentCfgFile (std::string cfgfile,
                                          std::string xmlFile,
                                          std::string destFolder);

    static HRESULT     WriteDevicesFileHeader (std::string devicemodel,
                                               std::string xmlFile);

    /**
     * @brief ProbeDeviceXml returns an xml string of a template device.
     * Containing templated:
     * #### is substituted with device name, used for ids which must be unique.
     * NNNN is sustituted with the device name
     * @return xml string.
     */
    static std::string ProbeDeviceXml (void);

    /**
     * @brief WriteAgentCfgFile writes an MTConnect compliant agent.cfg file given
     * associated parameters.
     * @param ServerName name of service
     * @param HttpPort port for agent to listen for client http requests
     * @param cfgfile name of output agent.cfg file
     * @param xmlFile name of devices file (typically devices.xml)
     * @param destFolder folder iin which to store agent.cfg file
     * @return 0 for sucess.
     */
    static LRESULT     WriteAgentCfgFile (std::string ServerName,
                                          std::string HttpPort,
                                          std::string cfgfile,
                                          std::string xmlFile,
                                          std::string destFolder);

    /**
     * @brief ConfigureRobotDeviceXml given robot pameters constructs a robot
     * devices.xml file.
     * @param devicename name of device to use
     * @param jointnames names of joints to generate device data items
     * @param tagnames output of tagnames created for generation in adapter
         * @param conditionIds output of condition ids generated for adapter
     * device model
     * @return xml string containing #### to be substituted with device name for
     * ids.
     */
    static std::string ConfigureRobotDeviceXml (std::string                devicename,
                                                std::vector<std::string>   jointnames,
                                                std::vector<std::string> & tagnames,
                                                std::vector<std::string> & conditionIds);
};
