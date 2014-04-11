//
// ShdrParser.h
//

#pragma once
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <ATLComTime.h>

#include "DataDictionary.h"
#include "StdStringFcn.h"
#include "File.h"

class CShdrParser
{
public:
	CShdrParser::CShdrParser(void)
	{
		_bRepeat=true;
		_bSetLastTime=true;
	}	
	~CShdrParser(void){}

	/////////////////////
	COleDateTime							_lasttime;
	std::string								_buffer;
	std::string                             _shdrfilename;
	bool									_bRepeat;
	bool									_bSetLastTime;
	std::ifstream							in;
	std::vector<DataDictionary>             _datum;
	std::vector<std::string>				_names;

	static COleDateTime GetOleDateTime(std::string s)
	{
		// parse 2012-02-03T17:31:51.0968Z 
		int Year, Month, Day, Hour, Minute, Second, Millisecond;
		if(sscanf(s.c_str(), "%d-%d-%dT%d:%d:%d.%d", &Year, &Month, &Day, &Hour, &Minute,&Second, &Millisecond)==7){}
		//12/2/2009 2:42:25 PM
		else if(sscanf(s.c_str(), "%d/%d/%4d%d:%d:%d", &Month, &Day, &Year,  &Hour, &Minute,&Second)==6){}
		else throw std::exception("Unrecognized date-time format\n");
		return COleDateTime( Year, Month, Day, Hour, Minute, Second ); 
	}



	std::vector<DataDictionary>  ReadStream()
	{
		//	std::vector<DataDictionary>  _datum;
		DataDictionary dict;
		std::vector<std::string> tokens;
		COleDateTime							_currenttime;
		COleDateTime							_lasttime;
		std::string buffer;
		std::string type = "event";
		getline(in,_buffer);
		if(in.eof( ))
		{
			in.clear();					// forget we hit the end of file
			in.seekg(0, std::ios::beg); // move to the start of the file
			if(!_bRepeat)
				return _datum;
			getline(in,_buffer);
		}
		if(buffer.empty())
			return _datum;
		tokens=TrimmedTokenize(_buffer, "|");
		if(tokens.size()< 1)
			return _datum;

		if(!_datum[0]["TIMESTAMP"].empty())
		{
			_lasttime=GetOleDateTime(_datum[0]["TIMESTAMP"]);
			_currenttime=GetOleDateTime(tokens[0]);
			COleDateTimeSpan duration = _currenttime-_lasttime;
			dict["DURATION"]=ConvertToString(duration.GetTotalSeconds());
		}
		else
		{
			dict["DURATION"]="0";
		}

		dict["TIMESTAMP"]=tokens[0];

		for(int i=1; i< tokens.size(); ) // skip tokens[0] = date
		{
			dict[tokens[i]]=tokens[i+1];
			i=i+2;
		}
		_datum.push_back(dict);
		//{
		//	if(i+5 >= tokens.size())
		//		break;
		//	_backend->StoreValue(_bstr_t(tokens[i].c_str()), 
		//		_bstr_t(tokens[i+1].c_str()), type, 
		//		_bstr_t(tokens[i+2].c_str()), 
		//		_bstr_t(tokens[i+3].c_str()),
		//		_bstr_t(tokens[i+4].c_str()),
		//		_bstr_t(tokens[i+5].c_str())
		//		);
		//	i=i+6;
		//}
		return _datum;
	}

	void Initialize(std::string filename) 
	{
		// FIXME: blank lines and other stuff not handled well.
		_shdrfilename=filename;
		if(GetFileAttributesA(_shdrfilename.c_str())== INVALID_FILE_ATTRIBUTES)
		{
			_shdrfilename= File.ExeDirectory() + _shdrfilename;
		}

		in.open(_shdrfilename.c_str());

	}
};

