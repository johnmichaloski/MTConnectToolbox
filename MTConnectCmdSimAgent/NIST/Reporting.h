//
// Reporting.h
//

#pragma once

#include "CmdCell.h"
#include <string>

class Reporting
{
public:
	Reporting(void);
	~Reporting(void);
	static void AgentStatus(CWorkOrders * jobs, std::string &JobStatus, std::string &Jobs, std::string & DeviceStatus );
	static std::string GenerateHtmlReport(CWorkOrders * jobs, std::string filename);
	static std::string CreateHistory(double dPixelWidth, std::vector<int> _historian, std::vector<double> _timing) ;
	static std::string GenerateResourceReport(int i);
	static std::string Title() { return sTitle; }
	std::string CreateHtmlJobTable(CWorkOrders * jobs);
	std::string JobThrougput(std::string partid);
	std::string CreateHtmlResourceStatesTable();
	std::string CreateHtmlResourceKPITable();
	std::string GenerateHtmlHistory();
	std::string GenerateSetupUsage();
	std::string CreateHtmlFactoryUseTable(int shift, double hrs, double capacity, double actual);
	std::string CreateHorizontalHistogramTD(double dPixelWidth,double dPixelHeight,	std::vector<int> _historian, std::vector<double> _timing) ;
	std::string GeneratePieChart();
	////////////////////////////////////////////
	static std::string sTitle;

};

