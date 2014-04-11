//
// DataArchiver.cpp
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#include "StdAfx.h"
#include "CsvArchiver.h"
#include <algorithm>
#include "StdStringFcn.h"
#include "Snapshots.h"
#include "File.h"

void CsvArchiver::close()
{
	if(_CsvOutputFile.is_open())
		_CsvOutputFile.close();

}
void CsvArchiver::openCsv(std::string csvfilename, int opMode )
{
	_Csvfilename=csvfilename;
//	int opMode = std::fstream::out;
	_CsvOutputFile.open(csvfilename.c_str(), opMode, OF_SHARE_DENY_NONE);

}

void CsvArchiver::csvflush(std::string str, std::string separator)
{
	std::vector<std::string> fields = TrimmedTokenize(str,separator);
	std::string tmp;
	for(int i=0; i< fields.size(); i++)
	{
		std::string value = fields[i];
		if(fields[i]=="UNAVAILABLE")
		{
			value= "--";
		}
		if(i>1)		tmp+=",";
		tmp+=value;
	}
	_CsvOutputFile<< tmp << "\n";
}
void CsvArchiver::csvheader(std::string str, std::string separator)
{
	csvflush( str, separator);
}

void CsvArchiver::append(std::string csvfilename, std::string fields, DataDictionary typedata, std::vector<DataDictionary>  datum)
{
	if(!File.Exists(csvfilename))
	{
		write(csvfilename, fields, typedata, datum);
		return;
	}
	openCsv(csvfilename, std::fstream::out + std::fstream::app);
	if(_CsvOutputFile.bad())
	{
		return;
	}
	writedata(fields, typedata, datum);
	close();
}
void CsvArchiver::writedata(std::string fields, DataDictionary typedata, std::vector<DataDictionary>  datum)
{
	if(_CsvOutputFile.bad())
		return;
	std::vector<std::string> _fields = TrimmedTokenize(fields,",");
	for(int i=0; i< datum.size(); i++)
	{
		DataDictionary data=datum[i];
		for(int i=0; i< _fields.size(); i++)
		{
			std::string value = data[_fields[i]];
			if(value=="UNAVAILABLE")
				value = "";
			if(value.find("--")!= std::string::npos)
				value = "";

			// FIXME convert seconds to hr:minn:sec
			if(typedata[_fields[i]] == "adDBTime")
			{
				value =  DataDictionary::HrMinSecFormat(ConvertString<double>(value, 0.0)); 
			}
			if(i>0) _CsvOutputFile<<",";
			_CsvOutputFile <<  value;
		}
		_CsvOutputFile<<"\n";
	}
}
void CsvArchiver::write(std::string csvfilename, std::string fields, DataDictionary typedata, std::vector<DataDictionary>  datum)
{
	// FIXME: test
	// Recursively create folder to allow storage of csv files
	std::string path = ExtractDirectory(csvfilename);
	SHCreateDirectoryEx( NULL, path.c_str(), NULL );

	openCsv(csvfilename);
	if(_CsvOutputFile.bad())
		return;

	std::string tmp;
	DataDictionary::iterator it;
	if(datum.size() <1)
		return;
	// Write column header
	std::vector<std::string> _fields = TrimmedTokenize(fields,",");

	for(int i=0; i< _fields.size(); i++)
	{
		if(i>0) _CsvOutputFile<<",";
		_CsvOutputFile<< _fields[i];
	}
	_CsvOutputFile<<"\n";
	writedata(fields, typedata, datum);
	close();
}
void CsvArchiver::read(std::string csvfilename, std::vector<std::string> &fields, std::vector<DataDictionary> & datum)
{
	std::stringstream				strbuffer; 
	std::string						_buffer;	

	datum.clear();
	fields.clear();
	inCSV.open(csvfilename.c_str());

	if(inCSV.bad())
		return;

	getline(inCSV,_buffer);
	if(inCSV.eof( ))
		return;

	// Read column header
	fields = TrimmedTokenize(_buffer,",");

	while(getline(inCSV,_buffer))
	{
		std::vector<std::string> line  = TrimmedTokenize(_buffer,",");
		DataDictionary data;
		// parse lines of data
		for(int i=0; i< fields.size(); i++)
		{
			if(fields.empty())
				continue;
			data[fields[i]] = line[i];
		}
		datum.push_back(data);
	}
	inCSV.close();
}
#if 0

void CArchiver::open(std::string filename)
{
	_filename=filename;
	int opMode = std::fstream::out;
	//	if(bAppend)
	//		opMode |= std::fstream::app;

	_OutputFile.open(filename.c_str(), opMode, OF_SHARE_DENY_NONE);
}

void CArchiver::reset()
{
	_updatedvalues.clear();
}
void CArchiver::SetTimestamp(std::string timestamp)
{
	_timestamp=timestamp;
}
void CArchiver::SetValue(std::string name, std::string value)
{
	if(_valuemapping.find(name) != _valuemapping.end())
	{
		if(_valuemapping[name]!=value)
			_updatedvalues.push_back(name);
	}
	else
	{
		_updatedvalues.push_back(name);
	}

	_valuemapping[name]=value;
}
#if 0
void CArchiver::shdrflush()
{
	if(_updatedvalues.size()==0)
		return;
	int cnt=0;
	std::string tmp;
	tmp+=_timestamp ;
	for(int i=0; i< _updatedvalues.size(); i++)
	{
		// check if not keeping this value
		if(std::find(_excludedvalues.begin(), _excludedvalues.end(),_updatedvalues[i]) !=  _excludedvalues.end())
			continue;
		cnt++;
		tmp+="|";
		tmp+= _updatedvalues[i] ;
		tmp+="|";
		std::string value = _valuemapping[_updatedvalues[i]];
		double d;
		if((d=atof(_valuemapping[_updatedvalues[i]].c_str()))!=0)
		{
			_valuemapping[_updatedvalues[i]]= StdStringFormat("%8.4f", d);
		}
		tmp+=_valuemapping[_updatedvalues[i]] ;
	}
	// after filterning, no values to update
	if(cnt==0)
		return;
	OutputDebugString(tmp.c_str());
	if(_OutputFile.is_open())
		_OutputFile<< tmp << std::endl;
	//reset();
}
#endif

#endif
