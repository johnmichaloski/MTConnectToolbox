//
// DevicesXML.h
//

#pragma once
#include <string>
#include <vector>

class CDevicesXML
{
public:
	CDevicesXML(void);
	~CDevicesXML(void);
	static HRESULT	WriteDevicesFile(std::vector<std::string> devicenames, std::string xml,  std::string xmlFile, std::string destFolder);
	static std::string ProbeDeviceXml(void);
	static LRESULT WriteAgentCfgFile(std::string ServerName, std::string HttpPort, std::string cfgfile, std::string xmlFile, std::string destFolder);
};

