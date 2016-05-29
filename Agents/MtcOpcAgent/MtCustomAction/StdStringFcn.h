//
// StdStringFcn.h
//
//
// StdStringFcn.h
//

#pragma once

#include <algorithm> 
#include <functional> 
#include <locale>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iosfwd>

#include <string>
#include <iomanip>
#include <vector>
#include <map>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

#ifdef UNICODE
typedef std::wstring  tstring;
typedef	std::wifstream tifstream;
typedef std::wstringstream  tstringstream;
typedef std::wstreambuf tstreambuf;
typedef std::wfstream tfstream;
typedef	std::wofstream tofstream;
#define t_str w_str
#else
typedef std::string  tstring;
typedef	std::ifstream tifstream;
typedef std::stringstream  tstringstream;
typedef std::streambuf tstreambuf;
typedef std::fstream tfstream;
#define t_str c_str
typedef	 std::ofstream tofstream;
#define A2T(X) X

#endif

inline void RedirectIOToConsole()
{
static const WORD MAX_CONSOLE_LINES = 500;
	int hConHandle;
	long lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;
	try {
	// allocate a console for this app, if none
	AllocConsole();
	// set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),	&coninfo);
	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),	coninfo.dwSize);
	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );
	// redirect unbuffered STDIN to the console
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );
	// redirect unbuffered STDERR to the console
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );
	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
	// point to console as well
	std::ios::sync_with_stdio();
	}
	catch(...) {}
}
inline std::string UrlEncode(const std::string& input)
{
    std::ostringstream ssOut;
    ssOut << std::setbase(16);
    for(std::string::const_iterator i = input.begin(); i != input.end(); ++i)
    {
        if(isalnum(*i))
            ssOut << *i;
        else
            ssOut << '%' << std::setw(2) << std::setfill('0') << ((unsigned int)(unsigned char)*i);
    }
    return ssOut.str();
} 

// std::string test = urldecode("http%3A%2F%2F");
inline std::string UrlDecode ( const std::string& str ) 
{
	std::string result;
	std::string::size_type i;
	for (i = 0; i < str.size(); ++i)
	{
		if (str[i] == '+')
		{
			result += ' ';
		}
		else if (str[i] == '%' && str.size() > i+2)
		{
			std::istringstream iss("0x" + str.substr(i+1,2));
			long h=0;
			if(iss>>std::hex>>h)
				result += (unsigned char) h;
			i += 2;
		}
		else
		{
			result += str[i];
		}
	}
	return result;
}
inline std::string & RemoveSpaces(std::string& str)
{
	str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
	return str;
}
inline std::string & RemoveChar(std::string& str, char c)
{
	std::string::iterator end_pos = std::remove(str.begin(), str.end(), c); 
	str.erase(end_pos, str.end()); 
	return str;
}

inline std::string StdStringFormat(const TCHAR* format, ...)
{
	va_list ap;
	va_start(ap, format);
	
	int m;
	int n= strlen(format) + 1028;
	std::string tmp(n,'0');	


	// Kind of a bogus way to insure that we don't
	// exceed the limit of our buffer
	while((m=_vsnprintf(&tmp[0], n-1, format, ap))<0)
	{
		n=n+1028;
		tmp.resize(n,'0');
	}
	va_end(ap);
	return tmp.substr(0,m);

}

//////////////////////////////////////////////////
// std::string Exensions
//////////////////////////////////////////////////

// Convert the string to lowercase
inline std::string MakeUpper(std::string str)
{
	std::transform(str.begin(),str.end(),str.begin(),toupper);
	return str;
}

inline std::string  MakeLower(std::string str)
{
	std::transform(str.begin(),str.end(),str.begin(),tolower);
	return str;
}

// trim from start
inline std::string LeftTrim(std::string str) 
{

	size_t startpos = str.find_first_not_of(" \t\r\n"); 
	if( std::string::npos != startpos )
		str = str.substr( startpos ); 
	return str;
}

// trim from end
inline std::string RightTrim(std::string str) 
{
 		size_t endpos = str.find_last_not_of(" \t\r\n"); 
		if(std::string::npos != endpos ) 
			str = str.substr( 0, endpos+1 ); 		 
		return str;
}

// trim from both ends
inline std::string Trim(std::string s) 
{
        return LeftTrim(RightTrim(s));
}

inline bool EndsWith (std::string const &fullString, std::string const &ending) 
{ 
    if (fullString.length() > ending.length()) { 
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending)); 
    } else { 
        return false; 
    } 
} 
inline std::string ExtractDirectory( const std::string& path )
  {
  return path.substr( 0, path.find_last_of( '\\' ) +1 );
  }
inline std::string ExtractFilename( const std::string& path )
  {
  return path.substr( path.find_last_of( '\\' ) +1 );
  }
inline std::string ChangeExtension( const std::string& path, const std::string& ext )
  {
  std::string filename = ExtractFilename( path );
  return ExtractDirectory( path ) +filename.substr( 0, filename.find_last_of( '.' ) ) +ext;
  }
inline std::string nowtimestamp()
{
  SYSTEMTIME st;
  GetSystemTime(&st);
  return  StdStringFormat("%4d-%02d-%02dT%02d:%02d:%02d.%04d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
}
inline std::string fileToText( const std::string &filename )
{
   std::ifstream fin(filename.c_str());
   return std::string((std::istreambuf_iterator<char>(fin)),std::istreambuf_iterator<char>());
}
inline void ReadFile(std::string filename, std::string & contents)
{
	std::ifstream in(filename.c_str()); 
	std::stringstream buffer; 
	buffer << in.rdbuf(); 
	contents= buffer.str();
} 
inline void WriteFile(std::string filename, std::string & contents)
{
	std::ofstream outFile(filename.c_str());
	//Write the string and its null terminator !!
	outFile << contents.c_str();
}

inline std::vector<std::string> &Split(const std::string &s, char delim, std::vector<std::string> &elems, bool bKeepEmpty=false) 
{ 
	std::stringstream ss(s); 
	std::string item; 
	while(std::getline(ss, item, delim)) { 
		if(item.empty() && !bKeepEmpty)
			continue;
		elems.push_back(item); 
	} 
	return elems; 
} 

inline bool CompareInsensitive(std::string &s1, std::string &s2)
{
	return stricmp(s1.c_str(), s2.c_str()) == 0;
}

inline std::vector<std::string> Split(const std::string &s, char delim) { 
	std::vector<std::string> elems; 
	return Split(s, delim, elems); 
} 

inline std::string ReplaceOnce(
							   std::string result, 
							   const std::string& replaceWhat, 
							   const std::string& replaceWithWhat)
{
	const int pos = result.find(replaceWhat);
	if (pos==-1) return result;
	result.replace(pos,replaceWhat.size(),replaceWithWhat);
	return result;
}


// usual one can have infinite loop (e.g., \r\n for \n
inline void ReplaceAll( tstring& tInput, tstring tFind, tstring tReplace ) 
{ 
	size_t uPos = 0; 
	size_t uFindLen = tFind.length(); 
	size_t uReplaceLen = tReplace.length();
	if( uFindLen == 0 )
	{
		return;
	}

	for( ;(uPos = tInput.find( tFind, uPos )) != tstring::npos; )
	{
		tInput.replace( uPos, uFindLen, tReplace );
		uPos += uReplaceLen;
	}

}

inline bool ReplacePattern(tstring & inStr, tstring oldBeginningStr, tstring oldEndingStr, tstring newStr)
{
	unsigned int i,j,k;

	// Sanity checks
	if(inStr.size()<1) return false;
	if(oldBeginningStr.size()<1) return false;
	if(oldEndingStr.size()<1) return false;

	// Setup up StringEx sizes
	int newCnt = newStr.size();  

	i=0; // Let find determine where to start.

	int cnt=0;
	for(;i<inStr.size();){
		if((j=inStr.find(oldBeginningStr,  i))<0)break;
		if((k=inStr.find(oldEndingStr, j))<0) break;
		inStr = inStr.substr(0,j)+ newStr + inStr.substr(k+1);
		i=j+newCnt;
		cnt++;
	}
	if(cnt>0) return true;
	return false;
}

typedef std::map<std::string, std::string>::iterator StrMapIterator;

inline std::string GetIniSection(std::string filename, std::string section)
{
	int n,m;
	std::string contents;
	ReadFile(filename, contents);
	if((n=contents.find("["+section +"]"))==std::string::npos)
		return "";
	contents=contents.substr(n+section.size()+2);
	if((m=contents.find("["))==std::string::npos)
		m=contents.size();
	return Trim(contents.substr(0, m-1));
}

inline std::map<std::string, std::string> ParseIniString(std::string s)
{	
	std::map<std::string, std::string> symbols;
	std::vector<std::string> lines = Split(s, '\n');
	for(int i=0; i< lines.size(); i++)
	{
		std::vector<std::string> tokens=Split(lines[i], '=');
		if(tokens.size()<2)
			continue;
		symbols[Trim(tokens[0])]=Trim(tokens[1]);
		
	}
	return symbols;

}


inline std::map<std::string, std::string> ParseIniSection(std::string filename, std::string section)
{
	std::string s=GetIniSection(filename, section);
	return ParseIniString(s);
}
inline std::string ExeDirectory()
{
	TCHAR buf[1000];
	GetModuleFileName(NULL, buf, 1000);
	std::string path(buf);
	path=path.substr( 0, path.find_last_of( '\\' ) +1 );
	return path;
}
inline std::string GetIniProfileString(std::string filename, std::string section, std::string key)
{
	std::map<std::string, std::string> symbols = ParseIniSection(filename, section);
	return symbols[key];
}

template<typename T>
inline T ConvertString(std::string data, T defaultval)
{
	T result;
	std::istringstream stream(data);

	if(stream >> result)
		return result;
	return defaultval;
}

template<typename T>
inline std::string ToString(T data)
{
	std::ostringstream stream;
	try{
	 stream << data;
	}
	catch(...){ return "";}
	return stream.str();;
}



// From: http://www.ccp4.ac.uk/ccp4bin/viewcvs/pointless/string_util.cpp?rev=1.1&content-type=text/vnd.viewcvs-markup

// Return string of length <fieldwidth> with text centred on position
// cenpos (numbered from 0)
#ifndef MAX
#define MAX(X,Y) ((X>Y) ?  X : Y)
#endif
#ifndef MIN
#define MIN(X,Y) ((X<Y) ?  X : Y)
#endif

inline std::string CenterString(const std::string& text, const int& fieldwidth,
			 const int& cenposition)
{
  int cenpos = cenposition;
  if (cenpos == 0) cenpos = fieldwidth/2;
  //  ASSERT (cenpos < fieldwidth);
  int len = text.size();
  if (len >= fieldwidth) return text;
  // number of trailing spaces
  int p2 = MAX(0,MIN(fieldwidth-1-(cenpos+len/2), fieldwidth-1)); 
  int p1 = fieldwidth - len - p2; // number of leading spaces
  std::string pad1;
  if (p1 > 0) pad1 = std::string(p1, ' ');
  std::string pad2;
  if (p2 > 0) {
    pad2 = std::string(p2, ' ');
  }
  return pad1+text+pad2;
}
// Return string of length Max(fieldwidth, Length(text))
inline std::string PadString(const std::string& text, const int& fieldwidth)
{
  int len = text.size();
  if (len >= fieldwidth) return text;
  std::string pad1((fieldwidth-len), ' ');
  return text+pad1;
}
// Return right-justified string
inline std::string RightString(const std::string& text, const int& fieldwidth)
{
  int len = text.size();
  if (len >= fieldwidth) return text;
  std::string pad1((fieldwidth-len), ' ');
  return pad1+text;
}
// Return left-justified string
inline std::string LeftString(const std::string& text, const int& fieldwidth)
{
  int len = text.size();
  if (len >= fieldwidth) return text;
  std::string pad1((fieldwidth-len), ' ');
  return text+pad1;
}
// <tag><data</tag>
inline std::string MakeXMLtag(const std::string& tag, const std::string& data)
{
  return "<"+tag+">"+data+"</"+tag+">";
}



#include <streambuf>
#include <ostream>

template <class cT, class traits = std::char_traits<cT> >
class basic_nullbuf: public std::basic_streambuf<cT, traits> {
	typename traits::int_type overflow(typename traits::int_type c)
	{
		return traits::not_eof(c); // indicate success
	}
};

template <class cT, class traits = std::char_traits<cT> >
class basic_onullstream: public std::basic_ostream<cT, traits> {
public:
	basic_onullstream():
	  std::basic_ios<cT, traits>(&m_sbuf),
		  std::basic_ostream<cT, traits>(&m_sbuf)
	  {
		  init(&m_sbuf);
	  }

private:
	basic_nullbuf<cT, traits> m_sbuf;
};

typedef basic_onullstream<char> onullstream;
typedef basic_onullstream<wchar_t> wonullstream;

// off the internet - who knows if it works...
inline std::vector<std::string> Tokenize(const std::string& str,const std::string& delimiters)
{
 std::vector<std::string> tokens;
 std::string::size_type delimPos = 0, tokenPos = 0, pos = 0;

 if(str.length()<1)  return tokens;
 while(1){
   delimPos = str.find_first_of(delimiters, pos);
   tokenPos = str.find_first_not_of(delimiters, pos);

   if(std::string::npos != delimPos){
     if(std::string::npos != tokenPos){
       if(tokenPos<delimPos){
         tokens.push_back(str.substr(pos,delimPos-pos));
       }else{
         tokens.push_back("");
       }
     }else{
       tokens.push_back("");
     }
     pos = delimPos+1;
   } else {
     if(std::string::npos != tokenPos){
       tokens.push_back(str.substr(pos));
     } else {
       tokens.push_back("");
     }
     break;
   }
 }
 return tokens;
}