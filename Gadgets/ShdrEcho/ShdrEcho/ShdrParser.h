//
// ShdrParser.h
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
#include "ShdrBackEnd.h"
#include "StdStringFcn.h"
#include "Timing.h"

using namespace Timing;

/**
* \brief Class provides a simple file shdr parser.
*/
class ShdrParser
{
public:
	/**
	* \brief Constructor that turn repeat of file off, set first time and captures current time.
	* Todo: the clock resolution is only second level, and MTConnect easily has millisecond SHDR updates.
	*/
	ShdrParser(ShdrBackEnd & backEnd) : _backEnd(backEnd)
	{
		_bRepeat=false;
		_bFirstTime=true;
		_realtimetime = Timing::Now(); // COleDateTime::GetCurrentTime();
		_linenum=0;
	}	
	/**
	* \brief Reset is done before the beginning of a file. Either first or each repeated time.
	*/
	void ShdrParser::Reset(void)
	{
		_bFirstTime=true;
		_realtimetime = Timing::Now(); // COleDateTime::GetCurrentTime();
		_linenum=0;
		_backEnd.Reset();
	}

	/**
	* \brief Processes a line from the given SHDR file.
	* Extracts timestamp (first field until | delimiter) and computes how long from last timestamp.
	* If the repeat flag is set, will repeat SHDR file from start, resetting all timestamp value to new current time.
	* \return amount of time to delay in seconds given shdr.
	*/
	double  ProcessStream()
	{
		//Get line from "in" stream into string _buffer until line feed found.
		// If the line feed delimiter is found, it is extracted and discarded
		getline(in,_buffer);

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

		// Skip lines that start with * like:
		// * uuid: MAZAK-M74KP230234
		// * serialNumber: M74KP230234
		// * description: Mazak FCA751PY-N08
		// return 0 wait and send information buffer
		if(Trim(_buffer)[0]=='*')
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
			_buffer=Timing::GetDateTimeString(_realtimetime)+_buffer.substr(delim); 
		}
		else
		{
			_duration = boost::posix_time::seconds(0); 
		}
		//http://www.boost.org/doc/libs/1_31_0/libs/date_time/doc/class_time_duration.html
		return _duration.total_milliseconds();
	}
	/**
	* \brief Attempts to open SHDR file for echoing, throw execption if unable to open file.
	* \param filename is the SHDR filename including path
	*/
	void Init(std::string filename) 
	{
		// FIXME: blank lines and other stuff not handled well.
		_shdrfilename=filename;
#ifdef WIN32
		if(GetFileAttributesA(_shdrfilename.c_str())== INVALID_FILE_ATTRIBUTES)
		{
			throw std::runtime_error(StrFormat("Bad SHDR file",_shdrfilename.c_str()));
		}
#endif
		in.open(_shdrfilename.c_str());
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

	/**
	* \brief Get latest buffer from shdr file, with updated timestamp.
	*/
	size_t & LineNumber() { return _linenum; }

	//////////////////////////////////////////////////////////////////////
protected:

	boost::posix_time::ptime				_realtimetime; /**< now + elapsed SHDR time */
	boost::posix_time::ptime				_currenttime; /**< shdr current timestamps */
	boost::posix_time::ptime				_lasttime; /**< last shdr current timestamps */
	boost::posix_time::time_duration        _duration; /**< duration between last and current shdr timestamps */

	std::string								_buffer;   /**< string with updated timestamp SHDR */
	std::string                             _shdrfilename; /**< string with SHDR filename incl path*/
	bool									_bRepeat; /**< repeat SHDR flag*/
	bool									_bFirstTime; /**< first time flag for deciphering timestamps*/
	std::ifstream							in; /**< input filestream*/
	size_t                                   _linenum; /**< current line number in file useful for exceptions*/
	ShdrBackEnd & _backEnd;
};



