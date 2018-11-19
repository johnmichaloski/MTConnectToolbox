
//
// moto_communication_2.cpp
//


// DISCLAIMER:
// This software was developed by U.S. Government employees as part of
// their official duties and is not subject to copyright. No warranty implied
// or intended.

#include <SDKDDKVer.h>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "Moto_communication.h"
#include "Moto_Adapter.h"

#include "NIST/hexdump.h"
#include "NIST/Logger.h"  // exedirectory, strformat, 

using boost::asio::ip::tcp;
static     boost::asio::io_service io_service;


////////////////////////////////////////////////////////////////////////////////////////////////////////

moto_communication::moto_communication( ) 
{ 
	mbRawDump=false;
}
HRESULT moto_communication::init (Moto_Adapter *Moto_Adapter, std::string host, std::string port)
{
	mPort      = port;
	mHost       = host;
	_Moto_Adapter = Moto_Adapter;                              // original code used conditional variable to wake

	mServer = gethostbyname(host.c_str( ));

	if ( mServer == NULL )
	{
		logError("ERROR, unknown host %s\n", mHost.c_str( ));
		return E_FAIL;
	}

	mConnected=false;
	//mConnected.set(false);
	if(mbRawDump)
		rawSocketFile.open((Logging::CLogger::ExeDirectory() + "status_dump.txt").c_str( ), std::fstream::out, OF_SHARE_DENY_NONE);
	return S_OK;
}
moto_message_t moto_communication::primary_socket ( )
{
	assert(0);
	uint8_t     buf[512];
	buf[0]=0;

	return moto_message_t(buf, buf + 1);
}
bool moto_communication::start ( )
{
	mKeepalive.set(true);
	mCommThread = moto_comm::thread(&moto_communication::run, this);
	return true;
}
void moto_communication::halt ( )
{
	mKeepalive.set(false);
	mCommThread.join( );
}

void moto_communication::run ( )
{
	mConnected=false;
	mKeepalive.set(true);
	tcp::resolver::iterator endpoint_iterator;
	// We use a boost::array to hold the received data. 
	boost::array<char, 2048> buf;
	boost::system::error_code error;

	// Convert the server name that was specified as a parameter to the application, to a TCP endpoint. 
	// To do this, we use an ip::tcp::resolver object.
	tcp::resolver resolver(io_service);
	//::SetThreadPriority(GetCurrentThread( ), ABOVE_NORMAL_PRIORITY_CLASS);

	std::string buffer; // this is the communication buffer

	while(mKeepalive.get())
	{
		try 
		{
			tcp::socket socket(io_service);
			//if(!mConnected.get())
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

			for (;mKeepalive.get();)
			{
#ifdef _DEBUG
				::Sleep(10);
#else
				::Sleep(50);
#endif

				// The boost::asio::buffer() function automatically determines 
				// the size of the array to help prevent buffer overruns.

				//size_t len = socket.read_some(boost::asio::buffer(buf), error);
				size_t len = socket.read_some(boost::asio::buffer(buf,2048), error);
				logDebug("read_some %s Len=%d\n", _Moto_Adapter->mDevice.c_str(), len);


				// When the server closes the connection, 
				// the ip::tcp::socket::read_some() function will exit with the boost::asio::error::eof error, 
				// which is how we know to exit the loop.
				if (error == boost::asio::error::eof)
					//break; // Connection closed cleanly by peer.
					throw boost::system::system_error(error); // Some other error.
				else if (error)
					throw boost::system::system_error(error); // Some other error.
				// Save raw socket stream
				if(mbRawDump)
				{
					rawSocketFile << Logging::CLogger::Timestamp ( ) << ":";
					rawSocketFile <<  Nist::RawDump (&buffer[0], len, "%02X,");
				}

				buffer.insert(buffer.end(), buf.data(), buf.data() + len) ; 
				//logDebug(Nist::HexDump(&buffer[0], buffer.size(), 16).c_str());
				//logDebug(Nist::HexDump (&msg[0], 32).c_str());
				_Moto_Adapter-> mMotoData.mutex.lock();
				size_t n = _Moto_Adapter->mMotoData.unpack((uint8_t *) &buffer[0], buffer.size( ));
				_Moto_Adapter-> mMotoData.mutex.unlock();
				logDebug("Decode n=%d buffer size=%d\n", n, buffer.size());
				buffer=buffer.substr(n);
				//logDebug(Nist::HexDump(&buffer[0], buffer.size(), 16).c_str());

			}
		}
		// handle any exceptions that may have been thrown.
		catch (std::exception& e)
		{
			logError("moto_communication::run() Exception=%s\n", e.what());
			::Sleep(2000); // needs to rest to let listener come up
		}
		catch(...)
		{
			logError("moto_communication::run() Exception\n");

		}
		//mConnected.set(false); 
		mConnected=false;
	}

}
