//
// SocketParser.h
//

/*
 * DISCLAIMER:
 * This software was produced by the National Institute of Standards
 * and Technology (NIST), an agency of the U.S. government, and by statute is
 * not subject to copyright in the United States.  Recipients of this software
 * assume all responsibility associated with its operation, modification,
 * maintenance, and subsequent redistribution.
 *
 * See NIST Administration Manual 4.09.07 b and Appendix I.
 */

#pragma once
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include "SocketBackEnd.h"
#include "StdStringFcn.h"
#include "Timing.h"

using namespace Timing;
typedef std::vector<uint8_t> raw_message_t;
/**
* \brief Class provides a simple file shdr parser.
*/
class SocketParser
{
public:
	/**
	* \brief Constructor that turn repeat of file off, set first time and captures current time.
	* Todo: the clock resolution is only second level, and MTConnect easily has millisecond Socket updates.
	*/
	SocketParser(SocketBackEnd & backEnd) : _backEnd(backEnd)
	{
		_bRepeat=false;
		_bFirstTime=true;
		_realtimetime = Timing::Now(); // COleDateTime::GetCurrentTime();
		_linenum=0;
	}	
	/**
	* \brief Reset is done before the beginning of a file. Either first or each repeated time.
	*/
	void Reset(void)
	{
		_bFirstTime=true;
		_realtimetime = Timing::Now(); // COleDateTime::GetCurrentTime();
		_linenum=0;
		_backEnd.Reset();
	}


	void split(std::string str, raw_message_t& msg)
	{
		const std::string::size_type size(str.size());
		size_t start=0;
		size_t range=0;
		msg.clear();

		/* Explanation: 
		* Range - Length of the word to be extracted without spaces.
		* start - Start of next word. During initialization, starts at space 0.
		* 
		* Runs until it encounters a ' ', then splits the string with a substr() function,
		* as well as making sure that all characters are lower-case (without wasting time
		* to check if they already are, as I feel a char-by-char check for upper-case takes
		* just as much time as lowering them all anyway.                                       
		*/
		size_t i;
		for(i=0; i < str.size(); i++ )
		{
			if(i==443)
				i=i;
			if( str[i] == ',' )
			{
				std::string buf = str.substr(start, range);
				int uc;
				OutputDebugString(StrFormat("Start=%d Range=%d i=%d Size=%d\n", start,range,i, size).c_str());

				sscanf(buf.c_str(), "%2X", &uc);
				msg.push_back((uint8_t) uc );
				start = i + 1;
				range = 0;
			} else
				++range;
		}
		//msg.push_back( toLower(str.substr(start, range)) );
		// skip trailing \n
	}
	/**
	* \brief Processes a line from the given Socket file.
	* Extracts timestamp (first field until | delimiter) and computes how long from last timestamp.
	* If the repeat flag is set, will repeat Socket file from start, resetting all timestamp value to new current time.
	* \return amount of time to delay in seconds given shdr.
	*/
	double  ProcessStream()
	{
		//Get line from "in" stream into string _buffer until line feed found.
		// If the line feed delimiter is found, it is extracted and discarded
		getline(in,_buffer);
		//_buffer.clear();
		//char ch;
		//do{
		//	ch = in.get();
		//	_buffer+=ch;
		//}
		//while(ch!='\n');
		//
		//_buffer=Trim(_buffer); // this will remove trailing \n

		if(in.eof( ))
		{
			if(!Repeat())
				return -1;
			Reset();
			in.clear();					// forget we hit the end of file
			in.seekg(0, std::ios::beg); // move to the start of the file
			getline(in, _buffer);
		}
		_linenum++;

		// Check for empty buffer - return 0 wait
		if(_buffer.empty())
			return 0;

		// Now get timestamp - in field 1
		size_t delim =  _buffer.find( "|");
		if( delim == std::string::npos)
			return 0;

		std::string timestamp = _buffer.substr(0,delim);

		if(!timestamp.empty())
		{
			_lasttime=_currenttime;
			_currenttime=Timing::GetDateTime(timestamp);
			if(_bFirstTime)
			{
				_duration = boost::posix_time::seconds(0) ; 
				_bFirstTime=false;
			}
			else
			{
				_duration = _currenttime-_lasttime;
				_realtimetime+=_duration;
			}
			// in shdr this returns a shdr string with new timestamp.

			_timestamp=Timing::GetDateTimeString(_realtimetime);
			_buffer=_buffer.substr(delim+1); // after the |
			split(_buffer, _msg);
			
			// For binary hex dump need to translate ascii hex into raw binary. 
			// also need to add timestamp for timing and remove trailing \n
		}
		else
		{
			_duration = boost::posix_time::seconds(0); 
		}
		//http://www.boost.org/doc/libs/1_31_0/libs/date_time/doc/class_time_duration.html
		return _duration.total_milliseconds();
	}
	/**
	* \brief Attempts to open Socket file for echoing, throw execption if unable to open file.
	* \param filename is the Socket filename including path
	*/
	void Init(std::string filename) 
	{
		// FIXME: blank lines and other stuff not handled well.
		_binaryfilename=filename;
#ifdef WIN32
		if(GetFileAttributesA(_binaryfilename.c_str())== INVALID_FILE_ATTRIBUTES)
		{
			throw std::runtime_error(StrFormat("Bad Socket file",_binaryfilename.c_str()));
		}
#endif
		in.open(_binaryfilename.c_str());
	}
	/**
	* \brief Flag to determine replaying of file. True replay. False one-time play.
	* \return reference to replay flag.
	*/
	bool & Repeat() { return	_bRepeat; }
	/**
	* \brief Get latest buffer from shdr file, with updated timestamp.
	*/
	std::string & GetLatestBuffer() { return _buffer; }
	raw_message_t & GetLatestMsg() { return _msg; }
	std::string & GetLatestTimestamp() { return _timestamp; }

	/**
	* \brief Get latest buffer from shdr file, with updated timestamp.
	*/
	size_t & LineNumber() { return _linenum; }

	//////////////////////////////////////////////////////////////////////
protected:

	boost::posix_time::ptime				_realtimetime; /**< now + elapsed Socket time */
	boost::posix_time::ptime				_currenttime; /**< shdr current timestamps */
	boost::posix_time::ptime				_lasttime; /**< last shdr current timestamps */
	boost::posix_time::time_duration        _duration; /**< duration between last and current shdr timestamps */

	std::string								_timestamp;   /**< string with updated timestamp Socket */
	std::string								_buffer;   /**< string with raw data as hex ascii */
	raw_message_t							_msg;   /**< buffer with raw binary data */
	std::string                             _binaryfilename; /**< string with Socket filename incl path*/
	bool									_bRepeat; /**< repeat Socket flag*/
	bool									_bFirstTime; /**< first time flag for deciphering timestamps*/
	std::ifstream							in; /**< input filestream*/
	size_t                                   _linenum; /**< current line number in file useful for exceptions*/
	SocketBackEnd & _backEnd;
};



inline void test_socket_parser()
{
	SocketBackEnd be;
	SocketParser test(be);
	raw_message_t msg;
	std::string str = "2017-11-03 09:00:09.0471 |00,00,00,90,00,00,00,0F,00,00,00,01,00,00,00,00,00,00,00,00,00,00,00,02,00,00,00,00,3F,4B,11,AF,3E,C1,FE,4D,00,00,00,00,BF,F7,DA,1F,BA,56,FF,35,BF,52,0B,20,BF,65,F7,BA,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,\n";
	size_t delim =  str.find( "|");
	str=str.substr(delim+1); 
	str=Trim(str);
	test.split(str, msg);
}