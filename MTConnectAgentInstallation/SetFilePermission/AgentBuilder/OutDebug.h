//
//http://209.85.165.104/search?q=cache:PcxdVldM-VEJ:69.10.233.10/KB/debug/debugout.aspx+cout+debugging+mfc+outputdebugstring&hl=en&ct=clnk&cd=8&gl=us
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.


#pragma once

#include <Windows.h>
#include <ostream>
#include <sstream>
#include <string>

#include <streambuf>
#include <fstream>
///#include  <iosfwd>

class Fstream
{
public:

	std::fstream fp;

	Fstream(const char * filename)
	{
		fp.open (filename, std::fstream::out );
	}

	~Fstream() {  fp.close();}
};

class basic_debugbuf : public std::streambuf
{
public:
	basic_debugbuf(int bufferSize=100) 
	{
		if (bufferSize)
		{
			char *ptr = new char[bufferSize];
			setp(ptr, ptr + bufferSize);
		}
		else
			setp(0, 0);
	}
	virtual ~basic_debugbuf() 
	{
		sync();
		delete[] pbase();
	}

    virtual void writeString(const std::string &str)
    {
        OutputDebugString(str.c_str());
    }

private:
    int	overflow(int c)
    {
        sync();

        if (c != EOF)
        {
            if (pbase() == epptr())
            {
                std::string temp;
                temp += char(c);
                writeString(temp);
            }
            else
                sputc(c);
        }

        return 0;
    }

    int	sync()
    {
        if (pbase() != pptr())
        {
            int len = int(pptr() - pbase());
            std::string temp(pbase(), len);
            writeString(temp);
			OutputDebugString(temp.c_str());
            setp(pbase(), epptr());
        }
        return 0;
    }
   
};
class DebugFileOut
	: public basic_debugbuf
{
public:
	Fstream _stream;
	DebugFileOut(const char * filename) : _stream (filename)
	{

	}
	virtual void writeString(const std::string &str)
	{
		 _stream.fp << str;
	}

};