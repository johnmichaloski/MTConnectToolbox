
//
// YamlReader.h
//

/*
* DISCLAIMER:
* This software was produced by the National Institute of Standards
* and Technology (NIST), an agency of the U.S. government, and by statute is
* not subject to copyright in the United States.  Recipients of this software
* assume all responsibility associated with its operation, modification,
* maintenance, and subsequent redistribution.
*
* See NIST Administration Manual 4.09.07 b and Appendix I.
*/
#pragma once

#include <list>
#include <map>
#include <string>
#include <vector>
#include <sstream>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/spirit/home/classic/tree/parse_tree.hpp>
#include <boost/spirit/include/classic.hpp>

using namespace std;
using namespace boost;
#define GRAMMAR    boost::spirit::classic::grammar
using namespace boost::spirit::classic;
#define SPIRIT     boost::spirit::classic

typedef position_iterator<char const *>   iterator_t;
typedef std::vector<parser_id>            TreeNodes;
typedef string::iterator                  iter_t;

struct YamlReader;
struct CYamlParser;
typedef boost::shared_ptr<CYamlParser>    CYamlParserPtr;

/*

YamlReader  yaml;

std::string  str =
"Devices = Devices.xml \n"
"ServiceName = UR_Agent\n"
"Port = 5000\n"
"CheckpointFrequency=10000\n"
"AllowPut=true\n"
"Adapters \n"
"{\n"
" Mazak_1 \n"
" { \n"
"    Device = Mazak101\n"
"    Host =  192.168.0.11\n"
"    Port = 7878 \n"
"  } \n"
"  \n"
"Mazak_2 \n"
" { \n"
"    Device = Mazak102\n"
"    Host =  192.168.0.12\n"
"    Port = 7878\n"
"  } \n"
"  \n"
"}\n"
"logger_config\n"
"{\n"
"	logging_level = debug\n"
"	output = cout\n"
"}\n";
yaml.Load(str);

OR

std::string filename =
"C:\\Users\\michalos\\Documents\\GitHub\\Agents\\UR_MTConnectAgent\\UR_Agent\\x64\\Debug\\Agent.cfg";
yaml.LoadFromFile(filename);


*/


/**
* \brief Class that reads and parses an MTConnect agent configuration file
* (typically agent.cfg) using Boost spirit.
* Will parse a mtconnect agent cfg file into oo naming (e.g.,
* ROOT.section1.key) .
* The uppermost section (assumes no section has been declared) is called ROOT.
* Each section under the ROOT is ROOT.section1,
* ROOT.section2, etc. Embedded sections within a section are  OO concatenated
* (e.g., ROOT.section1.section11).
*/
struct YamlReader
{
	/**
	* \brief Constructor that creates a boost spirit yaml parser.
	*/
	YamlReader( );

	/**
	* \brief Load takes a YAML string and parses the string into key/value pairs.
	* The key names have the embedded sections prepended to its name.
	*/
	int                                Load (std::string str);

	/**
	* \brief Reads a Yaml MTConnect agent config file into a string, and then load
	* takes the YAML string and parses
	* the string into key/value pairs. The key names have the embedded sections
	* prepended to its name.
	*/
	int                                LoadFromFile (std::string filename);

	/**
	* \brief Uses a full key path name (ROOT.section1.section2.keyname) to find
	* its value int the YAML.
	* the string into key/value pairs. The key names have the embedded sections
	* prepended to its name.
	* \param blank keyname is the full key path name (including section names
	* concatenated by period ("."))
	* \return blank string if not found, or the key value.
	*/
	std::string                        Find (std::string keyname);

	/**
	* \brief Returns all the fully qualified section names (each section
	* contatenated with period to embedded section).
	*/
	std::vector<std::string>           Sections ( );

	/**
	* \brief Returns all the  key/value pairs in a fully qualified section name.
	* \param  section fully qualified section name (each section contatenated with
	* period to embedded section)
	*/
	std::map<std::string, std::string> Section (std::string section);

	/**
	* \brief Returns all the  key (not subsections) in a fully qualified section
	* name.
	* \param  section fully qualified section name (each section contatenated with
	* period to embedded section)
	*/
	std::vector<std::string>           Keys (std::string section);

	/**
	* \brief Returns all the  subsections (not recursively embedded sections) in a
	* fully qualified section name.
	* \param  section fully qualified section name (each section contatenated with
	* period to embedded section)
	*/
	std::vector<std::string>           Subsections (std::string section);
	int AddSection(std::string);
	int AddKeyValue(std::string key, std::string value);
	void Clear();


	template<typename T>
	T Convert(std::string data, T default)
	{
		T result=default;
		std::istringstream stream(data);

		if(stream >> result)
			return result;
		return result;
	}
	std::string ToString ( );
protected:
	void Output (std::string branch, std::stringstream & os, int indent=0);
	boost::shared_ptr<CYamlParser>     parser;
	// Would really like multiple instances of yamlparser with unique variables.
	//std::string fullsectionname;
	//std::string keyname;
	//std::string valuestr;

	//std::vector<std::string>           sections;
	//std::map<std::string, std::string> sectionvalues;

};
