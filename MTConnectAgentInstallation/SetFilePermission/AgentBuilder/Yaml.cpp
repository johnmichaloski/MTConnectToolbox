

#include "stdafx.h"
#include "Yaml.h"
#include "StdStringFcn.h"

#include "OutDebug.h"

// To see the debugging output putp this code in, so cout will be redirected:

/*
	DebugFileOut debugfile(CString((::ExeDirectory() + "debug.txt").c_str()));
	basic_debugbuf debugbuf(2048);
	std::cout.rdbuf(&debugfile);
	cout<< "Test Me" << endl;


	Yaml storage ;
	ParseIniFile("C:\\Program Files\\MTConnect\\Agent\\Agent.cfg", storage );
*/


//#define BOOST_SPIRIT_DEBUG
//#define BOOST_SPIRIT_DEBUG_TRACENODE          (true)
//#define BOOST_SPIRIT_DEBUG_FLAGS  BOOST_SPIRIT_DEBUG_FLAGS_NODES 
//

#include <boost/spirit.hpp>
#include <boost/version.hpp>
//#include <boost/spirit/core.hpp>
#include <boost/spirit/home/classic/core.hpp>

#if BOOST_VERSION <= 103200
#define BOOST_SPIRIT_NO_REGEX_LIB
#endif

#include <boost/spirit/home/classic/tree/parse_tree.hpp>
#include <boost/spirit/home/classic/tree/ast.hpp>
#include <boost/spirit/home/classic/error_handling.hpp>
#include <boost/spirit/home/classic/iterator/position_iterator.hpp>


using namespace std;
using namespace boost;
using namespace boost::spirit;

typedef std::vector<parser_id> TreeNodes;

std::string ReadIniFile( const std::string& filename );

struct CYamlParser;
typedef std::string::iterator iterator_t;

inline void write(iterator_t begin, iterator_t end)
{
	std::string str(begin, end );
	OutputDebugString( str.c_str());
}

struct  CYamlParser :
	public boost::spirit::grammar<CYamlParser>
{

	enum  { entryID=1,pairID, sectionID, keyID, valueID, quotedStringID,linefeedID };
	std::map<parser_id, std::string> rule_names;


	template <typename ScannerT>
	struct definition		
	{
	public:

		const boost::spirit::rule<ScannerT> &start() const { return schema_start; }

		rule<ScannerT>  schema_start;
		rule<ScannerT, parser_context<>, parser_tag<entryID> >  entry;
		rule<ScannerT, parser_context<>, parser_tag<pairID> >  pair;
		rule<ScannerT, parser_context<>, parser_tag<keyID> >  key;
		rule<ScannerT, parser_context<>, parser_tag<valueID> >  value;
		rule<ScannerT, parser_context<>, parser_tag<quotedStringID> >  quotedString;
		rule<ScannerT, parser_context<>, parser_tag<sectionID> >  section;
		rule<ScannerT, parser_context<>, parser_tag<linefeedID> >  linefeed;


		definition( CYamlParser const &self )
		{  
			schema_start =  *(entry | linefeed);
			entry = pair | section  ;
			linefeed =  +(ch_p('\n') || ch_p('\r'));
			pair  =  key >> '=' >> ( value | quotedString ) >> linefeed;
			key   =  lexeme_d[alpha_p >> *(alnum_p|'_')];
			//value = +qi::char_("a-zA-Z_0-9") >> eol_p;
			value = lexeme_d[*( anychar_p - (ch_p('\n') || ch_p('\r')) )] ; 
			quotedString = '\"' >> lexeme_d[*( anychar_p - ch_p('\"'))] >> ch_p('\"') ;
			section  =  key >> linefeed >> ch_p('{') >>  linefeed  >> *(entry) >> ch_p('}') >> linefeed;

			BOOST_SPIRIT_DEBUG_RULE(schema_start); 
			BOOST_SPIRIT_DEBUG_RULE(entry); 
			BOOST_SPIRIT_DEBUG_RULE(pair); 
			BOOST_SPIRIT_DEBUG_RULE(key); 
			BOOST_SPIRIT_DEBUG_RULE(value); 
			BOOST_SPIRIT_DEBUG_RULE(quotedString); 
			BOOST_SPIRIT_DEBUG_RULE(section); 
			BOOST_SPIRIT_DEBUG_RULE(linefeed); 

		}
	};
	CYamlParser()
	{
		rule_names[entryID] =	"entry";
		rule_names[pairID] =	"pair";
		rule_names[keyID] =	"key";
		rule_names[valueID] =	"value";
		rule_names[quotedStringID] =	"quotedString";
		rule_names[sectionID] =	"section";
		rule_names[linefeedID] =	"linefeed";

	}
};

struct express_skip_grammar : public grammar<express_skip_grammar>
{
	template <typename ScannerT>
	struct definition
	{
		definition(express_skip_grammar const& /*self*/)
		{
			skip_until_eol = *(('\\' >> eol_p) | (anychar_p - eol_p)),

				skip = 
				+( confix_p("#",*anychar_p,eol_p)
				| blank_p  
				)
				;
		}

		rule<ScannerT> skip, skip_until_eol;
		rule<ScannerT> const& start() const { return skip; }
	};
};



// ----------------------------------------------------------------------------|
/**
* Utility function for reading the file into a string.
*
* \param filename the name of the file to read.
* \return a string containing the file contents.
*/
std::string ReadIniFile( const std::string& filename )
{
	std::ifstream in( filename.c_str() );
	assert( in );
	std::ostringstream oss;
	oss << in.rdbuf();
	return oss.str();
}


typedef std::map<std::string, std::string> SymbolTable;



template<typename TreeIter>
std::string GetTreeNodeValue(TreeIter i )
{
	std::string str;
	for(TreeIter it = i->children.begin();it!=i->children.end(); it++) 
		str+=std::string(it->value.begin(), it->value.end());
	return str; 
}

// depth search of tree for matching parser id.
template<typename TreeIter>
int findANode(const TreeIter &i, TreeIter& findit, parser_id id)
{
	for(TreeIter it = i->children.begin();it!=i->children.end(); it++) 
	{  
		int node  = it->value.id().to_long();
		if( id == node  )
		{
			findit=it;
			return 1;
		}
		if (findANode(it, findit, id))
			return 1;
	}
	return 0;
}


template<typename TreeIter>
int evalExpress(const TreeIter& i, Yaml & symtable)
{
	TreeIter findit;
	TreeNodes nodes;
	int n =  i->value.id().to_long();
	{
		if(n==CYamlParser::pairID) 
		{
			TreeIter it = i->children.begin();   // simpleid
			std::string key,value;
			if(findANode(i, findit, CYamlParser::keyID))
				key = GetTreeNodeValue(findit );
			if(findANode(i, findit, CYamlParser::valueID))
				value = GetTreeNodeValue(findit );

			symtable.keys[key]=value;
		}	

		if(n==CYamlParser::sectionID) 
		{
			std::string name ;
			if(findANode(i, findit, CYamlParser::keyID))
				name = GetTreeNodeValue(findit );
			symtable.sections[name] = Yaml();

			for(TreeIter it = i->children.begin(); it!=i->children.end(); it++) 
			{  
				evalExpress(it, symtable.sections[name]); 
			}		
		}
		else
		{
			for(TreeIter it = i->children.begin(); it!=i->children.end(); it++) 
				evalExpress(it,symtable); 
		}
	}
	return 0;
}

HRESULT ParseIt(std::string filename, Yaml &symtable)
{
	express_skip_grammar skip;
	CYamlParser parser;
	std::string contents = ReadIniFile( filename );
	contents +='\n'; //  must end with line feed

	//parse_info<> info =	boost::spirit::parse(contents.c_str(), parser, skip);
	tree_parse_info<const char*> info =		ast_parse((const char *) contents.c_str(), parser, skip);
	if(info.full )
	{
#if 1
		evalExpress(info.trees.begin(), symtable);
		Yaml yaml = symtable.GetSection("Adapters");
		std::vector<std::string> sections = yaml.GetSections();
		for(int i=0; i< sections.size(); i++)
		{
			Yaml y = yaml.GetSection(sections.at(i));
			for(NVMap::iterator it=y.keys.begin(); it!=y.keys.end(); it++)
			{
				AtlTrace(StdStringFormat("%s=%s\n", (*it).first.c_str(), (*it).second.c_str()).c_str());
			}
		}
#endif
		return S_OK;
	}
	return E_FAIL;
}


// ----------------------------------------------------------------------------|
/**
* Utility function for invoking the parser for ini files.
* 
* \param filename the name of the file to read.
* \param storage storage for the parsed values.
* \return true if parsing was successful.
*/
bool ParseIniFile( const std::string& filename, Yaml &symtable )
{
	ParseIt(filename, symtable);
	return true;
}

