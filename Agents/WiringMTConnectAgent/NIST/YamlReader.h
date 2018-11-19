
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
#include <string>
#include <map>
#include <vector>

//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/home/classic/tree/parse_tree.hpp>
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;
#define GRAMMAR boost::spirit::classic::grammar 
using namespace boost::spirit::classic;
#define SPIRIT  boost::spirit::classic

typedef position_iterator<char const*> iterator_t;
typedef std::vector<parser_id> TreeNodes;
typedef string::iterator iter_t;

struct ptree;
struct YamlReader;
struct CYamlParser;
typedef boost::shared_ptr<CYamlParser>   CYamlParserPtr;


/*

YamlReader  yaml;

std::string  str = 
"Devices = Devices.xml \n"
"ServiceName = WiringAgent\n"
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

std::string filename = "C:\\Users\\michalos\\Documents\\GitHub\\Agents\\WiringMTConnectAgent\\WiringAgent\\x64\\Debug\\Agent.cfg";
yaml.LoadFromFile(filename);


*/

struct ptree
{
	std::string data;                         // data associated with the node
	std::list< std::pair<std::string, ptree> > children; // ordered list of named children
};

/**
* \brief Class that reads and parses an MTConnect agent configuration file (typically agent.cfg) using Boost spirit.
* Will parse a mtconnect agent cfg file into oo naming (e.g., GLOBAL.section1.key) which is thanks to Al Chisolm of OPC Classic.
* The uppermost section (assumes no section has been declared) is called GLOBAL. Each section under the GLOBAL is GLOBAL.section1,
* GLOBAL.section2, etc. Embedded sections within a section are  OO concatenated (e.g., GLOBAL.section1.section11). 
*/
struct YamlReader
{
	/**
	* \brief Constructor that creates a boost spirit yaml parser.
	*/
	YamlReader();
	/**
	* \brief Load takes a YAML string and parses the string into key/value pairs. The key names have the embedded sections prepended to its name.
	*/
	int Load(std::string str);
	/**
	* \brief Reads a Yaml MTConnect agent config file into a string, and then load takes the YAML string and parses 
	* the string into key/value pairs. The key names have the embedded sections prepended to its name.
	*/
	int LoadFromFile(std::string filename);
	/**
	* \brief Uses a full key path name (GLOBAL.section1.section2.keyname) to find its value int the YAML.  
	* the string into key/value pairs. The key names have the embedded sections prepended to its name.
	* \param blank keyname is the full key path name (including section names concatenated by period ("."))
	* \return blank string if not found, or the key value.
	*/
	std::string Find(std::string keyname); 

	/**
	* \brief Returns all the fully qualified section names (each section contatenated with period to embedded section). 
	*/
	std::vector<std::string> Sections();
	/**
	* \brief Returns all the  key/value pairs in a fully qualified section name. 
	* \param  section fully qualified section name (each section contatenated with period to embedded section)
	*/ 
	std::map<std::string,std::string> Section(std::string section);
	/**
	* \brief Returns all the  key (not subsections) in a fully qualified section name. 
	* \param  section fully qualified section name (each section contatenated with period to embedded section)
	*/ 
	std::vector<std::string> Keys(std::string section);
	/**
	* \brief Returns all the  subsections (not recursively embedded sections) in a fully qualified section name. 
	* \param  section fully qualified section name (each section contatenated with period to embedded section)
	*/
	std::vector<std::string> Subsections(std::string section);
	/**
	* \brief parses all the fully qualified section names and key/value pairs into boost property tree equivalent. 
	* Unclear if this is even useful.
	*/
	void ParseIntoBoostPropertyTreee();
protected:
	void ParseIntoBoostPropertyTreee(std::string branch, ptree &pt);
	//////////////////////////////////////
	ptree pt; /**< boost property tree parse tree */ 

	//CYamlParserPtr parser;
	CYamlParser * parser;

};
