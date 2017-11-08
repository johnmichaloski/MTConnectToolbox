
//
// socket_communication_2.cpp
//


// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.
#include "stdafx.h"
#include <SDKDDKVer.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "socket_communication.h"

#include "hexdump.h"
#include "Logger.h"  // exedirectory, strformat, 

using boost::asio::ip::tcp;
static     boost::asio::io_service io_service;


////////////////////////////////////////////////////////////////////////////////////////////////////////

socket_communication::socket_communication( ) 
{ 
}
socket_communication::~socket_communication( ) 
{ 
	rawSocketFile.close();
}
HRESULT socket_communication::init (std::string host, std::string port, std::string filename)
{
	mPort       = port;
	mHost       = host;
	mFilename   = filename;

	mServer = gethostbyname(host.c_str( ));

	if ( mServer == NULL )
	{
		logError("ERROR, unknown host %s\n", mHost.c_str( ));
		return E_FAIL;
	}

	mConnected=false;
	rawSocketFile.open((Logging::CLogger::ExeDirectory() + filename).c_str( ), std::fstream::out, OF_SHARE_DENY_NONE);
	if(!rawSocketFile)
	{
		logError("ERROR, couldn't open %s\n", mFilename.c_str( ));
		return E_FAIL;
	}
	return S_OK;
}

bool socket_communication::start ( )
{
	mKeepalive=true;
	mCommThread = moto_comm::thread(&socket_communication::run, this);
	return true;
}
void socket_communication::halt ( )
{
	mKeepalive=false;;
	mCommThread.join( );
	rawSocketFile.close();
}
void socket_communication::run ( )
{
	mConnected=false;
	mKeepalive=true;
	tcp::resolver::iterator endpoint_iterator;
	// We use a boost::array to hold the received data. 
	boost::array<char, 2048> buf;
	boost::system::error_code error;

	// Convert the server name that was specified as a parameter to the application, to a TCP endpoint. 
	// To do this, we use an ip::tcp::resolver object.
	tcp::resolver resolver(io_service);
	//::SetThreadPriority(GetCurrentThread( ), ABOVE_NORMAL_PRIORITY_CLASS);

	std::string buffer; // this is the communication buffer

	while(mKeepalive)
	{
		try 
		{
			tcp::socket socket(io_service);
			if(!mConnected)
			{
				buffer.clear();
				tcp::resolver::query query(tcp::v4(), mHost.c_str( ), mPort.c_str( ));
				endpoint_iterator = resolver.resolve(query);
				// will hang here until connected - not reason to sleep
				boost::asio::connect(socket, endpoint_iterator);
			}
			// SIgnal to adapter that communication link is now connected 
			// turn off if raw dump of stream data
			mConnected=true;

			for (;mKeepalive;)
			{
				::Sleep(10);

				// The boost::asio::buffer() function automatically determines 
				// the size of the array to help prevent buffer overruns.
				size_t len = socket.read_some(boost::asio::buffer(buf), error);
				logDebug("read_some Len=%d\n", len);


				// When the server closes the connection, 
				// the ip::tcp::socket::read_some() function will exit with the boost::asio::error::eof error, 
				// which is how we know to exit the loop.
				if (error == boost::asio::error::eof)
					break; // Connection closed cleanly by peer.
				else if (error)
					throw boost::system::system_error(error); // Some other error.

				// Save raw socket stream
				if(len>0)
				{
					rawSocketFile << Logging::CLogger::Timestamp ( ) << "|";
					rawSocketFile <<  Nist::RawDump ((void *) &buf[0], len, "%02X,");
				}
			}
		}
		// handle any exceptions that may have been thrown.
		catch (std::exception& e)
		{
			logError("socket_communication::run() Exception=%s\n", e.what());
		}
		//mConnected.set(false); 
		mConnected=false;
	}

}
