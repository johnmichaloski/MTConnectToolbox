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
typedef std::vector<uint8_t>                              raw_message_t;

using boost::asio::ip::tcp;
typedef boost::shared_ptr<boost::asio::ip::tcp::socket>   socket_ptr;

/**
* \brief Background thread to accept connections to read shdr data.
*/
class SocketBackEnd
{
public:
    SocketBackEnd( );

    /**
     * @brief HandleAsyncAccept is asio callback to accept replay connection and start thread
     * to service connection.
     * @param error if connection had problem.
     */
    void HandleAsyncAccept (const boost::system::error_code & error);

    /**
     * @brief StartAsyncAccept starts listening for aychronous connection.
     */
    void StartAsyncAccept ( );

    /**
     * @brief StopAsyncAccept stops listening for aychronous connection.
     */
    void StopAsyncAccept ( );

    /**
     * @brief Init sets up asio for asynchronous connection and communication.
     * @param domain ip address to listen on
     * @param portnumber  port number ot listen for connection.
     */
    void Init (std::string domain, LONG portnumber);

    /**
     * @brief StoreSocketString raw array to send to all clients.
     * @param str raw byte array to send to listening clients.
     */
    void StoreSocketString (raw_message_t str);

    /**
     * @brief Quit stop all the communication.
     */
    void Quit (void);

    /**
     * @brief Reset changes flag to not inited.
     */
    void Reset (void);

    /**
     * @brief Ip return TCP/IP host that communication occurs on.
     * @return host name as ipv4 address.
     */
    std::string & Ip ( ) { return _domainname; }

    /**
     * @brief Port return TCP/IP port number that communication occurs on.
     * @return  port number as integer
     */
    UINT & Port ( ) { return _ipport; }

    /**
     * @brief Count number of communication connections listening to replay.
     * @return integer count.
     */
    int & Count ( ) { return nCount; }

    /**
     * @brief session is a separate communication thread to each listening client.
     * This thread sends raw messages to the listener.
     * @param sock pointer to the socket that is listening.
     */
    void session (socket_ptr sock);

    /**
     * @brief server thread that listens for connection.
     * @param io_service asio service that handles asychronous communication
     * @param port TCP/IP port number to listen for connections.
     */
    void server (boost::asio::io_service & io_service, short port);

    /////////////////////////////////////////////////////////////////////=
protected:
    static int nCount;                                     /**< count of connections */
    static bool bRunning;                                  /**< thread(s) listening and connected running flag */
    size_t _nbuffers;                                      /**< n of last buffer string */
    raw_message_t _buffer;                                 /**< latest */
    bool _bInited;                                         /**< class instance inited flag */
    std::vector<raw_message_t> _all_bufferssofar;          /**< all raw messages sent so far */
    tcp::acceptor *_pAcceptor;                             /**<  boost asio TCP acceptor type */
    socket_ptr _pSocket;                                   /**< thread socket to client */
    std::string _portnumber;                               /**< striong of TCP/IP socket port number */
    std::string _domainname;                               /**< TCP/IP ip or domain name */
    UINT _ipport;                                          /**<  inteter TCP/IP socket port number */
};
