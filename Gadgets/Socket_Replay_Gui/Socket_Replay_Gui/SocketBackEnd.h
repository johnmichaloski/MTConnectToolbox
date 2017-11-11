//
// SocketBackEnd.h 
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

#include <map>
#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

#include "Timing.h"
typedef std::vector<uint8_t> raw_message_t;


using boost::asio::ip::tcp;
typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;


/**
* \brief Background thread to accept connections to read shdr data.
*/
class  SocketBackEnd 
{
public:
	SocketBackEnd();
	void							HandleAsyncAccept(const boost::system::error_code& error);
	void							StartAsyncAccept();
	void							StopAsyncAccept();
	void 							Init(std::string domain, LONG portnumber);
	void 							StoreSocketString(raw_message_t str);
	void                            Quit(void);
	void                            Reset(void);
	std::string &                   Ip(){ return _domainname; }
	UINT &                          Port(){ return _ipport; }
//	std::string &                   Device(){ return _deviceName; }
	int &                           Count(){ return nCount; }
	/////////////////////////////////////////////////////////////////////
	void							session(socket_ptr sock);
	void							server(boost::asio::io_service& io_service, short port);
	/////////////////////////////////////////////////////////////////////
	static boost::asio::io_service	_io_service;
	static int                       nCount;
	static bool                      bRunning;

protected:
 	size_t                          _nbuffers; /**< n of last buffer string */
	raw_message_t                   _buffer;    /**< latest */
	bool							_bInited;
	std::vector<raw_message_t>		_all_bufferssofar;
	tcp::acceptor *					_pAcceptor;
	socket_ptr						_pSocket; /**< thread socket to client */
	std::string						_portnumber; /**< striong of TCP/IP socket port number */
	std::string						_domainname; /**< TCP/IP ip or domain name */
	UINT							_ipport; /**<  inteter TCP/IP socket port number */
};
