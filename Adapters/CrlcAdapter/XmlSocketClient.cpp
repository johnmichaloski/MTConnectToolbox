//
// XmlSocketClient.cpp
// 

#include "XmlSocketClient.h"
#include <istream>
#include <boost/regex.hpp>
#include <boost/exception/all.hpp>

#include "LinuxGlobals.h"
#include "CrclInterface.h"


//http://youku.io/questions/1088470/how-to-read-from-boost-asio-streambuf

XMLClient::XMLClient(boost::asio::io_service* ioservice) :
_io_service(ioservice) {
    _bConnected = false;
    TagReset();
    _nMSServerConnectRetry = 2000;
    _bWaitConnect = false;
    _socket = socket_ptr(new boost::asio::ip::tcp::socket(*ioservice));
    _timer = deadline_timer_ptr(new boost::asio::deadline_timer(*ioservice,
            boost::posix_time::milliseconds(2000)));
    //_timer->async_wait(b1oost::bind(&XMLClient::wait_callback, this, _1, boost::ref(*_socket)));
}

void XMLClient::Init(std::string ipv4, std::string port) {
    _ipv4 = ipv4;
    _port = port;
    // FIXME: add inited flag, didnt want XMLClient pointer
}

void XMLClient::ClearMsgQueue() {
    boost::mutex::scoped_lock lock(m);
    xml_msgs.clear();
}

void XMLClient::AddMsgQueue(std::string msg) {
    boost::mutex::scoped_lock lock(m);
    xml_msgs.push_back(msg);
    if(xml_msgs.size()>=10)
        for(int i=10; i<xml_msgs.size(); i++)
            xml_msgs.pop_front();
}

std::string XMLClient::LatestMsgQueue() {
    boost::mutex::scoped_lock lock(m);
    std::string msg = xml_msgs.back();
    xml_msgs.clear();
    return msg;
}

size_t XMLClient::SizeMsgQueue() {
    boost::mutex::scoped_lock lock(m);
    return xml_msgs.size();
}

void XMLClient::HandleConnect(const boost::system::error_code& error) {
    boost::mutex::scoped_lock lock(m);
    _bWaitConnect = false;
    _bConnected = true;
    // The async_connect() function automatically opens the socket at the start
    // of the asynchronous operation. If the socket is closed at this time then
    // the timeout handler must have run first.
    if (!_socket->is_open()) {
        Disconnect();
        return;
    }

    if (error == boost::asio::error::connection_refused) {
        Globals::ErrorMessage("HandleConnect connect refused\n");
        Globals::Sleep(_nMSServerConnectRetry);
        Connect();
        return;
    } else if (error == boost::asio::error::already_started) {
        Globals::ErrorMessage("HandleConnect connect already_started\n");
        Globals::Sleep(_nMSServerConnectRetry);
        Connect();
        return;
    }
    // On error, return early.
    if (error) {
        std::stringstream s;
        s << "HandleConnect unknown error = (" << error << ") " << error.message() << std::endl;
        Globals::ErrorMessage(s.str());
        return;
    }

    StartAyncRead();
    SyncWrite(CrclInterface().CRCLInitCanonCmd());
}

void XMLClient::StopConnecting() {
    // not sure if this works
    _bConnected = false;
    _socket->cancel();
}

void XMLClient::SyncConnect() {
    tcp::resolver resolver(*_io_service);
    tcp::resolver::query query(_ipv4.c_str(), _port.c_str());
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    tcp::resolver::iterator end;
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end) {
        //_socket->close();
        _socket->connect(*endpoint_iterator++, error);
    }
    if (error)
        return; //  throw boost::system::system_error(error);

    //TimerReset();
    // _timer = deadline_timer_ptr(new boost::asio::deadline_timer(*_io_service,
    //        boost::posix_time::milliseconds(2000)));
    //  _timer->async_wait(boost::bind(&XMLClient::wait_callback, this, _1, boost::ref(*_socket)));
    SyncWrite(CrclInterface().CRCLInitCanonCmd());
    Globals::Sleep(1000);

    StartAyncRead();

    //boost::mutex::scoped_lock lock(m);
    _bConnected = true;
#if 0
    tcp::resolver resolver(_io_service);
    tcp::resolver::query query(_ipv4.c_str(), _port.c_str());
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    //boost::asio::connect(_socket, endpoint_iterator);
    _bWaitConnect = true;
    _socket.async_connect(*endpoint_iterator, boost::bind(&XMLClient::HandleConnect,
            this,
            boost::asio::placeholders::error));
#endif
    // Start the deadline actor. You will note that we're not setting any
    // particular deadline here. Instead, the connect and input actors will
    // update the deadline prior to each asynchronous operation.
    //deadline_.async_wait(boost::bind(&client::check_deadline, this));

}

void XMLClient::Connect() {

    tcp::resolver resolver(*_io_service);
    tcp::resolver::query query(_ipv4.c_str(), _port.c_str());
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    _bWaitConnect = true;
    _socket->async_connect(*endpoint_iterator, boost::bind(&XMLClient::HandleConnect,
            this,
            boost::asio::placeholders::error));
}

void XMLClient::Disconnect() {
    _bConnected = false;
    _timer->cancel();
    _socket->close();
    Connect(); // start infinite wait for connection
}

bool XMLClient::IsConnected() {
    // Does not mean that server is listening.
    //return _socket.is_open();  // socket created
    return _bConnected;
}

std::string XMLClient::makeString(boost::asio::streambuf& streambuf, std::size_t n) {
    return std::string(buffers_begin(streambuf.data()),
            buffers_begin(streambuf.data()) + n);
}

void XMLClient::wait_callback(const boost::system::error_code& error, socket_ptr _socket) {
    Globals::ErrorMessage("wait_callback");

    if (error == boost::asio::error::operation_aborted) {
        // Data was read and this timeout was canceled
        return;
    } else if (error) {
        std::cout << "read_timeout Error - " << error.message() << std::endl;
        // Data was read and this timeout was canceled
        return;
    }
    _socket->cancel(); // will cause read_callback to fire with an timeout error
}

void XMLClient::TimerReset() {
    // Setup a deadline time to implement our timeout.
    boost::system::error_code ec;
    int n = _timer->expires_from_now(boost::posix_time::milliseconds(2000), ec);
    Globals::ErrorMessage(Globals::StrFormat("Timer reset %d Error code=0x%x\n", n, ec));
    _timer->async_wait(boost::bind(&XMLClient::wait_callback, this, _1, _socket));
}

void XMLClient::StartAyncRead() {
    try {
        Globals::ErrorMessage("StartAyncRead\n");
        if (!_socket->is_open()) {
            raise(SIGTRAP);
        }
        TimerReset();
        boost::asio::async_read(*_socket,
                boost::asio::buffer(data_, max_length), // buff, //boost::asio::buffer(&readBuffer[0], readBuffer.size()),
                boost::asio::transfer_at_least(1),
                boost::bind(&XMLClient::bytesToRead,
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        std::string statcmd = CrclInterface().CRCLGetStatusCmd();
        SyncWrite(statcmd);
        Globals::Sleep(1000);
    } catch (boost::exception & ex) {
        //http://theboostcpplibraries.com/boost.exception
        std::cerr << boost::diagnostic_information(ex);
        _socket->close();
    } catch (...) {
        _socket->close();
    }
}

std::string XMLClient::FindLeadingElement(std::string xml) {

    boost::match_results<std::string::const_iterator> matchResult;
    bool found;
    boost::regex e("<[A-Za-z0-9_]+");
    found = boost::regex_search(xml, matchResult, e);
    if (found) {
        std::string elem(matchResult[0]);
        elem.insert(1, 1, '/');
        elem = Globals::Trim(elem);
        elem.append(">"); // not space
        return elem;
    }
    return "";
}

void XMLClient::AppendBuffer(std::string read) {
    if (_next.size() > 0) {
        _current.append(_next);
        _next.clear();
    }
    size_t oldsize = _current.size();
    _current.append(read);
}

bool XMLClient::BufferHandler(std::string endtag) {
    std::size_t found;
    Globals::ErrorMessage(_current);
    if ((found = _current.find(endtag)) != std::string::npos) {
        found = found + endtag.size();
        _next = _current.substr(found);
        _current = _current.substr(0, found);
        AddMsgQueue(_current);
        _current.clear();
        TagReset();
        ;
        return true;
    }
    return false;
}

size_t XMLClient::bytesToRead(const error_code& error, size_t bytes_read) {
    size_t result = bytes_read;
    _timer->cancel();

    // Connection closed cleanly by peer
    // asio errors http://www.boost.org/doc/libs/1_44_0/doc/html/boost_asio/reference/error__basic_errors.html 
    if (error == boost::asio::error::eof || boost::asio::error::connection_reset == error) {
        Disconnect();
        return 0;
    } else if (error == boost::asio::error::operation_aborted) {
        Globals::ErrorMessage(" In bytesToRead Timer expired error \n");
    } else if (error) {
        // This error stops asynchronous reads
        std::stringstream s;
        s << "unknown bytesToRead error = (" << error << ") " << error.message() << std::endl;
        Globals::ErrorMessage(s.str());
        return 0;
    }

    if (bytes_read > 0) {
        AppendBuffer(std::string(data_, data_ + bytes_read));
        if (_endtag == NonsenseTag()) {
            _endtag = FindLeadingElement(_current);
            if (_endtag.empty()) _endtag = NonsenseTag();
        }
        //BufferHandler(std::string(data_, data_+bytes_read), "</CRCLStatus>") ;
        BufferHandler(_endtag);
    }
    StartAyncRead();
    return result;
}

void XMLClient::SyncWrite(std::string str) {
    try {
        _socket->send(boost::asio::buffer(str, str.size()));
    } catch (boost::exception & ex) {
        //http://theboostcpplibraries.com/boost.exception
        Globals::ErrorMessage("SyncWrite Exception\n");
        std::cerr << boost::diagnostic_information(ex);
        Disconnect();
    } catch (...) {
        Globals::ErrorMessage("SyncWrite Exception\n");
        Disconnect();
    }
}
//http://stackoverflow.com/questions/291871/how-to-set-a-timeout-on-blocking-sockets-in-boost-asio

