// CMTConnectTrayIcon.cpp : Implementation of CCMTConnectTrayIcon

#include "stdafx.h"
#include "CMTConnectTrayIcon.h"
#include "MainFrm.h"
#include "StdStringFcn.h"

// CCMTConnectTrayIcon
//extern CMainDlg wndMain;
extern CMainFrame wndMain;
extern DWORD	dwWebBrowserCookie; //Save this to get the interface later
std::vector<DataDictionary>		CCMTConnectTrayIcon::_faultdatum;
std::string					CCMTConnectTrayIcon::_faultitems = "Severity,RaisedBy,Description,Program,RaisedAt,ResolvedAt,TBF,TTR";

HRESULT CCMTConnectTrayIcon::FinalConstruct()
{
	return S_OK;
}
STDMETHODIMP CCMTConnectTrayIcon::SetHeaderColumns(BSTR csv)
{
	std::string fields((LPCSTR) _bstr_t( csv));
	if(fields.find("History") == std::string::npos)
		fields+=",History";
	if(fields.size() < 1)
		return E_FAIL;
	_fields = TrimmedTokenize(fields,",");
	return S_OK;
}


STDMETHODIMP CCMTConnectTrayIcon::Clear(void)
{
	_faultdatum.clear();
	return S_OK;
}


STDMETHODIMP CCMTConnectTrayIcon::GetPointerBrowser(IUnknown** pUnk)
{
	HRESULT hr=S_OK;
	CComPtr<IWebBrowser2> pWebBrowser=NULL;
	try {
		if( pUnk == NULL)
			return E_POINTER;

		if( *pUnk != NULL)
			return E_INVALIDARG;

		if(wndMain.m_hWnd==NULL)
			return E_FAIL;

		if(_htmlpage!=NULL)
			pWebBrowser=_htmlpage->GetWebBrowser();
		if(pWebBrowser==NULL)
		{
			pWebBrowser=NULL;
			return E_FAIL;
		}
		else
			hr = pWebBrowser->QueryInterface(IID_IUnknown, (void **) pUnk);

	}
	catch(...)
	{
		pWebBrowser=NULL;
		return E_FAIL;
	}
	return hr;
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
	int state;
	// Determine current status, then save
	if(_datamap["power"] == "OFF")
		state= 4; 
	else  if(_datamap["mode"] == "MANUAL")
		state=1;
	else if(_datamap["mode"] == "AUTO" && _datamap["execution"] == "RUNNING")
		state=2;
	else if((_datamap["mode"] == "AUTO" && _datamap["execution"] != "RUNNING")
		|| _datamap["mode"] == "PAUSED")
		state=3;
	else  state=0;
	return state;
}
STDMETHODIMP CCMTConnectTrayIcon::AddRow(BSTR csvFields, BSTR csvValues)
{
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
		_historiantimestamp.push_back(GetTimeStamp(LOCAL));
		statetimer.restart();
	}

	int seconds = statetimer.elapsed();
	if(seconds > nSampleTimeSecs)
		statetimer.restart();

	while(seconds > nSampleTimeSecs)
	{
		std::string msg = GetTimeStamp(LOCAL);
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
	return S_OK;
}

static std::string HrMinSecFormat(std::string sec)
{
	
	int nsec=ConvertString<double>(sec,0.0);
	return StdStringFormat("%02d:%02d:%02d", nsec/3600,  (nsec%3600)/60,  nsec%60);
}

std::string Reformat(std::string value, std::string format)
{
	double dVal;
	if(sscanf(value.c_str(),"%lf",&dVal))
		value = StdStringFormat(format.c_str(), dVal);
	return value;
}
std::string CCMTConnectTrayIcon::GetHtmlStatus() 
{ 	
	std::string html;
	for(int i=0; i< _fields.size(); i++)
	{
		//if(_fields[i]=="Duration")
		if(_typedatum[_fields[i]]==	"adDBTime")
		{
			html+= "<TD align=\"right\">" +   HrMinSecFormat(_datamap[_fields[i]]) + "  </TD>" ; 
		}
		else if(_typedatum[_fields[i]]=="adDouble" && _formats.find(_fields[i]) != _formats.end())
		{
			html+= "<TD align=\"right\">" + Reformat(_datamap[_fields[i]], _formats[_fields[i]] ) + "  </TD>" ;
		}
		else if(_fields[i]=="Machine")
		{ 
			if (_ipaddr.size() > 0)
			{
				//html+= "<TD><a href='"+ _ipaddr +" ' target='_blank'>" + _datamap["Machine"] + "</a></TD>";
				//html+= "<TD><a href='"+ _ipaddr +" ' target='_blank'>" + _datamap["Machine"] + "</a></TD>";
				std::string url(_ipaddr);
				url = ReplaceOnce(url, "http://", "");
				url = ReplaceOnce(url, "/current", "");
				std::string path = ::ExeDirectory();
				ReplaceAll(path,"\\", "/");
				ReplaceAll(path,":", "|");
				ReplaceAll(path," ", "%20");

				//html+= "<TD><a href='file:///" + path + "SeeMTConnect.html?url="+ url +"  ' target='_blank'>" + _datamap["Machine"] + "</a></TD>";
				html+= "<TD><a href='http://localhost/monitor?url="+ url + "'>" + _datamap["Machine"] + "</a></TD>";
			}
			else
			{
				html+= "<TD>" +  _datamap["Machine"] + " </TD>";
			}
		}
		else if(_fields[i]=="PieChart")
		{
			std::string datafile = ExeDirectory() + _datamap["Machine"]+ ".html";
			//html+= "<TD><a href='file://localhost/" + datafile + "' target='_blank'>" +  Reformat(_datamap["OEE"], "%03.2f%%") + "</a></TD>";
			//html+= "<TD><a href='file:///C:\Piechart\M2.html' target='_blank'>" +  Reformat(_datamap["OEE"], "%03.2f%%") + "</a></TD>";
			//html+= "<TD><a href=\"file:///c:/Piechart/M2.html\">" +  std::string("BBB") + "</a></TD>";
			//html+= "<TD><a href=\"file:\\\C:\\Autoexec.bat\">" +  std::string("BBB") + "</a></TD>";
			//html+= "<TD><a href='http://www.google.com'>" +  std::string("BBB") + "</a></TD>";
			html+= "<TD><a href='about:blank' target='_blank'>" +  std::string("BBB") + "</a></TD>";
		}
		else if(_fields[i]=="History")
		{
			html+= CreateHistory() ;  
		}
		else
		{
			html+="<TD>" + _datamap[_fields[i]] + "</TD>";
		}
	}
	html+="\n";
	WriteFile(::ExeDirectory() + "Test.html", "<HTML><BODY>" + html+ "</BODY></HTML>");
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
std::string CCMTConnectTrayIcon::GetHeaderRow()
{
	std::string html;
	for(int i=0; i< _fields.size(); i++)
	{
		html+="<TH>" + _fields[i] + "</TH>";
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
		_typedatum[fields[i]] = types[i];

	return S_OK;
}
STDMETHODIMP CCMTConnectTrayIcon::Formats(BSTR csvFields, BSTR csvFormats)
{
	std::vector<std::string> fields = TrimmedTokenize(std::string((LPCSTR) _bstr_t(csvFields)), ",");
	std::vector<std::string> formats = TrimmedTokenize(std::string((LPCSTR) _bstr_t(csvFormats)),",");
	if(fields.size() != formats.size())
		return E_FAIL;

	for(int i=0; i< fields.size(); i++)
		_formats[fields[i]] = formats[i];
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
<<<<<<< HEAD
	//std::vector<std::string> _faultfields = TrimmedTokenize(_faultitems, ",");
=======
>>>>>>> 352b2c2430392443c49cd6840fdc93f1050afb6d
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