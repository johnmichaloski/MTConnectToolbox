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
#include "atlstr.h"
#include <fstream>
#include <iostream>
#include <atlcomtime.h>

#include "StdStringFcn.h"
#pragma message("Compiling c:\\Program Files\\NIST\\proj\\MTConnect\\Nist\\MTConnectGadgets\\Agent - fanuc iSeries\\Logger.h")

//#define LOGLISTENERS

#ifdef LOGLISTENERS
// This is for notifying listeners to logged messages - mnany to many relationship
#include <boost/function.hpp> 
#include <boost/function_equal.hpp> 
typedef boost::function<void (const TCHAR * msg)> FcnLognotify;
#include <vector>
#endif

// This is useful for preventing endless repeated error messages 
#define LOGONCE  static long nLog##__LINE__=0; if( 0 == nLog##__LINE__++) 
//#define LOGONCE  

#include <ostream>
#include <Windows.h>

/// \brief This class is a derivate of basic_stringbuf which will output all the written data using the OutputDebugString function
#define nullptr NULL
/**
Use:
#ifdef _WIN32
static basic_debugbuf<char> charDebugOutput;
std::cerr.rdbuf(&charDebugOutput);
std::clog.rdbuf(&charDebugOutput);
*/
template<typename TChar, typename TTraits = std::char_traits<TChar>>
class basic_debugbuf : public std::basic_stringbuf<TChar,TTraits> {
public:
#ifdef LOGLISTENERS
	std::vector<FcnLognotify> listeners;
#endif

	explicit basic_debugbuf() : _buffer(256) {
		setg(nullptr, nullptr, nullptr);
		// Visual Studio 2008 code
		setp(&_buffer[0], &_buffer[0], &_buffer[0] + _buffer.size());
		//setp(_buffer.data(), _buffer.data(), _buffer.data() + _buffer.size());
	}

	~basic_debugbuf() {
	}

	//	static_assert(std::is_same<TChar,char>::value || std::is_same<TChar,wchar_t>::value, "basic_debugbuf only supports char and wchar_t types");

	int sync()
	{
		try {
			MessageOutputer<TChar,TTraits>()(pbase(), pptr());
			//Visual Studio 2010 has data() member in vector class
			// setp(_buffer.data(), _buffer.data(), _buffer.data() + _buffer.size());

			// Visual Studio 2008 code
			setp(&_buffer[0], &_buffer[0], &_buffer[0] + _buffer.size());
			return 0;
		} catch(...) {
			return -1;
		}
	}

	std::char_traits<char>::int_type overflow(std::char_traits<char>::int_type c = TTraits::eof()) {
		int syncRet = sync();
		if (c != TTraits::eof()) {
			_buffer[0] = c;
			// Visual Studio 2010 code
			// setp(_buffer.data(), _buffer.data() + 1, _buffer.data() + _buffer.size());

			// Visual Studio 2008 code
			setp(&_buffer[0], &_buffer[0] + 1, &_buffer[0] + _buffer.size());
		}
		return syncRet == -1 ? TTraits::eof() : 0;
	}

private:
	std::vector<TChar>	 _buffer;

	template<typename TChar, typename TTraits>
	struct MessageOutputer;

	template<>
	struct MessageOutputer<char,std::char_traits<char>> {
		template<typename TIterator>
		void operator()(TIterator begin, TIterator end) const {
			std::string s(begin, end);
			OutputDebugStringA(s.c_str());
		}
	};

	template<>
	struct MessageOutputer<wchar_t,std::char_traits<wchar_t>> {
		template<typename TIterator>
		void operator()(TIterator begin, TIterator end) const {
			std::wstring s(begin, end);
			OutputDebugStringW(s.c_str());
		}
	};
};
#if 0
/** Allow redirected of std::cout to vc console output
std::cout.rdbuf(&Logger);
std::cout << "Hello World\n";
std::cout.flush();
*/
class basic_debugbuf : public std::streambuf
{
public:
	std::vector<FcnLognotify> listeners;
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
		for(int i=0; i< listeners.size(); i++)
			listeners[i](str.c_str());
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
#endif
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
__declspec(selectany)  	  CDebugLevel FATAL      (-1, "FATAL");
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

class CLogger : public basic_debugbuf<char>
{
	struct nullstream: std::fstream {
		nullstream(): std::ios(0),  std::fstream((_Filet *) NULL) {}
	};
public:
	CLogger() 
	{
		DebugLevel()=7;
		filename=::ExeDirectory() + "debug.txt";
		std::cout.rdbuf(&_outputdebugstream);
		std::cerr.rdbuf(&_outputdebugstream);
		Timestamping()=false;
		DebugString()=false;
		_nCounter=1;
	}
	~CLogger()
	{
		if(DebugFile.is_open())
			DebugFile.close();
	}

	int & DebugLevel()				{ return _debuglevel; }
	bool & Timestamping()			{ return  bTimestamp; }
	bool & DebugString()			{ return  bDebugString; }
	std::ofstream & operator()(void) { return this->DebugFile; }

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
		if(DebugString())
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
			s << (std::string) level <<  " [" << (int ) _nCounter++ <<  "] ";
		}
		return s;
	} 
	int LogMessage(std::string msg, int level=-1)
	{
		if( DebugLevel() < level)
			return level;

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
	int Fatal(std::string msg){ return LogMessage(msg,FATAL); }
	int Error(std::string msg){ return LogMessage(msg,LOWERROR); }
	int Warning(std::string msg){return LogMessage(msg,WARNING); }
	int Info(std::string msg){ return LogMessage(msg,INFO); }
	int Status(std::string msg){return LogMessage(msg,FATAL); }

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
	basic_debugbuf<char> _outputdebugstream;
	std::ofstream DebugFile;
protected:
	int _debuglevel;
	bool bTimestamp;
	bool bDebugString;  
	int nDebugReset;
	std::string filename;
	nullstream devnull;
	int _nCounter;

#ifdef LOGLISTENERS
public:
	void AddListener(FcnLognotify notify)
	{
		_outputdebugstream.listeners.push_back(notify);
	}
#endif
};
__declspec(selectany)  CLogger GLogger;


