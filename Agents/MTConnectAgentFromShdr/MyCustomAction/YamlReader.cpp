
//
// YamlReader.cpp
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

#include <stdafx.h>
#include "YamlReader.h"
#include <algorithm>

#include "StdStringFcn.h"
#include "boost/lexical_cast.hpp"
#include <boost/spirit/home/classic/actor/assign_actor.hpp>
#include <boost/spirit/home/classic/actor/push_back_actor.hpp>

struct CYamlTokens
{
public:
    enum
    {
        yamlID = 1,
        keyID,
        valueID,
        sectionID,
        expressionID,
        sectionnameID,
        insectionID,
        eolID,
        blanklinesID
    };
};

//  the lexeme directive turns off white space skipping

struct CYamlParser : public GRAMMAR<CYamlParser>, public CYamlTokens
{
    CYamlParser(void) { }
    ~CYamlParser(void) { }
    void Clear ( )
    {
        fullsectionname.clear( );
        keyname.clear( );
        valuestr.clear( );
        sections.clear( );
        sectionvalues.clear( );
        sections.push_back("ROOT");
        fullsectionname = "ROOT";
    }

    static void write (const iterator_t first, const iterator_t last)
    {
        std::string str(first, last);
        OutputDebugString(str.c_str( ));
    }

    static void savekey (const iterator_t first, const iterator_t last)
    {
        std::string str(first, last);
        keyname = Trim(str);
    }

    static void savevalue (const iterator_t first, const iterator_t last)
    {
        std::string str(first, last);
        valuestr = Trim(str);
    }

    static void save (const iterator_t first, const iterator_t last)
    {
        std::string str(first, last);
        sectionvalues[fullsectionname + "." + keyname] = valuestr;
    }

    static std::string                        fullsectionname;
    static std::string                        keyname;
    static std::string                        valuestr;
    static std::vector<std::string>           sections;
    static std::map<std::string, std::string> sectionvalues;
    static void push (const iterator_t first, const iterator_t last)
    {
        std::string str(first, last);
        str              = Trim(str);
        fullsectionname += "." + str;

        sections.push_back(fullsectionname);
    }

    static void pop (const iterator_t first, const iterator_t last)
    {
        if ( fullsectionname.find('.') == std::string::npos )
        {
            return;
        }
        fullsectionname
            = fullsectionname.substr(0, fullsectionname.find_last_of('.'));
    }

    template<typename ScannerT>
    struct definition
    {
public:
        const SPIRIT::rule<ScannerT> & start ( ) const { return yaml_start; }
        rule<ScannerT>                                               yaml_start;
        rule<ScannerT, parser_context<>, parser_tag<yamlID> >        yaml;
        rule<ScannerT, parser_context<>, parser_tag<keyID> >         key;
        rule<ScannerT, parser_context<>, parser_tag<valueID> >       value;
        rule<ScannerT, parser_context<>, parser_tag<sectionID> >     section;
        rule<ScannerT, parser_context<>, parser_tag<insectionID> >   insection;
        rule<ScannerT, parser_context<>, parser_tag<expressionID> >  expression;
        rule<ScannerT, parser_context<>, parser_tag<sectionnameID> > sectionname;
        rule<ScannerT, parser_context<>, parser_tag<eolID> >         eol;
        rule<ScannerT, parser_context<>, parser_tag<blanklinesID> >  blankline;

        definition(CYamlParser const & self)
        {
            yaml_start = yaml;
            yaml       = *( expression | section | blankline );
            blankline  = +eol;
            section    = ( sectionname )[&push] >> *( eol ) >> str_p("{") >> *( eol )
                         >> insection >> str_p("}")[&pop] >> *( eol );
            sectionname = lexeme_d[alnum_p >> *( alnum_p | ch_p('_') )];
            key         = lexeme_d[+( alnum_p | '_' )];
            value       = *( anychar_p - eol ) >> eol;
            eol         = str_p("\n");
            insection   = *( expression | section | blankline );
            expression  = ( ( lexeme_d[+( alnum_p | '_' )] )[&savekey] >> str_p("=")
                            >> ( *( anychar_p - '\n' ) )[&savevalue] >> ch_p('\n') )[&save];
        }
    };
};



struct yaml3_skip_parser : grammar<yaml3_skip_parser>
{
    template<typename ScannerT>
    struct definition
    {
        definition(yaml3_skip_parser const & /*self*/)
        {
            skip = ( ch_p(' ') | ch_p('\t') ) | "#" >> *( anychar_p - '\n' ) >> '\n';
        }

        rule<ScannerT>         skip;
        rule<ScannerT> const & start ( ) const { return skip; }
    };
};

// Static declarations... only 1 yaml reader per app?
std::string CYamlParser::fullsectionname;
std::string CYamlParser::keyname;
std::string CYamlParser::valuestr;

std::vector<std::string>           CYamlParser::sections;
std::map<std::string, std::string> CYamlParser::sectionvalues;

//////////////////////////////////////////////////////////////////////////////////////
YamlReader::YamlReader( ) 
{ 
	parser = boost::shared_ptr<CYamlParser>(new CYamlParser( )); 
		std::string fullsectionname;

}
void YamlReader::Clear()
{
	CYamlParser::fullsectionname.clear();
	CYamlParser::keyname.clear();
	CYamlParser::valuestr.clear();

	CYamlParser::sections.clear();
	CYamlParser::sectionvalues.clear();
}
//////////////////////////////////////////////////////////////////////////////////////
int YamlReader::Load (std::string str)
{
    // Line feed is used as delimiter, so can't be in skip
    // Clueless on how iterator_t work - either const char *, or string, or spirit
    yaml3_skip_parser skip3;
    const char *      str1 = str.c_str( );
    const char *      str2 = str.c_str( ) + str.size( );
    iterator_t        first(str1, str2, (const char *) str.c_str( ));
    iterator_t        last;

    parser->Clear( );
    parse_info<iterator_t> info = parse(first, last, *parser, skip3);
#ifdef DEBUG
    for ( std::map<std::string, std::string>::iterator it
              = parser->sectionvalues.begin( );
          it != parser->sectionvalues.end( ); it++ )
    {
        OutputDebugString(
            StdStringFormat("%s=%s\n", ( *it ).first.c_str( ), ( *it ).second.c_str( ))
                .c_str( ));
    }
#endif
    return info.full;
}
//////////////////////////////////////////////////////////////////////////////////////
int YamlReader::LoadFromFile (std::string filename)
{
    std::string contents;
    ReadFile(filename, contents);
    return Load(contents);
}
//////////////////////////////////////////////////////////////////////////////////////
std::string YamlReader::Find (std::string key)
{
    std::map<std::string, std::string>::iterator it
        = parser->sectionvalues.find(key);

    if ( it != parser->sectionvalues.end( ) )
    {
        return ( *it ).second;
    }
    return "";
}
//////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> YamlReader::Sections ( ) 
{ 
	return parser->sections; 
}
//////////////////////////////////////////////////////////////////////////////////////
std::map<std::string, std::string> YamlReader::Section (std::string section)
{
    std::map<std::string, std::string> pairs;
    std::vector<std::string>           keys = Keys(section);

    for ( size_t i = 0; i < keys.size( ); i++ )
    {
        std::string value = Find(keys[i]);
        pairs[keys[i]] = value;
    }
    return pairs;
}
//////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> YamlReader::Keys (std::string section)
{
    std::vector<std::string> keys;

    for ( std::map<std::string, std::string>::iterator it
              = parser->sectionvalues.begin( );
          it != parser->sectionvalues.end( ); it++ )
    {
        std::string fullkeyname = ( *it ).first;
        std::string sectionname
            = fullkeyname.substr(0, fullkeyname.find_last_of('.'));
        std::string keyname = fullkeyname.substr(fullkeyname.find_last_of('.') + 1);

        if ( sectionname == section )
        {
            keys.push_back(keyname);
        }
    }
    return keys;
}
//////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> YamlReader::Subsections (std::string section)
{
    std::vector<std::string> subsections;
    std::vector<std::string> allsubsections;
    std::vector<std::string> allsections = parser->sections;

    for ( size_t i = 0; i < allsections.size( ); i++ )
    {
        if ( allsections[i].find(section) != std::string::npos )
        {
            if ( allsections[i] == section )               /** skip matching section */
            {
                continue;
            }
            allsubsections.push_back(allsections[i]);
        }
    }

    // now we have all branches below current section - need to prune to children
    size_t n = std::count(section.begin( ), section.end( ), '.') + 1;

    for ( size_t i = 0; i < allsubsections.size( ); i++ )
    {
        size_t level
            = std::count(allsubsections[i].begin( ), allsubsections[i].end( ), '.');

        if ( level == n )
        {
            subsections.push_back(allsubsections[i]);
        }
    }
    return subsections;
}


int YamlReader::AddSection(std::string section)
{
	// Must start with ROOT. as new section otherwise error
	if(section.compare( 0, strlen("ROOT."), "ROOT.") != 0)
		return -1;

	// insure all pre susections exists  ROOT.X.Y.Z i.e., X and Y
	// or create
	std::string fullkeyname(section);
	std::string partialsection;
	while(!fullkeyname.empty())
	{
		partialsection += fullkeyname.substr(0, fullkeyname.find_first_of('.'));
		size_t n = fullkeyname.find_first_of('.');
		if( n != std::string::npos)
			fullkeyname = fullkeyname.substr(fullkeyname.find_first_of('.')+1);
		else
			fullkeyname.clear();
		if(std::find(parser->sections.begin(), parser->sections.end(), partialsection) == parser->sections.end())
			parser->sections.push_back(partialsection);
		partialsection += ".";
	}

	// This is redundant, as new section should already be added
	if(std::find(parser->sections.begin(), parser->sections.end(), section) == parser->sections.end())
		parser->sections.push_back(section);

	return 0;
}

int YamlReader::AddKeyValue(std::string key, std::string value)
{
	// insure all pre subsections exists  ROOT.X.Y.Z i.e., X and Y
	// or create
	// Must start with ROOT. as new section otherwise error
	if(key.compare( 0, strlen("ROOT."), "ROOT.") != 0)
		return -1;

		// insure all pre susections exists  ROOT.X.Y.Z i.e., X and Y
	// or create
	std::string fullkeyname(key);
	std::string partialsection;
	while(!fullkeyname.empty())
	{
		partialsection += fullkeyname.substr(0, fullkeyname.find_first_of('.'));
		size_t n = fullkeyname.find_first_of('.');
		if( n != std::string::npos)
			fullkeyname = fullkeyname.substr(fullkeyname.find_first_of('.')+1);
		else
		{
			fullkeyname.clear();
			continue;
		}
		if(std::find(parser->sections.begin(), parser->sections.end(), partialsection) == parser->sections.end())
			parser->sections.push_back(partialsection);
		partialsection += ".";
	}

	// Now add key value
	parser->sectionvalues[key] = value;
	return 0;
}

void YamlReader::Output (std::string branch, std::stringstream & os, int indent)
{

    std::vector<std::string> keys = Keys(branch);

    for ( size_t i = 0; i < keys.size( ); i++ )
    {
		std::string key,value;
		key=keys[i];
		value = Find(branch + "." + keys[i]);
		os << StdStringFormat("%*s", indent,"") << key << "=" << value << "\n";
    }

	// Now parse subbranches
    std::vector<std::string> sections = Subsections(branch);

    for ( size_t i = 0; i < sections.size( ); i++ )
	{
		std::string subbranch;
		std::size_t found = sections[i].find_last_of(".");
		if(found== std::string::npos)
			assert(0);
		subbranch=sections[i].substr(found+1);
		os << StdStringFormat("%*s", indent,"") << subbranch << "\n";
		os << StdStringFormat("%*s", indent,"") << "{\n";
		Output(sections[i], os,indent+4);
		os << StdStringFormat("%*s", indent,"") << "}\n";
	}
}
//////////////////////////////////////////////////////////////////////////////////////
std::string YamlReader::ToString ( )
{
	std::stringstream ss;
    Output("ROOT", ss);
	return ss.str();
}