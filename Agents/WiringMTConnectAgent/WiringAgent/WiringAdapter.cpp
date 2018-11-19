//

// WiringAdapter.cpp
//

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#include "stdafx.h"
#define BOOST_ALL_NO_LIB

#include "WiringAdapter.h"
#include "WiringAgent.h"

#include "config.hpp"
#include "Device.hpp"
#include "Agent.hpp"
#include "ATLComTime.h"
#include <fstream> // ifstream
#include <algorithm>
#include <numeric>

#include <stdio.h>
#include <stdlib.h>
#include <share.h>
#include "boost/assign.hpp"
using namespace std;
using namespace boost::assign;

#include "StdStringFcn.h"
#include "Globals.h"
#include "Logger.h"
#include "WinTricks.h"
#include "Logger.h"

using namespace Nist;

HRESULT AdapterT::ErrorMessage (std::string errmsg)
{
	AtlTrace(errmsg.c_str( ) );
	logError(errmsg.c_str( ));

	if ( Globals.Debug > 0 )
	{
		//		EventLogger.LogEvent(errmsg);
	}
	return E_FAIL;
}
HRESULT AdapterT::DebugMessage (std::string errmsg)
{
	AtlTrace(errmsg.c_str( ) );
	logWarning(errmsg.c_str( ));

	if ( Globals.Debug > 3 )
	{
		//		EventLogger.LogEvent(errmsg);
	}
	return E_FAIL;
}

HRESULT WiringAdapter::WarnWithMsg (HRESULT hr, std::string errmsg)
{

	if(Globals.DebugTags)
		SetMTCTagValue("d_message", errmsg);
	this->DebugMessage(errmsg);
	return S_OK;
}
/////////////////////////////////////////////////////////////////////
static void trans_func (unsigned int u, EXCEPTION_POINTERS *pExp)
{
	std::string errmsg = StdStringFormat("WiringAdapter In trans_func - Code = 0x%x\n", pExp->ExceptionRecord->ExceptionCode);

	OutputDebugString(errmsg.c_str( ) );
	throw std::exception(errmsg.c_str( ), pExp->ExceptionRecord->ExceptionCode);
}

static void Nop ( ) { }
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
	n = end;
	return true;
}
/////////////////////////////////////////////////////////////////////

AdapterT::AdapterT(AgentConfigurationEx *mtcagent, // mtconnect agent
	Nist::Config &                          config,
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
		LOG_ONCE( logFatal(" (%s) Could not find data item: %s  \n", _device.c_str( ), tag.c_str( )  ));
	}
}
void AdapterT::Off ( )
{
#if 0
	for ( int i = 0; i < items.size( ); i++ )
	{
		items[i]->_value     = "UNAVAILABLE";
		items[i]->_lastvalue = "UNAVAILABLE";
		SetMTCTagValue(items[i]->_tagname, "UNAVAILABLE");
	}
#endif
	// changed from unavailable. Causes agent to make everything unavailable.
	SetMTCTagValue("program", "");
	SetMTCTagValue("error", "");	
	SetMTCTagValue("controllermode", "MANUAL");
	SetMTCTagValue("execution", "IDLE");
	SetMTCTagValue("avail", "AVAILABLE");
	SetMTCTagValue("power", "OFF");
}
void AdapterT::On ( )
{
	SetMTCTagValue("avail", "AVAILABLE");
	SetMTCTagValue("power", "ON");
}
void WiringAdapter::CreateItem (std::string tag, std::string type)
{
	Item *item = new Item( );

	item->_type    = _T("Event");
	item->_tagname = tag;
	items.push_back(item);
}
std::string WiringAdapter::DumpHeader()
{
	std::string tmp;
	for(size_t i=0; i< items.size(); i++)
	{
		tmp+=StdStringFormat("%s,", items[i]->_tagname.c_str());
	}
	tmp+="\n";
	return tmp;
}
std::string WiringAdapter::DumpDataItems()
{
	std::string tmp;
	for(size_t i=0; i< items.size(); i++)
	{
		tmp+=StdStringFormat("%s,", items[i]->_value.c_str());
	}
	tmp+="\n";
	return tmp;
}
void WiringAdapter::Dump ( )
{
	std::stringstream str;

	str << _device << ":ProductionLog= " << unc << std::endl;
	str << _device << ":QueryServer= " << _nQueryServerPeriod << std::endl;
	str << _device << ":ServerRate= " << _nServerRate << std::endl;
	str << _device << ":Simulation= " << _simulation << std::endl;
	str << _device << ":LastLogDate= " << LastLogDate.Format("%Y/%m/%d") << std::endl;

	OutputDebugString(str.str( ).c_str( ) );
}
void WiringAdapter::Config ( )
{
	std::string cfgfile = Globals.inifile;
	std::string sLastLogDate;

	try
	{
		if ( GetFileAttributesA(cfgfile.c_str( ) ) == INVALID_FILE_ATTRIBUTES )
		{
			throw std::exception("Could not find ini file \n");
		}

		unc = config.GetSymbolValue<std::string>(_device + ".ProductionLog", File.ExeDirectory( ) + "ProductionLog.csv");
		std::vector<std::string> logday = TrimmedTokenize(config.GetSymbolValue<std::string>(_device + ".LastLogDate", "0/0/0"), "/");

		if ( logday.size( ) < 2 )
		{
			throw std::exception("Bad log date");
		}
		LastLogDate         = COleDateTime(ConvertString(logday[2], 0), ConvertString(logday[0], 0), ConvertString(logday[1], 0), 0, 0, 0); // .ParseDateTime(sLastLogDate.c_str(), VAR_DATEVALUEONLY);
		_nServerRate        = config.GetSymbolValue<int>(_device + ".ServerRate", Nist::stringy::strconvert<double>(Globals.ServerRate));
		_nQueryServerPeriod = config.GetSymbolValue<int>(_device + ".QueryServer", Nist::stringy::strconvert<double>(Globals.QueryServer));
		_simulation         = config.GetSymbolValue<int>(_device + ".Simulation", "0");

		// Network share addition - in case user SYSTEM cant access remote file
		_User         = config.GetSymbolValue<std::string>(_device + ".User", "");
		_Password     = config.GetSymbolValue<std::string>(_device + ".Pw", "");
		_LocalShare   = config.GetSymbolValue<std::string>(_device + ".LocalShare", "");
		_NetworkShare = config.GetSymbolValue<std::string>(_device + ".NetworkShare", "");

	}
	catch ( std::exception errmsg )
	{
		logAbort("Could not find ini file for device %s\n", _device.c_str( ) );
	}
	catch ( ... )
	{
		logAbort("Could not find ini file for device %s\n", _device.c_str( )  );
	}
	Dump( );
}
void WiringAdapter::Cycle ( )
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
	CreateItem("d_message");

	if(Globals.DebugTags)
	{
		CreateItem("d_file_size");
		CreateItem("d_message");
		CreateItem("d_cycle_time");
	}

	_bNotProductionMode=false;

#ifdef _DEBUG
    tagfile.open (File.ExeDirectory()+ "tags.csv", std::ofstream::out );
	if(!tagfile.is_open())
		this->DebugMessage("tagfile.open failed");
	tagfile<< DumpHeader().c_str( ); 
#endif
	_mRunning = true;
	Off( );

	while ( _mRunning )
	{
		HRESULT hr;
		try
		{
			// PING IP?
			SetMTCTagValue("heartbeat", StdStringFormat("%d", nHeartbeat++) );
			SetMTCTagValue("avail", "AVAILABLE");
			SetMTCTagValue("power", "ON");
			SetMTCTagValue("d_message", "");

			COleDateTime starting = COleDateTime::GetCurrentTime( ); 
			hr = GatherDeviceData( );
			if(Globals.DebugTags)
			{
				::COleDateTimeSpan duration = COleDateTime::GetCurrentTime( ) - starting ; 
				SetMTCTagValue("d_cycle_time", StdStringFormat("%d:%d:%d", duration.GetHours(), duration.GetMinutes(), duration.GetSeconds()));
			}

			if ( FAILED(hr ) )
			{
				::Sleep(this->_nQueryServerPeriod);
				continue;
			}
			::Sleep(_nServerRate);
		}
		catch ( std::exception e )
		{
			SetMTCTagValue("d_message", "Exception WiringAdapter::Cycle");
			Off( );
		}
		catch ( ... )
		{
			SetMTCTagValue("d_message", "Exception WiringAdapter::Cycle");
			Off( );
		}
	}
}


boolean GetLogTime (std::string s, COleDateTimeSpan & ts)
{

	int Hour, Min, Sec;
	// Wiring log timestamp: 2016-11-15 16:10:08.650
	if ( sscanf(s.c_str( ), "%02d:%02d:%02d",  &Hour, &Min, &Sec) == 3 )
	{
		ts = COleDateTimeSpan(0, Hour, Min, Sec);
		return true;
	}

	return false;
}

// Wiring log timestamp: 01 February 2017,05:48:16,ASL 


static int get_month_index( std::string name )
{
static     std::map<std::string, int> months =   map_list_of
        ( "January", 1 )
        ( "February", 2 )
        ( "March", 3 )
        ( "April", 4 )
        ( "May", 5 )
        ( "June", 6 )
        ( "July", 7 )
        ( "August", 8 )
        ( "September", 9 )
        ( "October", 10 )
        ( "November", 11 )
        ( "December", 12 );
    std::map<std::string, int>::iterator iter = months.find( name );

    if( iter != months.cend() )
        return iter->second;
    return -1;
}
static std::string get_month_alpha( size_t index )
{
	static     std::map<int, std::string> months =   map_list_of
		( 1, "January" )
		( 2, "February" )
		( 3, "March" )
		( 4, "April" )
		( 5, "May" )
		( 6, "June" )
		( 7, "July" )
		( 8, "August" )
		( 9, "September" )
		( 10, "October" )
		( 11, "November" )
		( 12, "December" );
	std::map<int, std::string >::iterator iter = months.find( index );

	if( iter != months.cend() )
		return iter->second;
	return "";
}
boolean GetLogDate (std::string s, COleDateTime & ts)
{

	int Year,  Day;
	char cMonth[256];
	if ( sscanf(s.c_str( ), "%02d %s %4d",  &Day, cMonth, &Year) == 3 )
	{
		int i ;
		if((i=get_month_index(cMonth)) < 0)
			return false;
		ts = COleDateTime(Year, i, Day, 0,0,0);
		return true;
	}

	return false;
}


HRESULT WiringAdapter::GatherDeviceData ( )
{
	USES_CONVERSION;
	HRESULT      hr = S_OK;
	DWORD        filesize;
	COleDateTime modtime;
	try
	{
		SYSTEMTIME lt;
		GetLocalTime(&lt);
		std::string  filename = StdStringFormat("%s\\Comlog %02d %s %4d.txt", 
			_NetworkShare.c_str(), 
			lt.wDay, 
			get_month_alpha(lt.wMonth).c_str(),
			lt.wYear);

#if 0
		// COleDateTime::GetCurrentTime(); is in UTC, so you need to convert it to your localtime
		// Not sure if this is true.
		COleDateTime today = COleDateTime::GetCurrentTime( );
		// Generate daily UNC name based on NetworkShare folder and daily event log filename
		//Comlog 14 February 2018
		std::string  filename = StdStringFormat("%s\\Comlog %02d %s %4d.txt", 
			_NetworkShare.c_str(), 
			today.GetDay(), 
			get_month_alpha(today.GetMonth()).c_str(),
			today.GetYear());
#endif
		//std::string  filename = unc;

		if ( ( items.GetTag("execution", "PAUSED") == "EXECUTING" ) && ( items.GetTag("controllermode", "MANUAL") == "AUTOMATIC" ) )
		{
			double X = fmod(ConvertString<double>(items.GetTag("Xabs", "0.0"), 0.0) + 0.1, 100.0);
			double Y = fmod(ConvertString<double>(items.GetTag("Yabs", "0.0"), 0.0) + 0.1, 100.0);
			double Z = fmod(ConvertString<double>(items.GetTag("Zabs", "0.0"), 0.0) + 0.1, 100.0);
			SetMTCTagValue("Xabs", ConvertToString(X) );
			SetMTCTagValue("Yabs", ConvertToString(Y) );
			SetMTCTagValue("Zabs", ConvertToString(Z) );
		}
		else
		{
			SetMTCTagValue("Xabs", "0");
			SetMTCTagValue("Yabs", "10");
			SetMTCTagValue("Zabs", "20");
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
			// We are now going to assume that the PC is turned off!
			// MAY have some file permission errors.
#if 0
			Off();
#endif
			_lastfilesize=0;
			lastmodtime   = COleDateTime(0, 0, 0, 0, 0, 0);
			return WarnWithMsg(E_FAIL, StdStringFormat("PC for device %s turned off\n",  _device.c_str( ) ) );
			//return WarnWithMsg(E_FAIL, StdStringFormat("UNC File %s not found for device %s\n", filename.c_str( ), _device.c_str( ) ) );
		}

		// Check mod time, if same, returns
		modtime = File.GetFileModTime(filename);

		if ( lastmodtime == modtime )
		{
			return WarnWithMsg(E_PENDING, StdStringFormat("File %s Device %s modtime unchanged %s\n", filename.c_str( ), _device.c_str( ), modtime.Format("%c") ) );
		}

		// Get file size 
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
		//COleDateTime today;
		//if ( !_simulation )
		//{
		//	today = COleDateTime(now.GetYear( ), now.GetMonth( ), now.GetDay( ), 0, 0, 0);
		//}
		//else
		//{
		//	today = LastLogDate;
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
			SetMTCTagValue("d_message", StdStringFormat("INVALID_HANDLE_VALUE File %s", filename.c_str( )));
			return WarnWithMsg(E_FAIL, StdStringFormat("INVALID_HANDLE_VALUE File \"%s\" for device %s\n", filename.c_str( ), _device.c_str( ) ) );
		}

		if(Globals.DebugTags)
			SetMTCTagValue("d_file_size", StdStringFormat("%d", filesize));

		if ( ( filesize > _lastfilesize ) && ( _lastfilesize != 0 ) )
		{
			// fseek(stream,_lastfilesize,SEEK_SET);
			DWORD dwPtr = SetFilePointer(hFile, _lastfilesize, NULL, FILE_BEGIN);

			if ( dwPtr == INVALID_SET_FILE_POINTER )     // Test for failure
			{
				SetMTCTagValue("d_message", StdStringFormat("INVALID_SET_FILE_POINTER File %s", GetLastError( )));
				return WarnWithMsg(E_FAIL, StdStringFormat("INVALID_SET_FILE_POINTER File \"%s\" Error = %x for device %s\n", filename.c_str( ), GetLastError( ), _device.c_str( ) ) );
			}

			ulFileSize = filesize - _lastfilesize;
		}

		if(Globals.DebugTags)
			SetMTCTagValue("d_message", StdStringFormat("Actual Reading File Size %d\n", ulFileSize ));

		// read the original file byte in the buffer
		DWORD dwNewSize;
		void *pAddr = &data[0];
		ReadFile(hFile, pAddr, ulFileSize, &dwNewSize, NULL);
		CloseHandle(hFile);

		// Now parse buffer full of data
		std::string line;

		long         m           = (long) data.size( ) - 1;
		COleDateTime beginningts = now - COleDateTimeSpan(7, 0, 0, 0); // 1 week ago

		// OutputDebugString("Beginning:");
		// OutputDebugString(beginningts.Format("%A, %B %d, %Y"));
		// This performs a reverse search in the file until the CAMIO Studio started
		// 
		while ( getlinebackwards(m, data, line) )
		{
			if( line.find("New Run Started") != std::string::npos)
			{
				SetMTCTagValue("d_message", StdStringFormat("Found 'New Run Started' at %d", m));
				data = data.substr(m);
				break;
			}
		}

		size_t n = 0;
		 
		while ( getline(n, data, line) )
		{
			// Some basic MTConnect tag updates
			items.SetTag("last_update",(LPCSTR)  modtime.Format(_T("%A, %B %d, %Y %H:%M:%S")));
			items.SetTag("power", "ON");
			items.SetTag("avail", "AVAILABLE");

			// Prune log of simple non-entries
			if(line.find("RX") != std::string::npos)
				continue;
			if(line.find("TX") != std::string::npos)
				continue;

			items.SetTag("d_message", "Gather data");

			if ( line.find("New Run Started") != std::string::npos )
			{
				items.SetTag("controllermode", "MANUAL");
				items.SetTag("execution", "IDLE");
				items.SetTag("error", "");
				std::string prog;
				prog= extract(line, "JOB:", "]");
				items.SetTag("program", prog);
			}
			else if ( line.find("StartTheFeeder") != std::string::npos   )
			{
				items.SetTag("controllermode", "AUTOMATIC");
				items.SetTag("execution", "EXECUTING");
				items.SetTag("error", "");
			}
			else if ( line.find("ASL START") != std::string::npos   )
			{
				items.SetTag("controllermode", "MANUAL");
				items.SetTag("execution", "PAUSED");
				items.SetTag("error", "");
			}
			//else if ( line.find("ASL STOP") != std::string::npos   )
			//{
			//	items.SetTag("controllermode", "MANUAL");
			//	items.SetTag("execution", "IDLE");
			//	items.SetTag("error", "");
			//}
			else if ( line.find("/// CheckFullBufferRunResult") != std::string::npos   )
			{
				items.SetTag("controllermode", "MANUAL");
				items.SetTag("execution", "IDLE");
				items.SetTag("error", "");
			}
			else if ( line.find("Feeder Has Stopped") != std::string::npos  )
			{
				items.SetTag("controllermode", "AUTOMATIC");
				items.SetTag("execution", "IDLE");
				items.SetTag("error", "");
				items.SetTag("program", "");
			}
			else if ( line.find("The Run Finished") != std::string::npos  )
			{
				items.SetTag("controllermode", "MANUAL");
				items.SetTag("execution", "IDLE");
				items.SetTag("error", "");
				items.SetTag("program", "");
			}

#ifdef _DEBUG
			tagfile << DumpDataItems(); 
			tagfile.flush();
#endif
		}

		// Guess RPM
		if ( ( items.GetTag("execution", "PAUSED") == "EXECUTING" ) && ( items.GetTag("controllermode", "MANUAL") == "AUTOMATIC" ) )
		{
			items.SetTag("Srpm", "99");
		}
		else
		{
			items.SetTag("Srpm", "0");
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
		logError("WiringAdapter Exception in %s - WiringAdapter::GatherDeviceData() %s\n", _device.c_str( ), (LPCSTR) e.what( ) );
		Off( );
		Disconnect( );
		hr = E_FAIL;
		SetMTCTagValue("d_message", "Exception WiringAdapter::GatherDeviceData() ");
	}
	catch ( ... )
	{
		logError("WiringAdapter Exception in %s - WiringAdapter::GatherDeviceData()\n", _device.c_str( ) );
		Off( );
		Disconnect( );
		SetMTCTagValue("d_message", "Exception WiringAdapter::GatherDeviceData() ");
		hr = E_FAIL;
	}
	_lastfilesize = filesize;
	lastmodtime   = modtime;
	return hr;
}
