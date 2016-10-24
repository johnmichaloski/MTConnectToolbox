//
// AgentCfg.h
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#include "StdAfx.h"

#include "AgentCfg.h"
#include "StdStringFcn.h"
#include "config.hpp"
static TCHAR strFilter[] = _T("CFG Files (*.cfg)\0*.cfg\0");


CAgentCfg::CAgentCfg(void)
{
	Clear();
}

CAgentCfg::~CAgentCfg(void)
{
}
static std::string GetFanucDeviceXml(std::string config)
{
	std::string xml;
	config=MakeLower(config);
	xml="		<Device id=\"####d1\" uuid=\"####xxx\" name=\"NNNNNN\">\n"
		"     			<Description serialNumber=\"####\">FanucCNC</Description>\n"
		" 			<DataItems>\n"
		"				<DataItem id=\"####avail1\" name=\"avail\"  type=\"AVAILABILITY\" category=\"EVENT\"/>\n"
		"			</DataItems>\n"
		"			<Components>\n"
		"				<Axes id=\"####a1\" name=\"base\">\n"
		"			<Components>\n";

	if(config.find("x")!= std::string::npos)
		xml+="						<Linear id=\"####x1\" name=\"X\">\n"
		"							<DataItems>\n"
		"								<DataItem type=\"POSITION\" subType=\"ACTUAL\" id=\"####xp1\" category=\"SAMPLE\" name=\"Xabs\" units=\"MILLIMETER\" nativeUnits=\"MILLIMETER\" coordinateSystem=\"MACHINE\"/>\n"
		"							</DataItems>\n"
		"						</Linear>\n";

	if(config.find("y")!= std::string::npos)
		xml+="						<Linear id=\"####y1\" name=\"Y\">\n"
		"							<DataItems>\n"
		"								<DataItem type=\"POSITION\" subType=\"ACTUAL\" id=\"####yp1\" category=\"SAMPLE\" name=\"Yabs\" units=\"MILLIMETER\" nativeUnits=\"MILLIMETER\" coordinateSystem=\"MACHINE\"/>\n"
		"							</DataItems>\n"
		"						</Linear>\n";

	if(config.find("z")!= std::string::npos)
		xml+="						<Linear id=\"####z1\" name=\"Z\">\n"
		"							<DataItems>\n"
		"								<DataItem type=\"POSITION\" subType=\"ACTUAL\" id=\"####zp1\" category=\"SAMPLE\" name=\"Zabs\" units=\"MILLIMETER\" nativeUnits=\"MILLIMETER\" coordinateSystem=\"MACHINE\"/>\n"
		"							</DataItems>\n"
		"						</Linear>\n";

	if(config.find("a")!= std::string::npos)
		xml+="						<Rotary id=\"####d1\" name=\"A\">\n"
		"							<DataItems>\n"
		"								<DataItem type=\"POSITION\" id=\"####a\" category=\"SAMPLE\" name=\"Aabs\" subType=\"ACTUAL\" nativeUnits=\"DEGREE\" />\n"
		"							</DataItems>\n"
		"						</Rotary>\n";

	if(config.find("b")!= std::string::npos)
		xml+="						<Rotary id=\"####d1\" name=\"B\">\n"
		"							<DataItems>\n"
		"								<DataItem type=\"POSITION\" id=\"####b\" category=\"SAMPLE\" name=\"Babs\" subType=\"ACTUAL\" nativeUnits=\"DEGREE\" />\n"
		"							</DataItems>\n"
		"						</Rotary>\n";

	if(config.find("c")!= std::string::npos)
		xml+="						<Rotary id=\"####d1\" name=\"C\">\n"
		"							<DataItems>\n"
		"								<DataItem type=\"POSITION\" id=\"####c\" category=\"SAMPLE\" name=\"Cabs\" subType=\"ACTUAL\" nativeUnits=\"DEGREE\" />\n"
		"							</DataItems>\n"
		"						</Rotary>\n";

	if(config.find("u")!= std::string::npos)
		xml+="						<Rotary id=\"####d1\" name=\"U\">\n"
		"							<DataItems>\n"
		"								<DataItem type=\"POSITION\" id=\"####u\" category=\"SAMPLE\" name=\"Uabs\" subType=\"ACTUAL\" nativeUnits=\"DEGREE\" />\n"
		"							</DataItems>\n"
		"						</Rotary>\n";

	if(config.find("v")!= std::string::npos)
		xml+="						<Rotary id=\"####d1\" name=\"V\">\n"
		"							<DataItems>\n"
		"								<DataItem type=\"POSITION\" id=\"####v\" category=\"SAMPLE\" name=\"Vabs\" subType=\"ACTUAL\" nativeUnits=\"DEGREE\" />\n"
		"							</DataItems>\n"
		"						</Rotary>\n";

	xml+="						<Rotary id=\"####d1\" name=\"D\">\n"
		"							<DataItems>\n"
		"								<DataItem type=\"SPINDLE_SPEED\" id=\"####cs1\" category=\"SAMPLE\" name=\"Srpm\"  units=\"REVOLUTION/MINUTE\" nativeUnits=\"REVOLUTION/MINUTE\"/>\n"
		"								<DataItem type=\"SPINDLE_SPEED\" id=\"####cso1\" category=\"SAMPLE\" name=\"Sovr\" subType=\"OVERRIDE\" units=\"PERCENT\" nativeUnits=\"PERCENT\"/>\n"
		"							</DataItems>\n"
		"						</Rotary>\n"
		"					</Components>\n"
		"				</Axes>\n"
		"				<Controller id=\"####cont1\" name=\"controller\">\n"
		"					<Components>\n"
		"						<Path id=\"####path1\" name=\"path\">\n"
		"							<DataItems>\n"
		"								<DataItem type=\"PROGRAM\" id=\"####pgm1\" category=\"EVENT\" name=\"program\"/>\n"
		"								<DataItem type=\"PART_COUNT\" id=\"####pc1\" category=\"EVENT\" name=\"PartCountAct\" />\n"
		"								<DataItem type=\"PATH_FEEDRATE\" id=\"####pf1\" category=\"SAMPLE\" name=\"path_feedratefrt\" units=\"MILLIMETER/SECOND\" subType=\"ACTUAL\" coordinateSystem=\"WORK\"/>\n"
		"								<DataItem type=\"PATH_FEEDRATE\" id=\"####pfo1\" category=\"SAMPLE\" name=\"path_feedrateovr\" units=\"PERCENT\" nativeUnits=\"PERCENT\" subType=\"OVERRIDE\" />\n"
		"								<DataItem type=\"TOOL_ID\" id=\"####tid1\" category=\"EVENT\" name=\"Tool_number\"/>\n"
		"								<DataItem type=\"CODE\"     id=\"####hb1\"  category=\"EVENT\" name=\"heartbeat\" />\n"
		"								<DataItem type=\"EXECUTION\" id=\"####exec1\" category=\"EVENT\" name=\"execution\"/>\n"
		"								<DataItem type=\"CONTROLLER_MODE\" id=\"####mode1\" category=\"EVENT\" name=\"controllermode\"/>\n"
		"							</DataItems>\n"
		"						</Path>\n"
		"					</Components>\n"
		"				</Controller>\n"
		"				<Systems id=\"####systems1\" name=\"systems\">\n"
		"					<Components>\n"
		"						<Electric name=\"electric\" id=\"####elec1\">\n"
		"							<DataItems>\n"
		"								<DataItem type=\"POWER_STATE\" category=\"EVENT\" id=\"####pwr1\" name=\"power\" />\n"
		"							</DataItems>\n"
		"						</Electric>\n"
		"					</Components>\n"
		"				</Systems>\n"
		"			</Components>\n"
		"		</Device>\n";
	return xml;
}



static std::string GetTimeStamp()
{
	char aBuffer[256];
	SYSTEMTIME st;
	GetSystemTime(&st);
	sprintf(aBuffer, "%4d-%02d-%02dT%02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, 
		st.wMinute, st.wSecond);
	return aBuffer;
}

void CAgentCfg::Clear() 
{ 

	ips.clear(); 
	ports.clear(); 
	rates.clear(); 
	debugs.clear(); 
	devices.clear(); 
	names.clear(); 
	alldevices.clear(); 
	_cfgFolder=::ExeDirectory() + "Devices\\";
	_xmlFile="devices.xml";
	_agenthttpport=80;
	_cfgFile="agent.cfg";
	_destFolder=::ExeDirectory();
	_serviceName="MTConnectAgent";
}
void CAgentCfg::AddFanucDevice(std::string devicename,std::string config)
{
	_fanucconfigs.push_back(config);
	names.push_back(devicename);
}


void CAgentCfg::AddDevice(std::string cfgname, std::string devicename, std::string ip, std::string port)
{
	names.push_back(cfgname); 
	devices.push_back(devicename); 
	ips.push_back(ip); 
	ports.push_back(port); 
}
void CAgentCfg::UpdateDevice(std::string cfgname, 
							 std::string ip, 
							 std::string port)
{
	std::vector<std::string>::iterator it = std::find(names.begin(), names.end(), cfgname);
	if(it==names.end())
		return;
	int i= it - names.begin() ;
	if(i>=names.size())
		return;
	if(!ip.empty())
		ips[i]=ip;
	if(!port.empty())
		ports[i]=port;
}

std::string CAgentCfg::ReadXmlDevicesFile(std::string name)
{
	std::string contents;
	ReplaceAll(name, ".", "\\");
	if(!EndsWith(_cfgFolder,"\\"))
		_cfgFolder+="\\";

	std::string filename(_cfgFolder + name + ".txt");
	std::ifstream in( filename.c_str() );
	if(!in)
	{
		filename = RecursiveFileFind(_cfgFolder, name + ".txt");
	}

	ReadFile(filename,contents);
	return contents;
}
std::string CAgentCfg::FindXmlDevicesFilePath(std::string name)
{
	std::string contents;
	ReplaceAll(name, ".", "\\");
	if(!EndsWith(_cfgFolder,"\\"))
		_cfgFolder+="\\";

	std::string filename(_cfgFolder + name + ".txt");
	std::ifstream in( filename.c_str() );
	if(!in)
	{
		filename = RecursiveFileFind(_cfgFolder, name + ".txt");
	}
	return filename;
}

long CAgentCfg::WriteCfgFile(std::string cfgfile, std::string xmlFile, std::string destFolder)
{
	WriteDevicesFile(xmlFile,   destFolder);
	WriteAgentCfgFile(cfgfile, xmlFile,  destFolder);
	return 0;
}
long	CAgentCfg::WriteDevicesFile(std::string xmlFile,  std::string destFolder)
{
	std::string contents;
	contents+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	contents+="<MTConnectDevices xmlns=\"urn:mtconnect.org:MTConnectDevices:1.1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"urn:mtconnect.org:MTConnectDevices:1.1 http://www.mtconnect.org/schemas/MTConnectDevices_1.1.xsd\">\n";
	contents+=StdStringFormat("<Header bufferSize=\"130000\" instanceId=\"1\" creationTime=\"%s\" sender=\"local\" version=\"1.1\"/>\n",
		GetTimeStamp().c_str());
	contents+="<Devices>\n";

	// Check all machine names unique

	// Generate Devices.xml file with all devices in it.
	for(UINT i=0; i<names.size(); i++)
	{
		std::string config = GetFanucDeviceXml(_fanucconfigs[i]); // ReadXmlDevicesFile(devices[i]);
		ReplaceAll(config,"####", names[i]); 
		config=ReplaceOnce(config,"name=\"NNNNNN\"","name=\""+names[i]+"\"");
		contents+=config+"\n";
	}
	contents+="</Devices>\n";
	contents+="</MTConnectDevices>\n";

	if(!xmlFile.empty())
		WriteFile(destFolder + "\\" + xmlFile  , contents);
	return 0;
}
long CAgentCfg::WriteAgentCfgFile(std::string cfgfile, std::string xmlFile, std::string destFolder)
{
	// Generate agent.cfg file with all devices in it.
	std::string cfg;
	cfg= "Devices = " + xmlFile + "\n";
	cfg+="Port = " + StdStringFormat("%d",_agenthttpport) + "\n";
	cfg+="ServiceName = " + _serviceName + "\n"; // MTConnectAgent\n";
	cfg+="CheckpointFrequency=10000\n";
	cfg+="AllowPut=true\n";

	cfg+="Adapters \n{\n";
	for(UINT i=0; i<devices.size(); i++)
	{
		if(ports[i]!="0")
		{
			cfg+="\t"+names[i] + "\n";
			cfg+="\t{\n";
			cfg+="\t\tHost = " + ips[i] +  "\n";
			cfg+="\t\tPort = " + ports[i] + "\n";
			cfg+="\t\tDeviceXml = " + ExtractFiletitle(devices[i]) + "\n";
			cfg+="\t}\n";
		}
	}
	cfg+="}\n";

	cfg+="# Logger Configuration\n";
	cfg+="logger_config\n";
	cfg+="{\n";
	cfg+="\tlogging_level = fatal\n";
	cfg+="\toutput = cout\n";
	cfg+="}\n";
	if(!cfgfile.empty())
		WriteFile(destFolder + "\\" + cfgfile, cfg);
	return 0;
}

void CAgentCfg::Run()
{
	char *argv[4] = {
		"agent.exe",
		"run",
		"" 
	};
	//std::string file = _cfgFolder +_cfgFile;
	std::string file = _cfgFile;
	argv[2]= new char[file.size() + 1]; // leak
	strcpy(argv[2], file.c_str()); 

	WriteCfgFile(_cfgFile, _xmlFile, _destFolder);
	SetCurrentDirectory(_destFolder.c_str());
	if(_config==NULL)
		_config = new AgentConfiguration();
	_config->main(3, (const char **) argv);
}


inline HRESULT CreateChildProcess(const TCHAR *argv)
{ 
	TCHAR  szCmdline[1028];
	BOOL bSuccess = FALSE; 
	PROCESS_INFORMATION piProcInfo; 
	STARTUPINFO siStartInfo;
	_stprintf( szCmdline, _T("%s"), argv );

	// Set up members of the PROCESS_INFORMATION structure. 

	ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

	// Set up members of the STARTUPINFO structure. 
	// This structure specifies the STDIN and STDOUT handles for redirection.

	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO); 
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
	siStartInfo.wShowWindow = SW_HIDE;

	// Create the child process. 

	bSuccess = CreateProcess(NULL, 
		szCmdline,     // command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited 
		0,             // creation flags 
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&siStartInfo,  // STARTUPINFO pointer 
		&piProcInfo);  // receives PROCESS_INFORMATION 

	// If an error occurs, exit the application. 
	if ( ! bSuccess ) 
		return E_FAIL;
	return S_OK;

}

void CAgentCfg::SpawnProcess()
{
	WriteCfgFile(_cfgFile, _xmlFile, _destFolder);

	std::string sCmdLine;
	sCmdLine+=::ExeDirectory() + "agent.exe ";
	sCmdLine+= " debug ";
	sCmdLine+=  _cfgFile ;
	CreateChildProcess(sCmdLine.c_str());
}
