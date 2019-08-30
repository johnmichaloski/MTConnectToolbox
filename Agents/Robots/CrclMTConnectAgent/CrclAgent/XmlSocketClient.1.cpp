//
// XmlClient.cpp
// 
#include "targetver.h"

#include "XmlSocketClient.h"
#include <istream>
#include <boost/regex.hpp>
#include <boost/exception/all.hpp>

#include "WinGlobals.h"
#include "CrclInterface.h"


//http://youku.io/questions/1088470/how-to-read-from-boost-asio-streambuf

XMLClient::XMLClient(boost::asio::io_service&  io_service) :
_io_service(io_service), 
	_socket( io_service), 
	_timer(io_service)	
{
	_bConnected=false;
	TagReset();
	_nMSServerConnectRetry=2000;
	_bWaitConnect=false;
}
void XMLClient::Init(std::string ipv4, std::string port)
{
	_ipv4=ipv4;
	_port=port;
	// FIXME: add inited flag, didnt want XMLClient pointer
}
void XMLClient::ClearMsgQueue()
{
	boost::mutex::scoped_lock lock(m);
	xml_msgs.clear();
}
void XMLClient::AddMsgQueue(std::string msg)
{
	boost::mutex::scoped_lock lock(m);
	xml_msgs.push_back(msg);
}
std::string XMLClient::LatestMsgQueue()
{
	boost::mutex::scoped_lock lock(m);
	std::string msg= xml_msgs.back();
	xml_msgs.clear();
	return msg;
}
size_t XMLClient::SizeMsgQueue()
{
	boost::mutex::scoped_lock lock(m);
	return xml_msgs.size();
}
void XMLClient::HandleConnect(const boost::system::error_code& error)
{
	boost::mutex::scoped_lock lock(m);
	_bWaitConnect=false;
	// The async_connect() function automatically opens the socket at the start
  // of the asynchronous operation. If the socket is closed at this time then
  // the timeout handler must have run first.
  if (!_socket.is_open()) 
	  return;

  if (error==boost::asio::error::connection_refused  )
  {
	  Globals::ErrorMessage("HandleConnect connect refused\n");
	  ::Sleep(_nMSServerConnectRetry);
	  Connect();
	  return;
  }
  else if (error==boost::asio::error::already_started  )
  {
	  Globals::ErrorMessage("HandleConnect connect already_started\n");
	  ::Sleep(_nMSServerConnectRetry);
	  Connect();
	  return;
  }
  // On error, return early.
  if (error) 
  {
	  std::stringstream s;
	  s << "HandleConnect unknown error = (" << error << ") " << error.message()  << std::endl;
	  Globals::ErrorMessage(s.str());
	  return;
  }

  _bConnected=true;
  StartAyncRead();
  SyncWrite(CrclInterface().CRCLInitCanonCmd());
}

void XMLClient::StopConnecting()
{
	// not sure if this works
	_bConnected=false;
	_socket.cancel();
}
void XMLClient::Connect()
{
	tcp::resolver resolver(_io_service);
	tcp::resolver::query query(_ipv4.c_str(), _port.c_str());
	tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
	//boost::asio::connect(_socket, endpoint_iterator);
	_bWaitConnect=true;
	_socket.async_connect(*endpoint_iterator, boost::bind(&XMLClient::HandleConnect, 
		this,
		boost::asio::placeholders::error ));

	
}
void XMLClient::Disconnect()
{
	_bConnected=false;
	_timer.cancel();
	_socket.close();
}
bool  XMLClient::IsConnected()
{
	// Does not mean that server is listening.
	//return _socket.is_open();  // socket created
	return _bConnected;
}

std::string XMLClient::makeString(boost::asio::streambuf& streambuf, std::size_t n)
{
 return std::string(buffers_begin(streambuf.data()),
         buffers_begin(streambuf.data()) + n);
}


void wait_callback(const boost::system::error_code& error, boost::asio::ip::tcp::socket& _socket)
{
	OutputDebugString("wait_callback");

	if (error==boost::asio::error::operation_aborted)
	{
		// Data was read and this timeout was canceled
		return;
	}
	else if (error)
	{
		std::cout  << "read_timeout Error - " << error.message() << std::endl;
		// Data was read and this timeout was canceled
		return;
	}
	_socket.cancel();  // will cause read_callback to fire with an timeout error
}
void XMLClient::TimerReset()
{
	// Setup a deadline time to implement our timeout.
	_timer.expires_from_now(boost::posix_time::milliseconds(2000));
	_timer.async_wait(boost::bind(&wait_callback,
		boost::asio::placeholders::error, boost::ref(_socket)));
}
void XMLClient::StartAyncRead()
{
	try {
		if(!_socket.is_open())
		{
			DebugBreak();
		}
		boost::asio::async_read(_socket,
			boost::asio::buffer(data_, max_length), // buff, //boost::asio::buffer(&readBuffer[0], readBuffer.size()),
			boost::bind(&XMLClient::bytesToRead, 
			this,
			boost::asio::placeholders::error, 
			boost::asio::placeholders::bytes_transferred));
		
		TimerReset();
	}
	catch(boost::exception & ex)
	{
		//http://theboostcpplibraries.com/boost.exception
		std::cerr << boost::diagnostic_information(ex);
		_socket.close();
	}	
	catch(...)
	{
		_socket.close();
	}
}

std::string XMLClient::FindLeadingElement(std::string  xml)
{

	boost::match_results<std::string::const_iterator> matchResult;
	bool found;
	boost::regex e("<[A-Za-z0-9_]+");
	found = boost::regex_search( xml,matchResult, 	e);
	if(found)
	{
		std::string elem (matchResult[0]);
		elem.insert(1, 1, '/');
		elem=Globals::Trim(elem);
		elem.append(">"); // not space
		return elem;
	}
	return "";
}

void XMLClient::AppendBuffer(std::string read)
{
	if(_next.size()> 0)
	{
		_current.append(_next);
		_next.clear();
	}
	size_t oldsize= _current.size();
	_current.append(read);
}

bool XMLClient::BufferHandler( std::string endtag)
{
	std::size_t found;
	if((found=_current.find(endtag))!= std::string::npos)
	{
		found = found+endtag.size();
		_next=_current.substr(found);
		_current = _current.substr(0,found);
		AddMsgQueue(_current);
		_current.clear();
		TagReset();;
		return true;
	}
	return false;
}
size_t XMLClient::bytesToRead(const error_code& error, size_t bytes_read)
{
	size_t result=bytes_read;
	_timer.cancel();

	// Connection closed cleanly by peer
	// asio errors http://www.boost.org/doc/libs/1_44_0/doc/html/boost_asio/reference/error__basic_errors.html 
	if (error == boost::asio::error::eof || boost::asio::error::connection_reset == error)
	{
		Disconnect();
		return  0;                                       
	}
	else if(error == boost::asio::error::operation_aborted  )
	{
		Globals::ErrorMessage(" In bytesToRead Timer expired error \n");
	}
	else if (error)
	{
		// This error stops asynchronous reads
		std::stringstream s;
		s << "unknown bytesToRead error = (" << error << ") " << error.message()  << std::endl;
		Globals::ErrorMessage(s.str());
		return  0;  
	}
	
	if(bytes_read>0)
	{
		AppendBuffer(std::string(data_, data_+bytes_read));
		if(_endtag==NonsenseTag())
		{
			_endtag=FindLeadingElement(_current);
			if(_endtag.empty()) _endtag=NonsenseTag();
		}
		//BufferHandler(std::string(data_, data_+bytes_read), "</CRCLStatus>") ;
		BufferHandler(_endtag) ;

		//buff.consume(bytes_read);  // this is unnecessary since we are using char string not bufstream
	}
	StartAyncRead();
	return result;
}

void XMLClient::SyncWrite(std::string str)
{
  // Write data to server that contains a delimiter.
  _socket.send(boost::asio::buffer(str, str.size()));
}

#if 0

timer_.expires_from_now(boost::posix_time::seconds(30));
timer_.async_wait(&handle_wait);
socket_.async_connect(endpoint_, handle_connect);
boost::this_thread::sleep(boost::posix_time::seconds(60));
io_service_.run_one();

void handle_wait(const boost::system::error_code& error)
{
  // On error, return early.
  if (error) return;

  // If the timer expires in the future, then connect handler must have
  // first.
  if (timer_.expires_at() > deadline_timer::traits_type::now()) return;

  // Timeout has occurred, so close the socket.
  socket_.close();
}

void handle_connect(const boost::system::error_code& error)
{
  // The async_connect() function automatically opens the socket at the start
  // of the asynchronous operation. If the socket is closed at this time then
  // the timeout handler must have run first.
  if (!socket_.is_open()) return;

  // On error, return early.
  if (error) return;

  // Otherwise, a connection has been established.  Update the timer state
  // so that the timeout handler does not close the socket.
  timer_.expires_at(boost::posix_time::pos_infin);
}
#endif