#pragma once

// ---------------------------------------------------------------------------|
// Boost Includes
// ---------------------------------------------------------------------------|
//#include <boost/variant/recursive_variant.hpp>
//#include <boost/fusion/include/adapt_struct.hpp>
//#include <boost/spirit/include/qi.hpp>          
//#include <boost/spirit/include/support_istream_iterator.hpp>
//#include <boost/spirit/include/phoenix_core.hpp>
//#include <boost/spirit/include/phoenix_operator.hpp>
//#include <boost/spirit/include/phoenix_fusion.hpp>
//#include <boost/spirit/include/phoenix_stl.hpp>
//#include <boost/spirit/include/phoenix_object.hpp>

// ---------------------------------------------------------------------------|
// Standard Includes
// ---------------------------------------------------------------------------|
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>

/**
	Use:
	Yaml storage;
	ParseIniFile( ::ExeDirectory() + "Test.ini",  storage );

	*/
// ---------------------------------------------------------------------------|
struct Yaml;

struct ltstr
{
  bool operator()(const std::string & s1, const std::string &  s2) const
  {
    return false;
  }
};

typedef std::map<std::string, std::string> NVMap;
typedef std::map<std::string, Yaml> YamlMap;
struct Yaml
{
	NVMap keys;
	YamlMap sections;
	std::vector<std::string> GetSections() 
	{ 
		std::vector<std::string> ids;
		YamlMap::iterator it;
		for(it=sections.begin(); it!=sections.end(); it++)
			ids.push_back((*it).first);
		return ids;
	}
	Yaml& GetSection(std::string name)
	{
		YamlMap::iterator it=sections.find(name);
		if(it!= sections.end())
			return (*it).second;
		throw std::exception("no section");
	}

};

extern bool ParseIniFile( const std::string& filename, Yaml& storage );
