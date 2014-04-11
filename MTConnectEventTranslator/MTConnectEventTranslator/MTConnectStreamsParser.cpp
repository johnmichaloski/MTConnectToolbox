//
// MTConnectStreamsParser.cpp
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#include "StdAfx.h"
#include "MTConnectStreamsParser.h"
#include "dcomutils.h"

#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

std::map<std::string, std::string>	MTConnectStreamsParser::_TagRenames;
std::vector<std::string> 	MTConnectStreamsParser::_saveddata;
std::map<std::string, std::string> MTConnectStreamsParser::_valuerenames;

static _bstr_t checkParseError(MSXML2::IXMLDOMParseErrorPtr pError)
{
	_bstr_t parseError =_bstr_t("At line ")+ _bstr_t(pError->Getline()) + _bstr_t("\n")+ _bstr_t(pError->Getreason());
	//MessageBox(NULL,parseError, "Parse Error",MB_OK);
	return parseError;

}

static 	void dump_com_error(_com_error &e)
{
	::AtlTrace("Error\n");
	//TRACE1("\a\tCode = %08lx\n", e.Error());
	//TRACE1("\a\tCode meaning = %s", e.ErrorMessage());
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());
	::AtlTrace("\a\tSource = %s\n", (LPCSTR) bstrSource);
	::AtlTrace("\a\tDescription = %s\n", (LPCSTR) bstrDescription);
}
	

MTConnectStreamsParser::MTConnectStreamsParser(void)
{
	_nLastProbed=0;
	_nLastProbedSeq=-1;

}

MTConnectStreamsParser::~MTConnectStreamsParser(void)
{
}
void MTConnectStreamsParser::Release(void)
{
	m_pSchemaCache=NULL;
	m_pXSDDoc=NULL;
	m_pXMLDoc=NULL;
}

_bstr_t MTConnectStreamsParser::GetAttribute(MSXML2::IXMLDOMNodePtr node, _bstr_t attribute)
{
	_bstr_t text= L"";;
	CComPtr<MSXML2::IXMLDOMNamedNodeMap> attributes;
	node->get_attributes( &attributes );
	if( attributes ) 
	{
		MSXML2::IXMLDOMNodePtr attr = attributes->getNamedItem(attribute);
		if(attr==NULL)
			return text;
		if(attr->nodeValue.vt == VT_BSTR)
			return attr->nodeValue.bstrVal;
	}

	return text;
}

std::string MTConnectStreamsParser::ParseXMLDocument(_bstr_t xmlfile, _bstr_t xsdfile, _bstr_t xsdname)
{ 
	_bstr_t parseError(L"");
	try{
		IXMLDOMParseErrorPtr  pError;

		// load the XML file
		// ****** you need to use IXMLDOMDocument2 interface *********
		HRESULT hr = m_pXMLDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
		m_pXMLDoc->async =  VARIANT_FALSE; 

		hr = m_pXMLDoc->load(xmlfile);      

		//check on the parser error      
		if(hr!=VARIANT_TRUE)
		{
			return (LPCSTR) checkParseError(m_pXMLDoc->parseError);
		}
#if 0
		//load the XSD file
		if(xsdfile!=NULL)
		{
			hr = m_pXSDDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));
			m_pXSDDoc->async =  VARIANT_FALSE; 

			hr = m_pXSDDoc->load(xsdfile /*"books.xsd"*/);

			//check on the parser error      
			if(hr!=VARIANT_TRUE)
			{         
				return checkParseError(m_pXSDDoc->parseError);
			}

			//create schemacache
			hr = m_pSchemaCache.CreateInstance(__uuidof(MSXML2::XMLSchemaCache));
			m_pXMLDoc->schemas = m_pSchemaCache.GetInterfacePtr();

			//hook it up with XML Document
			hr = m_pSchemaCache->add(xsdname /*"urn:books"*/, m_pXSDDoc.GetInterfacePtr());   

			//call validate 
			pError = m_pXMLDoc->validate();
		}
#endif
		//
		//if(pError->errorCode != S_OK)
		//{
		//	parseError = _bstr_t("Error code: ")+ _bstr_t(pError->errorCode) +_bstr_t("\n") + _bstr_t("Reason: ")+ pError->Getreason();
		//	MessageBox(NULL, (char*)parseError, "Parse Error",MB_OK);
		//}

	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return  e.ErrorMessage();
	}

	return (LPCSTR) parseError;
}
// Replace into data with newer from data
static DataDictionary  Merge(DataDictionary &into, DataDictionary &from)
{
	DataDictionary newdata = into;
	for(DataDictionary::iterator it=from.begin(); it!=from.end(); it++)
	{
		into[(*it).first]=(*it).second;
	}
	return newdata;
}
std::string	MTConnectStreamsParser::GetMTConnectUrl(std::string ippath)
{
	std::string _devicesfilename = "http://" + ippath + "/current";
	return _devicesfilename;
}

std::vector<DataDictionary> MTConnectStreamsParser::ReadStream(std::string filename)
{
	DataDictionary data;
	std::vector<DataDictionary> datums;
	std::string ip;
	ip = filename.substr( 0, filename.find_last_of( '/' ) );
	ip = ip.substr( 0, ip.find_last_of( ':' ) );
	//if(!TestPing(ip))
	//	return datums;


	_devicesfilename = "http://" + filename + "/current";

	// Not empty means error
	if(!ParseXMLDocument(_devicesfilename.c_str()).empty())
		return datums;

	data=ParseDataItems();

#if 0
	nProbed = ConvertString<int>(data["probed"], 0);
	nProbeSeq = streamdata["probed"].sequence;
	int nlastprobeseq = laststreamdata["probed"].sequence;
	if(nProbeSeq !=nlastprobeseq)
	//if(nProbed > (_nLastProbed+1))
	{
		if(_nLastProbedSeq<0)
		{
			_nLastProbedSeq=nProbeSeq;
			_lastprobedata=data;
		}
		TimedDataDictionary::iterator _nexttimestamp =timeddata.end();
		_nexttimestamp--;
		
		// Get all the previous probed and other sample/event information
		for(int i=_nLastProbedSeq; i< nProbeSeq; i++)
		{
			_devicesfilename = "http://" + filename + "//current?at=" + ToString<int>(i);
			if(ParseXMLDocument(_devicesfilename.c_str()).empty())
				ParseDataItems(); // will build up the timestamp data
		}
		OutputDebugString(DumpTimedDataDictionary().c_str());
		//TimedDataDictionary::iterator it=_attimestamp;
		//for(it++; it!=_nexttimestamp; it++)
		//{
		//	DataDictionary newdata;
		//	DataDictionary  dict = (*it).second;
		//	if(dict.find("probed") != dict.end())
		//	{
		//		// Merge data into _lastprobedata
		//		// Save copy of updated _lastprobedata
		//		// Dict is a partial snapshot, _lastprobedata should be complete
		//		newdata=Merge(_lastprobedata, dict);
		//	}
		//}
	}

	_nLastProbed=nProbed;
	_nLastProbedSeq=nProbeSeq;
	_lastprobedata=data;
	laststreamdata=streamdata;
#endif
	datums.push_back(data);
	m_pXMLDoc=NULL;
	return datums;
}

DataDictionary MTConnectStreamsParser::ParseDataItems()
{
	MSXML2::IXMLDOMNodePtr root = m_pXMLDoc->GetdocumentElement();
	MSXML2::IXMLDOMNodeListPtr nodes = root->selectNodes(_bstr_t("//DeviceStream"));
	DataDictionary data;
	try
	{
		for(int i=0; i< nodes->length; i++)
		{
			MSXML2::IXMLDOMNodePtr pNode = NULL;					
			nodes->get_item(i, &pNode);

			_bstr_t items[3] = {_bstr_t(".//Samples"), _bstr_t(".//Events") , _bstr_t(".//Condition") };
			for(int ii=0; ii<3 ; ii++)
			{
				MSXML2::IXMLDOMNodeListPtr samples = pNode->selectNodes(items[ii]);
				for(int j=0; j< samples->length; j++)
				{
					MSXML2::IXMLDOMNodePtr pSampleHive = NULL;					
					samples->get_item(j, &pSampleHive);

					// Get each child
					MSXML2::IXMLDOMNodeListPtr childs = pSampleHive->childNodes;
					for(int k=0; k< childs->length; k++)
					{
						MSXML2::IXMLDOMNodePtr pSample = NULL;
						ptime datetime;
						std::string name ;
						std::string value;
						std::string timestamp;
						std::string sequence;

						childs->get_item(k, &pSample);

						name = (LPCSTR)  GetAttribute(pSample, "name");

						if(name.empty())
							name = (LPCSTR)  GetAttribute(pSample, "dataItemId");
						

						if(name.empty())
							continue;

						// Lookup any name remapping to shorten - e.g., Frt or path_feedratefrt => Feed
						if(_TagRenames.find(name)!= _TagRenames.end())
						{
							name = _TagRenames[name];
						}
#if 0
						// FIXME: is this necessary?
						// Check to see if data item to be saved
						if(std::find(_saveddata.begin(), _saveddata.end(), name) == _saveddata.end())
							continue;
#endif

						value = (LPCSTR) pSample->Gettext();
						if(items[ii]== bstr_t(".//Condition") )
							value =  std::string((LPCSTR) pSample->nodeName) + "."  + value  ;

						if(_valuerenames.find(name+"."+value)!=_valuerenames.end())
							value=_valuerenames[name+"."+value];

						timestamp = (LPCSTR)  GetAttribute(pSample, "timestamp");
						sequence = (LPCSTR)  GetAttribute(pSample, "sequence");
			
						data[name]= value;
#if 0
						if(!timestamp.empty())
						{
							datetime=GetDateTime(timestamp);
							DataDictionary & dict(timeddata[datetime]) ;
							dict[name]=data[name];
							//for(DataDictionary::iterator it=dict.begin(); it!=dict.end(); it++)
							//{
							//	OutputDebugString(StdStringFormat("%s %s=%s\n", timestamp.c_str(), 
							//		(*it).first.c_str(), (*it).second.c_str()).c_str());
							//}
						}

						streamdata[name]=StreamData(name,data[name], timestamp, sequence);
#endif
					}
				}
			}
		}
	}
	catch(...)
	{
		std::cout<< "MTConnectStreamsParser::ParseDataItems() Exception\n";
	}
	return data;

}
