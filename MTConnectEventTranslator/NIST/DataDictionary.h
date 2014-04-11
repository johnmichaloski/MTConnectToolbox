//
// DataDictionary.h
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.

#pragma once
#include <string>
#include <vector>
#include "StdStringFcn.h"

class less_NoCaseString
{
public:
	bool operator() (const std::string& x, const std::string& y) const
	{return (stricmp( x.c_str(), y.c_str() )<0); }
};
extern std::string String(std::vector<std::string> &list);
class DataDictionary : public std::map<std::string, std::string, less_NoCaseString> 
{
public:
	typedef std::map<std::string, std::string, less_NoCaseString>  DataDictionaryBaseType;
	std::string blank;
	DataDictionary& operator=(const std::map<std::string, std::string>& base)
	{
		clear();
		insert(base.begin(), base.end());
		return *this;
	}
	std::vector<std::string> Keys()
	{
		std::vector<std::string> v;
		for(DataDictionaryBaseType::iterator it = begin(); it != end(); ++it) 
			v.push_back(it->first);
		return v;
	}
	std::string ValueString(std::string csvfields)
	{
		std::string tmp;
		std::vector<std::string> _fields = TrimmedTokenize(csvfields,",");
		for(int i=0; i< _fields.size(); i++)
		{
			if(i>0) tmp+=",";
			tmp+= this->operator [](_fields[i]);
		}
		return tmp;
	}
	std::vector<std::string> ValueList(std::vector<std::string> fields)
	{
		std::vector<std::string> tmp;
		for(int i=0; i< fields.size(); i++)
		{
			tmp.push_back( this->operator [](fields[i]));
		}
		return tmp;
	}
	void Mixin(std::vector<std::string> items, std::string defaultVal)
	{
		for(int i=0; i< items.size(); i++)
				this->operator [](items[i]) = defaultVal;
	}
	void SelectiveMerge(std::vector<std::string> itemlist, DataDictionary & itemstomerge)
	{
		for(int i=0; i< itemlist.size(); i++)
		{
			this->operator [](itemlist[i]) = itemstomerge[itemlist[i]];
		}

	}

	static std::string HrMinSecFormat(int sec);
	static std::string HrMinSecFormat(double sec);
	void Update(double seconds);
	std::map<std::string, double, less_NoCaseString>  accumulations;

	std::string & operator[](const std::string& cstr)
	{
		std::string str=cstr;
		std::transform(str.begin(),str.end(),str.begin(),tolower);
		DataDictionary::iterator it = this->find(str);
		if(it==this->end()) 
			this->insert(std::make_pair(str,""));
		it = this->find(str);
		return (*it).second;
	} 
	std::string & operator[](const char * cstr)
	{
		std::string str=cstr;
		std::transform(str.begin(),str.end(),str.begin(),tolower);
		DataDictionary::iterator it = this->find(str);
		if(it==this->end()) 
			this->insert(std::make_pair(str,""));		
		it = this->find(str);
		return (*it).second;
	}
	template<typename T>
	std::vector<T> Column( std::vector<DataDictionary> data, std::string column)
	{
		std::vector<T> items;
		for(int i=0; i< data.size(); i++)
		{
			try{
			items.push_back(ConvertString<T>(data[i][column], T()));
			}
			catch(...){}
		}
		return items;
	}
};
