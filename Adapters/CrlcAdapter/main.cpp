/* 
 * File:   main.cpp
 * Author: michalos
 *
 * Created on September 15, 2015, 8:46 AM
 */

#include <cstdlib>

using namespace std;
#include "internal.hpp"
#include "fake_adapter.hpp"
#include "server.hpp"
#include "string_buffer.hpp"
#include "LinuxGlobals.h"
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

/*
 * 
 */
int main(int argc, char** argv) {
    //std::string exe=  Globals::ExeDirectory();
#ifdef DEBUG
    argv[argc] = (char *) new char("debug");
    argc++;
#endif
    xercesc::XMLPlatformUtils::Initialize();
    //Globals::ErrorMessage("Hello World\n");
    //FakeAdapter *adapter = new FakeAdapter(ioservice,7878);
    FakeAdapter *adapter = new FakeAdapter(7878);
    adapter->setName("CRCL MTConnect Adapter");
    //boost::thread t(boost::bind(&1boost::asio::io_service::run, &ioservice, boost::asio::placeholders::error)); 
    boost::thread t(boost::bind(&FakeAdapter::main, adapter, argc, (const char **) argv));
#if 0
    boost::thread t1(boost::bind(&FakeAdapter::SocketConnectReadDeviceData, adapter));
    boost::asio::io_service *ios = adapter->GetIos();
    while (1)
    {
        ios->run();
        Globals::Sleep(100);
    }
#endif
    boost::asio::io_service *ios = adapter->GetIos();
    adapter->connect();
    while (1) {
        boost::this_thread::yield();
        //ios->run();
        Globals::Sleep(1000);
        ios->run_one();
    }

    //adapter->main(argc, (const char **) argv);
    t.join();

    xercesc::XMLPlatformUtils::Terminate();
    return 0;
}

