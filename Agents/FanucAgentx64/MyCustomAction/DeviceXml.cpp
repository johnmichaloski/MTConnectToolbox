//
// c:\Users\michalos\Documents\GitHub\MTConnectGadgets\MTConnectAgentFromShdr\MTConnectAgentFromShdr\DeviceXml.cpp
//
#include "stdafx.h"
#include "DeviceXml.h"
#include "StdStringFcn.h"
#include "File.h"
///////////////////////////////////////////////////////////////////////////////////
static std::string GetTimeStamp()
{
	char aBuffer[256];
	SYSTEMTIME st;
	GetSystemTime(&st);
	sprintf(aBuffer, "%4d-%02d-%02dT%02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, 
		st.wMinute, st.wSecond);
	return aBuffer;
}
///////////////////////////////////////////////////////////////////////////////////
CDeviceXml::CDeviceXml(void)
{
}


CDeviceXml::~CDeviceXml(void)
{
}

void CDeviceXml::Setup(std::vector<std::string> devicenames)
{
	std::string xmldevices=WriteDevicesFileXML(devicenames);
	WriteDevicesFile(File.ExeDirectory()+"Devices.xml", "", xmldevices);
}

std::string	CDeviceXml::WriteDevicesFileXML(std::vector<std::string> devicenames)
{
	std::string contents;
	// Generate Devices.xml file with all devices in it.
	for(UINT i=0; i<devicenames.size(); i++)
	{
		std::string config = MTConnect_device; 
		ReplaceAll(config,"####", devicenames[i]); 
		config=ReplaceOnce(config,"name=\"NNNNNN\"","name=\""+devicenames[i]+"\"");
		contents+=config+"\n";
	}
	return contents;
}

HRESULT	CDeviceXml::WriteDevicesFile(std::string xmlFile, std::string destFolder, std::string devicesXML)
{
	std::string contents;
	contents+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	contents+="<MTConnectDevices xmlns=\"urn:mtconnect.org:MTConnectDevices:1.1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"urn:mtconnect.org:MTConnectDevices:1.1 http://www.mtconnect.org/schemas/MTConnectDevices_1.1.xsd\">\n";
	contents+=StdStringFormat("<Header bufferSize=\"130000\" instanceId=\"1\" creationTime=\"%s\" sender=\"local\" version=\"1.1\"/>\n",
		GetTimeStamp().c_str());
	contents+="<Devices>\n";

	// Check all machine names unique
	contents +=devicesXML;

	contents+="</Devices>\n";
	contents+="</MTConnectDevices>\n";

	WriteFile(destFolder + xmlFile  , contents);
	return 0;
}

HRESULT CDeviceXml::WriteAgentCfgFile(std::string cfgfile, std::string xmlFile, std::string destFolder, std::string HttpPort,
	std::vector<std::string> names, std::vector<std::string> ips, std::vector<std::string> ports, std::string ServiceName )
{
	// Generate agent.cfg file with all devices in it.
	std::string cfg;
	cfg= "Devices = " + xmlFile + "\n";
	cfg+="ServiceName = "+ ServiceName + "\n"; 
	cfg+="Port = "+ HttpPort +"\n";

	cfg+="CheckpointFrequency=10000\n";
	cfg+="AllowPut=true\n";

	cfg+="Adapters \n{\n";
	for(UINT i=0; i<names.size(); i++)
	{
		if(ports[i]!="0")
		{
			cfg+="\t"+names[i] + "\n";
			cfg+="\t{\n";
			cfg+="\t\tHost = " + ips[i] +  "\n";
			cfg+="\t\tPort = " + ports[i] + "\n";
			cfg+="\t}\n";
		}
	}
	cfg+="}\n";

	cfg+="# Logger Configuration\n";
	cfg+="logger_config\n";
	cfg+="{\n";
	//	cfg+="\tlogging_level = debug\n";
	cfg+="\tlogging_level = fatal\n";
	cfg+="\toutput = cout\n";
	//cfg+="\toutput =  file Agent.log\n";



	cfg+="}\n";
	if(!cfgfile.empty())
		WriteFile(destFolder +  cfgfile, cfg);
	return 0;
}

#ifdef MAZAKAGENT
std::string CDeviceXml::MTConnect_device =  

	"			 <Device id=\"####id\" uuid=\"####xxx\" name=\"NNNNNN\">\n"
	"			<Description>MTConnect Siemens 840D Device Using Mazak Shdr as Backend</Description>\n"
	"			<DataItems>\n"
	"				<DataItem id=\"####avail\" name=\"avail\"  type=\"AVAILABILITY\" category=\"EVENT\"/>\n"
	"			</DataItems>\n"
	"			<Components>\n"
	"				<Axes id=\"####a1\" name=\"base\">\n"		
	"					<Components>\n"	
	"						<Rotary id=\"####d1\" name=\"SPINDLE\">\n"
	"							<DataItems>\n"
	"								<DataItem type=\"SPINDLE_SPEED\" id=\"####cs1\" category=\"SAMPLE\" name=\"Srpm\"  units=\"REVOLUTION/MINUTE\" nativeUnits=\"REVOLUTION/MINUTE\"/>\n"
	"								<DataItem type=\"SPINDLE_SPEED\" id=\"####cso1\" category=\"SAMPLE\" name=\"Sovr\" subType=\"OVERRIDE\" units=\"PERCENT\" nativeUnits=\"PERCENT\"/>\n"
	"								<DataItem category=\"SAMPLE\" id=\"####cl3\" name=\"Sload\" nativeUnits=\"PERCENT\" type=\"LOAD\" units=\"PERCENT\"/>\n"
	"							</DataItems>\n"
	"						</Rotary>\n"
	"						<Linear id=\"####x1\" name=\"X\">\n"
	"							<DataItems>\n"
	"								<DataItem type=\"POSITION\" subType=\"ACTUAL\" id=\"####xp1\" category=\"SAMPLE\" name=\"Xabs\" units=\"MILLIMETER\" nativeUnits=\"MILLIMETER\" coordinateSystem=\"MACHINE\"/>\n"
	"							</DataItems>\n"
	"						</Linear>\n"
	"						<Linear id=\"####y1\" name=\"Y\">\n"
	"							<DataItems>\n"
	"								<DataItem type=\"POSITION\" subType=\"ACTUAL\" id=\"####yp1\" category=\"SAMPLE\" name=\"Yabs\" units=\"MILLIMETER\" nativeUnits=\"MILLIMETER\" coordinateSystem=\"MACHINE\"/>\n"
	"							</DataItems>\n"
	"						</Linear>\n"
	"						<Linear id=\"####z1\" name=\"Z\">\n"
	"							<DataItems>\n"
	"								<DataItem type=\"POSITION\" subType=\"ACTUAL\" id=\"####zp1\" category=\"SAMPLE\" name=\"Zabs\" units=\"MILLIMETER\" nativeUnits=\"MILLIMETER\" coordinateSystem=\"MACHINE\"/>\n"
	"							</DataItems>\n"
	"						</Linear>\n"
	"					</Components>\n"
	"				</Axes>\n"
	"				<Controller id=\"####cont\" name=\"controller\">\n"
	"					<Components>\n"
	"						<Path id=\"####path1\" name=\"path\">\n"
	"							<DataItems>\n"
	"								<DataItem type=\"PART_COUNT\" id=\"####pc1\" category=\"EVENT\" name=\"PartCountAct\" />\n"
	"								<DataItem type=\"PATH_FEEDRATE\" id=\"####pf1\" category=\"SAMPLE\" name=\"path_feedratefrt\" units=\"MILLIMETER/SECOND\" subType=\"ACTUAL\" coordinateSystem=\"WORK\"/>\n"
	"								<DataItem type=\"TOOL_ID\" id=\"####tid1\" category=\"EVENT\" name=\"Tool_number\"/>\n"
	"								<DataItem type=\"PROGRAM\" id=\"####pgm\" category=\"EVENT\" name=\"program\"/>\n"
	"								<DataItem type=\"EXECUTION\" id=\"####exec\" category=\"EVENT\" name=\"execution\"/>\n"
	"								<DataItem type=\"CONTROLLER_MODE\" id=\"####mode\" category=\"EVENT\" name=\"controllermode\"/>\n"
	"								<DataItem type=\"PATH_FEEDRATE\" id=\"####pfo1\" category=\"SAMPLE\" name=\"path_feedrateovr\" units=\"PERCENT\" nativeUnits=\"PERCENT\" subType=\"OVERRIDE\" />\n"
	"        							<DataItem category=\"EVENT\"  id=\"####fault\"  name=\"fault\" type=\"CODE\"></DataItem>\n"
	"        							<DataItem category=\"EVENT\"  id=\"####heartbeat\"  name=\"heartbeat\" type=\"CODE\"></DataItem>\n"
	"								<DataItem type=\"SYSTEM\" category=\"CONDITION\" id=\"####alarm_cond\" name=\"alarm\" />\n"
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
#endif

#ifdef FANUCAGENT
std::string CDeviceXml::MTConnect_device =  
	"			 <Device id=\"####id\" uuid=\"####xxx\" name=\"NNNNNN\">\n"
	"			<Description>TechSolve MTConnect Siemens 840D Solutionline CNC </Description>\n"
	"			<DataItems>\n"
	"				<DataItem id=\"####avail\" name=\"avail\"  type=\"AVAILABILITY\" category=\"EVENT\"/>\n"
	"			</DataItems>\n"
	"			<Components>\n"
	"				<Axes id=\"####a1\" name=\"base\">\n"		
	"					<Components>\n"	
	"						<Rotary id=\"####d1\" name=\"SPINDLE\">\n"
	"							<DataItems>\n"
	"								<DataItem type=\"SPINDLE_SPEED\" id=\"####cs1\" category=\"SAMPLE\" name=\"Srpm\"  units=\"REVOLUTION/MINUTE\" nativeUnits=\"REVOLUTION/MINUTE\"/>\n"
	"								<DataItem type=\"SPINDLE_SPEED\" id=\"####cso1\" category=\"SAMPLE\" name=\"Sovr\" subType=\"OVERRIDE\" units=\"PERCENT\" nativeUnits=\"PERCENT\"/>\n"
	"								<DataItem category=\"SAMPLE\" id=\"####cl3\" name=\"Sload\" nativeUnits=\"PERCENT\" type=\"LOAD\" units=\"PERCENT\"/>\n"
	"							</DataItems>\n"
	"						</Rotary>\n"
	"						<Linear id=\"####x1\" name=\"X\">\n"
	"							<DataItems>\n"
	"								<DataItem type=\"POSITION\" subType=\"ACTUAL\" id=\"####xp1\" category=\"SAMPLE\" name=\"Xabs\" units=\"MILLIMETER\" nativeUnits=\"MILLIMETER\" coordinateSystem=\"MACHINE\"/>\n"
	"							</DataItems>\n"
	"						</Linear>\n"
	"						<Linear id=\"####y1\" name=\"Y\">\n"
	"							<DataItems>\n"
	"								<DataItem type=\"POSITION\" subType=\"ACTUAL\" id=\"####yp1\" category=\"SAMPLE\" name=\"Yabs\" units=\"MILLIMETER\" nativeUnits=\"MILLIMETER\" coordinateSystem=\"MACHINE\"/>\n"
	"							</DataItems>\n"
	"						</Linear>\n"
	"						<Linear id=\"####z1\" name=\"Z\">\n"
	"							<DataItems>\n"
	"								<DataItem type=\"POSITION\" subType=\"ACTUAL\" id=\"####zp1\" category=\"SAMPLE\" name=\"Zabs\" units=\"MILLIMETER\" nativeUnits=\"MILLIMETER\" coordinateSystem=\"MACHINE\"/>\n"
	"							</DataItems>\n"
	"						</Linear>\n"
	"					</Components>\n"
	"				</Axes>\n"
	"				<Controller id=\"####cont\" name=\"controller\">\n"
	"					<Components>\n"
	"						<Path id=\"####path1\" name=\"path\">\n"
	"							<DataItems>\n"
	"								<DataItem type=\"PART_COUNT\" id=\"####pc1\" category=\"EVENT\" name=\"PartCountAct\" />\n"
	"								<DataItem type=\"TOOL_ID\" id=\"####tid1\" category=\"EVENT\" name=\"toolid\"/>\n"
	"								<DataItem type=\"PROGRAM\" id=\"####pgm\" category=\"EVENT\" name=\"program\"/>\n"
	"								<DataItem type=\"EXECUTION\" id=\"####exec\" category=\"EVENT\" name=\"execution\"/>\n"
	"								<DataItem type=\"CONTROLLER_MODE\" id=\"####mode\" category=\"EVENT\" name=\"controllermode\"/>\n"
	"								<DataItem type=\"PATH_FEEDRATE\" id=\"####pfo1\" category=\"SAMPLE\" name=\"path_feedrateovr\" units=\"PERCENT\" nativeUnits=\"PERCENT\" subType=\"OVERRIDE\" />\n"
	"								<DataItem type=\"PATH_FEEDRATE\" id=\"####pf1\" category=\"SAMPLE\" name=\"path_feedratefrt\" units=\"MILLIMETER/SECOND\" subType=\"ACTUAL\" coordinateSystem=\"WORK\"/>\n"
	"        							<DataItem category=\"EVENT\"  id=\"####DeviceName\"  name=\"DeviceName\" type=\"CODE\"></DataItem>\n"
	"        							<DataItem category=\"EVENT\"  id=\"####fault\"  name=\"fault\" type=\"CODE\"></DataItem>\n"
	"        							<DataItem category=\"EVENT\"  id=\"####status\"  name=\"status\" type=\"CODE\"></DataItem>\n"
	"        							<DataItem category=\"EVENT\"  id=\"####heartbeat\"  name=\"heartbeat\" type=\"CODE\"></DataItem>\n"
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
#endif