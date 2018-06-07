//
//
//
#include "stdafx.h"
#define BOOST_ALL_NO_LIB

#include "AgfmAdapter.h"
#include "Logger.h"
#include "AgfmAgent.h"

#include "config.hpp"
#include "Device.hpp"
#include "Agent.hpp"
#include "ATLComTime.h"
#include <fstream> // ifstream
#include <algorithm>
#include "StdStringFcn.h"
#include "Globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <share.h>
#include "WinTricks.h"

//
#include "Logger.h"


using namespace crp;

HRESULT AdapterT::ErrorMessage(std::string errmsg) 
{
	AtlTrace(errmsg.c_str());
	GLogger.LogMessage(errmsg, LOWERROR);
	if(Globals.Debug>0)
	{
//		EventLogger.LogEvent(errmsg);
	}
	return E_FAIL;
}
HRESULT AdapterT::DebugMessage(std::string errmsg) 
{
	AtlTrace(errmsg.c_str());
	GLogger.LogMessage(errmsg, WARNING);
	if(Globals.Debug>3)
	{
//		EventLogger.LogEvent(errmsg);
	}
	return E_FAIL;
}

/////////////////////////////////////////////////////////////////////
static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	std::string errmsg =  StdStringFormat("COpcAdapter In trans_func - Code = 0x%x\n",  pExp->ExceptionRecord->ExceptionCode);
	OutputDebugString(errmsg.c_str() );
	throw std::exception(errmsg.c_str() , pExp->ExceptionRecord->ExceptionCode);
} 

AdapterT::AdapterT(AgentConfigurationEx * mtcagent, // mtconnect agent
	crp::Config& config,
	std::string machine,  // ip address or pc name
	std::string device) : 

_agentconfig(mtcagent), 
	_device(device),
	_config(config)
{
	inifile=Globals.inifile.c_str();
	_nServerRate=1000;
	_nQueryServerPeriod=10000;
}


AdapterT::~AdapterT(void)
{
}


void AdapterT::SetMTCTagValue(std::string tag, std::string value)
{
#if 1
	items.SetTag(tag, value);


	Agent * agent = _agentconfig->getAgent();
	Device *pDev = agent->getDeviceByName(_device);
	DataItem *di = pDev->getDeviceDataItem(tag);
	if (di != NULL)
	{
		std::string time = getCurrentTime(GMT_UV_SEC);
		agent->addToBuffer(di, value, time);
	}
	else
	{
		LOGONCE GLogger.LogMessage(StdStringFormat(" (%s) Could not find data item: %s  \n",  _device.c_str(), tag.c_str()));
	}

#endif
}


void AdapterT::Off() 
{ 	
	for (int i=0; i<items.size(); i++) 
	{
		items[i]->_value="UNAVAILABLE";
		items[i]->_lastvalue="UNAVAILABLE";
		SetMTCTagValue(items[i]->_tagname, "UNAVAILABLE");
	}
	// changed from unavailable. Causes agent to make everything unavailable.
	SetMTCTagValue("avail", "UNAVAILABLE");
	SetMTCTagValue("power", "OFF");
}
void AdapterT::On() 
{ 			 
	SetMTCTagValue("avail", "AVAILABLE");
	SetMTCTagValue("power", "ON");
}

void AgfmAdapter::CreateItem(std::string tag, std::string type)
{	
	Item * item = new Item();
	item->_type=_T("Event");
	item->_tagname=tag;
	items.push_back(item);
}

void AgfmAdapter::Dump()
{
	std::stringstream str;
	str << _device << ":ProductionLog " << unc << std::endl;
	str << _device << ":QueryServer " << _nQueryServerPeriod << std::endl;
	str << _device << ":ServerRate " << _nServerRate << std::endl;
	str << _device << ":Simulation " << _simulation << std::endl;
	str << _device << ":LastLogDate " << LastLogDate.Format("%Y/%m/%d") << std::endl;
	str << _device << ":User " << _User << std::endl;
	str << _device << ":Pw " << _Password << std::endl;
	str << _device << ":LocalShare " << _LocalShare << std::endl;
	str << _device << ":NetworkShare " << _NetworkShare << std::endl;
	OutputDebugString(str.str().c_str());
}
void AgfmAdapter::Config()
{
	//GLogger.LogMessage(StdStringFormat("COpcAdapter::Config() for IP = %s\n",_device.c_str()));
	std::string cfgfile = Globals.inifile; 
	std::string sLastLogDate;
	try {
		if(GetFileAttributesA(cfgfile.c_str())== INVALID_FILE_ATTRIBUTES)
			throw std::exception("Could not find ini file \n");

		unc=config.GetSymbolValue(_device + ".ProductionLog", File.ExeDirectory()+"ProductionLog.csv").c_str();
		std::vector<std::string> logday = TrimmedTokenize(config.GetSymbolValue(_device + ".LastLogDate", "0/0/0").c_str(),"/");
		if(logday.size()< 2) 
			throw std::exception("Bad log date");
		LastLogDate=COleDateTime( ConvertString(logday[2],0), ConvertString(logday[0],0),ConvertString(logday[1],0),0,0,0); // .ParseDateTime(sLastLogDate.c_str(), VAR_DATEVALUEONLY);
		_nServerRate =config.GetSymbolValue(_device + ".ServerRate", Globals.ServerRate ).toNumber<int>();
		_nQueryServerPeriod =config.GetSymbolValue(_device + ".QueryServer", Globals.QueryServer ).toNumber<int>();
		_simulation =config.GetSymbolValue(_device + ".Simulation", 0 ).toNumber<int>();

		_User =config.GetSymbolValue(_device + ".User", "" ).c_str();
		_Password =config.GetSymbolValue(_device + ".Pw", "" ).c_str();
		_LocalShare =config.GetSymbolValue(_device + ".LocalShare", "" ).c_str();
		_NetworkShare =config.GetSymbolValue(_device + ".NetworkShare", "" ).c_str();

	}
	catch(std::exception errmsg)
	{
		_agentconfig->AbortMsg(StdStringFormat("Could not find ini file for device %s\n", _device.c_str()));
	}
	catch(...)
	{
		_agentconfig->AbortMsg(StdStringFormat("Could not find ini file for device %s\n", _device.c_str()));
	}
	Dump();
}

void AgfmAdapter::Cycle()
{

	Config();

	_set_se_translator( trans_func );  // correct thread?
	SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);

	int nHeartbeat=0;
	CreateItem("heartbeat");
	CreateItem("program");
	CreateItem("controllermode");
	CreateItem("execution");
	CreateItem("power");
	CreateItem("avail");
	CreateItem("alarm");
	CreateItem("path_feedrateovr");
	CreateItem("operator");
	CreateItem("Srpm");
	CreateItem("Xabs");
	CreateItem("Yabs");
	CreateItem("Zabs");
	CreateItem("last_update");

	Off();

	//GLogger.LogMessage(StdStringFormat("COpcAdapter::Cycle() Enter Loop for IP = %s\n",_device.c_str()));
	_mRunning=true;
	Off();
	while(_mRunning)
	{
		try {
			// PING IP?
			SetMTCTagValue("heartbeat", StdStringFormat("%d", nHeartbeat++ ));
			if(FAILED(GatherDeviceData()))
			{
				::Sleep(this->_nQueryServerPeriod);
				continue;
			}
			On();
			::Sleep(_nServerRate);
		}
		catch(std::exception e)
		{
			Off();
		}
		catch(...)
		{
			Off();
		}
	}
}

COleDateTime GetDateTime(std::string s)
{
	std::string::size_type delimPos = s.find_first_of(Globals.Delimiter, 0);
	if(std::string::npos != delimPos)
		s=s.substr(0,delimPos);
	// parse 2012/11/03 Skip time 
	int Year, Month, Day;
	if(sscanf(s.c_str(), "%4d/%d/%d",&Year,  &Month, &Day)==3){}
	else throw std::exception("Unrecognized date-time format\n");

	return COleDateTime( Year, Month, Day, 0, 0, 0 ); 
}

static bool getline(size_t &n, std::string & data, std::string &line)
{
	line.clear();
	size_t N=data.size();
	if(n>=N)
		return false;

	size_t start=n;
	size_t end=n;

	while(end < N && data[end] != '\n')
	{
		end++;
	}
	while(end < N && data[end] == '\n')
	{
		end++;
	}
	line=data.substr(start,end-start);
	n=end; // skip over \n
	return true;
}
static bool getlinebackwards(long &n, std::string &data, std::string &line)
{
	line.clear();
	if(n<0)
		return false;

	long start=n;
	long end=n;

	while(end >=0 && data[end] == '\n')
	{
		end--;
	}
	while(end >= 0 && data[end] != '\n')
	{
		end--;
	}
	line=data.substr(end+1, start - end);
	//OutputDebugString(line.c_str());
	n=end; 
	return true;
}
static void Nop(){}

HRESULT AgfmAdapter::FailWithMsg(HRESULT hr, std::string errmsg)
{
	this->DebugMessage(errmsg);
	return hr;
}

HRESULT AgfmAdapter::WarnWithMsg(HRESULT hr, std::string errmsg)
{
	this->DebugMessage(errmsg);
	return S_OK;
}
std::string AgfmAdapter::FormatDate(COleDateTime d)
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

HRESULT AgfmAdapter::GatherDeviceData()
{
	USES_CONVERSION;
	HRESULT hr=S_OK;
	DWORD filesize;
	COleDateTime modtime;
	try 
	{
		std::string filename= unc;
		//std::string tofilename=File.TempDirectory()+"ProductionLog" + _device + ".csv";
		COleDateTime today; 

		if(items.GetTag("execution", "PAUSED") == "EXECUTING" && items.GetTag("controllermode", "MANUAL") == "AUTOMATIC")
		{
			double X = fmod(ConvertString<double>(items.GetTag("Xabs", "0.0"),0.0)+ 0.1, 100.0);
			double Y = fmod(ConvertString<double>(items.GetTag("Yabs", "0.0"),0.0)+ 0.1, 100.0);;
			double Z = fmod(ConvertString<double>(items.GetTag("Zabs", "0.0"),0.0) + 0.1, 100.0);;
			SetMTCTagValue("Xabs",ConvertToString(X));
			SetMTCTagValue("Yabs",ConvertToString(Y));
			SetMTCTagValue("Zabs",ConvertToString(Z));
		}
		else
		{
			SetMTCTagValue("Xabs", "0");
			SetMTCTagValue("Yabs", "10");
			SetMTCTagValue("Zabs", "20");
		}

		if(!_User.empty())
		{
			if(!WinTricks::DriveExists(_LocalShare+"\\")) // "Z:\\"))
			{
				WarnWithMsg(E_FAIL, StdStringFormat("Network share: %s doesnt exist\n", _LocalShare.c_str()).c_str());
				WinTricks::MapNetworkDrive(_NetworkShare, _LocalShare, _User, _Password);
			}
		}
		// Check if connected - this could take too long if 

		if(!File.Exists(filename))
		{
			//DebugMessage(StdStringFormat("UNC File not found for device %s\n", _device.c_str()));
			return WarnWithMsg(E_FAIL, StdStringFormat("UNC File %s not found for device %s\n", filename.c_str(), _device.c_str())) ;
		}
		// Check mod time, if same, returns
		modtime = File.GetFileModTime(filename);
		if(lastmodtime==modtime)
			return WarnWithMsg(E_PENDING, StdStringFormat("File %s Device %s modtime unchanged %s\n", filename.c_str(), _device.c_str(), modtime.Format("%c"))) ;

		// Check file size, if same, returns
		if(FAILED(File.Size(filename, filesize)))
			return WarnWithMsg(E_FAIL, StdStringFormat("File %s Device %s filesize  Failed\n", filename.c_str(), _device.c_str())) ;

		if(filesize==_lastfilesize)
			return WarnWithMsg(E_PENDING, StdStringFormat("File %s Device %s filesize:%d = lastfilesize:%d\n", filename.c_str(), _device.c_str(),filesize,_lastfilesize)) ;
		if(filesize==0)
			return WarnWithMsg(E_PENDING, StdStringFormat("File %s Device %s filesize:%d = 0\n", filename.c_str(), _device.c_str(),filesize)) ;
		COleDateTime now = COleDateTime::GetCurrentTime();//-COleDateTimeSpan(/* days, hours, mins, secs*/  0,0,0,0);

		//
		// This sets what today is since we may have old log files to test against, 
		// The code expects log files to reflect today's activity. If not nada happens.
		if(!_simulation)
		{
			today =  COleDateTime(now.GetYear(), now.GetMonth(), now.GetDay(), 0, 0, 0);
		}
		else
		{
			today = LastLogDate;
		}
		long ulFileSize=filesize;
		std::string data;
		DWORD numread;

		data.resize(ulFileSize);

		HANDLE hFile = CreateFile(filename.c_str(),
			GENERIC_READ, // access (read) mode
			FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, // share mode
			NULL, // pointer to security attributes
			OPEN_EXISTING, // how to create
			FILE_ATTRIBUTE_NORMAL,// file attributes
			NULL); // handle to file with attributes to copy

		if (hFile == INVALID_HANDLE_VALUE)    // did we succeed?
			return WarnWithMsg(E_FAIL, StdStringFormat("INVALID_HANDLE_VALUE File \"%s\" for device %s\n", filename.c_str(), _device.c_str())) ;
		
		//
		// New - log file will be reduced by 20% when it reaches the max file size (default 10M)
		// Should be ok, since  filesize<_lastfilesize! - will ignore seeking to new position
		// FIXME: skip reparse of all the seen old dates 
		// This works, since update of agent after all of log file read, even again

		GLogger.Warning(StdStringFormat("Check Filesize %d vs Last Filesize %d\n", filesize,_lastfilesize));
		if(filesize>_lastfilesize && _lastfilesize != 0 )
		{
			//fseek(stream,_lastfilesize,SEEK_SET);
			DWORD dwPtr = SetFilePointer(hFile, _lastfilesize, NULL, FILE_BEGIN) ;
			if (dwPtr == INVALID_SET_FILE_POINTER) // Test for failure
				return WarnWithMsg(E_FAIL, StdStringFormat("INVALID_SET_FILE_POINTER File \"%s\" Error = %x for device %s\n", filename.c_str(), GetLastError() ,  _device.c_str())) ;
		
			ulFileSize=filesize-_lastfilesize;
		}

		GLogger.Warning(StdStringFormat("Read File Size %d\n", ulFileSize));
		// read the original file byte in the buffer
		DWORD dwNewSize;
		void  * pAddr = &data[0];
		ReadFile(hFile, pAddr, ulFileSize, &dwNewSize, NULL );
		CloseHandle(hFile);

		if(data.size() == 0)
			return WarnWithMsg(E_FAIL, StdStringFormat("Zero log file size for device %s\n",  _device.c_str())) ;

		// Now parse buffer full of data
		std::string line;
		if(filesize>_lastfilesize && _lastfilesize != 0 )
		{	
			// Trim nothing - new data
		}
		else
		{
			// Search back for starting date
			size_t n=0;
			std::string date = FormatDate(today);  
			// Search for todays date
			if((n=data.find(date)) == std::string::npos)
			{
				// Go back extra day - since could be 12:05
				COleDateTime newdate = today - COleDateTimeSpan(1,0,0,0);
				date = FormatDate(newdate);  
				if((n=data.find(date)) == std::string::npos)
				{
					// DIdnt find previous day - skip shrinking file since small anyway
					n=0; 
				}
				else
				{
					data=data.substr(n); // trim data to today - 1 day in log file
				}
			}
			else
			{
				WarnWithMsg(E_FAIL, StdStringFormat("Cant find todays date \"%s\" for device %s\n", date.c_str(),  _device.c_str())) ;
			}
			//if(n!=0)
			//	data=data.substr(n); // trim data to today - 1 day in log file
		}

		if(data.size() == 0)
			return WarnWithMsg(E_FAIL, StdStringFormat("Zero log file size for device %s\n",  _device.c_str())) ;

		GLogger.Warning(StdStringFormat("Get Lines\n"));
		std::vector<std::string> lines;
		size_t n=0;
		while (getline (n,data, line) )
			lines.push_back(line);

		GLogger.Warning(StdStringFormat("Parse log file Lines\n"));
		for(int i=0; i< lines.size(); i++)
		{
			std::vector<std::string> tokens = TrimmedTokenize(lines[i],Globals.Delimiter);
			std::vector<std::string>::iterator it;
			size_t m;
			if((m=lines[i].find_first_of(Globals.Delimiter)) != std::string::npos)
			{
				items.SetTag("last_update",lines[i].substr(0,m)); 
				this->SetMTCTagValue("last_update",lines[i].substr(0,m)); // this call updates agent
			}

			if(tokens.size() >1)
			{
				SetMTCTagValue("operator",tokens[1]);
			}
			// Already today
			if((it=std::find(tokens.begin(), tokens.end(),"Override")) != tokens.end())
			{
				//Override;0,,,,,,,
				if((it+1)<  tokens.end()) 
				{
					std::vector<std::string> feeds = TrimmedTokenize(*(it+1),",");
					(feeds.size()>0) ? items.SetTag("path_feedrateovr",feeds[0]) : Nop();
				}
			}
			if((it=std::find(tokens.begin(), tokens.end(),"Feedhold")) != tokens.end())
			{
				if((it+1)>=  tokens.end())
					continue;

				long n = ConvertString<long>(*(it+1),99);
				switch(n)
				{
				case 1:
					items.SetTag("execution","PAUSED");
					break;
				case 0:
					items.SetTag("execution","EXECUTING");
					break;

				default:
					break;
				}

			}
			if((it=std::find(tokens.begin(), tokens.end(),"Program load")) != tokens.end())
			{
				if((it+1)<  tokens.end()  )
				{
					std::vector<std::string> progs = TrimmedTokenize(*(it+1),",");
					if(progs.size()>0) 
						items.SetTag("program",progs.back());
				}

				items.SetTag("controllermode","MANUAL");
				items.SetTag("execution","IDLE");
			}
			if((it=std::find(tokens.begin(), tokens.end(),"Program start")) != tokens.end())
			{
				items.SetTag("controllermode","AUTOMATIC");
				items.SetTag("execution","EXECUTING");
			}
			if((it=std::find(tokens.begin(), tokens.end(),"Program end")) != tokens.end())
			{
				items.SetTag("controllermode","MANUAL");
				items.SetTag("execution","IDLE");
			}

			if((it=std::find(tokens.begin(), tokens.end(),"Error message")) != tokens.end())
			{
				((it+1)<  tokens.end())? items.SetTag("alarm",*(it+1)):Nop();			
			}
			if((it=std::find(tokens.begin(), tokens.end(),"Program abort")) != tokens.end())
			{
				((it+1)<  tokens.end())? items.SetTag("execution","PAUSED"):Nop();			
				((it+1)<  tokens.end())? items.SetTag("controllermode","MANUAL"):Nop();			
			}		

		}
		if(items.GetTag("execution", "PAUSED") == "EXECUTING" && items.GetTag("controllermode", "MANUAL") == "AUTOMATIC")
		{
			 items.SetTag("Srpm","99");
		}
		else
		{
			 items.SetTag("Srpm","0");
		}
		for(int i = 0 ; i < items.size(); i++)
		{
			if(items[i]->_type==_T("Event") || items[i]->_type==_T("Sample"))
			{
				if(items[i]->_value != items[i]->_lastvalue)
				{
					this->SetMTCTagValue(items[i]->_tagname,items[i]->_value);
					items[i]->_lastvalue=items[i]->_value;
				}
			}
		}



	}
	catch(std::exception e)
	{
		GLogger.LogMessage(StdStringFormat("COpcAdapter Exception in %s - COpcAdapter::GatherDeviceData() %s\n",_device.c_str(), (LPCSTR)  e.what() ),LOWERROR);
		Off();
		Disconnect();
		hr= E_FAIL;
	}
	catch(...)
	{
		GLogger.LogMessage(StdStringFormat("COpcAdapter Exception in %s - COpcAdapter::GatherDeviceData()\n",_device.c_str() ),LOWERROR);
		Off();
		Disconnect();
		hr= E_FAIL;
	}
	_lastfilesize=filesize;
	lastmodtime=modtime;
	return hr;
}