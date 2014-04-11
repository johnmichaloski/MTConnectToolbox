//
// HtmlTable.h
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#pragma once

#include <vector>
#include <string>
#include "StdStringFcn.h"

class CHtmlTable
{
public:
	CHtmlTable(void);
	~CHtmlTable(void);
	std::vector<std::string>			_fields;
	std::vector<std::vector<std::string> >			_values;
	std::vector<std::string>			_aligns;
	std::string  CreateHtmlTable();
	std::string CreateHtmlAndTable();
	static std::string CreateHtmlDocument(std::string title="");
	static std::string CreateHtmlSytlesheetFrontEnd(std::string title="");
	static std::string CreateHtmlFrontEnd(std::string title="");
	static std::string HtmlStateInfo();
	static std::string TableStyle();
	std::string HtmlGoogleChart(std::map<std::string,double> &states);

	STDMETHOD(SetHeaderColumns)(std::string csv); // adds history to end of tab
	STDMETHOD(SetAlignment)(std::string csv); // adds align to TH items
	STDMETHOD(AddRow)(std::string csvFields, std::string values);
	STDMETHOD(AddRows)(std::string csvFields, std::string values, std::string delimiter=",");
	STDMETHOD(ClearValues)(){_values.clear(); return S_OK;}
private:
	static std::string HtmlStyle();
	std::string GetHeaderRow();

};

class HtmlTableMaker
{
public:
	int _cols;
	std::vector<std::string> elements;

	HtmlTableMaker( int cols) : _cols(cols)
	{

	}

	std::string MakeTable() 
	{
		std::string tmp;
		tmp += "<table border=\"0\">\n"; 
		for(int i=0; i< elements.size(); i++)
		{
				if((i>0) && (i%_cols) == 0)
					tmp += "</tr>\n"; 
				if((i%_cols) == 0)
					tmp += "<tr>"; 
				if(!elements[i].empty())
					tmp+= "<td>"+ elements[i] + "</td>";
				else
					tmp+="<td>&nbsp;</td>";

		}

		tmp += "</tr></table>\n"; 
		return tmp;
	}
	void AppendTableCell(std::string s) 
	{
		elements.push_back(s);
	}

};
class Raphael
{
public:
	static std::string HtmlRaphaeleChart( );
	std::string HtmlRaphaelPieChartData(std::map<std::string,double> &states );
	static std::string TestDrive(std::vector<double> &data) 
	{
		static int n=1;
		std::string tmp;
		std::string name = StdStringFormat("paper%d", n++);
		tmp+=StdStringFormat("<div id=\"%s\"></div>\n",name.c_str());	
	
		tmp+="<script type=\"text/javascript\">\n";
		tmp+=StdStringFormat("var paper = Raphael(\"%s\", 500,500)\n",name.c_str());
		tmp+="var rect1 = paper.rect(20,30,100,12).attr({fill: \"orange\"})\n";;
		tmp+="</script>\n";
		return tmp;
	}
	//static std::string InlinePieChart(std::map<std::string,double> &states , std::string title = "") 
	static std::string InlinePieChart(std::vector<double> &values , std::vector<std::string> &names , std::string title = "") 
	{
		static int n=1;
		std::string tmp;
		std::string name = StdStringFormat("InlinePieChart%d", n++);

		tmp+=StdStringFormat("<div id=\"%s\"></div>\n",name.c_str());	
		tmp+="<script type=\"text/javascript\">\n";
		tmp+="var values = [";
		//for(std::map<std::string,double>::iterator it = states.begin(); it!=states.end(); it++)
		for(int i=0; i< values.size(); i++)
		{
			if(i!=0) tmp +=","; //if(it!=states.begin() ) tmp +=",";
			tmp+=StdStringFormat("%8.4f", values[i]); // (*it).second) ;
		}
		tmp +=" ];\n";

//		tmp=tmp.substr(0,tmp.size()-1); // skip last ,

		tmp+=StdStringFormat("var r%d = Raphael(\"%s\");\n",n,name.c_str());
		tmp +=StdStringFormat("var pie = r%d.piechart(320, 240, 100, values, ", n);
		tmp +="               { legend: [";
	
		for(int i=0; i< names.size(); i++)
		{
			tmp+=StdStringFormat("\"%%.%%  -  %s\",", names[i].c_str()); // (*it).second) ;
		}

		//for(std::map<std::string,double>::iterator it = states.begin(); it!=states.end(); it++)
		//	tmp+="\"%%.%% - " +  (*it).first + "\"," ;
		tmp=tmp.substr(0,tmp.size()-1); // skip last ,

		tmp +="],";

		tmp +="	legendpos: \"west\"});\n";

		if(!title.empty()) 
			tmp +=StdStringFormat(" r%d.text(320, 100, \"%s\").attr({ font: \"20px sans-serif\" });", n,title.c_str());
               

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

		tmp+="\n</script>\n";
		return tmp;

	}
		//static std::string InlinePieChart(std::map<std::string,double> &states , std::string title = "") 
	static std::string InlineLineChart(std::vector<double> &values , std::vector<std::string> &names , std::string title = "") ;
};