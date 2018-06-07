
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
        //tagnames.push_back(jointnames[i] + "_mode");
        //tagnames.push_back(jointnames[i] + "_I");
        //tagnames.push_back(jointnames[i] + "_V");
        //tagnames.push_back(jointnames[i] + "_T");
        //tagnames.push_back(jointnames[i] + "_actvel");

        // tagnames.push_back(jointnames[i]+"_actacc");
    }
    tagnames.push_back("power");
    tagnames.push_back("program");
    tagnames.push_back("robotmode");
    tagnames.push_back("controlmode");
    tagnames.push_back("execution");
    tagnames.push_back("heartbeat");
    tagnames.push_back("fovr");
    tagnames.push_back("frt");
    tagnames.push_back("pose");
    tagnames.push_back("velocity");
    tagnames.push_back("version");
    //tagnames.push_back("motherboard_temp");
    tagnames.push_back("estop");
    conditionIds.push_back(devicename + "system");

    std::string tmp
        = "\t\t<Device id=\"####id\" uuid=\"####xxx\" name=\"NNNNNN\">\n"
          "\t\t\t<Description>moto_####</Description>\n"
          "\t\t\t<DataItems>\n"
          "\t\t\t\t<DataItem id=\"####avail\" name=\"avail\"  type=\"AVAILABILITY\" category=\"EVENT\"/>\n"
          "\t\t\t\t<DataItem id=\"####version\" name=\"version\"  type=\"MESSAGE\" category=\"EVENT\"/>\n"
          "\t\t\t\t<DataItem id=\"####motherboard_temp\" name=\"motherboard_temp\" type=\"TEMPERATURE\" category=\"SAMPLE\"/>\n"
          "\t\t\t\t<DataItem id=\"####_urdf_asset\"  type=\"ASSET_CHANGED\" category=\"EVENT\"/>\n"
          "\t\t\t</DataItems>\n"
          "\t\t\t<Components>\n"
          "\t\t\t\t<Axes id=\"####a1\" name=\"base\">\n"
          "\t\t\t\t\t<Components>\n";

    for ( size_t i = 0; i < jointnames.size( ); i++ )
    {
        tmp += "\t\t\t\t\t\t<Rotary id=\"####d1\" name=\"JOINT\">\n"
               "\t\t\t\t\t\t\t<DataItems>\n";
        tmp += StdStringFormat("\t\t\t\t\t\t\t\t<DataItem type=\"POSITION\" "
                               "subType=\"ACTUAL\" id=\"####_%s_actpos_id\" "
                               "category=\"SAMPLE\" name=\"%s_actpos\"  "
                               "units=\"RADIANS\" />\n",
                               jointnames[i].c_str( ), jointnames[i].c_str( ));
        //tmp += StdStringFormat("\t\t\t\t\t\t\t\t<DataItem type=\"MESSAGE\"  "
        //                       "id=\"####_%s_mode_id\" category=\"EVENT\" "
        //                       "name=\"%s_mode\" />\n",
        //                       jointnames[i].c_str( ), jointnames[i].c_str( ));
        //tmp
        //    += StdStringFormat("\t\t\t\t\t\t\t\t<DataItem type=\"ELECTRICAL_ENERGY\"  "
        //                       "id=\"####_%s_I_id\" category=\"SAMPLE\" name=\"%s_I\" />\n",
        //                       jointnames[i].c_str( ), jointnames[i].c_str( ));
        //tmp
        //    += StdStringFormat("\t\t\t\t\t\t\t\t<DataItem type=\"VOLTAGE\"  "
        //                       "id=\"####_%s_V_id\" category=\"SAMPLE\" name=\"%s_V\" />\n",
        //                       jointnames[i].c_str( ), jointnames[i].c_str( ));
        //tmp
        //    += StdStringFormat("\t\t\t\t\t\t\t\t<DataItem type=\"TORQUE\"  "
        //                       "id=\"####_%s_T_id\" category=\"SAMPLE\" name=\"%s_T\" />\n",
        //                       jointnames[i].c_str( ), jointnames[i].c_str( ));
        //tmp += StdStringFormat("\t\t\t\t\t\t\t\t<DataItem type=\"VELOCITY\" "
        //                       "subType=\"ACTUAL\" id=\"####_%s_actvel_id\" "
        //                       "category=\"SAMPLE\" name=\"%s_actvel\"  "
        //                       "units=\"RADIANS PER SECOND\" />\n",
        //                       jointnames[i].c_str( ), jointnames[i].c_str( ));

        // tmp +=StdStringFormat(
        //	"\t\t\t\t\t\t\t\t<DataItem type=\"ACCELERATION\" subType=\"ACTUAL\"
        // id=\"%s_actacc_id\" category=\"SAMPLE\" name=\"%s_actacc\"
        // units=\"RADIANS PER SECOND PER SECOND\" />\n",
        //	jointnames[i].c_str(),jointnames[i].c_str());

        tmp += "\t\t\t\t\t\t\t</DataItems>\n"
               "\t\t\t\t\t\t</Rotary>\n";
    }

    tmp += "\t\t\t\t\t</Components>\n"
           "\t\t\t\t</Axes>\n"
           "\t\t\t\t<Controller id=\"####cont\" name=\"controller\">\n"
           "\t\t\t\t\t<DataItems>\n"
           "\t\t\t\t\t\t<DataItem category=\"EVENT\" id=\"####estop\" "
           "name=\"estop\" type=\"EMERGENCY_STOP\"/>\n"
           "\t\t\t\t\t\t<DataItem category=\"CONDITION\" id=\"####system\" "
           "type=\"SYSTEM\"/>\n"
           "\t\t\t\t\t</DataItems>\n"
           "\t\t\t\t\t<Components>\n"
           "\t\t\t\t\t\t<Path id=\"####path1\" name=\"path\">\n"
           "\t\t\t\t\t\t\t<DataItems>\n"
           "\t\t\t\t\t\t\t\t<DataItem type=\"PATH_POSITION\" id=\"####pp\" "
           "category=\"SAMPLE\" name=\"pose\" subType=\"ACTUAL\" />\n"
           "\t\t\t\t\t\t\t\t<DataItem type=\"PATH_FEEDRATE\" id=\"####pfr\" "
           "category=\"SAMPLE\" name=\"velocity\" subType=\"ACTUAL\" />\n"
           "\t\t\t\t\t\t\t\t<DataItem type=\"PROGRAM\" id=\"####pgm\" "
           "category=\"EVENT\" name=\"program\"/>\n"
           "\t\t\t\t\t\t\t\t<DataItem type=\"EXECUTION\" id=\"####exec\" "
           "category=\"EVENT\" name=\"execution\"/>\n"
           "\t\t\t\t\t\t\t\t<DataItem type=\"CONTROLLER_MODE\" "
           "id=\"####robotmode\" category=\"EVENT\" name=\"robotmode\"/>\n"
           "\t\t\t\t\t\t\t\t<DataItem type=\"CONTROLLER_MODE\" "
           "id=\"####controlmode\" category=\"EVENT\" name=\"controlmode\"/>\n"
           "\t\t\t\t\t\t\t\t<DataItem type=\"PATH_FEEDRATE\" id=\"####pfo1\" "
           "category=\"SAMPLE\" name=\"fovr\" units=\"PERCENT\" "
           "nativeUnits=\"PERCENT\" subType=\"OVERRIDE\" />\n"
           "\t\t\t\t\t\t\t\t<DataItem type=\"PATH_FEEDRATE\" id=\"####pfr1\" "
           "category=\"SAMPLE\" name=\"frt\" />\n"
           "\t\t\t\t\t\t\t\t<DataItem category=\"EVENT\"  id=\"####heartbeat\"  "
           "name=\"heartbeat\" type=\"CODE\"></DataItem>\n"
           "\t\t\t\t\t\t\t</DataItems>\n"
           "\t\t\t\t\t\t</Path>\n"
           "\t\t\t\t\t</Components>\n"
           "\t\t\t\t</Controller>\n"
           "\t\t\t\t<Systems id=\"####systems\" name=\"systems\">\n"
           "\t\t\t\t\t<Components>\n"
           "\t\t\t\t\t\t<Electric name=\"electric\" id=\"####elec\">\n"
           "\t\t\t\t\t\t\t<DataItems>\n"
           "\t\t\t\t\t\t\t\t<DataItem type=\"POWER_STATE\" category=\"EVENT\" "
           "id=\"####pwr\" name=\"power\" />\n"
           "\t\t\t\t\t\t\t</DataItems>\n"
           "\t\t\t\t\t\t</Electric>\n"
           "\t\t\t\t\t</Components>\n"
           "\t\t\t\t</Systems>\n"
           "\t\t\t</Components>\n"
           "\t\t</Device>\n";
    return tmp;
}
std::string CDevicesXML::ProbeDeviceXml (void)
{
    std::string moto__device
        = "			 <Device id=\"####id\" uuid=\"####xxx\" "
          "name=\"NNNNNN\">\n"
          "			<Description>moto_</Description>\n"
          "			<DataItems>\n"
          "				<DataItem id=\"####avail\" "
          "name=\"avail\"  type=\"AVAILABILITY\" category=\"EVENT\"/>\n"
          "			</DataItems>\n"
          "			<Components>\n"
          "				<Axes id=\"####a1\" name=\"base\">\n"
          "					<Components>\n"
          "						<Rotary id=\"####d1\" "
          "name=\"SPINDLE\">\n"
          "							<DataItems>\n"
          "								"
          "<DataItem type=\"SPINDLE_SPEED\" id=\"####cs1\" category=\"SAMPLE\" "
          "name=\"Srpm\"  units=\"REVOLUTION/MINUTE\" "
          "nativeUnits=\"REVOLUTION/MINUTE\"/>\n"
          "							</DataItems>\n"
          "						</Rotary>\n"
          "						<Linear id=\"####x1\" "
          "name=\"X\">\n"
          "							<DataItems>\n"
          "								"
          "<DataItem type=\"POSITION\" subType=\"ACTUAL\" id=\"####xp1\" "
          "category=\"SAMPLE\" name=\"Xabs\" units=\"MILLIMETER\" "
          "nativeUnits=\"MILLIMETER\" coordinateSystem=\"MACHINE\"/>\n"
          "							</DataItems>\n"
          "						</Linear>\n"
          "						<Linear id=\"####y1\" "
          "name=\"Y\">\n"
          "							<DataItems>\n"
          "								"
          "<DataItem type=\"POSITION\" subType=\"ACTUAL\" id=\"####yp1\" "
          "category=\"SAMPLE\" name=\"Yabs\" units=\"MILLIMETER\" "
          "nativeUnits=\"MILLIMETER\" coordinateSystem=\"MACHINE\"/>\n"
          "							</DataItems>\n"
          "						</Linear>\n"
          "						<Linear id=\"####z1\" "
          "name=\"Z\">\n"
          "							<DataItems>\n"
          "								"
          "<DataItem type=\"POSITION\" subType=\"ACTUAL\" id=\"####zp1\" "
          "category=\"SAMPLE\" name=\"Zabs\" units=\"MILLIMETER\" "
          "nativeUnits=\"MILLIMETER\" coordinateSystem=\"MACHINE\"/>\n"
          "							</DataItems>\n"
          "						</Linear>\n"
          "					</Components>\n"
          "				</Axes>\n"
          "				<Controller id=\"####cont\" "
          "name=\"controller\">\n"
          "					<Components>\n"
          "						<Path id=\"####path1\" "
          "name=\"path\">\n"
          "							<DataItems>\n"
          "								"
          "<DataItem type=\"MESSAGE\" id=\"####d_file_size\" category=\"EVENT\" "
          "name=\"d_file_size\"/>\n"
          "								"
          "<DataItem type=\"MESSAGE\" id=\"####d_cycle_time\" category=\"EVENT\" "
          "name=\"d_cycle_time\"/>\n"
          "								"
          "<DataItem type=\"MESSAGE\" id=\"####d_message\" category=\"EVENT\" "
          "name=\"d_message\"/>\n"
          "								"
          "<DataItem type=\"MESSAGE\" id=\"####last_update\" category=\"EVENT\" "
          "name=\"last_update\"/>\n"
          "								"
          "<DataItem type=\"PROGRAM\" id=\"####pgm\" category=\"EVENT\" "
          "name=\"program\"/>\n"
          "								"
          "<DataItem type=\"EXECUTION\" id=\"####exec\" category=\"EVENT\" "
          "name=\"execution\"/>\n"
          "								"
          "<DataItem type=\"CONTROLLER_MODE\" id=\"####mode\" category=\"EVENT\" "
          "name=\"controllermode\"/>\n"
          "								"
          "<DataItem type=\"PATH_FEEDRATE\" id=\"####pfo1\" category=\"SAMPLE\" "
          "name=\"path_feedrateovr\" units=\"PERCENT\" nativeUnits=\"PERCENT\" "
          "subType=\"OVERRIDE\" />\n"
          "                                 <DataItem category=\"EVENT\"  "
          "id=\"####operator\"  name=\"operator\" type=\"CODE\"></DataItem>\n"
          "                                 <DataItem category=\"EVENT\"  "
          "id=\"####alarm\"  name=\"error\" type=\"CODE\"></DataItem>\n"
          "                                 <DataItem category=\"EVENT\"  "
          "id=\"####heartbeat\"  name=\"heartbeat\" type=\"CODE\"></DataItem>\n"
          "                             </DataItems>\n"
          "						</Path>\n"
          "					</Components>\n"
          "				</Controller>\n"
          "				<Systems id=\"####systems\" name=\"systems\">\n"
          "					<Components>\n"
          "						<Electric "
          "name=\"electric\" id=\"####elec\">\n"
          "							<DataItems>\n"
          "								"
          "<DataItem type=\"POWER_STATE\" category=\"EVENT\" id=\"####pwr\" "
          "name=\"power\" />\n"
          "							</DataItems>\n"
          "						</Electric>\n"
          "					</Components>\n"
          "				</Systems>\n"
          "			</Components>\n"
          "		</Device>\n";

    return moto__device;
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
    contents += StdStringFormat("<Header bufferSize=\"130000\" instanceId=\"1\" "
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
    contents += StdStringFormat("<Header bufferSize=\"130000\" instanceId=\"1\" "
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
