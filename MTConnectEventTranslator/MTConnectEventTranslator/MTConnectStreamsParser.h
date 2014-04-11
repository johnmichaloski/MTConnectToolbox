//
// MTConnectStreamsParser.h
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#pragma once

#pragma comment( lib, "msxml2.lib" )
#import <msxml6.dll>
#include <string>
#include <vector>
#include <map>
#include "StdStringFcn.h"
#include "DataDictionary.h"

#include "boost/date_time/posix_time/posix_time.hpp"
#include <AtlComTime.h>

using namespace boost::posix_time;
  using namespace boost::gregorian;


struct StreamData
{
	ptime	datetime;
	int				sequence;
	std::string		value;
	std::string		name;
	StreamData(std::string	_name="", std::string _value="", std::string timestamp="", std::string _sequence=""):
	name(_name),value(_value) 
	{
		if(!timestamp.empty())
			datetime=GetDateTime(timestamp);
		sequence=ConvertString<int>(_sequence, 0);
	}

};


// name, type, id
typedef std::map<std::string, StreamData> StreamDataDictionary;
typedef std::map<ptime, DataDictionary > TimedDataDictionary;

class MTConnectStreamsParser
{
	MSXML2::IXMLDOMSchemaCollectionPtr m_pSchemaCache;
	MSXML2::IXMLDOMDocumentPtr         m_pXSDDoc;
	MSXML2::IXMLDOMDocument2Ptr        m_pXMLDoc;
	std::string							_devicesfilename;
	int									_nLastProbed;
	int									_nLastProbedSeq;
	StreamDataDictionary				streamdata;
	TimedDataDictionary					timeddata;
	DataDictionary						_lastprobedata;
	ptime								_lasttimestamp; 
	StreamDataDictionary				laststreamdata;

public:
	static std::map<std::string, std::string>		_TagRenames;
	static std::vector<std::string> 				_saveddata;
	static std::map<std::string, std::string>		_valuerenames;

public:
	MTConnectStreamsParser(void);
	~MTConnectStreamsParser(void);

	_bstr_t						GetAttribute(MSXML2::IXMLDOMNodePtr node, _bstr_t attribute);
	void						Release();
	std::string					ParseXMLDocument(_bstr_t xmlfile, _bstr_t xsdfile="", _bstr_t xsdname="");
	DataDictionary				ParseDataItems();
	std::vector<DataDictionary> ReadStream(std::string filename);
	static std::string			GetMTConnectUrl(std::string ippath);
};
