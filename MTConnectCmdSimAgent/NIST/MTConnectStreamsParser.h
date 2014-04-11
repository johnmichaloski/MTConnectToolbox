//
// MTConnectStreamsParser.h
//

#pragma once

#pragma comment( lib, "msxml2.lib" )
#import <msxml6.dll>
#include <string>
#include <vector>
#include <map>
#include "StdStringFcn.h"

#include "boost/date_time/posix_time/posix_time.hpp"
#include <AtlComTime.h>

using namespace boost::posix_time;
  using namespace boost::gregorian;


/*class less_NoCaseString
{
public:
	bool operator() (const std::string& x, const std::string& y) const
	{return (stricmp( x.c_str(), y.c_str() )<0); }
}*/;


  struct StreamData
  {
	  enum TimeFormat
	  {
		  HUM_READ,
		  GMT,
		  GMT_UV_SEC,
		  LOCAL
	  };
	  std::string GetTimeStamp(TimeFormat format=GMT_UV_SEC);
	  ptime GetDateTime(std::string s);	ptime	datetime;
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
//typedef std::map<std::string, std::string, less_NoCaseString> DataDictionary;
#include "DataDictionary.h"

typedef std::map<std::string, StreamData> StreamDataDictionary;
typedef std::map<ptime, DataDictionary > TimedDataDictionary;

class MTConnectStreamsParser
{
	MSXML2::IXMLDOMSchemaCollectionPtr m_pSchemaCache;
	MSXML2::IXMLDOMDocumentPtr         m_pXSDDoc;
	MSXML2::IXMLDOMDocument2Ptr        m_pXMLDoc;
	// Handling the device specification or probe?
	std::string _devicesfilename;
	int _nLastProbed;
	int _nLastProbedSeq;
	StreamDataDictionary streamdata;
	TimedDataDictionary timeddata;
	DataDictionary _lastprobedata;
	ptime _lasttimestamp; 

	StreamDataDictionary laststreamdata;

public:

	MTConnectStreamsParser(void);
	~MTConnectStreamsParser(void);
	std::string							ParseXMLDocument(_bstr_t xmlfile, _bstr_t xsdfile="", _bstr_t xsdname="");
	DataDictionary						ParseDataItems();
	std::map<std::string, std::string> TagRenames;

	_bstr_t								GetAttribute(MSXML2::IXMLDOMNodePtr node, _bstr_t attribute);
	_bstr_t								GetElement(MSXML2::IXMLDOMNodePtr node);
	std::vector<DataDictionary>			ReadStream(std::string filename);
	std::string							CreateHtmlTable(DataDictionary data);
	void								DumpData(DataDictionary data);
	std::string							HtmlStyle();
	std::string							CreateHtmlDocument();
	std::string DumpTimedDataDictionary()
	{
			std::string tmp;
		for(TimedDataDictionary::iterator it=  timeddata.begin(); it != timeddata.end(); it++)
		{
			tmp+= to_simple_string((*it).first) ;
			DataDictionary d = (*it).second;
			for(DataDictionary::iterator dit=d.begin(); dit!=d.end(); dit++)
			{
				tmp+="|";
				tmp+=(*dit).first;
				tmp+="=";
				tmp+=(*dit).second;

			}
			tmp+="\n";

		}
		return tmp;
	}


};
