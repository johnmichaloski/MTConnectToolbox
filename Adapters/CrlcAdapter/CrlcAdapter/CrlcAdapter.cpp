//
// CrlcAdapter.cpp : Defines the entry point for the console application.
//

#include "targetver.h"

#include <cstdlib>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "internal.hpp"
#include "server.hpp"
#include "string_buffer.hpp"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>


#include "LinuxGlobals.h"
#include "fake_adapter.hpp"


#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "Userenv.lib")

#if defined(WIN64) && defined( _DEBUG) 
#pragma message( "DEBUG x64" )
#pragma comment(lib, "libboost_system-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_thread-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "xerces-c_3")
#elif !defined( _DEBUG) && defined(WIN64)
#pragma message( "RELEASE x64" )
#pragma comment(lib, "libxml2_64.lib")
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")
#pragma comment(lib, "xerces-c_3D")

#elif defined(_DEBUG) && defined(WIN32)
#pragma message( "DEBUG x32" )
#pragma comment(lib, "libboost_thread-vc100-mt-sgd-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-sgd-1_54.lib")

#elif !defined( _DEBUG) && defined(WIN32)
#pragma message( "RELEASE x32" )
#pragma comment(lib, "libboost_thread-vc100-mt-s-1_54.lib")
#pragma comment(lib, "libboost_system-vc100-mt-s-1_54.lib")

#endif

int main(int argc, char* argv[])
{
	argv[argc] = "debug";
	argc++;

	xercesc::XMLPlatformUtils::Initialize();
	FakeAdapter *adapter = new FakeAdapter(7878);
	adapter->setName("CRCL MTConnect Adapter");

	boost::thread t(boost::bind(&FakeAdapter::main, adapter, argc, (const char **) argv));
	boost::asio::io_service *ios = adapter->GetIos();
	adapter->SocketInit("129.6.32.176","64444");
	adapter->connect();
	while (1) {
		boost::this_thread::yield();
		//ios->run();
		Globals::Sleep(100);
		ios->run_one();
	}
	t.join();

	xercesc::XMLPlatformUtils::Terminate();
	return 0;

}

