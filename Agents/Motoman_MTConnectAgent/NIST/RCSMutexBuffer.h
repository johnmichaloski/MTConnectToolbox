
//
// RCSMutexBuffer.h
//

/*
DISCLAIMER:
This software was produced by the National Institute of Standards
and Technology (NIST), an agency of the U.S. government, and by statute is
not subject to copyright in the United States.  Recipients of this software
assume all responsibility associated with its operation, modification,
maintenance, and subsequent redistribution.

See NIST Administration Manual 4.09.07 b and Appendix I.
*/

#ifndef __RCSMutexBuffer_h
#define __RCSMutexBuffer_h

#include <vector>
//#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>


namespace RCS
{
	class CMutexBuffer
	{
	public:

		CMutexBuffer() 
		{ 
		}
		size_t size()
		{
			boost::shared_lock< boost::shared_mutex > lock(mMutex);
			return buffer.size();
		}
		void append(std::string &str)
		{
			boost::unique_lock< boost::shared_mutex > lock(mMutex);
			buffer+=str;
		}
		void append(char * inbuf, size_t nchars)
		{
			boost::unique_lock< boost::shared_mutex > lock(mMutex);

			buffer.insert(buffer.size(), (const char*) inbuf, nchars);
		}
		//void append(std::vector<uint8_t> inbuf)
		//{
		//	boost::unique_lock< boost::shared_mutex > lock(mMutex);
		//	buffer.insert(buffer.size(), (const char*) &inbuf[0], inbuf.size());
		//}
		
		void append(std::vector<uint8_t> &buf)
		{
			boost::unique_lock< boost::shared_mutex > lock(mMutex);
			buffer.insert(buffer.begin(), buf.begin(), buf.begin()+buf.size());
		}
		void get(std::string &str)
		{
			boost::shared_lock< boost::shared_mutex > lock(mMutex);
			str.clear();
			str=buffer;
		}
		void get(std::vector<uint8_t> &buf)
		{
			boost::shared_lock< boost::shared_mutex > lock(mMutex);
			buf = std::vector<uint8_t>(buffer.begin(), buffer.begin()+buffer.size());
		}

		void substr(size_t n)
		{
			boost::unique_lock< boost::shared_mutex > lock(mMutex);
			buffer=buffer.substr(n);
		}
		void clear()
		{
			boost::unique_lock< boost::shared_mutex > lock(mMutex);
			buffer.clear();
		}
	protected:
		boost::shared_mutex  mMutex;
		std::string buffer;
	};

}
#endif