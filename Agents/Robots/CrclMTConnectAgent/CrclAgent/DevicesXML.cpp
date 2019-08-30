
//
// DevicesXML.cpp
//

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#include "stdafx.h"
#include "DevicesXML.h"
#include "Globals.h"
#include "NIST/StdStringFcn.h"

static std::string GetTimeStamp ( )
{
	char       aBuffer[256];
	SYSTEMTIME st;

	GetSystemTime(&st);
	sprintf(aBuffer, "%4d-%02d-%02dT%02d:%02d:%02d", st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond);
	return aBuffer;
}
CDevicesXML::CDevicesXML(void) { }
CDevicesXML::~CDevicesXML(void) { }
std::string CDevicesXML::ConfigureRobotDeviceXml (std::string                devicename,
	std::vector<std::string>   jointnames,
	std::vector<std::string> & tagnames,
	std::vector<std::string> & conditionIds)
{
	// Save all  the tagname, need to creating mapping later
	tagnames.clear( );
	tagnames.push_back("avail");

	for ( size_t i = 0; i < jointnames.size( ); i++ )
	{
		tagnames.push_back(jointnames[i] + "_actpos");
	}
	tagnames.push_back("power");
	tagnames.push_back("controllermode");
	tagnames.push_back("execution");
	tagnames.push_back("heartbeat");
	tagnames.push_back("pose");
	tagnames.push_back("estop");
	conditionIds.push_back(devicename + "system");

	std::string crcl_device = 
		"			 <Device id=\"####id\" uuid=\"####xxx\" name=\"NNNNNN\">\n"
		"			<Description>Crcl</Description>\n"
		"			<DataItems>\n"
		"				<DataItem id=\"####avail\" name=\"avail\"  type=\"AVAILABILITY\" category=\"EVENT\"/>\n"
		"			</DataItems>\n"
		"			<Components>\n"
		"				<Axes id=\"####a1\" name=\"base\">\n"		
		"					<Components>\n"	;

	for ( size_t i = 0; i < jointnames.size( ); i++ )
	{
		crcl_device += StrFormat("\t\t\t\t\t\t<Rotary id=\"####%s_id\" name=\"%s\">\n", jointnames[i].c_str(), jointnames[i].c_str());
		crcl_device += "\t\t\t\t\t\t\t<DataItems>\n";
		crcl_device += StrFormat("\t\t\t\t\t\t\t\t<DataItem type=\"POSITION\" "
			"subType=\"ACTUAL\" id=\"####_%s_actpos_id\" "
			"category=\"SAMPLE\" name=\"%s_actpos\"  "
			"units=\"RADIANS\" />\n",
			jointnames[i].c_str( ), jointnames[i].c_str( ));
		crcl_device += "\t\t\t\t\t\t\t</DataItems>\n"
			"\t\t\t\t\t\t</Rotary>\n";
	}
	crcl_device += 
		"					</Components>\n"
		"				</Axes>\n"
		"				<Controller id=\"####cont\" name=\"controller\">\n"
		"					<Components>\n"
		"						<Path id=\"####path1\" name=\"path\">\n"
		"							<DataItems>\n"
		"								<DataItem type=\"CODE\" id=\"####alarm\" category=\"EVENT\" name=\"alarm\"/>\n"
		"								<DataItem type=\"EXECUTION\" id=\"####exec\" category=\"EVENT\" name=\"execution\"/>\n"
		"								<DataItem type=\"CONTROLLER_MODE\" id=\"####mode\" category=\"EVENT\" name=\"controllermode\"/>\n"
		"								<DataItem type=\"PATH_POSITION\" id=\"####position\" category=\"SAMPLE\" name=\"pose\"  />\n"
		"        							<DataItem category=\"EVENT\"  id=\"####alarm\"  name=\"alarm\" type=\"CODE\"></DataItem>\n"
		"        							<DataItem category=\"EVENT\"  id=\"####heartbeat\"  name=\"heartbeat\" type=\"CODE\"></DataItem>\n"
		"        							<DataItem category=\"EVENT\"  id=\"####last_update\"  name=\"last_update\" type=\"CODE\"></DataItem>\n"
		" 							</DataItems>\n"
		"						</Path>\n"
		"					</Components>\n"
		"				</Controller>\n"
		"				<Systems id=\"####systems\" name=\"systems\">\n"
		"					<Components>\n"
		"						<Electric name=\"electric\" id=\"####elec\">\n"
		"							<DataItems>\n"
		"								<DataItem type=\"POWER_STATE\" category=\"EVENT\" id=\"####pwr\" name=\"power\" />\n"
		"							</DataItems>\n"
		"						</Electric>\n"
		"					</Components>\n"
		"				</Systems>\n"
		"			</Components>\n"
		"		</Device>\n";
	return crcl_device;
}
std::string CDevicesXML::ProbeDeviceXml (void)
{
	std::string crcl_device = 
		"			 <Device id=\"####id\" uuid=\"####xxx\" name=\"NNNNNN\">\n"
		"			<Description>Crcl</Description>\n"
		"			<DataItems>\n"
		"				<DataItem id=\"####avail\" name=\"avail\"  type=\"AVAILABILITY\" category=\"EVENT\"/>\n"
		"			</DataItems>\n"
		"			<Components>\n"
		"				<Axes id=\"####a1\" name=\"base\">\n"		
		"					<Components>\n"	;
	/* for ( size_t i = 0; i < jointnames.size( ); i++ )
	{
	crcl_device += StrFormat("\t\t\t\t\t\t<Rotary id=\"####d1\" name=\"JOINT%d\">\n", i);
	crcl_device += "\t\t\t\t\t\t\t<DataItems>\n";
	crcl_device += StrFormat("\t\t\t\t\t\t\t\t<DataItem type=\"POSITION\" "
	"subType=\"ACTUAL\" id=\"####_%s_actpos_id\" "
	"category=\"SAMPLE\" name=\"%s_actpos\"  "
	"units=\"RADIANS\" />\n",
	jointnames[i].c_str( ), jointnames[i].c_str( ));
	crcl_device += "\t\t\t\t\t\t\t</DataItems>\n"
	"\t\t\t\t\t\t</Rotary>\n";
	}*/
	crcl_device += 
		"					</Components>\n"
		"				</Axes>\n"
		"				<Controller id=\"####cont\" name=\"controller\">\n"
		"					<Components>\n"
		"						<Path id=\"####path1\" name=\"path\">\n"
		"							<DataItems>\n"
		"								<DataItem type=\"CODE\" id=\"####alarm\" category=\"EVENT\" name=\"alarm\"/>\n"
		"								<DataItem type=\"PROGRAM\" id=\"####pgm\" category=\"EVENT\" name=\"program\"/>\n"
		"								<DataItem type=\"EXECUTION\" id=\"####exec\" category=\"EVENT\" name=\"execution\"/>\n"
		"								<DataItem type=\"CONTROLLER_MODE\" id=\"####mode\" category=\"EVENT\" name=\"controllermode\"/>\n"
		"								<DataItem type=\"PATH_POSITION\" id=\"####position\" category=\"SAMPLE\" name=\"position\"  />\n"
		"								<DataItem type=\"PATH_POSITION\" id=\"####xorient\" category=\"SAMPLE\" name=\"xorient\"  />\n"
		"								<DataItem type=\"PATH_POSITION\" id=\"####zorient\" category=\"SAMPLE\" name=\"zorient\"  />\n"
		"        							<DataItem category=\"EVENT\"  id=\"####alarm\"  name=\"alarm\" type=\"CODE\"></DataItem>\n"
		"        							<DataItem category=\"EVENT\"  id=\"####heartbeat\"  name=\"heartbeat\" type=\"CODE\"></DataItem>\n"
		"        							<DataItem category=\"EVENT\"  id=\"####last_update\"  name=\"last_update\" type=\"CODE\"></DataItem>\n"
		" 							</DataItems>\n"
		"						</Path>\n"
		"					</Components>\n"
		"				</Controller>\n"
		"				<Systems id=\"####systems\" name=\"systems\">\n"
		"					<Components>\n"
		"						<Electric name=\"electric\" id=\"####elec\">\n"
		"							<DataItems>\n"
		"								<DataItem type=\"POWER_STATE\" category=\"EVENT\" id=\"####pwr\" name=\"power\" />\n"
		"							</DataItems>\n"
		"						</Electric>\n"
		"					</Components>\n"
		"				</Systems>\n"
		"			</Components>\n"
		"		</Device>\n";
	return crcl_device;
}
LRESULT CDevicesXML::WriteAgentCfgFile (std::string ServerName,
	std::string HttpPort,
	std::string cfgfile, std::string xmlFile,
	std::string destFolder)
{
	// Generate agent.cfg file with all devices in it.
	std::string cfg;

	cfg  = "Devices = " + xmlFile + "\n";
	cfg += "ServiceName = " + ServerName + "\n";           // MTConnectAgent\n";
	cfg += "Port = " + HttpPort + "\n";

	cfg += "CheckpointFrequency=10000\n";
	cfg += "AllowPut=true\n";

	cfg += "Adapters \n{\n";
	cfg += "}\n";

	cfg += "# Logger Configuration\n";
	cfg += "logger_config\n";
	cfg += "{\n";
	cfg += "\tlogging_level = debug\n";
	cfg += "\toutput = cout\n";
	cfg += "}\n";

	if ( !cfgfile.empty( ) )
	{
		WriteFile(destFolder + cfgfile, cfg);
	}
	return 0;
}
HRESULT CDevicesXML::WriteDevicesFile (std::vector<std::string> devicenames,
	std::string xml, std::string xmlFile,
	std::string destFolder)
{
	std::string contents;

	contents += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	contents += "<MTConnectDevices "
		"xmlns=\"urn:mtconnect.org:MTConnectDevices:1.1\" "
		"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
		"xsi:schemaLocation=\"urn:mtconnect.org:MTConnectDevices:1.1 "
		"http://www.mtconnect.org/schemas/MTConnectDevices_1.1.xsd\">\n";
	contents += StrFormat("<Header bufferSize=\"130000\" instanceId=\"1\" "
		"creationTime=\"%s\" sender=\"local\" "
		"version=\"1.1\"/>\n",
		GetTimeStamp( ).c_str( ));
	contents += "<Devices>\n";

	// Check all machine names unique

	// Generate Devices.xml file with all devices in it.
	for ( UINT i = 0; i < devicenames.size( ); i++ )
	{
		std::string config = xml;
		ReplaceAll(config, "####", devicenames[i]);
		config = ReplaceOnce(config, "name=\"NNNNNN\"",
			"name=\"" + devicenames[i] + "\"");
		contents += config + "\n";
	}
	contents += "</Devices>\n";
	contents += "</MTConnectDevices>\n";

	if ( !xmlFile.empty( ) )
	{
		WriteFile(destFolder + "\\" + xmlFile, contents);
	}
	return 0;
}
HRESULT CDevicesXML::WriteDevicesFileHeader (std::string devicemodel,
	std::string xmlFile)
{
	std::string contents;

	contents += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	contents += "<MTConnectDevices "
		"xmlns=\"urn:mtconnect.org:MTConnectDevices:1.1\" "
		"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
		"xsi:schemaLocation=\"urn:mtconnect.org:MTConnectDevices:1.1 "
		"http://www.mtconnect.org/schemas/MTConnectDevices_1.1.xsd\">\n";
	contents += StrFormat("<Header bufferSize=\"130000\" instanceId=\"1\" "
		"creationTime=\"%s\" sender=\"local\" "
		"version=\"1.1\"/>\n",
		GetTimeStamp( ).c_str( ));
	contents += "<Devices>\n";
	contents += devicemodel;
	contents += "</Devices>\n";
	contents += "</MTConnectDevices>\n";

	if ( !xmlFile.empty( ) )
	{
		WriteFile(xmlFile, contents);
	}
	DWORD filesize;

	if ( FAILED(File.Size(xmlFile, filesize)) )
	{
		logError("Devices xml file write failed\n");
	}
	return 0;
}
LRESULT CDevicesXML::WriteAgentCfgFile (std::string cfgfile,
	std::string xmlFile,
	std::string destFolder)
{
	// Generate agent.cfg file with all devices in it.
	std::string cfg;

	cfg  = "Devices = " + xmlFile + "\n";
	cfg += "ServiceName = " + Globals.mServerName + "\n";  // MTConnectAgent\n";
	cfg += "Port = " + Globals.mHttpPort + "\n";

	cfg += "CheckpointFrequency=10000\n";
	cfg += "AllowPut=true\n";

	cfg += "Adapters \n{\n";

	// for(UINT i=0; i<devices.size(); i++)
	// {
	//	if(ports[i]!="0")
	//	{
	//		cfg+="\t"+names[i] + "\n";
	//		cfg+="\t{\n";
	//		cfg+="\t\tHost = " + ips[i] +  "\n";
	//		cfg+="\t\tPort = " + ports[i] + "\n";
	//		cfg+="\t\tDeviceXml = " + ExtractFiletitle(devices[i]) + "\n";
	//		cfg+="\t}\n";
	//	}
	// }
	cfg += "}\n";

	cfg += "# Logger Configuration\n";
	cfg += "logger_config\n";
	cfg += "{\n";
	cfg += "\tlogging_level = debug\n";
	cfg += "\toutput = cout\n";
	cfg += "}\n";

	if ( !cfgfile.empty( ) )
	{
		WriteFile(destFolder + cfgfile, cfg);
	}
	return 0;
}
