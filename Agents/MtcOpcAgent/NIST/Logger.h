// **************************************************************************
// Logger.h 
//
// Description:
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.
// **************************************************************************
#pragma once
//#include "atlstr.h"
#include <fstream>
#include <iostream>
#include <atlcomtime.h>
#include "StdStringFcn.h"

#ifndef _DEBUG
#pragma comment( user, "NO LOGGING: Compiled on " __DATE__ " at " __TIME__ ) 
#define LogMessage(X,Y)
#define LOGONCE 
#else
#pragma comment( user, "LOGGING: Compiled on " __DATE__ " at " __TIME__ ) 
#define LOGONCE  static long nLog##__LINE__=0; if( 0 == nLog##__LINE__++) 
#define LogMessage(X,Y)  GLogger.Message(X,Y)
#endif

//#define LOGLISTENERS

#ifdef LOGLISTENERS
// This is for notifying listeners to logged messages - mnany to many relationship
#include <boost/function.hpp> 
#include <boost/function_equal.hpp> 
typedef boost::function<void (const TCHAR * msg)> FcnLognotify;
#include <vector>
#endif



/** Allow redirected of std::cout to vc console output
std::cout.rdbuf(&Logger);
std::cout << "Hello World\n";
std::cout.flush();
*/
class basic_debugbuf : public std::streambuf
{
public:
#ifdef LOGLISTENERS
	std::vector<FcnLognotify> listeners;
#endif
	basic_debugbuf(int bufferSize=1000) 
	{
		if (bufferSize)
		{
			char *ptr = new char[bufferSize];
			setp(ptr, ptr + bufferSize);
		}
		else
			setp(0, 0);
	}
	virtual ~basic_debugbuf() 
	{
		sync();
		delete[] pbase();
	}

	virtual void writeString(const std::string &str)
	{
#ifdef LOGLISTENERS
		for(int i=0; i< listeners.size(); i++)
			listeners[i](str.c_str());
#endif
		OutputDebugString(str.c_str());
	}
private:
	int	overflow(int c)
	{
		sync();
		if (c != EOF)
		{
			if (pbase() == epptr())
			{
				std::string temp;
				temp += char(c);
				writeString(temp);
			}
			else
				sputc(c);
		}
		return 0;
	}
	int	sync()
	{
		if (pbase() != pptr())
		{
			int len = int(pptr() - pbase());
			std::string temp(pbase(), len);
			writeString(temp);
			setp(pbase(), epptr());
		}
		return 0;
	}
};

struct CDebugLevel
{
	CDebugLevel(int n, char * name) : _debug(n), _name(name) {}
	int operator ()() { return _debug; }
	operator int () { return _debug; }
	operator std::string () { return _name; }
	int _debug;
	std::string _name;
};

__declspec(selectany)  	  CDebugLevel ENTRY      (0, "ENTRY");
__declspec(selectany)  	  CDebugLevel FATAL      (0, "FATAL");
__declspec(selectany)  	  CDebugLevel LOWERROR   (1, "ERROR");
__declspec(selectany)  	  CDebugLevel WARNING    (2, "WARN ");
__declspec(selectany)  	  CDebugLevel INFO       (3, "INFO ");
__declspec(selectany)  	  CDebugLevel DBUG       (4, "DEBUG");
__declspec(selectany)  	  CDebugLevel DETAILED   (5, "DETAIL ");
__declspec(selectany)  	  CDebugLevel HEAVYDEBUG (5, "HDEBUG ");



/**
Sample use:
CLogger logger;
logger.DebugString()=true;
logger << FATAL << "Fatal Message1\n"; 
logger << FATAL << "Fatal Message2\n"; 
logger.DebugString()=false;
logger.Open(::ExeDirectory() + "debug.txt");
logger << FATAL << "Fatal Message\n"; 
logger << DETAILED << "DETAILED Message\n"; 
logger << INFO << "INFO Message\n"; 
logger().close();
*/

class CLogger : public basic_debugbuf
{
	struct nullstream: std::fstream {
		nullstream(): std::ios(0),  std::fstream((_Filet *) NULL) {}
	};
public:
	std::string ExeDirectory()
	{
		TCHAR buf[1000];
		GetModuleFileName(NULL, buf, 1000);
		std::string path(buf);
		path=path.substr( 0, path.find_last_of( '\\' ) +1 );
		return path;
	}
	CLogger() 
	{
		DebugLevel()=0;
		filename=ExeDirectory() + "debug.txt";
		//std::cout.rdbuf(&_outputdebugstream);
		//std::cerr.rdbuf(&_outputdebugstream);
		OutputConsole()=0;
		Timestamping()=0;
		sDebugString="";
		_nCounter=1;
	}
	~CLogger()
	{
		if(DebugFile.is_open())
			DebugFile.close();
	}

	int & DebugLevel()				{ return _debuglevel; }
	bool & Timestamping()			{ return  bTimestamp; }
	std::string & DebugString()		{ return  sDebugString; }
	int & OutputConsole()			{ return  nOutputConsole; }
	std::ofstream & operator()(void) { return this->DebugFile; }
	std::string Dump()
	{
		std::stringstream ss;
		ss << "Global Logger\n";
		ss << "Debug Level = " <<  DebugLevel() << std::endl;
		ss << "DebugLevel = " <<  DebugLevel() << std::endl;
		return ss.str();
	}

	void Open(std::string filename, int bAppend=false)
	{
		this->filename=filename;
		Open(bAppend);
	}
	void Open(int bAppend=0)
	{	
		int opMode = std::fstream::out;
		if(bAppend)
			opMode |= std::fstream::app;

		DebugFile.open(filename.c_str(), opMode, OF_SHARE_DENY_NONE);
	}

	std::ostream  &operator<<( CDebugLevel level) 
	{ 
		std::fstream tmp;

		if( DebugLevel() < (int) level)
			return this->devnull;
		if(Timestamping())
		{
			return PrintTimestamp(std::cout,level );
		}
		//return PrintTimestamp(std::ostream(static_cast<std::streambuf*>( &_outputdebugstream)) ,level);

#if ( _MSC_VER >= 1600)   // 2010
		return PrintTimestamp(std::ostream(static_cast<std::streambuf*>( &_outputdebugstream)) ,level);
#else
		return PrintTimestamp(this->DebugFile,level);
#endif
	}
	std::ostream  & PrintTimestamp(std::ostream  & s, CDebugLevel level)
	{
		if(Timestamping())
		{
			s << Timestamp() << ": ";
			if(!sDebugString.empty())
				s << sDebugString << ": ";

			s << (std::string) level <<  " [" << (int ) _nCounter++ <<  "] ";
		}
		return s;
	} 
	int Message(std::string msg, int level=5)
	{
		if( DebugLevel() < level)
			return level;

		if(OutputConsole())
			OutputDebugString(msg.c_str());

		if(!DebugFile.is_open())
			return level;
		if(Timestamping())
			DebugFile << Timestamp();
		DebugFile << msg;
		// LOGLISTENERS is for notifying windows or other listeners
#ifdef LOGLISTENERS
		for(int i=0; i< listeners.size(); i++)
			listeners[i](msg.c_str());
#endif
		DebugFile.flush();
		return level;
	}
	int Abort(std::string msg){ return Message(msg,FATAL); abort(); }
	int Fatal(std::string msg){ return Message(msg,FATAL); }
	int Error(std::string msg){ return Message(msg,LOWERROR); }
	int Warning(std::string msg){return Message(msg,WARNING); }
	int Info(std::string msg){ return Message(msg,INFO); }
	int Status(std::string msg){return Message(msg,FATAL); }

	/////////////////////////////////////////////////////////////////////////////
	//2012-12-22T03:06:56.0984Z
	static std::string Timestamp()
	{
		SYSTEMTIME st;
		GetSystemTime(&st);
		return StdStringFormat("%4d-%02d-%02dT%02d:%02d:%02d.%04dZ", st.wYear, st.wMonth, st.wDay, st.wHour, 
			st.wMinute, st.wSecond, st.wMilliseconds);
		//return (LPCSTR) COleDateTime::GetCurrentTime().Format();
	}
public:
	basic_debugbuf _outputdebugstream;
	std::ofstream DebugFile;
protected:
	int _debuglevel;
	bool bTimestamp;
	std::string sDebugString; 
	int nOutputConsole;
	int nDebugReset;
	std::string filename;
	nullstream devnull;
	int _nCounter;

#ifdef LOGLISTENERS
	/** 
	void CMainFrame::MutexStep(std::string s)
	{
	OutputDebugString(s.c_str());
	}
	...
	m_view.AddListener("Step", boost::bind(&CMainFrame::MutexStep, this,_1))
	*/
public:
	void AddListener(FcnLognotify notify)
	{
		_outputdebugstream.listeners.push_back(notify);
	}
#endif
};
__declspec(selectany)  CLogger GLogger;


