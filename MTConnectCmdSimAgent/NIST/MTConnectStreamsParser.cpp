//
// MTConnectStreamsParser.cpp
//

#include "StdAfx.h"
#include "MTConnectStreamsParser.h"
#include <boost/thread/mutex.hpp>

ptime StreamData::GetDateTime(std::string s)
{
	// parse 2012-02-03T17:31:51.0968Z 
	int Year, Month, Day, Hour, Minute, Second, Millisecond;
	if(sscanf(s.c_str(), "%d-%d-%dT%d:%d:%d.%d", &Year, &Month, &Day, &Hour, &Minute,&Second, &Millisecond)==7){}
	//12/2/2009 2:42:25 PM
	else if(sscanf(s.c_str(), "%d/%d/%4d%d:%d:%d", &Month, &Day, &Year,  &Hour, &Minute,&Second)==6){}
	else throw std::exception("Unrecognized date-time format\n");

	return ptime( date(Year,Month,Day),
		hours(Hour)  +
		minutes(Minute) +
		seconds(Second) +
		boost::posix_time::millisec(int(Millisecond)) );

	//return COleDateTime( Year, Month, Day, Hour, Minute, Second ); 
}

std::string StreamData::GetTimeStamp(TimeFormat format)
{
	SYSTEMTIME st;
	char timestamp[64];
	GetSystemTime(&st);
	sprintf(timestamp, "%4d-%02d-%02dT%02d:%02d:%02d", st.wYear, st.wMonth,
		st.wDay, st.wHour, st.wMinute, st.wSecond);

	if (format == GMT_UV_SEC)
	{
		sprintf(timestamp + strlen(timestamp), ".%04dZ", st.wMilliseconds);
	}
	else
	{
		strcat(timestamp, "Z");
	}

	return timestamp;
}
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

_bstr_t MTConnectStreamsParser::GetElement(MSXML2::IXMLDOMNodePtr node)
{
	_bstr_t text= L"";;
	CComPtr<IXMLDOMElement> domElement;
	text=node->GetnodeName(); 
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

static boost::mutex io_mutex;

std::vector<DataDictionary> MTConnectStreamsParser::ReadStream(std::string filename)
{
	DataDictionary data;
	std::vector<DataDictionary> datums;
	int nProbed ;
	int nProbeSeq ;
	//	TimedDataDictionary::iterator _attimestamp =timeddata.end(); 
	boost::mutex::scoped_lock    lock(io_mutex);

	_devicesfilename = "http://" + filename + "/current";

	// Not empty means error
	bool bFlag;
	TIME(bFlag=ParseXMLDocument(_devicesfilename.c_str()).empty());
	if(!bFlag)
		return datums;

	TIME(data=ParseDataItems());

	datums.push_back(data);
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

			_bstr_t items[3] = {_bstr_t(".//Samples"), _bstr_t(".//Events"), _bstr_t(".//Condition") };
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
						std::string timestamp;
						std::string sequence;


						childs->get_item(k, &pSample);
						name = (LPCSTR)  GetAttribute(pSample, "name");
						if(name.empty())
							name = (LPCSTR)  GetAttribute(pSample, "dataItemId");
						if(name.empty())
							continue;
						timestamp = (LPCSTR)  GetAttribute(pSample, "timestamp");
						sequence = (LPCSTR)  GetAttribute(pSample, "sequence");

						// Skip asset stuff for now
						if(name.find("asset")!= std::string::npos)
							continue;

						// Lookup any name remapping to shorten
						if(TagRenames.find(name)!= TagRenames.end())
						{
							name = TagRenames[name];
						}
						if(items[ii] == _bstr_t(".//Samples"))
							data[name]=(LPCSTR) pSample->Gettext();
						else if(items[ii] == _bstr_t(".//Events"))
							data[name]=(LPCSTR) pSample->Gettext();
						else if(items[ii] == _bstr_t(".//Condition"))
						{
							std::string tagname  = (LPCSTR)  GetElement(pSample);
							if(stricmp(tagname.c_str(),"FAULT")==0)
								data[name]="fault";
							else
								data[name]="normal";
						}
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

std::string  MTConnectStreamsParser::CreateHtmlTable(DataDictionary data)
{
	std::string updatetable;
	std::string style;

	updatetable += "<TABLE>\n";
	updatetable +=   "<TR>";
	std::string skip( "estop,controllermode,PartCountActual,avail,PartCountBad,power,probed,ProbeNumber,probe,path_feedratefrt" );
	for(DataDictionary::iterator it = data.begin(); it!=data.end(); it++)
	{
		if(skip.find((*it).first) != std::string::npos)
			continue;
		updatetable +=   "<TH> " + (*it).first    + "</TH> ";
	}
	updatetable +=   "</TR>\n";
	updatetable +=   "<TR>\n";
	for(DataDictionary::iterator it = data.begin(); it!=data.end(); it++)
	{
		style.clear();
		if(skip.find((*it).first) != std::string::npos)
			continue;
		if((*it).first == "Outtol")
		{

		}
		if((*it).first == "execution")
		{	
			if((*it).second == "READY")  
				style =   "style=\"background-color:lightblue;\"";
			if((*it).second == "ACTIVE")  
				style =    "style=\"background-color:lightgreen;\"";
		}

		updatetable +=   "<TD " + style + "> "+ (*it).second + "</TD>" ;
	}
	updatetable +=   "</TR>\n";

	updatetable += "</TABLE>\n";
	return updatetable;
}

void  MTConnectStreamsParser::DumpData(DataDictionary data)
{
	for(DataDictionary::iterator it = data.begin(); it!=data.end(); it++)
	{
		AtlTrace("%s = %s\n", (*it).first.c_str(), (*it).second.c_str());
	}
}

std::string MTConnectStreamsParser::CreateHtmlDocument()
{
	std::string html;
	html += "<HTML>\n";
	html += "<HEAD><STYLE>" + HtmlStyle() + "</STYLE>"; 
	html +=" <title>MTConnect Data Trace</title>";
	html +="</HEAD>\n";
	html +="<BODY>\n";

	html += "<H1> MTConnect Readings</H1>\n";
	html += "<DIV id=""Device""> Loading... </DIV>\n";

	//html +="<input type=\"button\" value=\"BACK\" onClick=\"window.location='http://back.com' \">";
	//html +="<input type=\"button\" value=\"FWD\" onClick=\"window.location='http://fwd.com' \">";
	//html +="<input type=\"button\" value=\"RUN\" onClick=\"window.location='http://run.com' \">";

	html += "</BODY>\n";
	html += "</HTML>\n";
	return html;
}

std::string MTConnectStreamsParser::HtmlStyle()
{
	std::string style;
	style+= "P\n";
	style+= "{\n";
	style+= "	FONT-FAMILY: ""Myriad"", sans-serif;\n";
	//	style+= "	FONT-SIZE: 70%;\n";
	style+= "	LINE-HEIGHT: 12pt;\n";
	style+= "	MARGIN-BOTTOM: 0px;\n";
	style+= "	MARGIN-LEFT: 10px;\n";
	style+= "	MARGIN-TOP: 10px\n";
	style+= "}\n";

	style+= "H1\n";
	style+= "{\n";
	style+= "	BACKGROUND-COLOR: #003366;\n";
	style+= "	BORDER-BOTTOM: #336699 6px solid;\n";
	style+= "	COLOR: #ffffff;\n";
	style+= "	FONT-SIZE: 130%;\n";
	style+= "	FONT-WEIGHT: normal;\n";
	style+= "	MARGIN: 0em 0em 0em -20px;\n";
	style+= "	PADDING-BOTTOM: 8px;\n";
	style+= "	PADDING-LEFT: 30px;\n";
	style+= "	PADDING-TOP: 16px\n";
	style+= "}\n";
	style+= "table {\n";
	style+= " 	BACKGROUND-COLOR: #f0f0e0;\n";
	style+= "	BORDER-BOTTOM: #ffffff 0px solid;\n";
	style+= "	BORDER-COLLAPSE: collapse;\n";
	style+= "	BORDER-LEFT: #ffffff 0px solid;\n";
	style+= "	BORDER-RIGHT: #ffffff 0px solid;\n";
	style+= "	BORDER-TOP: #ffffff 0px solid;\n";
	//style+= "	FONT-SIZE: 70%;\n";
	style+= "	FONT-SIZE: 9pt;\n";
	style+= "	MARGIN-LEFT: 10px\n";
	style+= "  }\n";

	style+= "td {\n";
	style+= "	BACKGROUND-COLOR: #e7e7ce;\n";
	style+= "	BORDER-BOTTOM: #ffffff 1px solid;\n";
	style+= "	BORDER-LEFT: #ffffff 1px solid;\n";
	style+= "	BORDER-RIGHT: #ffffff 1px solid;\n";
	style+= "	BORDER-TOP: #ffffff 1px solid;\n";
	style+= "	PADDING-LEFT: 3px\n";
	style+= "  }\n";
	style+= "th {\n";
	style+= "	BACKGROUND-COLOR: #cecf9c;\n";
	style+= "	BORDER-BOTTOM: #ffffff 1px solid;\n";
	style+= "	BORDER-LEFT: #ffffff 1px solid;\n";
	style+= "	BORDER-RIGHT: #ffffff 1px solid;\n";
	style+= "	BORDER-TOP: #ffffff 1px solid;\n";
	style+= "	COLOR: #000000;\n";
	style+= "	FONT-WEIGHT: bold\n";
	style+= "  }\n";
	return style;
}