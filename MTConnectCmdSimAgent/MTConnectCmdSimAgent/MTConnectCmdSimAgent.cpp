// MTConnectCmdSimAgent.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define BOOST_ALL_NO_LIB

#include <string>
#include <exception>
#include "MTConnectCmdSimAgent.h"
#include "MTConnectCmdSimAdapter.h"
#include "StdStringFcn.h"
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include "Globals.h"
#include "CmdSim.h"

// should not be included in header
//#include "jsmtconnect.txt"
#include "logger.h"
#pragma comment(lib, "Ws2_32.lib")


#if defined(WIN64) && defined( _DEBUG) 
#pragma message( "DEBUG x64" )
#pragma comment(lib, "libxml2_64d.lib")
#pragma comment(lib, "libboost_system-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_thread-vc100-mt-sgd-1_54.lib")

#elif !defined( _DEBUG) && defined(WIN64)
#pragma message( "RELEASE x64" )
#pragma comment(lib, "libxml2_64.lib")
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")

#elif defined(_DEBUG) && defined(WIN32)
#pragma message( "DEBUG x32" )
#pragma comment(lib, "libxml2d.lib")
#pragma comment(lib, "libboost_thread-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-sgd-1_54.lib")
#elif !defined( _DEBUG) && defined(WIN32)
#pragma message( "RELEASE x32" )
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libxml2d.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")
#endif


int AgentConfigurationT::main(int aArgc, const char *aArgv[])
{
	GLogger.Fatal("Main\n");
	SetCurrentDirectory(File.ExeDirectory().c_str()); // fixes Agent file lookup issue
	AgentConfiguration::main( aArgc, (const char **) aArgv);
	GLogger.Fatal("Return Main\n");

	return 0;
}

// Start the server. This blocks until the server stops.
void AgentConfigurationEx::start()
{
	GLogger.Fatal("Start\n");
	for(int i=0; i< _devices.size(); i++)
	{
		MTConnectCmdSimAdapter *  _cmdHandler = new MTConnectCmdSimAdapter(this,  _devices[i]);
		_cncHandlers.push_back(_cmdHandler);
		_cmdHandler->Configure();
		_group.create_thread(boost::bind(&MTConnectCmdSimAdapter::Cycle, _cncHandlers[i]));
	}
/*	for(int i=0; i< _cmds.size(); i++)
	{
		CCmdSim *  _cmdIssuer = new CCmdSim(this, _devices);
		_cncIssuers.push_back(_cmdIssuer);
		_group.create_thread(boost::bind(&CCmdSim::Cycle, _cmdIssuer));
	}*/	
	CCmdSim *  _cmdIssuer = new CCmdSim(this, _devices);
	_cncIssuers.push_back(_cmdIssuer);
	_group.create_thread(boost::bind(&CCmdSim::Cycle, _cmdIssuer));

	AgentConfigurationT::start(); // does not return
}
void AgentConfigurationEx::stop()
{
	for(int i=0; i< _cncHandlers.size(); i++)
		_cncHandlers[i]->Stop();

	for(int i=0; i< _cncIssuers.size(); i++)
		_cncIssuers[i]->Stop();

	_group.join_all();

	AgentConfigurationT::stop();
}


void AgentConfigurationEx::initialize(int aArgc, const char *aArgv[])
{
	GLogger.Fatal("initialize\n");

	std::string cfgfile = Globals.inifile; 

	if(GetFileAttributesA(cfgfile.c_str())!= INVALID_FILE_ATTRIBUTES)
	{
		config.load( cfgfile );
		Globals.ServerName=config.GetSymbolValue("GLOBALS.ServiceName", Globals.ServerName).c_str();
		MTConnectService::setName(Globals.ServerName);
		Globals.QueryServer =config.GetSymbolValue("GLOBALS.QueryServer", 10000).toNumber<int>();
		Globals.ServerRate  =config.GetSymbolValue("GLOBALS.ServerRate", 2000).toNumber<int>();
		//std::string sLevel = config.GetSymbolValue("GLOBALS.logging_level", "FMTConnectCmdSimL").c_str();
		//sLevel=MakeUpper(sLevel);
		//Globals.Debug  = (sLevel=="FMTConnectCmdSimL")? 0 : (sLevel=="ERROR") ? 1 : (sLevel=="WARN") ? 2 : (sLevel=="INFO")? 3 : 5;
		_devices = config.GetTokens("GLOBALS.MTConnectDevice", ",");
		_cmds = config.GetTokens("GLOBALS.MTConnectCmds", ",");
		Globals.HttpPort = config.GetSymbolValue("GLOBALS.AgentPort", "5001").c_str();
		Globals.ResetAtMidnight = config.GetSymbolValue("GLOBALS.ResetAtMidnight", "0").toNumber<int>();
		GLogger.OutputConsole()= config.GetSymbolValue("GLOBALS.OutputConsole", "0").toNumber<int>();;
		GLogger.DebugLevel() = config.GetSymbolValue("GLOBALS.Debug", "-1").toNumber<int>();

		Globals.CfgFile = File.ExeDirectory() + config.GetSymbolValue("GLOBALS.CfgFile", "Agent.cfg").c_str();
		Globals.DevicesFile  = File.ExeDirectory() +  config.GetSymbolValue("GLOBALS.DevicesFile", "Devices.xml").c_str();

		Globals.programs = config.getvaluemap<int>("Programs");
		Globals.machineprograms=config.GetTokens("GLOBALS.Machining", ",");;
		Globals.setupprograms=config.GetTokens("GLOBALS.Setup", ",");
		Globals.inspectionprograms=config.GetTokens("GLOBALS.Inspection", ",");
		Globals.yield= config.getvaluemap<int>("YIELD");
		std::string jobname = config.GetSymbolValue("GLOBALS.Job", "JOB1").c_str();
		Globals.workorder = config.getvaluemap<int>(jobname);

	}
	else
	{
		AbortMsg("Could not find ini file \n");
	}
	std::string xmldevices;
	xmldevices+=WriteCmdedDeviceXML(_devices);
	xmldevices+=WriteCmdDeviceXML(_cmds);
	std::vector<std::string> kpis;
	kpis.push_back("KPI");
	xmldevices+=WriteDeviceXML(kpis,KPIEcho_device);
	
	WriteDevicesFile(Globals.DevicesFile, "", xmldevices);
	
	WriteAgentCfgFile(Globals.CfgFile,  File.Filename(Globals.DevicesFile) , "");

	Globals.Dump();
	AgentConfigurationT::initialize(aArgc, aArgv);
}

std::string AgentConfigurationEx::MTConnectCmdSim_device = 
"	<Device id=\"####M1\" uuid=\"####xxx\" name=\"NNNNNN\">\n"
"			<Description>Cmded Agent ####</Description>\n"
"			<DataItems>\n"
"				<DataItem id=\"####avail\" name=\"avail\"  type=\"AVAILABILITY\" category=\"EVENT\"/>\n"
"			</DataItems>\n"
"			<Components>\n"
"				<Controller id=\"####cont\" name=\"controller\">\n"
"					<DataItems>\n"
"						<DataItem type=\"CLOCK_TIME\" id=\"####faultTime\" category=\"EVENT\" name=\"faultTime\"/>	\n"
"						<DataItem type=\"CLOCK_TIME\" id=\"####idleTime\" category=\"EVENT\" name=\"idleTime\"/>	\n"
"						<DataItem type=\"CLOCK_TIME\" id=\"####programTime\" category=\"EVENT\" name=\"programTime\"/>	\n"
"        				<DataItem type=\"CODE\"       id=\"####utilization\" category=\"EVENT\"    name=\"utilization\"></DataItem>\n"
"						<DataItem type=\"CLOCK_TIME\" id=\"####estimatedCompletion\" category=\"SAMPLE\" name=\"estimatedCompletion\"/>	\n"					
"						<DataItem type=\"LINE\" id=\"####heartbeat1\" category=\"EVENT\" name=\"heartbeat\" />\n"
"						<DataItem type=\"PROGRAM\" id=\"####pgm\" category=\"EVENT\" name=\"program\"/>\n"
"						<DataItem type=\"TOOL_ID\" id=\"####tid\" category=\"EVENT\" name=\"tools\"/>\n"
"						<DataItem type=\"PART_ID\" id=\"####pid\" category=\"EVENT\" name=\"partid\"/>\n"
"						<DataItem type=\"PART_ID\" id=\"####lid\" category=\"EVENT\" name=\"lotid\"/>\n"
"						<DataItem type=\"PART_ID\" id=\"####lotsize\" category=\"EVENT\" name=\"lotsize\"/>\n"
"						<DataItem type=\"PART_ID\" id=\"####lotnum\" category=\"EVENT\" name=\"lotnum\"/>\n"
"						<DataItem type=\"CONTROLLER_MODE\" id=\"####command\" category=\"EVENT\" name=\"command\"/>\n"
"						<DataItem type=\"CONTROLLER_MODE\" id=\"####commandnum\" category=\"EVENT\" name=\"cmdnum\"/>\n"
"						<DataItem type=\"MESSAGE\" category=\"EVENT\" id=\"####opcmt\" name=\"operator_cmt\" />\n"
"					</DataItems>\n"
"				</Controller>\n"
"			</Components>\n"
"		</Device>\n";

std::string AgentConfigurationEx::MTConnectCmdEchoSim_device = 
"			 <Device id=\"####id\" uuid=\"####xxx\" name=\"NNNNNN\">\n"
"			<Description>MTConnectCmd Echo Sim</Description>\n"
"			<DataItems>\n"
"				<DataItem id=\"####avail\" name=\"avail\"  type=\"AVAILABILITY\" category=\"EVENT\"/>\n"
"			</DataItems>\n"
"			<Components>\n"
"				<Axes id=\"####a1\" name=\"base\">\n"		
"					<Components>\n"	
"						<Rotary id=\"####d1\" name=\"SPINDLE\">\n"
"							<DataItems>\n"
"								<DataItem type=\"SPINDLE_SPEED\" id=\"####cs1\" category=\"SAMPLE\" name=\"Srpm\"  units=\"REVOLUTION/MINUTE\" nativeUnits=\"REVOLUTION/MINUTE\"/>\n"
"								<DataItem type=\"SPINDLE_SPEED\" id=\"####cso\" category=\"SAMPLE\" name=\"Sovr\" subType=\"OVERRIDE\" units=\"PERCENT\" nativeUnits=\"PERCENT\"/>\n"
"							</DataItems>\n"
"						</Rotary>\n"
"					</Components>\n"
"				</Axes>\n"
"				<Controller id=\"####cont\" name=\"controller\">\n"
"					<DataItems>\n"
"						<DataItem type=\"COMMUNICATIONS\" category=\"CONDITION\" id=\"####ccond\" name=\"comms_cond\" />\n"
"						<DataItem type=\"LOGIC_PROGRAM\" category=\"CONDITION\" id=\"####logic\" name=\"logic_cond\" />\n"
"						<DataItem type=\"SYSTEM\" category=\"CONDITION\" id=\"####system\" name=\"system_cond\" />\n"
"					</DataItems>\n"
"					<Components>\n"
"						<Path id=\"####path1\" name=\"path\">\n"
"							<DataItems>\n"
"								<DataItem type=\"CLOCK_TIME\" id=\"####prograTime\" category=\"EVENT\" name=\"programTime\"/>	\n"
"								<DataItem type=\"CLOCK_TIME\" id=\"####programDuration\" category=\"EVENT\" name=\"programDuration\"/>	\n"
"								<DataItem type=\"PROGRAM\" id=\"####pgm\" category=\"EVENT\" name=\"program\"/>\n"
"								<DataItem type=\"EXECUTION\" id=\"####exec\" category=\"EVENT\" name=\"execution\"/>\n"
"								<DataItem type=\"CONTROLLER_MODE\" id=\"####mode\" category=\"EVENT\" name=\"controllermode\"/>\n"
"								<DataItem type=\"PATH_FEEDRATE\" id=\"####pfo1\" category=\"SAMPLE\" name=\"path_feedrateovr\" units=\"PERCENT\" nativeUnits=\"PERCENT\" subType=\"OVERRIDE\" />\n"
"        						<DataItem type=\"CODE\"          id=\"####heartbeat\" category=\"EVENT\"    name=\"heartbeat\" />\n"
"								<DataItem type=\"CONTROLLER_MODE\" id=\"####command\" category=\"EVENT\" name=\"cmd_echo\"/>\n"
"								<DataItem type=\"CONTROLLER_MODE\" id=\"####commandnum\" category=\"EVENT\" name=\"cmdnum_echo\"/>\n"
"								<DataItem type=\"PART_COUNT\" id=\"####pcgood\" category=\"EVENT\" name=\"partcountgood\" />\n"
"								<DataItem type=\"PART_COUNT\" id=\"####pcbad\" category=\"EVENT\" name=\"partcountbad\" />\n"
"								<DataItem type=\"PART_COUNT\" id=\"####pc\" category=\"EVENT\" name=\"partcount\" />\n"
"								<DataItem type=\"PART_ID\" id=\"####pid\" category=\"EVENT\" name=\"partid\"/>\n"
" 							</DataItems>\n"
"						</Path>\n"
"					</Components>\n"
"				</Controller>\n"
"				<Systems id=\"####systems\" name=\"systems\">\n"
"					<Components>\n"
"						<Electric name=\"electric\" id=\"####elec\">\n"
"							<DataItems>\n"
"								<DataItem type=\"POWER_STATE\" category=\"EVENT\" id=\"####pwr\" name=\"power\" />\n"
"								<DataItem type=\"TEMPERATURE\" category=\"CONDITION\" id=\"####pwrt\" name=\"electric_temp\" />\n"
"							</DataItems>\n"
"						</Electric>\n"
"					</Components>\n"
"				</Systems>\n"
"			</Components>\n"
"		</Device>\n";



std::string AgentConfigurationEx::KPIEcho_device = 
"	<Device id=\"####M1\" uuid=\"####xxx\" name=\"NNNNNN\">\n"
"			<Description>KPI Agent Summarizing Factory ####</Description>\n"
"			<DataItems>\n"
"				<DataItem id=\"####avail\" name=\"avail\"  type=\"AVAILABILITY\" category=\"EVENT\"/>\n"
"			</DataItems>\n"
"			<Components>\n"
"				<Controller id=\"####cont\" name=\"controller\">\n"
"					<DataItems>\n"
"						<DataItem type=\"CLOCK_TIME\" id=\"####factoryTime\" category=\"EVENT\" name=\"factoryTime\"/>	\n"
"						<DataItem type=\"CLOCK_TIME\" id=\"####TotalBusyTime\" category=\"EVENT\" name=\"TotalBusyTime\"/>	\n"
"						<DataItem type=\"CLOCK_TIME\" id=\"####TotalFaultTime\" category=\"EVENT\" name=\"TotalFaultTime\"/>	\n"
"						<DataItem type=\"CLOCK_TIME\" id=\"####TotalIdleTime\" category=\"EVENT\" name=\"TotalIdleTime\"/>	\n"				
"						<DataItem type=\"CLOCK_TIME\" id=\"####ShiftStart\" category=\"EVENT\" name=\"ShiftStart\"/>	\n"				
"						<DataItem type=\"CLOCK_TIME\" id=\"####ShiftNow\" category=\"EVENT\" name=\"ShiftNow\"/>	\n"				
"						<DataItem type=\"CLOCK_TIME\" id=\"####ShiftEnd\" category=\"EVENT\" name=\"ShiftEnd\"/>	\n"				
"						<DataItem type=\"CODE\"       id=\"####heartbeatid\" category=\"EVENT\" name=\"heartbeat\" />\n"
"						<DataItem type=\"PART_ID\" id=\"####parts\" category=\"EVENT\" name=\"AllParts\"/>\n"
"						<DataItem type=\"PART_ID\" id=\"####quantities\" category=\"EVENT\" name=\"AllQuantities\"/>\n"
"						<DataItem type=\"PART_ID\" id=\"####QueuedPartids\" category=\"EVENT\" name=\"QueuedPartids\"/>\n"
"						<DataItem type=\"PART_ID\" id=\"####ActivePartids\" category=\"EVENT\" name=\"ActivePartids\"/>\n"
"						<DataItem type=\"PART_ID\" id=\"####finished\" category=\"EVENT\" name=\"AllFinished\"/>\n"
"						<DataItem type=\"LINE\" id=\"####throughput\" category=\"EVENT\" name=\"Throughput\" />\n"
"						<DataItem type=\"LINE\" id=\"####yield\" category=\"EVENT\" name=\"Yield\" />\n"
"						<DataItem type=\"LINE\" id=\"####bottleneck\" category=\"EVENT\" name=\"Bottleneck\" />\n"
"					</DataItems>\n"
"				</Controller>\n"
"			</Components>\n"
"		</Device>\n";


static std::string GetTimeStamp()
{
	char aBuffer[256];
	SYSTEMTIME st;
	GetSystemTime(&st);
	sprintf(aBuffer, "%4d-%02d-%02dT%02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, 
		st.wMinute, st.wSecond);
	return aBuffer;
}
std::string	AgentConfigurationEx::WriteCmdedDeviceXML(std::vector<std::string> devicenames)
{
	return  WriteDeviceXML(devicenames,MTConnectCmdEchoSim_device);
}

std::string	AgentConfigurationEx::WriteCmdDeviceXML(std::vector<std::string> devicenames)
{
	return  WriteDeviceXML(devicenames,MTConnectCmdSim_device);
}

std::string	AgentConfigurationEx::WriteDeviceXML(std::vector<std::string> devicenames, std::string xmlcofig)
{
	std::string contents;
	// Generate Devices.xml file with all devices in it.
	for(UINT i=0; i<devicenames.size(); i++)
	{
		std::string config = xmlcofig; 
		ReplaceAll(config,"####", devicenames[i]); 
		config=ReplaceOnce(config,"name=\"NNNNNN\"","name=\""+devicenames[i]+"\"");
		contents+=config+"\n";
	}
	return contents;
}

HRESULT	AgentConfigurationEx::WriteDevicesFile(std::string xmlFile, std::string destFolder, std::string devicesXML)
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

	if(!xmlFile.empty())
		WriteFile(destFolder + xmlFile  , contents);
	DWORD filesize;
	if(FAILED(File.Size(destFolder + xmlFile, filesize)))
			ErrMsg("Devices xml file write failed\n");
	return 0;
}

LRESULT AgentConfigurationEx::WriteAgentCfgFile(std::string cfgfile, std::string xmlFile, std::string destFolder)
{
	// Generate agent.cfg file with all devices in it.
	std::string cfg;
	cfg= "Devices = " + xmlFile + "\n";
	cfg+="ServiceName = "+ Globals.ServerName +"\n"; // MTConnectAgent\n";
	cfg+="Port = "+ Globals.HttpPort +"\n";

	cfg+="CheckpointFrequency=10000\n";
	cfg+="AllowPut=true\n";
	
	cfg+="Adapters \n{\n";
	//for(UINT i=0; i<devices.size(); i++)
	//{
	//	if(ports[i]!="0")
	//	{
	//		cfg+="\t"+names[i] + "\n";
	//		cfg+="\t{\n";
	//		cfg+="\t\tHost = " + ips[i] +  "\n";
	//		cfg+="\t\tPort = " + ports[i] + "\n";
	//		cfg+="\t\tDeviceXml = " + ExtractFiletitle(devices[i]) + "\n";
	//		cfg+="\t}\n";
	//	}
	//}
	cfg+="}\n";

	cfg+="# Logger Configuration\n";
	cfg+="logger_config\n";
	cfg+="{\n";
//	cfg+="\tlogging_level = debug\n";
	cfg+="\tlogging_level = fatal\n";
//	cfg+="\toutput = cout\n";
	cfg+="\toutput =  file debug.log\n";

	

	cfg+="}\n";
	if(!cfgfile.empty())
		WriteFile(destFolder +  cfgfile, cfg);
	return 0;
}

