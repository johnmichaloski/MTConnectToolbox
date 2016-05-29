
//http://msdn.microsoft.com/en-us/library/6k3ybftz.aspx 
/**
* A simple INI file parser based on the NVP Query parser from:
* <a href="http://spirit.sourceforge.net/home/?p=371"></a>
* 
* This parser is used to parse ini files of the following below, 
* placing results on a std::map<std::string, std::string>:
*
*    key1=AnotherValue1 
*
*    # comment
*    key2=Value2
*    key3 =Value3 
*    key4 = Value4
*/
#include "stdafx.h"
#include "Yaml.h"

#define BOOST_SPIRIT_AUTO 

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>


namespace spirit = boost::spirit;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

// ----------------------------------------------------------------------------|
namespace client
{
	/** Type of results produced */

	// ----------------------------------------------------------------------------|
	/**
	* The Qi Grammar for the parser.
	*
	* \tparam Iterator the type of iterator the parser uses.
	* \tparam Skipper the skipper, for skipping over comments and whitespace.
	*/
	template <typename Iterator, typename Skipper>
	struct key_value_sequence : qi::grammar< Iterator , NVPMapT() , Skipper >
	{
		/** 
		* Constructor. 
		*
		*  nvp_map = nvp
		*            *nvp
		*  nvp = key "=" value
		*  key = asciiChar (asciiChar | digit)*
		*  value = +(asciiChar | digit)* | quotedString
		*  quotedString = '"' character* '"'
		*  asciiChar = [a-zA-Z]
		*  digit = [0-9]
		*
		* */
		key_value_sequence() : key_value_sequence::base_type( nvp_map )
		{
			nvp_map = pair >> *pair;
			pair  =  key >> '=' >> ( value | quotedString );
			key   =  qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9");
			//value = +qi::char_("a-zA-Z_0-9") >> eol_p;
			value = *(qi::char_ - "\n") >> "\n"; 
			quotedString = '\"' >> qi::lexeme[+(qi::char_ - '\"')] >> '\"' >> qi::eol;
		}
		qi::rule<Iterator, NVPMapT(), Skipper> nvp_map;
		qi::rule<Iterator, std::pair<std::string, std::string>() > pair;
		qi::rule<Iterator, std::string()> key;
		qi::rule<Iterator, std::string()> value;
		qi::rule<Iterator, std::string()> quotedString;
	};

	// ----------------------------------------------------------------------------|
	/**
	* Parse the supplied input.
	*
	* \tparam Input the type of input to parse.
	* \param in the input to parse.
	* \param storage storage for the parsed values.
	* \return true if parsing was successful.
	*/
	template< class Input >
	bool parse( Input& in, NVPMapT& storage )
	{
		Input::iterator iter = in.begin();
		Input::iterator end = in.end();

		auto skipper =  ascii::space | '#' >> *(qi::char_ - qi::eol) >> qi::eol;
		typedef decltype( skipper ) skipper_type;

		client::key_value_sequence<std::string::iterator, skipper_type> parser;

		bool r = phrase_parse(iter, end, parser, skipper, storage);
		return (r && iter == end);
	}

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

	// ----------------------------------------------------------------------------|
	/**
	* Utility function for invoking the parser for ini files.
	* 
	* \param filename the name of the file to read.
	* \param storage storage for the parsed values.
	* \return true if parsing was successful.
	*/
	bool ParseIniFile( const std::string& filename, NVPMapT& storage )
	{
		std::string in = ReadIniFile( filename );
		return parse( in, storage );
	}

}
