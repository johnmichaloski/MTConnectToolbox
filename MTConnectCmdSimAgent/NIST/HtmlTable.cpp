//
// HtmlTable.cpp
//


#include "StdAfx.h"
#include "HtmlTable.h"


CHtmlTable::CHtmlTable(void)
{
}


CHtmlTable::~CHtmlTable(void)
{
}
std::string  CHtmlTable::CreateHtmlTable()
{
	std::string updatetable;
	std::string style;
	
	updatetable += "<TABLE class=\"gridtable\">\n";
	updatetable +=   "<TR>";
	updatetable +=  GetHeaderRow();
	updatetable +=   "</TR>\n";

	for(int i = 0; i!=_values.size(); i++)
	{
		std::vector<std::string> values = _values[i];
		if(values.size() > 0 && values[0][0]=='#')
		{
			updatetable += values[0].substr(1);
			continue;
		}
	

		updatetable +=   "<TR>\n";
		for(int j=0; j< values.size(); j++)
		{
			updatetable +=   "<TD";
			if((((int) _aligns.size())-1) > j )
				if( !_aligns[j].empty())
					updatetable += " align=\""+ _aligns[j] + "\"";
			updatetable +=   "> "+ values[j] + "</TD>" ;
		}
		updatetable +=   "</TR>\n";
	}

	updatetable += "</TABLE>\n";
	return updatetable;
}
std::string CHtmlTable::CreateHtmlAndTable()
{
	std::string html = CreateHtmlDocument();
	html=ReplaceOnce(html, "Loading", CreateHtmlTable());
	return html;

}
STDMETHODIMP CHtmlTable::AddRow(std::string csvFields, std::string csvValues)
{
	std::vector<std::string> values = TrimmedTokenize(csvValues,",");
	std::vector<std::string> fields = TrimmedTokenize(csvFields,",");
	if(fields.size() != values.size())
		return E_FAIL;
	_values.push_back(values);
	return S_OK;
}
STDMETHODIMP CHtmlTable::AddRows(std::string csvFields, std::string csvValues, std::string delimiter)
{
	std::vector<std::string> valuelines = TrimmedTokenize(csvValues,"\n");
	std::vector<std::string> fields = TrimmedTokenize(csvFields,delimiter);

	for(int i=0; i< valuelines.size(); i++)
	{
		std::vector<std::string> values = TrimmedTokenize(valuelines[i],delimiter);
	//	if(fields.size() != values.size())
	//		return E_FAIL;
		_values.push_back(values);
	}
	return S_OK;
}	


STDMETHODIMP CHtmlTable::SetAlignment(std::string aligns)
{
	if(aligns.size() < 1)
		return E_FAIL;
	_aligns = TrimmedTokenize(aligns,",");
	return S_OK;
}

STDMETHODIMP CHtmlTable::SetHeaderColumns(std::string csv)
{
	std::string fields( csv);
	if(fields.size() < 1)
		return E_FAIL;
	_fields = TrimmedTokenize(fields,",");
	return S_OK;
}

std::string CHtmlTable::GetHeaderRow()
{
	std::string html;
	for(int i=0; i< _fields.size(); i++)
	{
		html+="<TH>" + _fields[i] + "</TH>";
	}
	html+="\n";
	return html;
}

std::string CHtmlTable::CreateHtmlFrontEnd(std::string title)
{
	std::string html;
	html += "<HTML>\n";
	html += "<HEAD>\n";
	html +=" <title>" + title + "</title>";
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
	return html;
}

std::string CHtmlTable::CreateHtmlDocument(std::string title)
{
	std::string html= CreateHtmlFrontEnd(title);
	html += "<DIV id=""Header""> Plant Performance</DIV>\n";
	html += "<DIV id=""JobStatus"">  </DIV>\n";
	html += "<DIV id=""Jobs"">  </DIV>\n";
	html += "<DIV id=""Device""> Loading </DIV>\n";
	html += "<DIV id=""Buttons"">  </DIV>\n";
	html += "</BODY>\n";
	html += "</HTML>\n";
	return html;
}

std::string CHtmlTable::CreateHtmlSytlesheetFrontEnd(std::string title)
{
	std::string html;
	html += "<HTML>\n";
	html += "<HEAD>\n";
	html +=" <title>" + title + "</title>\n";
	//html +=" <link rel=\"stylesheet\" href=\"file:///" + ReplaceAll(::ExeDirectory(),"\\","/") + "tank.css\" type=\"text/css\">\n";
	html += "<STYLE>\n" ; 
	html+=TableStyle();
	html += "</STYLE>\n"; 



	//html += "</BODY>\n";
	//html += "</HTML>\n";
	return html;
}
std::string CHtmlTable::HtmlStyle()
{
	std::string style;
	style+= "P\n";
	style+= "{\n";
	style+= " FONT-FAMILY: ""Myriad"", sans-serif;\n";
	// style+= " FONT-SIZE: 70%;\n";
	style+= " LINE-HEIGHT: 12pt;\n";
	style+= " MARGIN-BOTTOM: 0px;\n";
	style+= " MARGIN-LEFT: 10px;\n";
	style+= " MARGIN-TOP: 10px\n";
	style+= "}\n";
	style+= "input.btn {   color:#050;   font: bold 84% 'trebuchet ms',helvetica,sans-serif;   background-color:#fed;   border: 1px solid;   border-color: #696 #363 #363 #696;   filter:progid:DXImageTransform.Microsoft.Gradient   (GradientType=0,StartColorStr='#ffffffff',EndColorStr='#ffeeddaa'); } \n";
	style+= "input.btnhov {   border-color: #c63 #930 #930 #c63; <input type=\"button\" value=\"Submit\" class=\"bt\n\"   onmouseover=\"this.className='btn btnhov'\" onmouseout=\"this.className='btn'\"/> }\n";

	style+= "H1\n";
	style+= "{\n";
	style+= " BACKGROUND-COLOR: #003366;\n";
	style+= " BORDER-BOTTOM: #336699 6px solid;\n";
	style+= " COLOR: #ffffff;\n";
	style+= " FONT-SIZE: 130%;\n";
	style+= " FONT-WEIGHT: normal;\n";
	style+= " MARGIN: 0em 0em 0em -20px;\n";
	style+= " PADDING-BOTTOM: 8px;\n";
	style+= " PADDING-LEFT: 30px;\n";
	style+= " PADDING-TOP: 16px\n";
	style+= "}\n";
	style+=TableStyle();
		return style;
}
std::string CHtmlTable::TableStyle()
{
	std::string style;
	style+= "table.gridtable {\n";
	style+= "  BACKGROUND-COLOR: #f0f0e0;\n";
	style+= " BORDER-BOTTOM: #ffffff 0px solid;\n";
	style+= " BORDER-COLLAPSE: collapse;\n";
	style+= " BORDER-LEFT: #ffffff 0px solid;\n";
	style+= " BORDER-RIGHT: #ffffff 0px solid;\n";
	style+= " BORDER-TOP: #ffffff 0px solid;\n";
	//style+= " FONT-SIZE: 70%;\n";
	style+= " FONT-SIZE: 9pt;\n";
	style+= " MARGIN-LEFT: 10px\n";
	style+= "  }\n";
	style+= "table.gridtable td {\n";
	style+= " BACKGROUND-COLOR: #e7e7ce;\n";
	style+= " BORDER-BOTTOM: #ffffff 1px solid;\n";
	style+= " BORDER-LEFT: #ffffff 1px solid;\n";
	style+= " BORDER-RIGHT: #ffffff 1px solid;\n";
	style+= " BORDER-TOP: #ffffff 1px solid;\n";
	style+= " PADDING-LEFT: 3px\n";
	style+= "  }\n";
	style+= "table.gridtable th {\n";
	style+= " BACKGROUND-COLOR: #cecf9c;\n";
	style+= " BORDER-BOTTOM: #ffffff 1px solid;\n";
	style+= " BORDER-LEFT: #ffffff 1px solid;\n";
	style+= " BORDER-RIGHT: #ffffff 1px solid;\n";
	style+= " BORDER-TOP: #ffffff 1px solid;\n";
	style+= " COLOR: #000000;\n";
	style+= " FONT-WEIGHT: bold\n";
	style+= "  }\n";

	
	style+= "table.histogram {\n";
	style+= " 	BACKGROUND-COLOR: #f0f0e0;\n";
	style+= "	border-width: 0px;\n";
	style+= "	BORDER-COLLAPSE: collapse;\n";
	style+= "	padding: 0px;\n";
	//style+= "	FONT-SIZE: 70%;\n";
	style+= "	FONT-SIZE: 9pt;\n";
	style+= "	MARGIN-LEFT: 0px\n";
	style+= "	MARGIN-RIGHT: 0px\n";
	style+= "	height:100px;\n";
	style+= "  }\n";
	style+= "table.histogram td {\n";
	style+= "	BORDER-BOTTOM: #ffffff 0px solid;\n";
	style+= "	BORDER-LEFT: #ffffff 0px solid;\n";
	style+= "	BORDER-RIGHT: #ffffff 0px solid;\n";
	style+= "	BORDER-TOP: #ffffff 0px solid;\n";
	style+= "	PADDING: 0px;\n";
	style+= "	height:100px;\n";
	style+= "	margin: 0px;\n";
	style+= "  }\n";

	/*style+= "  h1 {\n";
	style+= "  font-size : 11px;\n";
	style+= "  text-transform : uppercase;\n";
	style+= "  margin : 0;\n";
	style+= "  }\n";
	style+= "  h2 {\n";
	style+= "  color : #9A1212;\n";
	style+= "  font-size : 20px;\n";
	style+= "  font-weight : normal;\n";
	style+= "  padding : 5px 10px;\n";
	style+= "  margin : 0;\n";
	style+= "  }\n";*/

	return style;
}

std::string CHtmlTable::HtmlStateInfo()
{
	std::string tmp;
	tmp +="<P>";
	tmp +="Factory equipment can be modeled by state machines. \n";
	tmp +="Using a state model, resource cost-function can be modeled be time in a state.\n";
	tmp += "The basic state machine formalism is given as: OFF, PROCESSING(Busy), FAULTED(Down), BLOCKED and STARVED.\n";
		tmp += "<P>The Off state indicates that the process equipment is not in use (unpowered).  \n";
		tmp += "The Processing state indicates the equipment is working to produce product.  \n";
		tmp += "The Starved state indicates that there are missing input materials so the equipment is paused.  \n";
		tmp += "If the storage facility for the process output is full, the process is in the Blocked state and the equipment delays processing.\n";
		tmp += "When equipment has a break down or fault, the process stops and the equipment is in the Faulted state. \n";
		tmp += "The numbers for repair for a piece of equipment when it has a break down or fault, indicate the equipment is immediately in the repair state although this is unlikely. \n";

		tmp +="<P>";

	return tmp;
}
std::string CHtmlTable::HtmlGoogleChart(std::map<std::string,double> &states )
{
	std::string tmp;
	tmp +="<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>\n";
	tmp +="<script type=\"text/javascript\">\n";
	tmp +="google.load(\"visualization\", \"1\", {packages:[\"corechart\"]});\n";
	tmp +="google.setOnLoadCallback(drawChart);\n";
	tmp +="function drawChart() {\n";
	tmp +="var data = google.visualization.arrayToDataTable([\n";
	tmp +="['State', 'Percentage'],\n";
	for(std::map<std::string, double>::iterator it=states.begin(); it!= states.end(); it++)
	{
		tmp +="['" + (*it).first +"',"+ StdStringFormat("%8.4f",(*it).second) + "],\n";
	}
//	tmp +="['Eat',      2],\n";
//	tmp +="['Commute',  2],\n";
//	tmp +="['Watch TV', 2],\n";
//	tmp +="['Sleep',    7]\n";
	tmp +="]);\n";

	tmp +="var options = {\n";
	tmp +="  title: 'Cell Resource Utilization by State'\n";
	tmp +=" };\n";

	tmp +=" var chart = new google.visualization.PieChart(document.getElementById('chart_div'));\n";
	tmp +=" chart.draw(data, options);\n";
	tmp +="}\n";
	tmp +="</script>\n";
	return tmp;
}

std::string Raphael::HtmlRaphaeleChart( )
{
	std::string tmp;
	tmp +="<script language=\"javascript\" src=\"./js/graphael/raphael.js\" type=\"text/javascript\"></script>\n";
	tmp +="<script language=\"javascript\" src=\"./js/graphael/g.raphael.js\" type=\"text/javascript\"></script>\n";
	tmp +="<script language=\"javascript\" src=\"./js/graphael/g.pie.js\" type=\"text/javascript\"></script> \n";
	tmp +="<script language=\"javascript\" src=\"./js/graphael/g.line.js\" type=\"text/javascript\"></script> \n";
 	return tmp;
}
	
//std::string CHtmlTable::HtmlRaphaeleChartData(std::map<std::string,double> &states )
//{
//	std::string tmp;
//	tmp +="	  //Push our data into two separate arrays\n";
//	tmp +=" var labels = [];\n";
//	tmp +="var values = [];\n";
//	tmp +="	 for (i in data.items) {\n";
//	tmp +="  	 var item = data.items[i];\n";
//	tmp +="  	 labels.push(item.label);\n";
//	tmp +="  	 values.push(item.data);\n";
//	tmp +="	 }\n";
//	tmp +="//Lines below will draw the chart\n";
//	tmp +=" window.onload = function () {     //We will draw in our div\n";
//	tmp +="  var r = Raphael(\"State Example\");\n";
//	tmp +="   //Text settings\n";
//	tmp +="   r.g.txtattr.font = \"12px 'Fontin Sans', Fontin-Sans, sans-serif\";\n";
//	tmp +="   r.g.text(320, 100, \"Number of posts\").attr({\"font-size\": 20}) \n";;
//
//	tmp +="   //Create pie\n";
//	tmp +="   var pie = r.g.piechart(320, 240, 100, values, {legend: labels, legendpos: \"west\"});\n";
//	tmp +="     //We will adjust UI when mouse over the chart sector\n";
//	tmp +="   pie.hover(function () {\n";
//	tmp +="    this.sector.stop();\n";
//	tmp +="     this.sector.scale(1.1, 1.1, this.cx, this.cy);\n";
//	tmp +="     if (this.label) {\n";
//	tmp +="      this.label[0].stop();\n";
//	tmp +="      this.label[0].scale(1.5);\n";
//	tmp +="      this.label[1].attr({\"font-weight\": 800});\n";
//	tmp +="    }\n";
//	tmp +="   }, function () {\n";
//	tmp +="     this.sector.animate({scale: [1, 1, this.cx, this.cy]}, 500, \"bounce\");\n";
//	tmp +="     if (this.label) {\n";
//	tmp +="      this.label[0].animate({scale: 1}, 500, \"bounce\");\n";
//	tmp +="      this.label[1].attr({\"font-weight\": 400});\n";
//	tmp +="    }\n";
//	tmp +="   });\n";
//	tmp +=" };\n";
//
//	return tmp;
//}

std::string Raphael::HtmlRaphaelPieChartData(std::map<std::string,double> &states )
{
	std::string tmp;

	tmp +="	 <script>\n";
	tmp +="	         window.onload = function () {\n";
	tmp +="	             var r = Raphael(\"holder\"),\n";
	tmp +="	                pie = r.piechart(320, 240, 100, [";
	for(std::map<std::string,double>::iterator it = states.begin(); it!=states.end(); it++)
		tmp+=StdStringFormat("%8.4f,", (*it).second) ;

	tmp=tmp.substr(0,tmp.size()-1); // skip last ,
	tmp +=" ],\n";

	tmp +="               { legend: [";

	for(std::map<std::string,double>::iterator it = states.begin(); it!=states.end(); it++)
		tmp+="\"%%.%% - " +  (*it).first + "\"," ;
	tmp=tmp.substr(0,tmp.size()-1); // skip last ,

	tmp +="],";

	tmp +="	legendpos: \"west\", href: [\"http://raphaeljs.com\", \"http://g.raphaeljs.com\"]});\n";

	tmp +="	            r.text(320, 100, \"Cell Resource State Use\").attr({ font: \"20px sans-serif\" });\n";
	tmp +="	            pie.hover(function () {\n";
	tmp +="	                this.sector.stop();\n";
	tmp +="	                this.sector.scale(1.1, 1.1, this.cx, this.cy);\n";

	tmp +="	                if (this.label) {\n";
	tmp +="	                    this.label[0].stop();\n";
	tmp +="	                    this.label[0].attr({ r: 7.5 });\n";
	tmp +="	                    this.label[1].attr({ \"font-weight\": 800 });\n";
	tmp +="	                 }\n";
	tmp +="	             }, function () {\n";
	tmp +="	                this.sector.animate({ transform: 's1 1 ' + this.cx + ' ' + this.cy }, 500, \"bounce\");\n";

	tmp +="	                if (this.label) {\n";
	tmp +="	                   this.label[0].animate({ r: 5 }, 500, \"bounce\");\n";
	tmp +="	                   this.label[1].attr({ \"font-weight\": 400 });\n";
	tmp +="	               }\n";
	tmp +="	           });\n";
	tmp +="	       };\n";
	tmp +="	   </script>\n";


	return tmp;
}


std::string Raphael::InlineLineChart(std::vector<double> &values , std::vector<std::string> &names , std::string title ) 
{
	static int n=1;
	std::string tmp;
	std::string name = StdStringFormat("InlineLineChart%d", n++);

	//tmp+=StdStringFormat("<div id=\"%s\"></div>\n",name.c_str());	
	//tmp+="<script type=\"text/javascript\">\n";
	//tmp+="var values = [";
	////for(std::map<std::string,double>::iterator it = states.begin(); it!=states.end(); it++)
	//std::string index,vals;
	//for(int i=0; i< values.size(); i++)
	//{
	//	if(i==0)
	//	{
	//		index +="["; 
	//		vals+="[";
	//	}
	//	if(i!=0)
	//	{
	//		index +=","; 
	//		vals+=",";
	//	}
	//	index+=StdStringFormat("%df", i); 
	//	vals+=StdStringFormat("%8.4f", values[i]); 
	//}
	//index +="]"; 
	//vals+="]";
	//tmp +=" ];\n";

	////		tmp=tmp.substr(0,tmp.size()-1); // skip last ,

	//tmp+=StdStringFormat("var r%d = Raphael(\"%s\");\n",n,name.c_str());
	//tmp +=StdStringFormat("var line = r%d.linechart(320, 240, 100, values, ", n);
	//tmp +="               { legend: [";

	//for(int i=0; i< names.size(); i++)
	//{
	//	tmp+=StdStringFormat("\"%%.%%  -  %s\",", names[i].c_str()); // (*it).second) ;
	//}

	////for(std::map<std::string,double>::iterator it = states.begin(); it!=states.end(); it++)
	////	tmp+="\"%%.%% - " +  (*it).first + "\"," ;
	//tmp=tmp.substr(0,tmp.size()-1); // skip last ,

	//tmp +="],";

	//tmp +="	legendpos: \"west\"});\n";

	//if(!title.empty()) 
	//	tmp +=StdStringFormat(" r%d.text(320, 100, \"%s\").attr({ font: \"20px sans-serif\" });", n,title.c_str());


	//tmp +="	            pie.hover(function () {\n";
	//tmp +="	                this.sector.stop();\n";
	//tmp +="	                this.sector.scale(1.1, 1.1, this.cx, this.cy);\n";

	//tmp +="	                if (this.label) {\n";
	//tmp +="	                    this.label[0].stop();\n";
	//tmp +="	                    this.label[0].attr({ r: 7.5 });\n";
	//tmp +="	                    this.label[1].attr({ \"font-weight\": 800 });\n";
	//tmp +="	                 }\n";
	//tmp +="	             }, function () {\n";
	//tmp +="	                this.sector.animate({ transform: 's1 1 ' + this.cx + ' ' + this.cy }, 500, \"bounce\");\n";

	//tmp +="	                if (this.label) {\n";
	//tmp +="	                   this.label[0].animate({ r: 5 }, 500, \"bounce\");\n";
	//tmp +="	                   this.label[1].attr({ \"font-weight\": 400 });\n";
	//tmp +="	               }\n";
	//tmp +="	           });\n";

	//tmp+="\n</script>\n";
	return tmp;

}