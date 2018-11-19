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
	contents+=StdStringFormat("<Header bufferSize=\"130000\" instanceid=\"####1\" creationTime=\"%s\" sender=\"local\" version=\"1.1\"/>\n",
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

#ifdef TECHSOLVE_AGENT
std::string CDeviceXml::MTConnect_device =  
	"			 <Device id=\"####id\" uuid=\"####xxx\" name=\"NNNNNN\">\n"
	"			<Description>TechSolve MTConnect Fanuc CNC </Description>\n"
	"			<DataItems>\n"
	"				<DataItem id=\"####avail\" name=\"avail\"  type=\"AVAILABILITY\" category=\"EVENT\"/>\n"
    "               <DataItem category=\"EVENT\" id=\"####message\" name=\"message\" type=\"MESSAGE\" />\n"
	"			</DataItems>\n"
	"			<Components>\n"
	"                <Axes id=\"####ax\" name=\"Axes\">\n"
	"                    <Components>\n"
	"                        <Linear id=\"####x\" name=\"X\">\n"
	"                            <DataItems>\n"
	"                                <DataItem category=\"SAMPLE\" id=\"####x_pos_act\" name=\"X_actual\" type=\"POSITION\" subType=\"ACTUAL\" units=\"MILLIMETER\" />\n"
	"                                <DataItem category=\"SAMPLE\" id=\"####x_pos_cmd\" name=\"X_command\" type=\"POSITION\" subType=\"COMMANDED\" units=\"MILLIMETER\" />\n"
	"                                <DataItem category=\"SAMPLE\" id=\"####x_load\" name=\"X_load\" type=\"LOAD\" units=\"PERCENT\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####x_travel\" name=\"X_travel\" type=\"POSITION\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####x_overheat\" name=\"X_overheat\" type=\"TEMPERATURE\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####x_servo\" name=\"X_servo\" type=\"LOAD\" />\n"
	"                            </DataItems>\n"
	"                        </Linear>\n"
	"                        <Linear id=\"####y\" name=\"Y\">\n"
	"                            <DataItems>\n"
 	"                               <DataItem category=\"SAMPLE\" id=\"####y_pos_act\" name=\"Y_actual\" type=\"POSITION\" subType=\"ACTUAL\" units=\"MILLIMETER\" />\n"
 	"                               <DataItem category=\"SAMPLE\" id=\"####y_pos_cmd\" name=\"Y_command\" type=\"POSITION\" subType=\"COMMANDED\" units=\"MILLIMETER\" />\n"
	"                                <DataItem category=\"SAMPLE\" id=\"####y_load\" name=\"Y_load\" type=\"LOAD\" units=\"PERCENT\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####y_travel\" name=\"Y_travel\" type=\"POSITION\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####y_overheat\" name=\"Y_overheat\" type=\"TEMPERATURE\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####y_servo\" name=\"Y_servo\" type=\"LOAD\" />\n"
	"                            </DataItems>\n"
	"                        </Linear>\n"
	"                        <Linear id=\"####z\" name=\"Z\">\n"
 	"                           <DataItems>\n"
	"                                <DataItem category=\"SAMPLE\" id=\"####z_pos_act\" name=\"Z_actual\" type=\"POSITION\" subType=\"ACTUAL\" units=\"MILLIMETER\" />\n"
	"                                <DataItem category=\"SAMPLE\" id=\"####z_pos_cmd\" name=\"Z_command\" type=\"POSITION\" subType=\"COMMANDED\" units=\"MILLIMETER\" />\n"
	"                                <DataItem category=\"SAMPLE\" id=\"####z_load\" name=\"Z_load\" type=\"LOAD\" units=\"PERCENT\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####z_travel\" name=\"Z_travel\" type=\"POSITION\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####z_overheat\" name=\"Z_overheat\" type=\"TEMPERATURE\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####z_servo\" name=\"Z_servo\" type=\"LOAD\" />\n"
	"                            </DataItems>\n"
	"                        </Linear>\n"
	"                        <Rotary id=\"####c\" name=\"C\">\n"
	"                            <DataItems>\n"
 	"                               <DataItem category=\"SAMPLE\" id=\"####c_pos_act\" name=\"C_actual\" type=\"ANGLE\" subType=\"ACTUAL\" units=\"DEGREE\" />\n"
 	"                               <DataItem category=\"SAMPLE\" id=\"####c_pos_cmd\" name=\"C_command\" type=\"ANGLE\" subType=\"COMMANDED\" units=\"DEGREE\" />\n"
 	"                               <DataItem category=\"SAMPLE\" id=\"####c_load\" name=\"C_load\" type=\"LOAD\" units=\"PERCENT\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####c_travel\" name=\"C_travel\" type=\"POSITION\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####c_overheat\" name=\"C_overheat\" type=\"TEMPERATURE\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####c_servo\" name=\"C_servo\" type=\"LOAD\" />\n"
	"                                <DataItem category=\"EVENT\" id=\"####c_mode\" name=\"C_mode\" type=\"ROTARY_MODE\">\n"
	"                                    <Constraints>\n"
	"                                        <Value>INDEX</Value>\n"
	"                                    </Constraints>\n"
	"                                </DataItem>\n"
	"                            </DataItems>\n"
	"                        </Rotary>\n"
	"                        <Rotary id=\"####a\" name=\"A\">\n"
	"                            <DataItems>\n"
 	"                               <DataItem category=\"SAMPLE\" id=\"####a_pos_act\" name=\"A_actual\" type=\"ANGLE\" subType=\"ACTUAL\" units=\"DEGREE\" />\n"
	"                                <DataItem category=\"SAMPLE\" id=\"####a_pos_cmd\" name=\"A_command\" type=\"ANGLE\" subType=\"COMMANDED\" units=\"DEGREE\" />\n"
	"                                <DataItem category=\"SAMPLE\" id=\"####a_load\" name=\"A_load\" type=\"LOAD\" units=\"PERCENT\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####a_travel\" name=\"A_travel\" type=\"POSITION\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####a_overheat\" name=\"A_overheat\" type=\"TEMPERATURE\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####a_servo\" name=\"A_servo\" type=\"LOAD\" />\n"
	"                                <DataItem category=\"EVENT\" id=\"####a_mode\" name=\"A_mode\" type=\"ROTARY_MODE\">\n"
	"                                    <Constraints>\n"
	"                                        <Value>INDEX</Value>\n"
	"                                    </Constraints>\n"
	"                                </DataItem>\n"
	"                            </DataItems>\n"
	"                        </Rotary>\n"
	"                        <Rotary id=\"####p1s1\" name=\"P1S1\">\n"
	"                            <DataItems>\n"
	"                                <DataItem category=\"SAMPLE\" id=\"####p1s1_speed_act\" name=\"P1S1_speed_act\" type=\"ROTARY_VELOCITY\" subType=\"ACTUAL\" units=\"REVOLUTION/MINUTE\" />\n"
	"                                <DataItem category=\"SAMPLE\" id=\"####p1s1_speed_com\" name=\"P1S1_speed_com\" type=\"ROTARY_VELOCITY\" subType=\"COMMANDED\" units=\"REVOLUTION/MINUTE\" />\n"
 	"                               <DataItem category=\"SAMPLE\" id=\"####p1s1_spindle_load\" name=\"P1S1_load\" type=\"LOAD\" units=\"PERCENT\" />\n"
 	"                               <DataItem category=\"CONDITION\" id=\"####p1s1_servo\" name=\"P1S1_servo\" type=\"LOAD\" />\n"
 	"                               <DataItem category=\"EVENT\" id=\"####p1s1_spindle_mode\" name=\"P1S1_mode\" type=\"ROTARY_MODE\">\n"
 	"                                   <Constraints>\n"
	"                                        <Value>SPINDLE</Value>\n"
	"                                        <Value>INDEX</Value>\n"
	"                                    </Constraints>\n"
	"                                </DataItem>\n"
	"                            </DataItems>\n"
	"                        </Rotary>\n"
 	"                   </Components>\n"
	"                </Axes>\n"
	"                <Controller id=\"####controller1\" name=\"cn1\">\n"
	"                    <DataItems>\n"
 	"                       <DataItem category=\"SAMPLE\" id=\"####lp_coolant_pressure\" name=\"lp_coolant_pressure\" type=\"x:LP_COOLANT_PRESSURE\" />\n"
	"                        <DataItem category=\"SAMPLE\" id=\"####hp_coolant_pressure\" name=\"hp_coolant_pressure\" type=\"x:HP_COOLANT_PRESSURE\" />\n"
	"                        <DataItem category=\"EVENT\" id=\"####optional_stop\" name=\"optional_stop\" type=\"CONTROLLER_MODE_OVERRIDE\" subType=\"OPTIONAL_STOP\" />\n"
	"                        <DataItem category=\"EVENT\" id=\"####dry_run\" name=\"dry_run\" type=\"CONTROLLER_MODE_OVERRIDE\" subType=\"DRY_RUN\" />\n"
	"                        <DataItem category=\"EVENT\" id=\"####single_block\" name=\"single_block\" type=\"CONTROLLER_MODE_OVERRIDE\" subType=\"SINGLE_BLOCK\" />\n"
	"                        <DataItem category=\"EVENT\" id=\"####machine_axis_lock\" name=\"machine_axis_lock\" type=\"CONTROLLER_MODE_OVERRIDE\" subType=\"MACHINE_AXIS_LOCK\" />\n"
	"                        <DataItem category=\"EVENT\" id=\"####estop\" name=\"estop\" type=\"EMERGENCY_STOP\" />\n"
	"                    </DataItems>\n"
 	"                   <Components>\n"
	"                        <Path id=\"####path1\" name=\"p1\">\n"
	"                            <DataItems>\n"
 	"                               <DataItem category=\"EVENT\" id=\"####path1_mode\" name=\"p1_mode\" type=\"CONTROLLER_MODE\" />\n"
 	"                               <DataItem category=\"EVENT\" id=\"####path1_execution\" name=\"p1_execution\" type=\"EXECUTION\" />\n"
 	"                               <DataItem category=\"EVENT\" id=\"####path1_part_count\" name=\"p1_part_count\" type=\"PART_COUNT\" />\n"
	"                                <DataItem category=\"EVENT\" id=\"####path1_program\" name=\"p1_program\" type=\"PROGRAM\" />\n"
	"                                <DataItem category=\"EVENT\" id=\"####path1_tool_number\" name=\"p1_tool_number\" type=\"TOOL_NUMBER\" />\n"
	"                                <DataItem category=\"EVENT\" id=\"####path1_tool_group\" name=\"p1_tool_group\" type=\"TOOL_GROUP\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####path1_condition_system\" name=\"p1_system\" type=\"SYSTEM\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####path1_condition_servo\" name=\"p1_servo\" type=\"HARDWARE\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####path1_condition_logic\" name=\"p1_logic\" type=\"LOGIC_PROGRAM\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####path1_condition_motion\" name=\"p1_motion\" type=\"MOTION_PROGRAM\" />\n"
	"                                <DataItem category=\"CONDITION\" id=\"####path1_communications\" name=\"p1_comms\" type=\"COMMUNICATIONS\" />\n"
	"                                <DataItem category=\"EVENT\" id=\"####path1_line\" name=\"p1_line\" type=\"LINE\" />\n"
	"                                <DataItem category=\"EVENT\" id=\"####path1_block\" name=\"p1_block\" type=\"BLOCK\" />\n"
	"                                <DataItem category=\"EVENT\" id=\"####path1_comment\" name=\"p1_comment\" type=\"PROGRAM_COMMENT\" />\n"
	"                                <DataItem category=\"EVENT\" id=\"####path1_feedrate_ovr\" name=\"p1_feedrate_override\" type=\"PATH_FEEDRATE_OVERRIDE\" subType=\"PROGRAMMED\" units=\"PERCENT\" />\n"
 	"                               <DataItem category=\"EVENT\" id=\"####path1_rapid_ovr\" name=\"p1_rapid_override\" type=\"PATH_FEEDRATE_OVERRIDE\" subType=\"RAPID\" units=\"PERCENT\" />\n"
	"                                <DataItem category=\"EVENT\" id=\"####path1_spindle_ovr\" name=\"p1_spindle_override\" type=\"ROTARY_VELOCITY_OVERRIDE\" units=\"PERCENT\" />\n"
  	"                              <DataItem category=\"SAMPLE\" id=\"####path1_feedrate_act\" name=\"p1_feedrate_act\" type=\"PATH_FEEDRATE\" subType=\"ACTUAL\" units=\"MILLIMETER/SECOND\" />\n"
	"                                <DataItem category=\"SAMPLE\" id=\"####path1_feedrate_com\" name=\"p1_feedrate_com\" type=\"PATH_FEEDRATE\" subType=\"COMMANDED\" units=\"MILLIMETER/SECON\" />\n"
	"                                <DataItem category=\"SAMPLE\" id=\"####path1_x_axis_motor_temperature\" name=\"p1_x_axis_motor_temperature\" type=\"x:X_AXIS_MOTOR_TEMPERATURE\" />\n"
	"                                <DataItem category=\"SAMPLE\" id=\"####path1_y_axis_motor_temperature\" name=\"p1_y_axis_motor_temperature\" type=\"x:Y_AXIS_MOTOR_TEMPERATURE\" />\n"
	"                                <DataItem category=\"SAMPLE\" id=\"####path1_z_axis_motor_temperature\" name=\"p1_z_axis_motor_temperature\" type=\"x:Z_AXIS_MOTOR_TEMPERATURE\" />\n"
	"                                <DataItem category=\"SAMPLE\" id=\"####path1_x_pulse_code_temperature\" name=\"p1_x_pulse_code_temperature\" type=\"x:X_PULSE_CODE_TEMPERATURE\" />\n"
  	"                              <DataItem category=\"SAMPLE\" id=\"####path1_y_pulse_code_temperature\" name=\"p1_y_pulse_code_temperature\" type=\"x:Y_PULSE_CODE_TEMPERATURE\" />\n"
 	"                               <DataItem category=\"SAMPLE\" id=\"####path1_z_pulse_code_temperature\" name=\"p1_z_pulse_code_temperature\" type=\"x:Z_PULSE_CODE_TEMPERATURE\" />\n"
 	"                               <DataItem category=\"SAMPLE\" id=\"####path1_spindle_motor_temperature\" name=\"p1_spindle_motor_temperature\" type=\"x:SPINDLE_MOTOR_TEMPERATURE\" />\n"
 	"                               <DataItem category=\"SAMPLE\" id=\"####path1_diagnostic_spindle_load\" name=\"p1_diagnostic_spindle_load\" type=\"x:DIAGNOSTIC_SPINDLE_LOAD\" />\n"
 	"                               <DataItem category=\"SAMPLE\" id=\"####path1_diagnostic_spindle_speed\" name=\"p1_diagnostic_spindle_speed\" type=\"x:DIAGNOSTIC_SPINDLE_SPEED\" />\n"
 	"                           </DataItems>\n"
    "                     </Path>\n"
	"					</Components>\n"
	"				</Controller>\n"
	"			</Components>\n"
	"		</Device>\n";
#endif