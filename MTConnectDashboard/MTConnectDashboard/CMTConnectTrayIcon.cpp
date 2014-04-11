// CMTConnectTrayIcon.cpp : Implementation of CCMTConnectTrayIcon

#include "stdafx.h"
#include "CMTConnectTrayIcon.h"
#include "MainFrm.h"
#include "StdStringFcn.h"
#include "File.h"
#include "ElTime.h"

// CCMTConnectTrayIcon
//extern CMainDlg wndMain;
extern CMainFrame wndMain;
extern DWORD	dwWebBrowserCookie; //Save this to get the interface later
std::vector<DataDictionary>		CCMTConnectTrayIcon::_faultdatum;
std::string					CCMTConnectTrayIcon::_faultitems = "Severity,RaisedBy,Description,Program,RaisedAt,ResolvedAt,TBF,TTR";





STDMETHODIMP CCMTConnectTrayIcon::Clear(void)
{
	_faultdatum.clear();
	_datum.erase(_datum.begin()+1, _datum.end());
	//_datum.clear();
	return S_OK;
}


STDMETHODIMP CCMTConnectTrayIcon::SetTrayIconColor(BSTR _traycolor, VARIANT_BOOL flashing)
{
	bstr_t traycolor(_traycolor);
	// EXECUTING,IDLE,STOPPED,FAULT, MANUAL
	if( traycolor == bstr_t("GREEN") && flashing == VARIANT_FALSE)
		wndMain.SetState("IDLE");
	else if( traycolor == bstr_t("GREEN") && flashing == VARIANT_TRUE)
		wndMain.SetState("EXECUTING");
	else if( traycolor == bstr_t("RED") && flashing == VARIANT_FALSE)
		wndMain.SetState("STOPPED");
	else if( traycolor == bstr_t("RED") && flashing == VARIANT_TRUE)
		wndMain.SetState("FAULT");
	else if( traycolor == bstr_t("BLUE") )
		wndMain.SetState("MANUAL");
	else
		wndMain.SetState("EXECUTING");

	return S_OK;
}

STDMETHODIMP CCMTConnectTrayIcon::SetTooltip(BSTR bstr)
{
	wndMain.SetTooltip((LPCSTR) _bstr_t(bstr));

	return S_OK;
}

STDMETHODIMP CCMTConnectTrayIcon::CreateTabWindow(BSTR name)
{
	_htmlpage= wndMain.AddWebTab((LPCSTR) _bstr_t(name), this);
	return S_OK;
}
int CCMTConnectTrayIcon::GetState()
{
	if(_datum.size()< 1)
		return 4;
	DataDictionary	&	_datamap(_datum[0]);

	int state;
	// Determine current status, then save
	if(stricmp(_datamap["power"].c_str(), "OFF")==0)
		state= 4; 
	else  if(stricmp(_datamap["mode"].c_str(), "MANUAL")==0) 
		state=1;
	else if(stricmp(_datamap["mode"].c_str(), "AUTOMATIC")==0 
		&& (stricmp(_datamap["execution"].c_str(), "RUNNING")==0 || stricmp(_datamap["execution"].c_str(), "EXECUTING")==0 ))
		state=2;
	else if(stricmp(_datamap["mode"].c_str(), "AUTOMATIC")==0 
		&& stricmp(_datamap["execution"].c_str(), "PAUSED")==0  )
		state=3;
	else  state=0;  // fault
	return state;
}
void CCMTConnectTrayIcon::SetTimestamp(COleDateTime logtime)
{
	lastlogtime=logtime;
}
void CCMTConnectTrayIcon::AddRow(std::vector<std::string> fields, DataDictionary data, COleDateTime logtime)
{
	DataDictionary						_datamap;
	for(int i=0; i< fields.size(); i++)
	{
		_datamap[fields[i]]= data[fields[i]];
	}
	_datum.insert(_datum.begin(), _datamap);
	_elapsedSeconds = HistoryUpdate(logtime,  _elapsedSeconds);
}
void CCMTConnectTrayIcon::ReplaceLastRowValues(DataDictionary data)
{
	for(DataDictionary::iterator it=data.begin(); it!=data.end(); it++)  
	{
		if(_datum.size()>0)
			_datum[0][(*it).first]= (*it).second;
	}
}

void CCMTConnectTrayIcon::ReplaceLastRowValues(std::vector<std::string> fields, DataDictionary data)
{

	for(int i=0; i< fields.size(); i++)
	{
		if(_datum.size()>0)
			_datum[0][fields[i]]= data[fields[i]];
	}
}


STDMETHODIMP CCMTConnectTrayIcon::AddRow(BSTR csvFields, BSTR csvValues)
{
	DataDictionary						_datamap;
	std::vector<std::string> _values = TrimmedTokenize(std::string((LPCSTR) _bstr_t(csvValues)),",");
	std::vector<std::string> _fields = TrimmedTokenize(std::string((LPCSTR) _bstr_t(csvFields)),",");
	for(int i=0; i< _fields.size(); i++)
	{
		if(i<_values.size())
			_datamap[_fields[i]]= _values[i];
	}

	if(_historian.size() == 0)
	{
		_historian.push_back(GetState());
		_historiantimestamp.push_back(Time.GetTimeStamp(LOCAL));
		statetimer.restart();
	}

	int seconds = statetimer.elapsed();
	if(seconds > nSampleTimeSecs)
		statetimer.restart();

	while(seconds > nSampleTimeSecs)
	{
		std::string msg = Time.GetTimeStamp(LOCAL);
		if(!_datamap["Alarm"].empty())
			msg+="\r\n" + _datamap["Alarm"];
		msg+="\r\n" +_datamap["program"];
		_historiantimestamp.push_back(msg);
		_historian.push_back(_laststate);
		seconds-=nSampleTimeSecs;
	}
	if(_historian.size() >= (UINT) wpx )
	{
		_historian.erase(_historian.begin()); // erase beginning entry
		_historiantimestamp.erase(_historiantimestamp.begin());
	}

	_laststate = GetState();
	//if(statetimer.elapsed() > sample)
	//{
	//	_state = GetState();
	//	_historian.push_back(_state);
	//	statetimer.restart();
	//}
	
#if 1
	_datum.insert(_datum.begin(), _datamap);
#endif
	return S_OK;
}

int CCMTConnectTrayIcon::HistoryUpdate(COleDateTime logtime, int seconds)
{
	if(_datum.size()< 1)
		return 0;
	DataDictionary	&	_datamap(_datum[0]);
	
	COleDateTimeSpan duration = logtime-lastlogtime;
	seconds+=  duration.GetTotalSeconds();
	lastlogtime=logtime;


	while(seconds > nSampleTimeSecs)
	{
		std::string msg = Time.GetTimeStamp(LOCAL);
		if(!_datamap["Alarm"].empty())
			msg+="\r\n" + _datamap["Alarm"];
		msg+="\r\n" +_datamap["program"];
		_historiantimestamp.push_back(msg);
		_historian.push_back(_laststate);
		seconds-=nSampleTimeSecs;
	}
	if(_historian.size() >= (UINT) wpx )
	{
		_historian.erase(_historian.begin()); // erase beginning entry
		_historiantimestamp.erase(_historiantimestamp.begin());
	}

	_laststate = GetState();
	return seconds;
}
std::string Reformat(std::string value, std::string format)
{
	double dVal;
	if(sscanf(value.c_str(),"%lf",&dVal))
		value = StdStringFormat(format.c_str(), dVal);
	return value;
}
std::string CCMTConnectTrayIcon::GetCurrentValue(std::string field)
{
	if(_datum.size()>0)
		return _datum[0][field];
	return "";
}
std::string CCMTConnectTrayIcon::GetHtmlStatus(std::vector<std::string> fields) 
{ 	
	std::string html;
	if(_datum.size()< 1)
		return "";
	DataDictionary	&	_datamap(_datum[0]);

	for(int i=0; i< fields.size(); i++)
	{
		//if(_fields[i]=="Duration")
		if(_typedatum[fields[i]]==	"adDBTime")
		{
			html+= "<TD align=\"right\">" +   Time.HrMinSecFormat(_datamap[fields[i]]) + "  </TD>" ; 
		}
		else if(_typedatum[fields[i]]=="adDouble" && _formats.find(fields[i]) != _formats.end())
		{
			html+= "<TD align=\"right\">" + Reformat(_datamap[fields[i]], _formats[fields[i]] ) + "  </TD>" ;
		}
		else if(fields[i]=="Machine")
		{ 
			//if (_ipaddr.size() > 0)
			//{
			//	//html+= "<TD><a href='"+ _ipaddr +" ' target='_blank'>" + _datamap["Machine"] + "</a></TD>";
			//	//html+= "<TD><a href='"+ _ipaddr +" ' target='_blank'>" + _datamap["Machine"] + "</a></TD>";
			//	std::string url(_ipaddr);
			//	url = ReplaceOnce(url, "http://", "");
			//	url = ReplaceOnce(url, "/current", "");
			//	std::string path = File.ExeDirectory();
			//	ReplaceAll(path,"\\", "/");
			//	ReplaceAll(path,":", "|");
			//	ReplaceAll(path," ", "%20");

			//	//html+= "<TD><a href='file:///" + path + "SeeMTConnect.html?url="+ url +"  ' target='_blank'>" + _datamap["Machine"] + "</a></TD>";
			//	//html+= "<TD><a href='http://localhost/monitor?url="+ url + "'>" + _datamap["Machine"] + "</a></TD>";
			//	html+= "<TD> " + _datamap["Machine"] + "</TD>";
			//}
			//else
			{
				html+= "<TD>" +  _datamap["Machine"] + " </TD>";
			}
		}
		else if(fields[i]=="PieChart")
		{
			std::string datafile = File.ExeDirectory() + _datamap["Machine"]+ ".html";
			//html+= "<TD><a href='file://localhost/" + datafile + "' target='_blank'>" +  Reformat(_datamap["OEE"], "%03.2f%%") + "</a></TD>";
			//html+= "<TD><a href='file:///C:\Piechart\M2.html' target='_blank'>" +  Reformat(_datamap["OEE"], "%03.2f%%") + "</a></TD>";
			//html+= "<TD><a href=\"file:///c:/Piechart/M2.html\">" +  std::string("BBB") + "</a></TD>";
			//html+= "<TD><a href=\"file:\\\C:\\Autoexec.bat\">" +  std::string("BBB") + "</a></TD>";
			//html+= "<TD><a href='http://www.google.com'>" +  std::string("BBB") + "</a></TD>";
			html+= "<TD><a href='about:blank' target='_blank'>" +  std::string("BBB") + "</a></TD>";
		}
		else if(fields[i]=="History")
		{
			html+= CreateHistory() ;  
		}
		else
		{
			html+="<TD>" + _datamap[fields[i]] + "</TD>";
		}
	}
	html+="\n";
	return html;
}

std::string CCMTConnectTrayIcon::CreateHistory() 
{ 
	std::string html;
	// History Table insertion
	html+= StdStringFormat("<TD cellpadding=0 cellspacing=0 border=0 width=%dpx> <table id=\"myTable\" class=\"histogram\" style='empty-cells: show; cellpadding=0; cellspacing=0; border=0;'>", (long) dTraceWidth);
	html+= "<tr>\n";

	int n=0;
	char  * colormap[5] = { "red", "blue", "green", "yellow", "black"};
	for(int i=0; _historian.size() >  0 && i< (_historian.size()-1); i++)
	{
		n++;
		if(_historian[i] !=_historian[i+1])
		{
			html+="<td style='background-color:" + std::string(colormap[_historian[i]])  + "' ";
			html+="title=\"" + _historiantimestamp[i] + "\"";
			html+=">";
			html+=StdStringFormat("	<DIV style='width:%dpx;'/></td>", (int) (n/wpx * dTraceWidth));
			n=0;
		}
	}

	if(_historian.size()!=0)
	{
		int m = _historian.size()-1;
		html+="<td style='background-color:" + std::string(colormap[_historian[m]])  + "' ";
		html+="title=\"" + _historiantimestamp[m] + "\"";
		html+=">";
		html+=StdStringFormat("	<DIV style='width:%dpx;'/></td>", (int) (n/wpx * dTraceWidth));
	}

	html+= "</tr>\n";
	html+= " </table>	\n";		
	html+= "<TD/>\n";
	html+= "\n";
	return html;
}

//STDMETHODIMP CCMTConnectTrayIcon::SetHeaderColumns(BSTR csv)
//{
//	std::string fields((LPCSTR) _bstr_t( csv));
//	if(fields.find("History") == std::string::npos)
//		fields+=",History";
//	if(fields.size() < 1)
//		return E_FAIL;
//	_fields = TrimmedTokenize(fields,",");
//	return S_OK;
//}
std::string CCMTConnectTrayIcon::GetHeaderRow(std::vector<std::string> fields)
{
	std::string html;
	for(int i=0; i< fields.size(); i++)
	{
		html+="<TH>" + fields[i] + "</TH>";
	}
	html+="\n";
	return html;
}


std::string CCMTConnectTrayIcon::CreateHtmlDocument()
{
	std::string html;
	html += "<HTML>\n";
	html += "<HEAD>\n";
	html +=" <title>MTConnect Data Trace</title>";
	html += "<STYLE>" + HtmlStyle() ; 

	html +="$('#myTable td[title]')\n";
	html +=".hover(function() {\n";
	html +="    showTooltip($(this));\n";
	html +=" }, function() {\n";
	html +="     hideTooltip();\n";
	html +=" })\n";
	html +=";\n";

	html += "</STYLE>"; 

	html +="<script type=\"text/javascript\">\n";
	html +="function showTooltip($el) {\n";
	html +="    // insert code here to position your tooltip element (which i'll call $tip)\n";
	html +="    $tip.html($el.attr('title'));\n";
	html +="}\n";
	html +="function hideTooltip() {\n";
	html +="    $tip.hide();\n";
	html +="}\n";
	html +="</script>\n";


	html +="</HEAD>\n";
	html +="<BODY>\n";

	html += "<DIV id=""Header""> MTConnect Readings</DIV>\n";
	html += "<DIV id=""Device""> Loading... </DIV>\n";

	//html +="<input type=\"button\" value=\"BACK\" onClick=\"window.location='http://back.com' \">";
	//html +="<input type=\"button\" value=\"FWD\" onClick=\"window.location='http://fwd.com' \">";
	//html +="<input type=\"button\" value=\"RUN\" onClick=\"window.location='http://run.com' \">";

	html += "</BODY>\n";
	html += "</HTML>\n";
	return html;
}

std::string CCMTConnectTrayIcon::HtmlStyle()
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

	style+= "input.btn {   color:#050;   font: bold 84% 'trebuchet ms',helvetica,sans-serif;   background-color:#fed;   border: 1px solid;   border-color: #696 #363 #363 #696;   filter:progid:DXImageTransform.Microsoft.Gradient   (GradientType=0,StartColorStr='#ffffffff',EndColorStr='#ffeeddaa'); } \n";
	style+= "input.btnhov {   border-color: #c63 #930 #930 #c63; <input type=\"button\" value=\"Submit\" class=\"bt\n\"   onmouseover=\"this.className='btn btnhov'\" onmouseout=\"this.className='btn'\"/> }\n";
	
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
	style+= "table.gridtable {\n";
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

	style+= "table.gridtable td {\n";
	style+= "	BACKGROUND-COLOR: #e7e7ce;\n";
	style+= "	BORDER-BOTTOM: #ffffff 1px solid;\n";
	style+= "	BORDER-LEFT: #ffffff 1px solid;\n";
	style+= "	BORDER-RIGHT: #ffffff 1px solid;\n";
	style+= "	BORDER-TOP: #ffffff 1px solid;\n";
	style+= "	PADDING-LEFT: 3px\n";
	style+= "  }\n";
	style+= "table.gridtable th {\n";
	style+= "	BACKGROUND-COLOR: #cecf9c;\n";
	style+= "	BORDER-BOTTOM: #ffffff 1px solid;\n";
	style+= "	BORDER-LEFT: #ffffff 1px solid;\n";
	style+= "	BORDER-RIGHT: #ffffff 1px solid;\n";
	style+= "	BORDER-TOP: #ffffff 1px solid;\n";
	style+= "	COLOR: #000000;\n";
	style+= "	FONT-WEIGHT: bold\n";
	style+= "  }\n";

	style+= "table.histogram {\n";
	style+= " 	BACKGROUND-COLOR: #f0f0e0;\n";
	style+= "	border-width: 0px;\n";
	style+= "	BORDER-COLLAPSE: collapse;\n";
	style+= "	padding: 0px;\n";
	//style+= "	FONT-SIZE: 70%;\n";
	style+= "	FONT-SIZE: 9pt;\n";
	style+= "	MARGIN-LEFT: 0px\n";
	style+= "  }\n";
	style+= "table.histogram td {\n";
	style+= "	BORDER-BOTTOM: #ffffff 0px solid;\n";
	style+= "	BORDER-LEFT: #ffffff 0px solid;\n";
	style+= "	BORDER-RIGHT: #ffffff 0px solid;\n";
	style+= "	BORDER-TOP: #ffffff 0px solid;\n";
	style+= "	PADDING: 0px\n";
	style+= "	margin: 0px\n";
	style+= "  }\n";
	return style;
}
STDMETHODIMP CCMTConnectTrayIcon::SetIpAddr(BSTR ipaddr)
{
	_ipaddr = (LPSTR) _bstr_t(ipaddr);

	return S_OK;
}

STDMETHODIMP CCMTConnectTrayIcon::Types(BSTR csvFields, BSTR csvTypes)
{
	std::vector<std::string> fields = TrimmedTokenize(std::string((LPCSTR) _bstr_t(csvFields)), ",");
	std::vector<std::string> types = TrimmedTokenize(std::string((LPCSTR) _bstr_t(csvTypes)),",");
	if(fields.size() != types.size())
		return E_FAIL;

	for(int i=0; i< fields.size(); i++)
		_typedatum[MakeLower(fields[i])] = MakeLower(types[i]);

	return S_OK;
}
STDMETHODIMP CCMTConnectTrayIcon::Formats(BSTR csvFields, BSTR csvFormats)
{
	std::vector<std::string> fields = TrimmedTokenize(std::string((LPCSTR) _bstr_t(csvFields)), ",");
	std::vector<std::string> formats = TrimmedTokenize(std::string((LPCSTR) _bstr_t(csvFormats)),",");
	if(fields.size() != formats.size())
		return E_FAIL;

	for(int i=0; i< fields.size(); i++)
		_formats[MakeLower(fields[i])] = formats[i];
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// Alarm tab
STDMETHODIMP CCMTConnectTrayIcon::CreateAlarmTab(BSTR name)
{
	_htmlpage= wndMain.AddWebTab((LPCSTR) _bstr_t(name), this);
	return S_OK;
}
STDMETHODIMP CCMTConnectTrayIcon::AddFaultRow(BSTR csvFields, BSTR csvValues)
{
	DataDictionary _datamap;
	std::vector<std::string> _faultfields = TrimmedTokenize(std::string((LPCSTR) _bstr_t(csvFields)), ",");
	std::vector<std::string> _values = TrimmedTokenize(std::string((LPCSTR) _bstr_t(csvValues)),",");

	//std::vector<std::string> _faultfields = TrimmedTokenize(_faultitems, ",");

	for(int i=0; i< _faultfields.size(); i++)
	{
		_datamap[_faultfields[i]]= _values[i];
	}
	_faultdatum.insert(_faultdatum.begin(), _datamap);
	return S_OK;
}

STDMETHODIMP CCMTConnectTrayIcon::SetFaultColumns(BSTR csv)
{
	_faultitems = (LPCSTR) _bstr_t(csv);
	return S_OK;
}
std::string CCMTConnectTrayIcon::GetFaultSummaryHtml() 
{ 	
	std::vector<std::string> _faultfields = TrimmedTokenize(_faultitems, ",");
	std::string html;
	//value="<a href='"+  +" ' target='_blank'>" + value + "</a>";

	html+= "<tr>\n";
	for(int i=0; i< _faultfields.size(); i++)
		html+= "<TH>" +  _faultfields[i] + " </TH>";
	html+= "</tr>\n";

	for(int i=0; i<_faultdatum.size(); i++)
	{
		DataDictionary datamap = _faultdatum[i];
	
		for(int i=0; i< _faultfields.size(); i++)
		{	
			if(_faultfields[i] == "Severity")
			{
				if(stricmp(datamap["Severity"].c_str(),"warning") == 0)
					html+= "<td style='background-color:yellow;'>\n";
				else if(stricmp(datamap["Severity"].c_str(),"fault") == 0)
					html+= "<td style='background-color:#FB7979;'>\n";
			}
			else
				html+= "<td>\n";

			html+=  datamap[_faultfields[i]];
			html+= "</td>\n";
		}
		html+= "</tr>\n";
	}
	html+= "<tr>\n";
	return html;
}
#if 1 
std::string  CCMTConnectTrayIcon::CreateHtmlTable(std::vector<std::string> fields)
{
	std::string htmltable;
	if(_datum.size() < 1)
		return "";

	htmltable += "<TABLE class=\"gridtable\">\n";
	htmltable +="<TR>"+GetHeaderRow(fields)+"</TR>\n";

	for(int i=0; i< _datum.size(); i++)
	{
		htmltable +=   "<TR>\n";
		htmltable += CreateHtmlRow(fields,_datum[i]);
		htmltable +=   "</TR>\n";
	}

	htmltable += "</TABLE>\n";
	return htmltable;
}
void  CCMTConnectTrayIcon::UpdateHtml(std::vector<std::string> fields)
{
	std::string html = CreateHtmlTable(fields);
	this->_htmlpage->SetElementId("Device", html);
}
std::string  CCMTConnectTrayIcon::CreateHtmlRow(std::vector<std::string> fields, DataDictionary data)
{
	std::string htmltable;
	std::string style;

	for(int i=0; i< fields.size(); i++)
	{
		double dVal=0.0;
		style.clear();
		std::string value = data[fields[i]]; //(*it).second;
		std::string field = MakeLower(fields[i]); //  (*it).first
		_numberfields=MakeLower(_numberfields); 
		

		if(stricmp(field.c_str(), "mode") == 0)
		{
			if(stricmp(value.c_str(), "MANUAL") == 0 )  
			{
				style =   "style=\"background-color:yellow;\"";
			}
			if(stricmp(value.c_str(), "AUTOMATIC") == 0 )  
			{
				style =    "style=\"background-color:#00FF33;\"";
			}	
		}
		if(stricmp(field.c_str(), "execution") == 0 )
		{	
			if(stricmp(value.c_str(), "IDLE") == 0 || stricmp(value.c_str(), "PAUSED") == 0)  
			{
				style =   "style=\"background-color:lightblue;\"";
			}
			if(stricmp(value.c_str(), "EXECUTING") == 0 || stricmp(value.c_str(), "RUNNING" )  ==0)
			{
				style =    "style=\"background-color:green;\"";
			}
			if(stricmp(value.c_str(), "STOPPED") == 0  )  
				style =    "style=\"background-color:red;\"";
		}

		if(value == "UNAVAILABLE")
			value="<CENTER>--</CENTER>";
		//if(_dateTimefields.find(field) != std::string::npos)
		//{
		//	value =  DataDictionary::HrMinSecFormat(ConvertString<double>(value,0.0)); 
		//	style += " align=\"right\"";
		//}
		if(_numberfields.find(field) != std::string::npos && sscanf(value.c_str(),"%lf",&dVal))
		{
			value = StdStringFormat("%9.6f", dVal);
			style += " align=\"right\"";
		}
		if(_formats.find(field)!=_formats.end())
		{
			if(sscanf(value.c_str(),"%lf",&dVal))
			value = StdStringFormat(_formats[field].c_str(), dVal);
		}
		htmltable +=   "<TD " + style + "> "+ value + "</TD>\n" ;
	}

	return htmltable;
}
#endif