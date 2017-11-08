//
// ShdrBackEnd.h 
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


using boost::asio::ip::tcp;
typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;


/**
* \brief Background thread to accept connections to read shdr data.
*/
class  ShdrBackEnd 
{
public:

	ShdrBackEnd();
	void							HandleAsyncAccept(const boost::system::error_code& error);
	void							StartAsyncAccept();
	void							StopAsyncAccept();
	void 							Init(std::string domain, LONG portnumber, std::string devicename);
	void 							StoreShdrString(std::string str);
	void                            Quit(void);
	void                            Reset(void);
	std::string &                   Ip(){ return _domainname; }
	UINT &                          Port(){ return _ipport; }
	std::string &                   Device(){ return _deviceName; }
	int &                           Count(){ return nCount; }
	/////////////////////////////////////////////////////////////////////
	void							session(socket_ptr sock);
	void							server(boost::asio::io_service& io_service, short port);
	/////////////////////////////////////////////////////////////////////
	static boost::asio::io_service	_io_service;
	static int                       nCount;
	static bool                      bRunning;

protected:
	std::string                     _1stshdr; /**< 1st shdr string - does not have all items?!?! */
	std::string                     _shdr;    /**< latest */
	size_t                          _nshdrs; /**< last  shdr string */
	bool							_bInited;
	std::vector<std::string>		_allshdrsofar;
	// Shdr socket service
	
	tcp::acceptor *					_pAcceptor;
	socket_ptr						_pSocket; /**< thread socket to client */
	int                             aHeartbeatFreq;
	int                             aHeartbeatCnt;
	// Shdr parameters - device name, port, devices.xml filename
	std::string						_deviceName; /**< device name -unused */
	std::string						_portnumber; /**< striong of TCP/IP socket port number */
	std::string						_domainname; /**< TCP/IP ip or domain name */
	UINT							_ipport; /**<  inteter TCP/IP socket port number */


};
