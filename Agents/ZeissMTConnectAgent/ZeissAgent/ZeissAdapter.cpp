//

// ZeissAdapter.cpp
//

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#include "stdafx.h"
#define BOOST_ALL_NO_LIB

#include "ZeissAdapter.h"
#include "ZeissAgent.h"

#include "config.hpp"
#include "Device.hpp"
#include "Agent.hpp"
#include "ATLComTime.h"
#include <fstream> // ifstream
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <share.h>

#include "StdStringFcn.h"
#include "Globals.h"
#include "Logger.h"
#include "WinTricks.h"
#include "Logger.h"

using namespace crp;

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
  crp::Config &                          config,
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
    ErrorMessage(StdStringFormat("AdapterT::SetMTCTagValue for %s NULL Agent Pointer\n", _device.c_str( ) ) );
    return;
  }

  Device *pDev = agent->getDeviceByName(_device);

  if ( pDev == NULL )
  {
    ErrorMessage(StdStringFormat("AdapterT::SetMTCTagValue for %s NULL Device Pointer\n", _device.c_str( ) ) );
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
void ZeissAdapter::CreateItem (std::string tag, std::string type)
{
  Item *item = new Item( );

  item->_type    = _T("Event");
  item->_tagname = tag;
  items.push_back(item);
}
void ZeissAdapter::Dump ( )
{
  std::stringstream str;

  str << _device << ":ProductionLog= " << unc << std::endl;
  str << _device << ":QueryServer= " << _nQueryServerPeriod << std::endl;
  str << _device << ":ServerRate= " << _nServerRate << std::endl;
  str << _device << ":Simulation= " << _simulation << std::endl;
  str << _device << ":LastLogDate= " << LastLogDate.Format("%Y/%m/%d") << std::endl;

  OutputDebugString(str.str( ).c_str( ) );
}
void ZeissAdapter::Config ( )
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
    std::vector<std::string> logday = TrimmedTokenize(config.GetSymbolValue(_device + ".LastLogDate", "00-00-00").c_str( ), "-");
    std::vector<std::string> logtime = TrimmedTokenize(config.GetSymbolValue(_device + ".LastLogTime", "00:00:00").c_str( ), ":");

    if ( logday.size( ) < 2 || logtime.size( ) < 2 )
    {
      throw std::exception("Bad log date");
    }
	LastLogDate         = COleDateTime(
		ConvertString(logday[0], 0), 
		ConvertString(logday[1], 0), 
		ConvertString(logday[2], 0), 
		ConvertString(logtime[0], 0), 
		ConvertString(logtime[2], 0), 
		ConvertString(logtime[1], 0)
		); 
    _nServerRate        = config.GetSymbolValue(_device + ".ServerRate", Globals.ServerRate).toNumber<int>( );
    _nQueryServerPeriod = config.GetSymbolValue(_device + ".QueryServer", Globals.QueryServer).toNumber<int>( );
    _simulation         = config.GetSymbolValue(_device + ".Simulation", 0).toNumber<int>( );

    // Network share addition - in case user SYSTEM cant access remote file
    _User         = config.GetSymbolValue(_device + ".User", "").c_str( );
    _Password     = config.GetSymbolValue(_device + ".Pw", "").c_str( );
    _LocalShare   = config.GetSymbolValue(_device + ".LocalShare", "").c_str( );
    _NetworkShare = config.GetSymbolValue(_device + ".NetworkShare", "").c_str( );
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

COleDateTime ZeissAdapter::NullDate()
{
	COleDateTime now=COleDateTime::GetCurrentTime( );
	if ( _simulation )
	{
		now = LastLogDate;
	}
	return now  - COleDateTimeSpan(7, 0, 0, 0); // 1 week ago
}
void ZeissAdapter::Cycle ( )
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
  CreateItem("path_feedrate");

  Off( );

  // GLogger.LogMessage(StdStringFormat("COpcAdapter::Cycle() Enter Loop for IP = %s\n",_device.c_str()));
  _mRunning = true;
  Off( );
  starttime = NullDate();
  endtime = NullDate(); 

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
boolean GetDateTime (std::string s, COleDateTime & ts)
{
  std::string::size_type delimPos = s.find_first_of("\t", 0);

  if ( std::string::npos != delimPos )
  {
    s = s.substr(0, delimPos);
  }

  // parse 2012/11/03 Skip time
  int Year, Month, Day;

  if ( sscanf(s.c_str( ), "%4d-%02d-%02d", &Year, &Month, &Day) == 3 )
  {
	  ts= COleDateTime(Year, Month, Day, 0, 0, 0);
	  return true;
  }
  else  if ( sscanf(s.c_str( ), "%02d/%02d/%4d",  &Month, &Day, &Year) == 3 )
  {
	  ts= COleDateTime(Year, Month, Day, 0, 0, 0);
	  return true;
  }
  return false;
}
// Format: 2015-03-11 17:04:39.998
boolean GetLogTimestamp (std::string s, COleDateTime & ts)
{
  std::string::size_type delimPos = s.find_first_of("\t", 0);

  if ( std::string::npos != delimPos )
  {
    s = s.substr(0, delimPos);
  }

  // parse 2012/11/03 Skip time
  int Year, Month, Day, Hour, Min, Sec;

  if ( sscanf(s.c_str( ), "%4d-%02d-%02d %02d:%02d:%02d", &Year, &Month, &Day, &Hour, &Min, &Sec) == 6 )
  {
    ts = COleDateTime(Year, Month, Day, Hour, Min, Sec);
    return true;
  }

  return false;
}

boolean GetLogTime (std::string s, COleDateTime & ts)
{
  int  Hour, Min, Sec;
  char AMPM[8];
  s=Trim(s);
  COleDateTime now = COleDateTime::GetCurrentTime( );
  // Dummy day for now
  int Year = now.GetYear();
  int Month = now.GetMonth();
  int Day = now.GetDay();
  if ( sscanf(s.c_str( ), "%02d:%02d:%02d %s",  &Hour, &Min, &Sec, AMPM) ==4 )
  {
	  std::string ampm(AMPM);
	  ampm=MakeLower(ampm);
	  if(ampm=="pm")
		  Hour=Hour+12;
	  ts = COleDateTime(Year, Month, Day, Hour, Min, Sec);
	  return true;
  }
  return false;
}
COleDateTime SetLogDate ( COleDateTime date, COleDateTime & ts)
{
  int Year = date.GetYear();
  int Month = date.GetMonth();
  int Day = date.GetDay();
  int Hour = ts.GetHour();
  int Min = ts.GetMinute();
  int Sec = ts.GetSecond();
  ts = COleDateTime(Year, Month, Day, Hour, Min, Sec);
  return ts;
}
static void Nop ( ) { }
HRESULT ZeissAdapter::FailWithMsg (HRESULT hr, std::string errmsg)
{
  this->DebugMessage(errmsg);
  return hr;
}
HRESULT ZeissAdapter::WarnWithMsg (HRESULT hr, std::string errmsg)
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

void  ZeissAdapter::Executing(std::string planname)
{
	items.SetTag("power", "ON");
	items.SetTag("controllermode", "AUTOMATIC");
	items.SetTag("execution", "EXECUTING");
	items.SetTag("error", "");
	items.SetTag("program", planname);
}

void  ZeissAdapter::Idle()
{
	// changed from unavailable. Causes agent to make everything unavailable.
	items.SetTag("execution", "IDLE");
	items.SetTag("program", "");
	items.SetTag("controllermode", "MANUAL");
	items.SetTag("path_feedrate", "0");

}


HRESULT ZeissAdapter::GatherDeviceData ( )
{
  USES_CONVERSION;
  HRESULT      hr = S_OK;
  DWORD        filesize;
  COleDateTime modtime;
  try
  {
    std::string  filename = unc;
   // COleDateTime today;
	COleDateTime now=COleDateTime::GetCurrentTime( );
	if ( _simulation )
	{
		now = LastLogDate;
	}

    if ( ( items.GetTag("execution", "PAUSED") == "EXECUTING" ) && ( items.GetTag("controllermode", "MANUAL") == "AUTOMATIC" ) )
    {
      double X = fmod(ConvertString<double>(items.GetTag("Xabs", "0.0"), 0.0) + 0.1, 100.0);
      double Y = fmod(ConvertString<double>(items.GetTag("Yabs", "0.0"), 0.0) + 0.1, 100.0);
      double Z = fmod(ConvertString<double>(items.GetTag("Zabs", "0.0"), 0.0) + 0.1, 100.0);
      SetMTCTagValue("Xabs", ConvertToString(X) );
	  SetMTCTagValue("Yabs", ConvertToString(Y) );
	  SetMTCTagValue("Zabs", ConvertToString(Z) );
	  SetMTCTagValue("Srpm", "99");
     }
    else
    {
		SetMTCTagValue("Xabs", "0");
		SetMTCTagValue("Yabs", "10");
		SetMTCTagValue("Zabs", "20");
		SetMTCTagValue("Srpm", "0");
     }

    if ( !_User.empty( ) )
    {
      if ( !WinTricks::DriveExists(_LocalShare + "\\") )  // "Z:\\"))
      {
        WarnWithMsg(E_FAIL, StdStringFormat("Network share: %s doesnt exist\n", _LocalShare.c_str( ) ).c_str( ) );
        WinTricks::MapNetworkDrive(_NetworkShare, _LocalShare, _User, _Password);
      }
    }

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
    //if ( !_simulation )
    //{
    //  today = COleDateTime(now.GetYear( ), now.GetMonth( ), now.GetDay( ), 0, 0, 0);
    //}
    //else
    //{
    //  today = LastLogDate;
    //}
    long        ulFileSize = filesize;
    std::string data;
    data.resize(ulFileSize);

    // data.resize(ulFileSize);

    // Posix file utilies dont work with UNC file names?

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
	std::string line1("Measurement Plan Name:");
	std::string line2("Run Speed:");
	std::string line3("Measurement Start Time:");
	std::string line4("Measurement End Time:");
	std::string line5("Duration of Run:");
	std::string line6("Date of Run:");

	
     while ( getline(n, data, line) )
    {
		size_t it;
		if(Trim(line).empty())
			continue;

		long   tmpspeed;
		COleDateTime tmpstarttime, tmpendtime;
		if ( line.find_first_of(":") == std::string::npos )
			continue;

		items.SetTag("last_update", (const TCHAR *) COleDateTime::GetCurrentTime( ).Format("%m/%d/%Y %H:%M:%S") );

		// Measurement Plan Name
		if ( line.find(line1) != std::string::npos )
		{
			tmpplanname = Trim(line.substr(line1.size()+1));
		}

		// Run Speed
		else if ( line.find(line2) != std::string::npos )
		{
			std::string funits(" ", 10);
			sscanf(Trim(line.substr(line2.size()+1)).c_str(), "%d%10s", &tmpspeed, &funits[0]);
			items.SetTag("path_feedrate", StdStringFormat("%d", tmpspeed));
		}

		//Measurement Start Time
		else if ( line.find(line3) != std::string::npos )
		{
			GetLogTime (Trim(line.substr(line3.size()+1)), tmpstarttime);
			Executing(tmpplanname);
		}

		//Measurement End Time
		else if ( line.find(line4) != std::string::npos )
		{
			GetLogTime (Trim(line.substr(line4.size()+1)), tmpendtime);
			Idle();
		}

		//Duration of Run
		else if ( line.find(line5) != std::string::npos )
		{
		}
		//Date of Run
		else if ( line.find(line6) != std::string::npos )
		{

			//COleDateTime date;
			//GetDateTime (Trim(line.substr(line6.size()+1)), date);
			//starttime=SetLogDate(date,tmpstarttime);
			//endtime=SetLogDate(date,tmpendtime);
			//planname=tmpplanname;
		}
	}


    // FIXME: should guess xyz motion if auto/exec and timestamp and now > threshold of time
    for ( int i = 0; i < items.size( ); i++ )
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
