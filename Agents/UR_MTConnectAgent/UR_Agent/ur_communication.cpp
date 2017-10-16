/*
* ur_communication.cpp
*
* Copyright 2015 Thomas Timm Andersen
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "ur_communication.h"
#include "UR_Adapter.h"

#ifdef _DEBUG
#define print_info(X)       OutputDebugString(X)
#define print_fatal(X)      OutputDebugString(X)
#define print_warning(X)    OutputDebugString(X)
#define print_debug(X)      OutputDebugString(X)
#else
#define print_info(X)
#define print_fatal(X)
#define print_warning(X)
#define print_debug(X)
#endif

std::string WhatIsWSAError (int code)
{
    switch ( code )
    {
    case 0:
        {
            return "";
        }

    case WSANOTINITIALISED:
        {
            return "A successful WSAStartup call must occur before using this "
                   "function.";
        }

    case WSAENETDOWN:
        {
            return "The network subsystem has failed.";
        }

    case WSAEFAULT:
        {
            return "The buffer pointed to by the optval parameter is not in a valid "
                   "part of the process address space or the optlen parameter is too "
                   "small.";
        }

    case WSAEINPROGRESS:
        {
            return "A blocking Windows Sockets 1.1 call is in progress, or the service "
                   "provider is still processing a callback function.";
        }

    case WSAEINVAL:
        {
            return "The level parameter is not valid, or the information in the buffer "
                   "pointed to by the optval parameter is not valid.";
        }

    case WSAENETRESET:
        {
            return "The connection has timed out when SO_KEEPALIVE is set.";
        }

    case WSAENOPROTOOPT:
        {
            return "The option is unknown or unsupported for the specified provider or "
                   "socket (see SO_GROUP_PRIORITY limitations: ";
        }

    case WSAENOTCONN:
        {
            return "The connection has been reset when SO_KEEPALIVE is set.";
        }

    case WSAENOTSOCK:
        {
            return "The descriptor is not a socket.";
        }

    case WSAEINTR:
        {
            return "The (blocking:  call was canceled via WSACancelBlockingCall(: ";
        }

    case WSAEBADF:
        {
            return "The socket descriptor is not valid.";

            //	case WSAEFAULT:  return "An invalid argument was supplied to the Windows
            // Sockets API.";
            //	case WSAEINVAL:  return "An invalid call was made to the Windows Sockets
            // API.";
        }

    case WSAEMFILE:
        {
            return "No more file descriptors are available.";
        }

    case WSAEWOULDBLOCK:
        {
            return "The socket is marked as non-blocking and no connections are "
                   "present to be accepted.";

            //	case WSAEINPROGRESS:  return "A blocking Windows Sockets call is in
            // progress.";
        }

    case WSAEALREADY:
        {
            return "The asynchronous routine being canceled has already completed.";

            //	case WSAENOTSOCK:  return "The descriptor is not a socket.";
        }

    case WSAEDESTADDRREQ:
        {
            return "A destination address is required.";
        }

    case WSAEMSGSIZE:
        {
            return "The datagram was too large to fit into the specified buffer and "
                   "was truncated.";
        }

    case WSAEPROTOTYPE:
        {
            return "The specified protocol is the wrong type for this socket.";

            //	case WSAENOPROTOOPT:  return "The option is unknown or unsupported.";
        }

    case WSAEPROTONOSUPPORT:
        {
            return "The specified protocol is not supported.";
        }

    case WSAESOCKTNOSUPPORT:
        {
            return "The specified socket type is not supported in this address family.";
        }

    case WSAEOPNOTSUPP:
        {
            return "The referenced socket is not a type that supports "
                   "connection-oriented service.";
        }

    case WSAEPFNOSUPPORT:
        {
            return "WSAEPFNOSUPPORT ";
        }

    case WSAEAFNOSUPPORT:
        {
            return "The specified address family is not supported by this protocol.";
        }

    case WSAEADDRINUSE:
        {
            return "The specified address is already in use.";
        }

    case WSAEADDRNOTAVAIL:
        {
            return "The specified address is not available from the local machine.";

            //	case WSAENETDOWN:  return "The Windows Sockets implementation has
            // detected that the network subsystem has failed. Usually displayed by Glink
            // as 'NET FAILURE'.";
        }

    case WSAENETUNREACH:
        {
            return "The network address can't be reached from this host. There is "
                   "probably a problem in the way you have set up TCP/IP routing for "
                   "your PC most likely you have not defined a default router: ";

            //	case WSAENETRESET:  return "The connection must be reset because the
            // Windows Sockets implementation dropped it.";
        }

    case WSAECONNABORTED:
        {
            return "The connection has been closed.";
        }

    case WSAECONNRESET:
        {
            return "  ";
        }

    case WSAENOBUFS:
        {
            return "Not enough buffers available, or too many connections.";
        }

    case WSAEISCONN:
        {
            return "The socket is already connected.";

            //	case WSAENOTCONN:  return "The socket is not connected.";
        }

    case WSAESHUTDOWN:
        {
            return "The socket has been shutdown";
        }

    case WSAETOOMANYREFS:
        {
            return "WSAETOOMANYREFS ";
        }

    case WSAETIMEDOUT:
        {
            return "Attempt to connect timed out without establishing a connection. "
                   "Usually displayed by Glink as 'Connection to host timed out'.";
        }

    case WSAECONNREFUSED:
        {
            return "The attempt to connect was forcefully rejected. Usually displayed "
                   "by Glink as 'Connection refused by host'.";
        }

    case WSAELOOP:
        {
            return "Too many symbolic links were encountered in translating the path "
                   "name.";
        }

    case WSAENAMETOOLONG:
        {
            return "WSAENAMETOOLONG ";
        }

    case WSAEHOSTDOWN:
        {
            return "The host machine is out of service. Usually displayed by Glink as "
                   "'Host machine is down'.";
        }

    case WSAEHOSTUNREACH:
        {
            return "The host machine is unreachable. Usually displayed by Glink as "
                   "'Host is unreachable'.";
        }

    case WSAENOTEMPTY:
        {
            return "WSAENOTEMPTY ";
        }

    case WSAEPROCLIM:
        {
            return "WSAEPROCLIM ";
        }

    case WSAEUSERS:
        {
            return "WSAEUSERS ";
        }

    case WSAEDQUOT:
        {
            return "WSAEDQUOT ";
        }

    case WSAESTALE:
        {
            return "WSAESTALE ";
        }

    case WSAEREMOTE:
        {
            return "WSAEREMOTE";
        }

    case WSASYSNOTREADY:
        {
            return "Indicates that the underlying network subsystem is not ready for "
                   "network communication.";
        }

    case WSAVERNOTSUPPORTED:
        {
            return "The version of Windows Sockets API support requested is not "
                   "provided by this particular Windows Sockets implementation.";

            //	case WSANOTINITIALISED:  return "A successful WSAStartup:  return " must
            // occur before using this API.";
        }

    case WSAHOST_NOT_FOUND:
        {
            return "Authoritative answer host not found. Usually displayed by Glink as "
                   "'Host not found''.";
        }

    case WSATRY_AGAIN:
        {
            return "Non-authoritative answer host not found, or SERVERFAIL. Usually "
                   "displayed by Glink as 'Server not responding'.";
        }

    case WSANO_RECOVERY:
        {
            return "Non-recoverable errors, FORMERR, REFUSED, NOTIMP. Usually "
                   "displayed by Glink as 'Server not responding'.";
        }

    case WSANO_DATA:
        {
            return "Valid name, no data record of requested type. Usually displayed by "
                   "Glink as 'Unknown host name'.";
        }

    default:
        return "Unknown code";
    }
}
ur_communication::ur_communication( ) { }
HRESULT ur_communication::init (UR_Adapter *ur_adapter, std::string host)
{
    mHost       = host;
    _ur_adapter = ur_adapter;                              // original code used conditional variable to wake

    mServer = gethostbyname(host.c_str( ));

    if ( mServer == NULL )
    {
        logError("ERROR, unknown host %s\n", mHost.c_str( ));
        return E_FAIL;
    }

    mConnected.set(false);
    mKeepalive = false;
    return S_OK;
}
ur_message_t ur_communication::primary_socket ( )
{
    SOCKET      server;
    SOCKADDR_IN addr;
    uint8_t     buf[512];
    int         bytes_read;

    server = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_addr.s_addr = inet_addr(mHost.c_str( ));
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(30001);

    if ( connect(server, (SOCKADDR *) &addr, sizeof( addr )) < 0 )
    {
        int errnum = WSAGetLastError( );
        logError("Error %x connecting to host %s\n", WhatIsWSAError(errnum).c_str( ),
                 mHost.c_str( ));
        return ur_message_t( );
    }

#ifdef _WINDOWS
    unsigned long ulValue = 1;

//  ioctlsocket (server, FIONBIO, &ulValue);
#else
    iValue  = fcntl(m_hSocket, F_GETFL);
    iValue |= O_NONBLOCK;
    fcntl(sec_sockfd_, F_SETFL, O_NONBLOCK);
#endif

    bytes_read = recv(server, (char *) buf, 512, 0);

    if ( bytes_read < 0 )
    {
        return ur_message_t( );
    }

#ifdef _WINDOWS

//	::Sleep(500);
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
#endif

    // windows uses closesocket not close
    closesocket(server);

    return ur_message_t(buf, buf + bytes_read);
}
bool ur_communication::start ( )
{
    mKeepalive  = true;
    mCommThread = ur_comm::thread(&ur_communication::run, this);
    return true;
}
void ur_communication::halt ( )
{
    mKeepalive = false;
    mCommThread.join( );
}
void ur_communication::run ( )
{
    SOCKET      server;
    SOCKADDR_IN addr;
    uint8_t     buf[2048];
    int         bytes_read;

    mConnected.set(false);
    mKeepalive = true;

    try
    {
        while ( mKeepalive )
        {
            ::Sleep(10);

            while ( mConnected.get() && mKeepalive )
            {
                ::Sleep(10);

                // No delay in read - skipping timeout
                unsigned long ulValue = 1;
                // ioctlsocket (server, FIONBIO, &ulValue);

                bytes_read = recv(server, (char *) buf, 2048, 0);

                if ( bytes_read > 0 )
                {
                    _ur_adapter->mUrQMsgs.AddMsgQueue(
                        ur_message_t(buf, buf + bytes_read));
                }
                else
                {
                    mConnected.set(false); // = false;
                    closesocket(server);
                }
            }

            if ( mKeepalive )
            {
                // reconnect
                server = socket(AF_INET, SOCK_STREAM, 0);

                addr.sin_addr.s_addr = inet_addr(mHost.c_str( ));
                addr.sin_family      = AF_INET;
                addr.sin_port        = htons(30002);

                if ( connect(server, (SOCKADDR *) &addr, sizeof( addr )) < 0 )
                {
                    int errnum = WSAGetLastError( );
                    logError("Error %x connecting to host %s\n",
                             WhatIsWSAError(errnum).c_str( ), mHost.c_str( ));
                }
                else
                {
                    mConnected.set(true);// = true;
                }
            }
        }
    }
    catch ( ... )
    {
        logError("ur_communication::run() Exception\n");
    }

// wait for some traffic so the UR socket doesn't die in version 3.1.
#ifdef _WINDOWS
    ::Sleep(500);
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
#endif
    closesocket(server);
}
