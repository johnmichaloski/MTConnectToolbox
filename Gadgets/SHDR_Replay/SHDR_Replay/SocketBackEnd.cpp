//
// ComMTCSocketBackEnd.cpp : Implementation of SocketBackEnd
//

// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied 
// or intended.


#include "stdafx.h"
#include "SocketBackEnd.h"
#include "StdStringFcn.h"
#include <ctime>


// SocketBackEnd
bool SocketBackEnd::bRunning=true;
int SocketBackEnd::nCount=0;
boost::asio::io_service	 SocketBackEnd::_io_service;




SocketBackEnd::SocketBackEnd()
{
	Reset();
	aHeartbeatFreq = 10000;
}

void SocketBackEnd::Reset()
{
	_bInited=false;
}
static int ReadLine(socket_ptr sock, std::string &tmp)
{
	boost::system::error_code ec;
	boost::asio::streambuf response;

	try {
		SOCKET native_sock = sock->native();
		int result = SOCKET_ERROR;

		if (INVALID_SOCKET != native_sock)
		{
			DWORD tv=2000;
			result = setsockopt(native_sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv,sizeof(DWORD) );
			// result should be zero
		}
		size_t len = boost::asio::read_until(*sock, response, "\n", ec);
		if(len==0)
			return -1;

		if(ec)
			throw boost::system::system_error(ec); // 
		if (ec == boost::asio::error::eof)
			throw boost::system::system_error(ec); // 

		std::istream response_stream(&response);
		getline(response_stream, tmp);
	}
	catch(std::exception & e) 
	{ 
		std::cout<< "ReadLine  Exception " << e.what() << "\n";
		return -1;
	}
	return tmp.size();
}


static void trans_func( unsigned int u, EXCEPTION_POINTERS* pExp )
{
	OutputDebugString( StrFormat("Shdr Echo Adapter trans_func - Code = 0x%x\n",  
		pExp->ExceptionRecord->ExceptionCode).c_str() );
	throw std::exception();
}


void SocketBackEnd::session(socket_ptr sock)
{
	try
	{
		_set_se_translator( trans_func );  // correct thread?

#if 0
		if(_1stshdr.size()>0)
			boost::asio::write(*sock, boost::asio::buffer(_1stshdr.c_str(), _1stshdr.size()));
#endif
		// FIXME need mutex  - cant add while we are consuming
		_nshdrs=_allshdrsofar.size();
		for(size_t i=0; i< _allshdrsofar.size(); i++)
		{
			boost::asio::write(*sock, boost::asio::buffer(_allshdrsofar[i].c_str(), _allshdrsofar[i].size()));
			Timing::Sleep(50);
		}
		
		aHeartbeatCnt=aHeartbeatFreq;

		for (nCount++;bRunning;)
		{
			aHeartbeatCnt-=100;  // heartbeat countdown
			boost::this_thread::sleep(boost::posix_time::milliseconds(100)); // sleeping 100 millisecond!
			boost::system::error_code ec;

			boost::system::error_code error;
			std::string tmp;

#if 0
			if( aHeartbeatCnt < 0)
			{
				aHeartbeatCnt=aHeartbeatFreq;
				std::string mPong= StrFormat( "* PONG %d\n", aHeartbeatFreq);
				boost::asio::write(*sock, boost::asio::buffer(mPong.c_str(), mPong.size()));

			}
#endif
			int n = ReadLine(sock, tmp); // n>=0 data...
			if (strncmp(tmp.c_str(), "* PING", 6) == 0)
			{			
				std::string mPong= StrFormat( "* PONG %d\n", aHeartbeatFreq);
				boost::asio::write(*sock, boost::asio::buffer(mPong.c_str(), mPong.size()));
				Timing::Sleep(50);
			}
			else
			{
				std::string mPong= StrFormat( "* PONG %d\n", aHeartbeatFreq);
				boost::asio::write(*sock, boost::asio::buffer(mPong.c_str(), mPong.size()));
				Timing::Sleep(50);
			}

			std::cout<< "Received: " << tmp << std::endl;
			while(_nshdrs<_allshdrsofar.size())
			{
				_shdr=_allshdrsofar[_nshdrs];
				_nshdrs++;
				if(_shdr.size() < 1)
					continue;
				aHeartbeatCnt=aHeartbeatFreq;
				// See what's going out
				std::cout <<  _shdr << std::endl;

				boost::asio::write(*sock, boost::asio::buffer(_shdr.c_str(), _shdr.size()));
				Timing::Sleep(50);
				if(ec)
					throw boost::system::system_error(ec); // 
				if (ec == boost::asio::error::eof)
					throw boost::system::system_error(ec); // 	
			}

		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception in thread: " << e.what() << "\n";
	}
	catch (...)
	{
		std::cerr << "Exception in thread\n";
	}	
	nCount--;
}

void SocketBackEnd::server(boost::asio::io_service& io_service, short port)
{
	try
	{
		_pAcceptor =  new tcp::acceptor(io_service, tcp::endpoint(tcp::v4(), port)) ;
		_pAcceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		StartAsyncAccept();
		// Thread will stop here...

	}
	catch(...)
	{
		std::cout << StrFormat("Fatal Error in SocketBackEnd::server\n");
	}
}

void SocketBackEnd::StopAsyncAccept()
{
	try{
	_pAcceptor->cancel();
	}
	catch(boost::system::system_error)
	{

	}
}

void SocketBackEnd::StartAsyncAccept()
{
//	tcp_connection::pointer new_connection =
//		tcp_connection::create(m_pAcceptor->io_service());

	_pSocket= socket_ptr( new tcp::socket(_io_service));

	_pAcceptor->async_accept(*_pSocket, bind(&SocketBackEnd::HandleAsyncAccept, this, boost::asio::placeholders::error));   
}

void SocketBackEnd::HandleAsyncAccept(const boost::system::error_code& error) // socket_ptr pSocket)
{
	if(bRunning)
	{
		boost::thread t(boost::bind(&SocketBackEnd::session, this, _pSocket));
		if(bRunning)
			StartAsyncAccept();
	}
}

void SocketBackEnd::Init(std::string domain, LONG portnumber, std::string devicename)
{
	_bInited=true;
	_domainname=domain;
	_portnumber=portnumber;
	_deviceName=devicename;
	boost::thread t(boost::bind(&SocketBackEnd::server, this, boost::ref( _io_service), portnumber)); 
}

void SocketBackEnd::Quit()
{
	StopAsyncAccept();
	bRunning=false;
	for(int i=0; i< 5 && nCount; i++)
		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
}

void SocketBackEnd::StoreShdrString(std::string str)
{
	if(!_bInited)
	{
		_bInited=true;
		_1stshdr=str;
		_allshdrsofar.clear();
	}
	_shdr=str;
	_allshdrsofar.push_back(str);
}
