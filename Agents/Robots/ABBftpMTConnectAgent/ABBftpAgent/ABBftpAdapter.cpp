//

// ABBAdapter.cpp
//

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#include "stdafx.h"
#define BOOST_ALL_NO_LIB

#include "ABBftpAdapter.h"
#include "ABBftpAgent.h"

#include "config.hpp"
#include "Device.hpp"
#include "Agent.hpp"
#include "ATLComTime.h"
#include <fstream> // ifstream
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <share.h>
#include <string>
#include <vector>
#include <wininet.h> //you will also need to link to wininet.lib

#include "StdStringFcn.h"
#include "Globals.h"
#include "Logger.h"
#include "WinTricks.h"
#include "Logger.h"

using namespace NIST;
#pragma comment(lib, "Wininet.lib")

HRESULT AdapterT::ErrorMessage (std::string errmsg)
{
  AtlTrace(errmsg.c_str( ) );
  GLogger.LogMessage(errmsg, LOWERROR);

  if ( Globals.Debug > 0 )
  {
//		EventLogger.LogEvent(errmsg);
  }
  return E_FAIL;
}
HRESULT AdapterT::DebugMessage (std::string errmsg)
{
  AtlTrace(errmsg.c_str( ) );
  GLogger.LogMessage(errmsg, WARNING);

  if ( Globals.Debug > 3 )
  {
//		EventLogger.LogEvent(errmsg);
  }
  return E_FAIL;
}
// ///////////////////////////////////////////////////////////////////
static void trans_func (unsigned int u, EXCEPTION_POINTERS *pExp)
{
  std::string errmsg = StdStringFormat("COpcAdapter In trans_func - Code = 0x%x\n", pExp->ExceptionRecord->ExceptionCode);

  OutputDebugString(errmsg.c_str( ) );
  throw std::exception(errmsg.c_str( ), pExp->ExceptionRecord->ExceptionCode);
}
AdapterT::AdapterT(AgentConfigurationEx *mtcagent, // mtconnect agent
  NIST::Config &                          config,
  std::string                            machine,  // ip address or pc name
  std::string                            device) :

  _agentconfig(mtcagent),
  _device(device),
  _config(config)
{
  inifile             = Globals.inifile.c_str( );
  _nServerRate        = 1000;
  _nQueryServerPeriod = 10000;
}
AdapterT::~AdapterT(void)
{ }
void AdapterT::SetMTCTagValue (std::string tag, std::string value)
{
#if 1
  items.SetTag(tag, value);

  Agent *agent = _agentconfig->getAgent( );

  if ( agent == NULL )
  {
    ErrorMessage(StdStringFormat("AdapterT::SetMTCTagValue for Device=%s  Tag=%s - NULL Agent Pointer\n", _device.c_str( ),tag.c_str( ) ) );
    return;
  }

  Device *pDev = agent->getDeviceByName(_device);

  if ( pDev == NULL )
  {
    ErrorMessage(StdStringFormat("AdapterT::SetMTCTagValue for Device=%s Tag=%s - NULL Device Pointer\n", _device.c_str( ), tag.c_str( ) ) );
    return;
  }
  DataItem *di = pDev->getDeviceDataItem(tag);

  if ( di != NULL )
  {
    std::string time = getCurrentTime(GMT_UV_SEC);
    agent->addToBuffer(di, value, time);
  }
  else
  {
    LOGONCE GLogger.LogMessage(StdStringFormat(" (%s) Could not find data item: %s  \n", _device.c_str( ), tag.c_str( ) ) );
  }
#endif
}
void AdapterT::Off ( )
{
  for ( int i = 0; i < items.size( ); i++ )
  {
    items[i]->_value     = "UNAVAILABLE";
    items[i]->_lastvalue = "UNAVAILABLE";
    SetMTCTagValue(items[i]->_tagname, "UNAVAILABLE");
  }

  // changed from unavailable. Causes agent to make everything unavailable.
  SetMTCTagValue("avail", "UNAVAILABLE");
  SetMTCTagValue("power", "OFF");
}
void AdapterT::On ( )
{
  SetMTCTagValue("avail", "AVAILABLE");
  SetMTCTagValue("power", "ON");
}
void ABBAdapter::CreateItem (std::string tag, std::string type)
{
  Item *item = new Item( );

  item->_type    = _T("Event");
  item->_tagname = tag;
  items.push_back(item);
}
void ABBAdapter::Dump ( )
{
  std::stringstream str;

  str << _device << ":ProductionLog= " << unc << std::endl;
  str << _device << ":QueryServer= " << _nQueryServerPeriod << std::endl;
  str << _device << ":ServerRate= " << _nServerRate << std::endl;
  str << _device << ":Simulation= " << _simulation << std::endl;
  str << _device << ":LastLogDate= " << LastLogDate.Format("%Y/%m/%d") << std::endl;

  OutputDebugString(str.str( ).c_str( ) );
}
void ABBAdapter::Config ( )
{
  // GLogger.LogMessage(StdStringFormat("COpcAdapter::Config() for IP = %s\n",_device.c_str()));
  std::string cfgfile = Globals.inifile;
  std::string sLastLogDate;

  try
  {
    if ( GetFileAttributesA(cfgfile.c_str( ) ) == INVALID_FILE_ATTRIBUTES )
    {
      throw std::exception("Could not find ini file \n");
    }

    unc = config.GetSymbolValue(_device + ".ProductionLog", File.ExeDirectory( ) + "ProductionLog.csv").c_str( );
    std::vector<std::string> logday = TrimmedTokenize(config.GetSymbolValue(_device + ".LastLogDate", "0/0/0").c_str( ), "/");

    if ( logday.size( ) < 2 )
    {
      throw std::exception("Bad log date");
    }
    LastLogDate         = COleDateTime(ConvertString(logday[2], 0), ConvertString(logday[0], 0), ConvertString(logday[1], 0), 0, 0, 0); // .ParseDateTime(sLastLogDate.c_str(), VAR_DATEVALUEONLY);
    _nServerRate        = config.GetSymbolValue(_device + ".ServerRate", Globals.ServerRate).toNumber<int>( );
    _nQueryServerPeriod = config.GetSymbolValue(_device + ".QueryServer", Globals.QueryServer).toNumber<int>( );
    _simulation         = config.GetSymbolValue(_device + ".Simulation", 0).toNumber<int>( );

    // Network share addition - in case user SYSTEM cant access remote file
    _User         = config.GetSymbolValue(_device + ".User", Globals._User).c_str( );
    _Password     = config.GetSymbolValue(_device + ".Pw",  Globals._Password).c_str( );
    _LocalShare   = config.GetSymbolValue(_device + ".LocalShare",  Globals._LocalShare).c_str( );
    _NetworkShare = config.GetSymbolValue(_device + ".NetworkShare",  Globals._NetworkShare).c_str( );
	_dDither = config.GetSymbolValue(_device + ".Dither", Globals.Dither).toNumber<double>( );
  }
  catch ( std::exception errmsg )
  {
    _agentconfig->AbortMsg(StdStringFormat("Could not find ini file for device %s\n", _device.c_str( ) ) );
  }
  catch ( ... )
  {
    _agentconfig->AbortMsg(StdStringFormat("Could not find ini file for device %s\n", _device.c_str( ) ) );
  }
  Dump( );
}
void ABBAdapter::Cycle ( )
{
  Config( );

  _set_se_translator(trans_func);      // correct thread?
  SetPriorityClass(GetCurrentProcess( ), ABOVE_NORMAL_PRIORITY_CLASS);

  int nHeartbeat = 0;
  CreateItem("heartbeat");
  CreateItem("program");
  CreateItem("controllermode");
  CreateItem("execution");
  CreateItem("power");
  CreateItem("avail");
  CreateItem("error");
  CreateItem("path_feedrateovr");
  CreateItem("operator");
  CreateItem("Srpm");
  CreateItem("Xabs");
  CreateItem("Yabs");
  CreateItem("Zabs");
  CreateItem("last_update");
#ifndef SIMPLE
  lastX.resize(lastsize,std::string()); 
  lastY.resize(lastsize,std::string()); 
  lastZ.resize(lastsize,std::string());
#endif
  Off( );

  // GLogger.LogMessage(StdStringFormat("COpcAdapter::Cycle() Enter Loop for IP = %s\n",_device.c_str()));
  _mRunning = true;
  Off( );

  while ( _mRunning )
  {
    try
    {
      // PING IP?
      SetMTCTagValue("heartbeat", StdStringFormat("%d", nHeartbeat++) );
      SetMTCTagValue("avail", "AVAILABLE");

      if ( FAILED(GatherDeviceData( ) ) )
      {
        ::Sleep(this->_nQueryServerPeriod);
        continue;
      }
      ::Sleep(_nServerRate);
    }
    catch ( std::exception e )
    {
      Off( );
    }
    catch ( ... )
    {
      Off( );
    }
  }
}

std::string FormatDateTime(COleDateTime d)
{
	if(Globals.DateFormat=="dd/mm/yyyy")
	{
		return StdStringFormat("%02d/%02d/%4d",  d.GetMonth(), d.GetDay(),d.GetYear()); 
	}
	else if(Globals.DateFormat=="yyyy/mm/dd")
	{
		return StdStringFormat("%4d/%02d/%02d", d.GetYear(), d.GetMonth(), d.GetDay()); 
	}
	return StdStringFormat("%4d/%02d/%02d", d.GetYear(), d.GetMonth(), d.GetDay()); 

}

COleDateTime GetDateTime (std::string s, std::string delimiter=",")
{
  std::string::size_type delimPos = s.find_first_of(delimiter, 0);

  if ( std::string::npos != delimPos )
  {
    s = s.substr(0, delimPos);
  }

  // parse 2012/11/03 Skip time
  int Year, Month, Day;

  if ( sscanf(s.c_str( ), "%4d-%02d-%02d", &Year, &Month, &Day) == 3 )
  { }
  else
  {
    throw std::exception("Unrecognized date-time format\n");
  }

  return COleDateTime(Year, Month, Day, 0, 0, 0);
}
// Format: 2015-03-11 17:04:39.998
boolean GetLogTimestamp (std::string s, COleDateTime & ts, std::string delimiter=",")
{
  std::string::size_type delimPos = s.find_first_of(delimiter, 0);

  if ( std::string::npos != delimPos )
  {
    s = s.substr(0, delimPos);
  }

  // parse 2012/11/03 Skip time
  int Year, Month, Day, Hour, Min, Sec;

  // Fixme: need to parse order or year month day etc.
  if ( sscanf(s.c_str( ), Globals.DateFormat.c_str(), &Year, &Month, &Day, &Hour, &Min, &Sec) == 6 )
  {
    ts = COleDateTime(Year, Month, Day, Hour, Min, Sec);
    return true;
  }
  return false;
}
static void Nop ( ) { }
HRESULT ABBAdapter::FailWithMsg (HRESULT hr, std::string errmsg)
{
  this->DebugMessage(errmsg);
  return hr;
}
HRESULT ABBAdapter::WarnWithMsg (HRESULT hr, std::string errmsg)
{
  this->DebugMessage(errmsg);
  return S_OK;
}
static bool getline (size_t & n, std::string & data, std::string & line)
{
  line.clear( );
  size_t N = data.size( );

  if ( n >= N )
  {
    return false;
  }

  size_t start = n;
  size_t end   = n;

  while ( end < N && data[end] != '\n' )
  {
    end++;
  }

  while ( end < N && data[end] == '\n' )
  {
    end++;
  }

  line = data.substr(start, end - start);
  n    = end; // skip over \n
  return true;

  // if((n=data.find_first_of('\n')) != std::string::npos)
  // {
  //	line=data.substr(0,n);
  //	data=data.substr(n+1);
  //	return true;
  // }
}
static bool getlinebackwards (long & n, std::string & data, std::string & line)
{
  line.clear( );

  if ( n < 0 )
  {
    return false;
  }

  long start = n;
  long end   = n;

  while ( end >= 0 && data[end] == '\n' )
  {
    end--;
  }

  while ( end >= 0 && data[end] != '\n' )
  {
    end--;
  }

  line = data.substr(end + 1, start - end);

  // OutputDebugString(line.c_str());
  n = end;
  return true;
}

// MTConnect
// EXECUTION	The execution status of the Controller. READY, ACTIVE, INTERRUPTED, FEED_HOLD, or STOPPED
// http://mtcup.org/wiki/Data_Item_Types:_EVENT
// EXECUTING STOPPED IDLE
// Or is it : IDLE, PAUSED, EXECUTING
void ABBAdapter::Run()
{
	items.SetTag("power", "ON");
	items.SetTag("avail", "AVAILABLE");
	items.SetTag("execution", "EXECUTING");
	items.SetTag("controllermode", "AUTOMATIC");
	items.SetTag("error", "");		
	items.SetTag("Srpm", "99") ;
}
void ABBAdapter::Paused()
{
	items.SetTag("power", "ON");
	items.SetTag("avail", "AVAILABLE");
	items.SetTag("execution", "PAUSED");
	items.SetTag("controllermode", "AUTOMATIC");
	items.SetTag("error", "");
	items.SetTag("Srpm", "0") ;
}
void ABBAdapter::Idle()
{
	items.SetTag("power", "ON");
	items.SetTag("avail", "AVAILABLE");
	items.SetTag("execution", "IDLE");
	items.SetTag("controllermode", "MANUAL");
	items.SetTag("error", "");
	items.SetTag("Srpm", "0") ;
}
void ABBAdapter::Setup()
{
	items.SetTag("power", "ON");
	items.SetTag("avail", "AVAILABLE");
	items.SetTag("execution", "PAUSED");
	items.SetTag("controllermode", "MANUAL");
	items.SetTag("error", "");
	items.SetTag("Srpm", "0") ;
	items.SetTag("Srpm", "0") ;
}
void ABBAdapter::Fault()
{
	items.SetTag("power", "ON");
	items.SetTag("avail", "AVAILABLE");
	items.SetTag("execution", "PAUSED");
	items.SetTag("controllermode", "MANUAL");
	items.SetTag("error", "FAULT");	
	items.SetTag("Srpm", "0") ;
}


void ABBAdapter::UpdateTags()
{
	for ( size_t i = 0; i < items.size( ); i++ )
	{
		if ( ( items[i]->_type == _T("Event") ) || ( items[i]->_type == _T("Sample") ) )
		{
			if ( items[i]->_value != items[i]->_lastvalue )
			{
				this->SetMTCTagValue(items[i]->_tagname, items[i]->_value);
				items[i]->_lastvalue = items[i]->_value;
			}
		}
	}
}
HRESULT ABBAdapter::GatherDeviceData ( )
{
  USES_CONVERSION;
  HRESULT      hr = S_OK;
  DWORD        filesize;
  COleDateTime modtime;
  try
  {
    std::string  filename = unc;
	COleDateTime today;
	std::string data;

	// The abb adapter updates the xyz position regularly?
#if 1
    if ( ( items.GetTag("execution", "PAUSED") == "EXECUTING" ) && ( items.GetTag("controllermode", "MANUAL") == "AUTOMATIC" ) )
    {
      double X = fmod(ConvertString<double>(items.GetTag("Xabs", "0.0"), 0.0) + 0.1, 100.0);
      double Y = fmod(ConvertString<double>(items.GetTag("Yabs", "0.0"), 0.0) + 0.1, 100.0);
      double Z = fmod(ConvertString<double>(items.GetTag("Zabs", "0.0"), 0.0) + 0.1, 100.0);
      SetMTCTagValue("Xabs", ConvertToString(X) );
      SetMTCTagValue("Yabs", ConvertToString(Y) );
      SetMTCTagValue("Zabs", ConvertToString(Z) );
    }
    //else
    //{
    //  SetMTCTagValue("Xabs", "0");
    //  SetMTCTagValue("Yabs", "10");
    //  SetMTCTagValue("Zabs", "20");
    //}
#endif
	if(unc.find("ftp")==0)
	{
		// ftp read of log file
		/**
		DWORD dwFileAttributes;
		FILETIME ftCreationTime;
		FILETIME ftLastAccessTime;
		FILETIME ftLastWriteTime;
		DWORD nFileSizeHigh;
		DWORD nFileSizeLow;
		DWORD dwReserved0;
		DWORD dwReserved1;
		CHAR   cFileName[ MAX_PATH ];
		CHAR   cAlternateFileName[ 14 ];*/
		WIN32_FIND_DATA FileData;

		HINTERNET m_hInternet;
		HINTERNET m_hConnection;

		bool overwrite=true;
		std::string ip = unc.substr(6); // skip ftp://
		ip = ip.substr( 0, ip.find_last_of( '/' )  );
		std::string remoteFile = unc.substr( unc.find_last_of( '/' ) + 1 );
		std::string localFile = File.ExeDirectory( ) + "LocalLogFileCopy.txt"; //  File.TempFileName( _device.c_str(), 99);

		m_hInternet = InternetOpen(("MTConnect" + _device).c_str() , INTERNET_OPEN_TYPE_DIRECT, 0, 0, 0);
		m_hConnection = InternetConnect(m_hInternet,ip.c_str(), INTERNET_DEFAULT_FTP_PORT, _User.c_str(), _Password.c_str(), INTERNET_SERVICE_FTP, 0, 0);
		// Use for computing last modification time
		FtpFindFirstFile(m_hConnection, remoteFile.c_str(), &FileData, INTERNET_FLAG_NEED_FILE, NULL);
		COleDateTime  filemodtime(FileData.ftLastWriteTime);
		if( lastftpmodtime == filemodtime)
		{
			InternetCloseHandle(m_hConnection);
			InternetCloseHandle(m_hInternet);
			return WarnWithMsg(E_PENDING, StdStringFormat("ftp remote log file Device %s  modtime unchanged %s\n", _device.c_str( ), lastmodtime.Format("%c") ) );; 
		}
		FtpGetFile(m_hConnection, remoteFile.c_str(), localFile.c_str(), (BOOL)overwrite, FILE_ATTRIBUTE_NORMAL, FTP_TRANSFER_TYPE_UNKNOWN, 0);
		lastftpmodtime=filemodtime;
		InternetCloseHandle(m_hConnection);
		InternetCloseHandle(m_hInternet);
		filename=localFile;
	}
#if 0
    else if ( !_User.empty( ) && !_LocalShare.empty())
    {
      if ( !WinTricks::DriveExists(_LocalShare + "\\") )  // "Z:\\"))
      {
        WarnWithMsg(E_FAIL, StdStringFormat("Network share: %s doesnt exist\n", _LocalShare.c_str( ) ).c_str( ) );
        WinTricks::MapNetworkDrive(_NetworkShare, _LocalShare, _User, _Password);
      }
	}
#endif
	//
	// Check if connected - this could take too long if
	if ( !File.Exists(filename) )
	{
		// DebugMessage(StdStringFormat("UNC File not found for device %s\n", _device.c_str()));
		return WarnWithMsg(E_FAIL, StdStringFormat("UNC File %s not found for device %s\n", filename.c_str( ), _device.c_str( ) ) );
	}

	// Check mod time, if same, returns
	modtime = File.GetFileModTime(filename);

	if ( lastmodtime == modtime )
	{
		return WarnWithMsg(E_PENDING, StdStringFormat("File %s Device %s modtime unchanged %s\n", filename.c_str( ), _device.c_str( ), modtime.Format("%c") ) );
	}

	// Check file size, if same, returns
	if ( FAILED(File.Size(filename, filesize) ) )
	{
		return WarnWithMsg(E_FAIL, StdStringFormat("File %s Device %s filesize  Failed\n", filename.c_str( ), _device.c_str( ) ) );
	}

	if ( filesize == _lastfilesize )
	{
		return WarnWithMsg(E_PENDING, StdStringFormat("File %s Device %s filesize:%d = lastfilesize:%d\n", filename.c_str( ), _device.c_str( ), filesize, _lastfilesize) );
	}

	if ( filesize == 0 )
	{
		return WarnWithMsg(E_PENDING, StdStringFormat("File %s Device %s filesize:%d = 0\n", filename.c_str( ), _device.c_str( ), filesize) );
	}
	COleDateTime now = COleDateTime::GetCurrentTime( );   // -COleDateTimeSpan(/* days, hours, mins, secs*/  0,0,0,0);

	if ( !_simulation )
	{
		today = COleDateTime(now.GetYear( ), now.GetMonth( ), now.GetDay( ), 0, 0, 0);
	}
	else
	{
		today = LastLogDate;
	}
	long        ulFileSize = filesize;
	data.resize(ulFileSize);

	// data.resize(ulFileSize);

	// Posix file utilies dont work with UNC file names?

	CopyFile(
		filename.c_str( ),
		std::string(File.ExeDirectory( ) + "LocalLogFileCopy.txt").c_str(),
		false
		);
	filename = File.ExeDirectory( ) + "LocalLogFileCopy.txt";
	HANDLE hFile = CreateFile(filename.c_str( ),
		GENERIC_READ,                                           // access (read) mode
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // share mode
		NULL,                                                   // pointer to security attributes
		OPEN_EXISTING,                                          // how to create
		FILE_ATTRIBUTE_NORMAL,                                  // file attributes
		NULL);                                                  // handle to file with attributes to copy

	if ( hFile == INVALID_HANDLE_VALUE )                      // did we succeed?
	{
		return WarnWithMsg(E_FAIL, StdStringFormat("INVALID_HANDLE_VALUE File \"%s\" for device %s\n", filename.c_str( ), _device.c_str( ) ) );
	}

	GLogger.Warning(StdStringFormat("Check Filesize %d vs Last Filesize %d\n", filesize, _lastfilesize) );

	if ( ( filesize > _lastfilesize ) && ( _lastfilesize != 0 ) )
	{
		// fseek(stream,_lastfilesize,SEEK_SET);
		DWORD dwPtr = SetFilePointer(hFile, _lastfilesize, NULL, FILE_BEGIN);

		if ( dwPtr == INVALID_SET_FILE_POINTER )     // Test for failure
		{
			return WarnWithMsg(E_FAIL, StdStringFormat("INVALID_SET_FILE_POINTER File \"%s\" Error = %x for device %s\n", filename.c_str( ), GetLastError( ), _device.c_str( ) ) );
		}

		ulFileSize = filesize - _lastfilesize;
	}

	GLogger.Warning(StdStringFormat("Read File Size %d\n", ulFileSize) );

	// read the original file byte in the buffer
	DWORD dwNewSize;
	void *pAddr = &data[0];
	ReadFile(hFile, pAddr, ulFileSize, &dwNewSize, NULL);
	CloseHandle(hFile);


    // Now parse buffer full of data
    std::string line;

    long         m           = (long) data.size( ) - 1;
    COleDateTime beginningts = now - COleDateTimeSpan(7, 0, 0, 0); // 1 week ago

	size_t n = 0;

	// Look for override
	bool bAutomode, bEE, bMoving;
	std::string program;
	bool bFlag=false;
			static const size_t Xi=5;
			static const size_t Yi=6;
			static const size_t Zi=7;

	// Could be multiple lines per update
	while ( getline(n, data, line) )
	{
#ifdef _DEBUG
		OutputDebugString(line.c_str());
#endif
		line = NIST::Config().trim(line,"\n\r\xb");
		std::vector<std::string> tokens = TrimmedTokenize(line,Globals.Delimiter);
		if(tokens.size() < Zi)
		{
			LOGONCE GLogger.Fatal(StdStringFormat("Not enough tokens=%d on line %s \n", tokens.size(), line.c_str()) );
			EXECONCE { CopyFile(filename.c_str( ) , (File.ExeDirectory()+"LogErrorFile.txt").c_str(), false); }
			continue ; // skip line not enough tokens
		}

		bFlag=true;

		items.SetTag("power", "ON");
		items.SetTag("avail", "AVAILABLE");


		items.SetTag("last_update",tokens[0]); 
		this->SetMTCTagValue("last_update",tokens[0]); // this call updates agent

		/**
		2016-04-05 17:11:47,TRUE,100,FALSE,testName1,847.225,-1052.83,685.211
		0 - date
		1- AUTO - T/F
		2- override
		3 - End Effector On- T/F
		4 - program name
		5 - x
		6 - y 
		7 - z
		*/

		if(stricmp(tokens[1].c_str(),"TRUE")==0)
		{
			bAutomode=true;
		}
		else 
		{
			bAutomode=false;
		}

		items.SetTag("path_feedrateovr", tokens[2]) ;
		
		if(stricmp(tokens[3].c_str(),"TRUE")==0) // TRUE/FALSE
		{
			bEE=true;
		}
		else
		{
			bEE=false;
		}

		program = tokens[4];
		items.SetTag("program", tokens[4]);

		bMoving = false;

		items.SetTag("Xabs", tokens[Xi]);
		// Fixme, check if real number
		double dX;
		if(1 != sscanf(tokens[Xi].c_str(), "%lf", &dX) )
			dX=0;

		if(fabs(dX-lastX) > _dDither)
			bMoving=true;
		lastX = dX;


		items.SetTag("Yabs", tokens[Yi]);
		double dY;
		if(1 != sscanf(tokens[Yi].c_str(), "%lf", &dY) )
			dY=0;
		if(fabs(dY-lastY) > _dDither)
			bMoving=true;
		lastY = dY;

		items.SetTag("Zabs", tokens[Zi]);
		double dZ;
		if(1 != sscanf(tokens[Zi].c_str(), "%lf", &dZ) )
			dZ=0;
		if(fabs(dZ-lastZ) > _dDither)
			bMoving=true;
		lastZ = dZ;



#ifdef _DEBUG
		GLogger.Fatal(StdStringFormat("ABBAdapter::GatherDeviceData bFLag=%s| bAutomode=%s|"
				"bEE=%s|"
				"program='%s'|"
				"bMoving=%s\n",
				bFlag? "TRUE" : "FALSE",
				bAutomode? "TRUE" : "FALSE",
				bEE? "TRUE" : "FALSE",
				program.c_str(),
				bMoving? "TRUE" : "FALSE"
				));

		if(bAutomode && bEE && !program.empty() && bMoving) Run();
		else if(bAutomode && bEE && !program.empty() && !bMoving) Paused();
		else if(bAutomode && !bEE && !program.empty() && !bMoving) Idle();
		else if(bAutomode &&  program.empty() ) Setup();
		else if(!bAutomode ) Fault();
		UpdateTags();
#endif
	}
	GLogger.Warning(StdStringFormat("ABBAdapter::GatherDeviceData bFLag=%s| bAutomode=%s|"
		"bEE=%s|"
		"program='%s'|"
		"bMoving=%s\n",
		bFlag? "TRUE" : "FALSE",
		bAutomode? "TRUE" : "FALSE",
		bEE? "TRUE" : "FALSE",
		program.c_str(),
		bMoving? "TRUE" : "FALSE"
		));
	if(bFlag)
	{
		if(bAutomode && bEE && !program.empty() && bMoving) Run();
		else if(bAutomode && bEE && !program.empty() && !bMoving) Paused();
		else if(bAutomode && !bEE && !program.empty() && !bMoving) Idle();
		else if(bAutomode &&  program.empty() ) Setup();
		else if(!bAutomode ) Fault();
		else  
		{
			GLogger.Fatal(StdStringFormat("ABBAdapter::GatherDeviceData Default State! bAutomode=%s|"
				"bEE=%s|"
				"program='%s'|"
				"bMoving=%s\n",
				bAutomode? "TRUE" : "FALSE",
				bEE? "TRUE" : "FALSE",
				program.c_str(),
				bMoving? "TRUE" : "FALSE"
				));

			Run();
		}
	}
	else
	{
		GLogger.Fatal(StdStringFormat("ABBAdapter::GatherDeviceData Error No bflag\n"	));
	}
	UpdateTags();
  }
  catch ( std::exception e )
  {
    GLogger.LogMessage(StdStringFormat("COpcAdapter Exception in %s - COpcAdapter::GatherDeviceData() %s\n", _device.c_str( ), (LPCSTR) e.what( ) ), LOWERROR);
    Off( );
    Disconnect( );
    hr = E_FAIL;
  }
  catch ( ... )
  {
    GLogger.LogMessage(StdStringFormat("COpcAdapter Exception in %s - COpcAdapter::GatherDeviceData()\n", _device.c_str( ) ), LOWERROR);
    Off( );
    Disconnect( );
    hr = E_FAIL;
  }
  _lastfilesize = filesize;
  lastmodtime   = modtime;
  return hr;
}
