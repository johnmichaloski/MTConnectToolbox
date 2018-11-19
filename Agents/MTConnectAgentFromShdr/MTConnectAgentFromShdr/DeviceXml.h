//
// DeviceXml.h
//

#pragma once
#include <string>
#include <vector>

class CDeviceXml
{
public:
	CDeviceXml(void);
	~CDeviceXml(void);
	static std::string MTConnect_device;
	std::string WriteDevicesFileXML(std::vector<std::string> names, std::vector<std::string> types);
	HRESULT WriteAgentCfgFile(std::string cfgfile, 
		std::string xmlFile, 
		std::string destFolder, 
		std::string HttpPort,
		std::vector<std::string> names, 
		std::vector<std::string> type, 
		std::vector<std::string> ips, 
		std::vector<std::string> ports, 
		std::string ServiceName="MTConnectAgent" );
};

