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
#include "Logger.h"
using namespace Logging;

// SocketBackEnd
bool                    SocketBackEnd::bRunning = true;
int                     SocketBackEnd::nCount   = 0;
boost::asio::io_service _io_service;

SocketBackEnd::SocketBackEnd( )
{
    Reset( );
}
void SocketBackEnd::Reset ( )
{
    _bInited = false;
}
static int ReadLine (socket_ptr sock, std::string & tmp)
{
    boost::system::error_code ec;
    boost::asio::streambuf    response;

    try
    {
        SOCKET native_sock = sock->native( );
        int    result      = SOCKET_ERROR;

        if ( INVALID_SOCKET != native_sock )
        {
            DWORD tv = 2000;
            result = setsockopt(native_sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof( DWORD ));

            // result should be zero
        }
        size_t len = boost::asio::read_until(*sock, response, "\n", ec);

        if ( len == 0 )
        {
            return -1;
        }

        if ( ec )
        {
            throw boost::system::system_error(ec);         //
        }

        if ( ec == boost::asio::error::eof )
        {
            throw boost::system::system_error(ec);         //
        }
        std::istream response_stream(&response);
        getline(response_stream, tmp);
    }
    catch ( std::exception & e )
    {
        logError("ReadLine  Exception %s\n", e.what( ));
        return -1;
    }
    return tmp.size( );
}
static void trans_func (unsigned int u, EXCEPTION_POINTERS *pExp)
{
    logFatal(StrFormat("Socket Echo Adapter trans_func - Code = 0x%x\n",
                       pExp->ExceptionRecord->ExceptionCode).c_str( ));
    throw std::exception( );
}
void SocketBackEnd::session (socket_ptr sock)
{
    try
    {
        _set_se_translator(trans_func);                    // correct thread?

        for ( nCount++; bRunning; )
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(100)); // sleeping 100 millisecond!
            boost::system::error_code ec;

            boost::system::error_code error;
            std::string               tmp;

            // int n = ReadLine(sock, tmp); // n>=0 data...
            // std::cout<< "Received: " << tmp << std::endl;

            while ( _nbuffers < _all_bufferssofar.size( ) )
            {
                _buffer = _all_bufferssofar[_nbuffers];
                _nbuffers++;

                if ( _buffer.size( ) < 1 )
                {
                    continue;
                }

                boost::asio::write(*sock, boost::asio::buffer(&_buffer[0], _buffer.size( )));
                Timing::Sleep(50);

                if ( ec )
                {
                    throw boost::system::system_error(ec); //
                }

                if ( ec == boost::asio::error::eof )
                {
                    throw boost::system::system_error(ec); //
                }
            }
        }
    }
    catch ( std::exception & e )
    {
        logError("Exception in thread: %s\n", e.what( ));
    }
    catch ( ... )
    {
        logError("Exception in thread\n");
    }
    nCount--;
}
void SocketBackEnd::server (boost::asio::io_service & io_service, short port)
{
    try
    {
        _pAcceptor = new tcp::acceptor(io_service, tcp::endpoint(tcp::v4( ), port));
        _pAcceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        StartAsyncAccept( );

        // Thread will stop here...
    }
    catch ( ... )
    {
        std::cout << StrFormat("Fatal Error in SocketBackEnd::server\n");
    }
}
void SocketBackEnd::StopAsyncAccept ( )
{
    try
    {
        _pAcceptor->cancel( );
    }
    catch ( boost::system::system_error )
    { }
}
void SocketBackEnd::StartAsyncAccept ( )
{
    _pSocket = socket_ptr(new tcp::socket(_io_service));

    _pAcceptor->async_accept(*_pSocket, bind(&SocketBackEnd::HandleAsyncAccept, this, boost::asio::placeholders::error));
}
void SocketBackEnd::HandleAsyncAccept (const boost::system::error_code & error) // socket_ptr pSocket)
{
    if ( bRunning )
    {
        boost::thread t(boost::bind(&SocketBackEnd::session, this, _pSocket));

        if ( bRunning )
        {
            StartAsyncAccept( );
        }
    }
}
void SocketBackEnd::Init (std::string domain, LONG portnumber)
{
    _bInited    = true;
    _domainname = domain;
    _portnumber = portnumber;
    boost::thread t(boost::bind(&SocketBackEnd::server, this, boost::ref(_io_service), portnumber));
    _bInited = true;
    _all_bufferssofar.clear( );
    _nbuffers = 0;
}
void SocketBackEnd::Quit ( )
{
    StopAsyncAccept( );
    bRunning = false;

    for ( int i = 0; i < 5 && nCount; i++ )
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
    }
}
void SocketBackEnd::StoreSocketString (raw_message_t str)
{
    _buffer = str;
    _all_bufferssofar.push_back(str);
}
