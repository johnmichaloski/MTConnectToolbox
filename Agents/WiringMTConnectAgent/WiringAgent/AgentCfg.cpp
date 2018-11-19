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

static std::string GetTimeStamp ( )
{
  char       aBuffer[256];
  SYSTEMTIME st;

  GetSystemTime(&st);
  sprintf(aBuffer, "%4d-%02d-%02dT%02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour,
    st.wMinute, st.wSecond);
  return aBuffer;
}
CAgentCfg::CAgentCfg(void)
{
  Clear( );
}
CAgentCfg::~CAgentCfg(void)
{ }
void CAgentCfg::Clear ( )
{
  ips.clear( );
  ports.clear( );
  rates.clear( );
  debugs.clear( );
  devices.clear( );
  names.clear( );
  alldevices.clear( );
  _cfgFolder     = ::ExeDirectory( ) + "Devices\\";
  _xmlFile       = "devices.xml";
  _agenthttpport = 80;
  _cfgFile       = "agent.cfg";
  _destFolder    = ::ExeDirectory( );
  _serviceName   = "MTConnectAgent";
}
void CAgentCfg::AddDevice (std::string cfgname, std::string devicename, std::string ip, std::string port)
{
  names.push_back(cfgname);
  devices.push_back(devicename);
  ips.push_back(ip);
  ports.push_back(port);
}
void CAgentCfg::UpdateDevice (std::string cfgname,
  std::string                             ip,
  std::string                             port)
{
  std::vector<std::string>::iterator it = std::find(names.begin( ), names.end( ), cfgname);

  if ( it == names.end( ) )
  {
    return;
  }
  int i = it - names.begin( );

  if ( i >= names.size( ) )
  {
    return;
  }

  if ( !ip.empty( ) )
  {
    ips[i] = ip;
  }

  if ( !port.empty( ) )
  {
    ports[i] = port;
  }
}
std::string CAgentCfg::ReadXmlDevicesFile (std::string name)
{
  std::string contents;

  ReplaceAll(name, ".", "\\");

  if ( !EndsWith(_cfgFolder, "\\") )
  {
    _cfgFolder += "\\";
  }

  std::string   filename(_cfgFolder + name + ".txt");
  std::ifstream in(filename.c_str( ) );

  if ( !in )
  {
    filename = RecursiveFileFind(_cfgFolder, name + ".txt");
  }

  ReadFile(filename, contents);
  return contents;
}
std::string CAgentCfg::FindXmlDevicesFilePath (std::string name)
{
  std::string contents;

  ReplaceAll(name, ".", "\\");

  if ( !EndsWith(_cfgFolder, "\\") )
  {
    _cfgFolder += "\\";
  }

  std::string   filename(_cfgFolder + name + ".txt");
  std::ifstream in(filename.c_str( ) );

  if ( !in )
  {
    filename = RecursiveFileFind(_cfgFolder, name + ".txt");
  }
  return filename;
}
LRESULT CAgentCfg::WriteCfgFile (std::string cfgfile, std::string xmlFile, std::string destFolder)
{
  WriteDevicesFile(xmlFile, destFolder);
  WriteAgentCfgFile(cfgfile, xmlFile, destFolder);
  return 0;
}
LRESULT CAgentCfg::WriteDevicesFile (std::string xmlFile, std::string destFolder)
{
  std::string contents;

  contents += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  contents += "<MTConnectDevices xmlns=\"urn:mtconnect.org:MTConnectDevices:1.1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"urn:mtconnect.org:MTConnectDevices:1.1 http://www.mtconnect.org/schemas/MTConnectDevices_1.1.xsd\">\n";
  contents += StdStringFormat("<Header bufferSize=\"130000\" instanceId=\"1\" creationTime=\"%s\" sender=\"local\" version=\"1.1\"/>\n",
    GetTimeStamp( ).c_str( ) );
  contents += "<Devices>\n";

  // Check all machine names unique

  // Generate Devices.xml file with all devices in it.
  for ( UINT i = 0; i < devices.size( ); i++ )
  {
    std::string config = ReadXmlDevicesFile(devices[i]);
    ReplaceAll(config, "####", names[i]);
    config    = ReplaceOnce(config, "name=\"NNNNNN\"", "name=\"" + names[i] + "\"");
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
LRESULT CAgentCfg::WriteAgentCfgFile (std::string cfgfile, std::string xmlFile, std::string destFolder)
{
  // Generate agent.cfg file with all devices in it.
  std::string cfg;

  cfg  = "Devices = " + xmlFile + "\n";
  cfg += "Port = " + StdStringFormat("%d", _agenthttpport) + "\n";
  cfg += "ServiceName = " + _serviceName + "\n"; // MTConnectAgent\n";
  cfg += "CheckpointFrequency=10000\n";
  cfg += "AllowPut=true\n";

  cfg += "Adapters \n{\n";

  for ( UINT i = 0; i < devices.size( ); i++ )
  {
    if ( ports[i] != "0" )
    {
      cfg += "\t" + names[i] + "\n";
      cfg += "\t{\n";
      cfg += "\t\tHost = " + ips[i] + "\n";
      cfg += "\t\tPort = " + ports[i] + "\n";
      cfg += "\t\tDeviceXml = " + ExtractFiletitle(devices[i]) + "\n";
      cfg += "\t}\n";
    }
  }
  cfg += "}\n";

  cfg += "# Logger Configuration\n";
  cfg += "logger_config\n";
  cfg += "{\n";
  cfg += "\tlogging_level = fatal\n";
  cfg += "\toutput = cout\n";
  cfg += "}\n";

  if ( !cfgfile.empty( ) )
  {
    WriteFile(destFolder + "\\" + cfgfile, cfg);
  }
  return 0;
}
void CAgentCfg::Run ( )
{
  char *argv[4] =
  {
    "agent.exe",
    "run",
    ""
  };

  // std::string file = _cfgFolder +_cfgFile;
  std::string file = _cfgFile;

  argv[2] = new char[file.size( ) + 1]; // leak
  strcpy(argv[2], file.c_str( ) );

  WriteCfgFile(_cfgFile, _xmlFile, _destFolder);
  SetCurrentDirectory(_destFolder.c_str( ) );

  if ( _config == NULL )
  {
    _config = new AgentConfiguration( );
  }
  _config->main(3, (const char **) argv);
}
inline HRESULT CreateChildProcess (const TCHAR *argv)
{
  TCHAR               szCmdline[1028];
  BOOL                bSuccess = FALSE;
  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO         siStartInfo;

  _stprintf(szCmdline, _T("%s"), argv);

  // Set up members of the PROCESS_INFORMATION structure.

  ZeroMemory(&piProcInfo, sizeof( PROCESS_INFORMATION ) );

  // Set up members of the STARTUPINFO structure.
  // This structure specifies the STDIN and STDOUT handles for redirection.

  ZeroMemory(&siStartInfo, sizeof( STARTUPINFO ) );
  siStartInfo.cb          = sizeof( STARTUPINFO );
  siStartInfo.dwFlags    |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  siStartInfo.wShowWindow = SW_HIDE;

  // Create the child process.

  bSuccess = CreateProcess(NULL,
    szCmdline,         // command line
    NULL,              // process security attributes
    NULL,              // primary thread security attributes
    TRUE,              // handles are inherited
    0,                 // creation flags
    NULL,              // use parent's environment
    NULL,              // use parent's current directory
    &siStartInfo,      // STARTUPINFO pointer
    &piProcInfo);      // receives PROCESS_INFORMATION

  // If an error occurs, exit the application.
  if ( !bSuccess )
  {
    return E_FAIL;
  }
  return S_OK;
}
void CAgentCfg::SpawnProcess ( )
{
  WriteCfgFile(_cfgFile, _xmlFile, _destFolder);

  std::string sCmdLine;
  sCmdLine += ::ExeDirectory( ) + "agent.exe ";
  sCmdLine += " debug ";
  sCmdLine += _cfgFile;
  CreateChildProcess(sCmdLine.c_str( ) );
}
void CAgentCfg::OnSaveCfg ( )
{
  CFileDialog dlg(FALSE,
                  _T("*"),
                  ( ::ExeDirectory( ) + "Agent.cfg" ).c_str( ),
                  OFN_HIDEREADONLY,
                  strFilter);

  if ( dlg.DoModal( ) != IDOK )
  {
    return;
  }

  WriteCfgFile(ExtractFilename(dlg.m_szFileName), "", ExtractDirectory(dlg.m_szFileName) );
}
