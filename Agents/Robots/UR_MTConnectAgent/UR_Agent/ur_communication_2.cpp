
//
// ur_communication_2.cpp
//
 

// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#include <SDKDDKVer.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "ur_communication.h"
#include "UR_Adapter.h"

#include "NIST/hexdump.h"

using boost::asio::ip::tcp;
static     boost::asio::io_service io_service;


ur_communication::ur_communication( ) { }
HRESULT ur_communication::init (UR_Adapter *ur_adapter, std::string host, std::string port)
{
    mHost       = host;
	mPort       = port;
    _ur_adapter = ur_adapter;                              // original code used conditional variable to wake

    mServer = gethostbyname(host.c_str( ));

    if ( mServer == NULL )
    {
        logError("ERROR, unknown host %s\n", mHost.c_str( ));
        return E_FAIL;
    }

    mConnected=false;
    //mConnected.set(false);
    mKeepalive.set(true);
    return S_OK;
}
ur_message_t ur_communication::primary_socket ( )
{
	assert(0);
    uint8_t     buf[512];
	buf[0]=0;

    return ur_message_t(buf, buf + 1);
}
bool ur_communication::start ( )
{
    mKeepalive.set(true);
    mCommThread = ur_comm::thread(&ur_communication::run, this);
    return true;
}
void ur_communication::halt ( )
{
    mKeepalive.set(false);
    mCommThread.join( );
}
void ur_communication::run ( )
{
    mConnected=false;
//     mConnected.set(false);
	 mKeepalive.set(true);
//	 uint8_t     buf[2048];
//	 int         bytes_read;
	 tcp::resolver::iterator endpoint_iterator;
	 // We use a boost::array to hold the received data. 
	 boost::array<char, 2048> buf;
	 boost::system::error_code error;

	// Convert the server name that was specified as a parameter to the application, to a TCP endpoint. 
    // To do this, we use an ip::tcp::resolver object.
    tcp::resolver resolver(io_service);
	//::SetThreadPriority(GetCurrentThread( ), BELOW_NORMAL_PRIORITY_CLASS);
 

	while(mKeepalive.get())
	{
		try 
		{
			tcp::socket socket(io_service);
			//if(!mConnected.get())
			if(!mConnected)
			{
				tcp::resolver::query query(tcp::v4(), mHost.c_str( ), mPort);  
				endpoint_iterator = resolver.resolve(query);
				// will hang here until connected
				boost::asio::connect(socket, endpoint_iterator);
			}
			mConnected=true;
			//			mConnected.set(true); 
			for (;mKeepalive.get();)
			{
#ifdef _DEBUG
				::Sleep(10);
#else
				::Sleep(50);
#endif

				// The boost::asio::buffer() function automatically determines 
				// the size of the array to help prevent buffer overruns.
				size_t len = socket.read_some(boost::asio::buffer(buf), error);
				logDebug("\read_some %s Len=%d\n", _ur_adapter->mDevice.c_str(), len);


				// When the server closes the connection, 
				// the ip::tcp::socket::read_some() function will exit with the boost::asio::error::eof error, 
				// which is how we know to exit the loop.
				if (error == boost::asio::error::eof)
					break; // Connection closed cleanly by peer.
				else if (error)
					throw boost::system::system_error(error); // Some other error.
				ur_message_t msg(buf.data(), buf.data() + len);
				//logDebug(Nist::HexDump (buf.data(), 32).c_str());
				_ur_adapter->mUrQMsgs.AddMsgQueue(msg);
			}
		}
		// handle any exceptions that may have been thrown.
		catch (std::exception& e)
		{
			logError("ur_communication::run() Exception=%s\n", e.what());
		}
		//mConnected.set(false); 
		mConnected=false;
	}

}
