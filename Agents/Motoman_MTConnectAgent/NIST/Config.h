
//
// Config.h
//
// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#ifndef __INICONFIG_H__
#define __INICONFIG_H__

//#include "StdStringFcn.h"
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

// #define CASE_SENSITIVE

#pragma comment(user, "Compiled on " __DATE__ " at " __TIME__)

namespace Nist
{

	/**
	* @brief The StringVariant class is a simple string-based variant implementation
	* that allows
	* the user to easily convert between simple numeric/string types.
	*/

	class StringVariant
	{
		std::string data;                                  //!< holds the data as string
	public:
		StringVariant( ) : data( ) { }

		/**
		* @brief StringVariant template constructor that takes any primitive
		* data type and converts into string
		* @param val value to convert into string
		*/
		template<typename ValueType>
		StringVariant(ValueType val)
		{
			std::ostringstream stream;
			try
			{
				stream << val;
			}
			catch ( ... )
			{ }
			data.assign(stream.str( ));
		}

		template<typename ValueType>
		StringVariant & operator = (const ValueType val)
		{
			std::ostringstream stream;
			stream << val;
			data.assign(stream.str( ));
			return *this;
		}

		/**
		* @brief toNumber converts data into type as defined by template
		* @return string into type as defined by template
		*/
		template<typename NumberType>
		NumberType toNumber ( ) const
		{
			NumberType result = 0;

			std::istringstream stream(data);

			if ( stream >> result )
			{
				return result;
			}
			else if ( ( data == "yes" ) || ( data == "true" ) )
			{
				return 1;
			}

			return 0;
		}

		template<typename T>
		T convert ( ) const
		{
			T result;
			try {
				std::istringstream stream(data);

				if ( stream >> result )
				{
					return result;
				}
				//else if ( ( data == "yes" ) || ( data == "true" ) )
				//{
				//	return 1;
				//}
			}
			catch(...)
			{
			}
			return T();
		}
		template<>
		std::string convert<std::string> ( ) const
		{
			return data;
		}
		template<>
		bool convert<bool> ( ) const
		{
			std::string truth = data;
			truth=MakeUpper(truth);
			if ( ( truth == "YES" ) || ( truth == "TRUE" ) )
				return 1;
			else if ( ( truth == "NO" ) || ( truth == "FALSE" ) )
				return 0;
			else
			{
				truth=trim(truth);
				if ( ( truth == "1" ))
					return 1;
			}
			return 0;
		}
	};

	/**
	* @brief The Config class can be used to load simple key/value pairs from a
	*file.
	*
	* @note An example of syntax:
	*	// An example of a comment
	*	username= Bob
	*	gender= male
	*	hair-color= black // inline comments are also allowed
	*	level= 42
	*
	* @note An example of usage:
	*	Config config;
	*	config.load("myFile.txt");
	*
	*	std::string username = config["username"].str();
	*	int level = config["level"].toNumber<int>();
	*
	*	Config config;
	*	config.load(inifile);
	*	OutputDebugString(config.GetSymbolValue("FANUC.INCHES",
	*L"INCHES").c_str());
	*	OutputDebugString(StrFormat("%f\n", config.GetSymbolValue("MAIN.MAXRPM",
	*"9000").toNumber<double>()));
	*/

	class Config
	{
	public:
		typedef std::map<std::string, std::vector<std::string> >   SectionMap;
		typedef std::map<std::string, StringVariant>::iterator     ConfigIt;
		/**
		* @brief Config empty constructor. Set case insensitive flag;
		*/
		Config( ) 
		{ 
			bCaseSensitive=false;
		}

		SectionMap::iterator FindSection(std::string sectionname)
		{
			SectionMap::iterator     iter;
#ifdef CASE_SENSITIVE
			iter = sections.find(sectionname);
#else
			// Key is now uppercase
			std::transform(sectionname.begin( ), sectionname.end( ), sectionname.begin( ), toupper);

			for(iter=sections.begin(); iter!=sections.end(); iter++)
			{
				std::string section = (*iter).first;
				std::transform(section.begin( ), section.end( ), section.begin( ), toupper);
				if(section==sectionname)
					return iter;
			}
#endif
			return sections.end();
		}
		/**
		* @brief Clear clears the configuration containers.
		*/
		void Clear ( )
		{
			sections.clear( );
			inimap.clear( );
		}

		/**
		* @brief GetSections returns the section names the ini file
		* @return std vector of sections as strings. Case is as is in inifile.
		*/
		std::vector<std::string> GetSections ( )
		{
			std::vector<std::string> s;

			for ( std::map<std::string, std::vector<std::string> >::iterator it
				= sections.begin( );
				it != sections.end( ); it++ )
			{
				s.push_back(( *it ).first);
			}
			return s;
		}

		/**
		* @brief getkeys returns the keys in a section
		* @param section name of the section
		* @return std vector of keys as strings
		*/
		std::vector<std::string> getkeys (std::string section)
		{
			std::vector<std::string> dummy;
			SectionMap::iterator     it = FindSection(section); // sections.find(section.c_str( ));

			if ( it != sections.end( ) )
			{
				return sections[section.c_str( )];    
			}
			return dummy;
		}

		/**
		* @brief getmap returns the keys/value pairs in a section
		* @param section name of the section
		* @return std map with key and values as strings
		*/
		std::map<std::string, std::string> getmap (std::string section)
		{
			std::map<std::string, std::string> mapping;

			if ( sections.end( ) == FindSection(section) )
			{
				return mapping;
			}
			std::vector<std::string> keys = sections[section];

			for ( size_t i = 0; i < keys.size( ); i++ )
			{
				mapping[keys[i]] = GetSymbolValue<std::string>(section + "." + keys[i]);
			}
			return mapping;
		}

		/**
		* @brief load parses key/value pairs by section from a file
		* @param filename path of file to parse
		* @return  whether or not this operation was successful true if sucessful,
		* false if failed
		*/
		bool load (const std::string filename)
		{
			std::string section;
			std::string line;
			std::string comment   = "#";
			std::string delimiter = "=";

			std::ifstream file(filename.c_str( ));

			if ( !file.is_open( ) )
			{
				return false;
			}

			while ( !file.eof( ) )
			{
				getline(file, line);

				// Remove any comments
				size_t commIdx = line.find(comment);

				if ( commIdx != std::string::npos )
				{
					line = line.substr(0, commIdx);
				}

				// This should only match [section], not a=b[3]

				line=trim(line);

				if ( line.size( ) < 1 )
				{
					continue;
				}
				size_t delimIdx = line.find("[");

				if ( ( line.find("[") ==
					0 ) )                               // && (line.rfind("]")==(line.size()-1)) ) // << BUG  here
				{
					line    = trim(line);
					line    = line.erase(line.find("]"));
					line    = line.erase(0, line.find("[") + 1);
					section = trim(line);
					continue;
				}

				delimIdx = line.find(delimiter);

				if ( delimIdx == std::string::npos )
				{
					continue;
				}

				std::string key   = trim(line.substr(0, delimIdx));
				std::string value = trim(line.substr(delimIdx + 1));
				sections[section].push_back(key);

				if ( !key.empty( ) )
				{
					if ( !section.empty( ) )
					{
						key = section + "." + key;
					}
					inimap[key] = value;
				}
			}

			file.close( );

			return true;
		}


		/**
		* @brief GetTokens returns a list of tokens associated with a key
		* @param keyName string containing keyname (uses section.keyname).
		* @param delimiter token used to tokenize values into vector
		* @return  vector of values from tokenizing
		*/
		std::vector<std::string> GetTokens (std::string keyName,
			std::string delimiter)
		{
			std::string              value  = GetSymbolValue<std::string>(keyName, "");
			std::vector<std::string> tokens = TrimmedTokenize(value, delimiter);

			for ( size_t i = 0; i < tokens.size( ); i++ )
			{
				if ( tokens[i].empty( ) )
				{
					tokens.erase(tokens.begin( ) + i);
					i--;
				}
				Trim(tokens[i]);
			}
			return tokens;
		}

		/**
		* @brief GetSymbolValue looks up a keys value in a section.
		* Assume tree by separating section and key name by period.
		* @param keyName string containing keyname (uses section.keyname).
		* @param szDefault default value associated with key name, if not found
		* @return  variant containing data
		*/
		template<typename T>
		T GetSymbolValue (std::string   keyName,
			StringVariant szDefault = StringVariant( ))
		{
			std::map<std::string, StringVariant>::const_iterator iter;
			#ifdef CASE_SENSITIVE
			            iter = inimap.find(keyName);
			#else
						// Key is now uppercase
						std::transform(keyName.begin( ), keyName.end( ), keyName.begin( ), toupper);
			
						for(iter=inimap.begin(); iter!=inimap.end(); iter++)
						{
							std::string key = (*iter).first;
							std::transform(key.begin( ), key.end( ), key.begin( ), toupper);
							if(key==keyName)
								break;
						}
			#endif
			if ( iter != inimap.end( ) )
			{
				return iter->second.convert<T>();
			}
			return szDefault.convert<T>();
		}

		/**
		* @brief IsSymbol determines if keyname (section.keyname) is in ini file.
		* @param keyName string containing keyname (uses section.keyname).
		* @return true if found, false if not.
		*/
	/*	bool IsSymbol (std::string keyName)
		{
			std::map<std::string, StringVariant>::const_iterator iter;
			iter = inimap.find_if(keyName, string_icompare);
			if ( iter != inimap.end( ) )
			{
				return true;
			}
			return false;
		}*/

		/////////////////////////////////////////////////////////////
		std::map<std::string, StringVariant>
			inimap;                                            //!< map of full keynames and values
	private:
		SectionMap sections;                               //!< map of each section and its string
		bool bCaseSensitive;

		
		/**
		* @brief trim Utility function to trim whitespace off the ends of a string
		* @param source input strings
		* @param delims string containing delimiters
		* @return trimmed string
		*/
		std::string trim (std::string source, std::string delims = " \t\r\n")
		{
			std::string result = source.erase(source.find_last_not_of(delims) + 1);
			return result.erase(0, result.find_first_not_of(delims));
		}

		/**
		* @brief LeftTrim trims leading spaces
		* @param str std string that is trimmed
		* @return trimmed string
		*/
		std::string & LeftTrim (std::string & str)
		{
			size_t startpos = str.find_first_not_of(" \t\r\n");

			if ( std::string::npos != startpos )
			{
				str = str.substr(startpos);
			}
			return str;
		}

		/**
		* @brief RightTrim trims trailing spaces
		* @param str std string that is trimmed
		* @return trimmed string
		*/
		std::string & RightTrim (std::string & str, std::string trim = " \t\r\n")
		{
			size_t endpos = str.find_last_not_of(trim);

			if ( std::string::npos != endpos )
			{
				str = str.substr(0, endpos + 1);
			}
			return str;
		}

		/**
		* @brief Trim trims leading and trailings spaces
		* @param str std string that is trimmed
		* @return trimmed string
		*/
		std::string & Trim (std::string & s) { return LeftTrim(RightTrim(s)); }

		/**
		* @brief Tokenize takes a string and delimiters and parses into vector
		* @param str string to tokenize
		* @param delimiters string containing delimiters
		* @return  std vector of tokens from parsed string
		*/
		std::vector<std::string> Tokenize (const std::string & str,
			const std::string & delimiters)
		{
			std::vector<std::string> tokens;
			std::string::size_type   delimPos = 0, tokenPos = 0, pos = 0;

			if ( str.length( ) < 1 )
			{
				return tokens;
			}

			while ( 1 )
			{
				delimPos = str.find_first_of(delimiters, pos);
				tokenPos = str.find_first_not_of(delimiters, pos);

				if ( std::string::npos != delimPos )
				{
					if ( std::string::npos != tokenPos )
					{
						if ( tokenPos < delimPos )
						{
							tokens.push_back(str.substr(pos, delimPos - pos));
						}
						else
						{
							tokens.push_back("");
						}
					}
					else
					{
						tokens.push_back("");
					}
					pos = delimPos + 1;
				}
				else
				{
					if ( std::string::npos != tokenPos )
					{
						tokens.push_back(str.substr(pos));
					}
					else
					{
						tokens.push_back("");
					}
					break;
				}
			}

			return tokens;
		}

		/**
		* @brief TrimmedTokenize takes a string and delimiters and parses into
		* vector,
		* but trims tokens of leading and trailing spaces before saving
		* @param str string to tokenize
		* @param delimiters string containing delimiters
		* @return  std vector of tokens from parsed string trimmed
		*  tokens of leading and trailing spaces
		*/
		std::vector<std::string> TrimmedTokenize (std::string value,
			std::string delimiter)
		{
			std::vector<std::string> tokens = Tokenize(value, delimiter);

			for ( size_t i = 0; i < tokens.size( ); i++ )
			{
				if ( tokens[i].empty( ) )
				{
					tokens.erase(tokens.begin( ) + i);
					i--;
					continue;
				}
				tokens[i] = Trim(tokens[i]);
			}
			return tokens;
		}
		std::string MakeUpper (std::string &str)
		{
			std::transform(str.begin( ), str.end( ), str.begin( ), toupper);
			return str;
		}
	};
}

#endif
