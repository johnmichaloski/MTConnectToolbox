//
// DataArchiver.h
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#pragma once

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include "Atlutil.h"
#include "DataDictionary.h"

class CsvArchiver
{
public:
	void append(std::string csvfilename, std::string fields, DataDictionary typedata, std::vector<DataDictionary>  datum);
	void write(std::string csvfilename, std::string fields, DataDictionary typedata, std::vector<DataDictionary>  datum);
	void read(std::string csvfilename, std::vector<std::string> &fields, std::vector<DataDictionary> & datum);
protected:
	void writedata(std::string fields, DataDictionary typedata, std::vector<DataDictionary>  datum);
	void openCsv(std::string csvfilename,int opMode = std::fstream::out);
	void close();
	void csvflush(std::string str, std::string separator=",");
	void csvheader(std::string, std::string separator=",");;
	std::string						_Csvfilename;
	std::fstream					_CsvOutputFile;
	std::ifstream					inCSV;



};
#if 0

class CShdrArchiver
{
public:
	//void shdrflush();
	//void open(std::string filename);
	std::string _filename;
	std::fstream _OutputFile;
};
#endif