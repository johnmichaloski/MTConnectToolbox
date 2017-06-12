

#include "qtasyncsocket.h"
#include <QProcess>
#include <QFuture>

//#include <QtConcurrentRun>

#include <stddef.h>		// NULL, sizeof
#include <stdio.h>		// printf
#include <stdlib.h>		// atoi, strtof
#include <ctype.h>		// isspace
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <algorithm>
#include <string>
#include <sstream>

#ifdef WIN32
# include <winsock2.h>
#include <conio.h>
//#include <winioctl.h>
#include <ws2tcpip.h>
#undef SendMessage
#else
#include <sys/socket.h>
#include <sys/ioctl.h>
#endif
#include <fcntl.h>
#include <iostream>

#include "globals.h"
#include "ShdrParser.h"

using namespace Globals;


////////////////////////////////////////////////////////
bool MyShdrServer::bRunning=true;
MyShdrServer::MyShdrServer(QObject *parent) :
    QTcpServer(parent)
{
    Reset();
    aHeartbeatFreq = 10000;
}

void MyShdrServer::Init(std::string domain, long portnumber, std::string devicename)
{
    _bInited=true;
    _domainname=domain;
    _portnumber=portnumber;
    _deviceName=devicename;
    start_server();
}

void MyShdrServer::Quit()
{
    bRunning=false;
    for(int i=0; i< clients.size(); i++)
        QThread::msleep(1000);
}

void MyShdrServer::Reset()
{
    _bInited=false;
    _allshdrsofar.clear();
    for(size_t i=0; i< clients.size(); i++)
        clients[i]->reset();
}
void MyShdrServer::store_shdr_string(std::string str)
{
    if(!_bInited)
    {
        _bInited=true;
        _allshdrsofar.clear();
    }
    _allshdrsofar.push_back(str);
}

void MyShdrServer::start_server()
{
    //if(listen(QHostAddress(_domainname.c_str()), Port()))
    if(listen(QHostAddress::Any, Port()))
    {
        qDebug() << "Server: started\n";
    }
    else
    {
        qDebug() << "Server: not started!\n";
    }
}

void MyShdrServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "MyShdrServer::incomingConnection!\n";
    // At the incoming connection, make a client
    // and set the socket
    MyShdrClient *client = new MyShdrClient(this);
    client->set_socket(socketDescriptor);
    clients.push_back(client);
    connect(
        this, SIGNAL( startClient( MyShdrClient *  ) ),
        client, SLOT( start_thread() )
    );

    //client->start_thread();
    emit startClient(client);
}
void MyShdrServer::remove_client( MyShdrClient *client)
{
    std::vector<MyShdrClient * >::iterator it;
    if((it=std::find(clients.begin(), clients.end(), client))!= clients.end())
         clients.erase(it);
}

/////////////////////////////////////////////////////////
MyShdrClient::MyShdrClient(MyShdrServer * server, QThread *parent) :
    QThread(parent)
{
    _server= server;
}
void MyShdrClient::reset()
{
     _nshdrs=0;
}

void MyShdrClient::set_socket(qintptr descriptor)
{
    // make a new socket
    socket = new QTcpSocket(this);

    qDebug() << "A new socket created!";

    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(ready_read()));

    socket->setSocketDescriptor(descriptor);

    qDebug() << " Client connected at " << descriptor;
}
void MyShdrClient::start_thread()
{
    mRunThread.set(1);
    start();
}

void MyShdrClient::stop_thread()
{
    mRunThread.set(0);
}

// asynchronous - runs separately from the thread we created
void MyShdrClient::connected()
{
    qDebug() << "Client connected event";
}

// asynchronous
void MyShdrClient::disconnected()
{
    stop_thread();
    qDebug() << "Client disconnected";
}

void MyShdrClient::ready_read()
{

    //First lets check that whole line of data can be read from socket
    if(!socket->canReadLine())
    {
        //Data not ready, return to wait next readReady() signal
        return;
    }

    while(socket->canReadLine())
    {
        QString q = socket->readLine();
        if (strncmp(q.toStdString().c_str(), "* PING", 6) == 0)
        {
            std::string mPong= StrFormat( "* PONG %d\n", _server->aHeartbeatFreq);
            socket->write(mPong.c_str(), mPong.size());
            QThread::msleep(50);
        }
    }
}

void MyShdrClient::run()
{
    // FIXME need mutex  - cant add while we are consuming
    // This is set to the maximum shdr - which will be streamed at once to listener
    _nshdrs=_server->_allshdrsofar.size();
    for(size_t i=0; i< _server->_allshdrsofar.size(); i++)
    {
        socket->write(_server->_allshdrsofar[i].c_str(), _server->_allshdrsofar[i].size());
        QThread::msleep(50);
    }
     aHeartbeatCnt=_server->aHeartbeatFreq;

    for (; MyShdrServer::bRunning && mRunThread.get();)
    {
        aHeartbeatCnt-=100;  // heartbeat countdown
        QThread::msleep(100);

        while(_nshdrs<_server->_allshdrsofar.size())
        {
            // fixme: this should be mutex copy
            std::string _shdr=_server->_allshdrsofar[_nshdrs++];
            if(_shdr.size() < 1)
                continue;
            aHeartbeatCnt=_server->aHeartbeatFreq;
            // See what's going out
            std::cout <<  _shdr << std::endl;

            if(socket->state() == QAbstractSocket::ConnectedState)
            {
                socket->write(_shdr.c_str(), _shdr.size());
                socket->flush(); socket->flush();
            }
            else
                mRunThread.set(0);
            QThread::msleep(50);
        }
    }
    _server->remove_client(this);
}
////////////////////////////////////////////////////////////////


ParseLoop::ParseLoop (QObject *parent) : QThread(parent)
{
    portnum=7878;
    ip = "127.0.0.1";
    bOptionWait=false;
    nSleep = 100;
    nTimeMultipler = 1;
    parser= new ShdrParser(backend);
    parser->Repeat()=false;
}
void ParseLoop::configure(std::string ExeDirectory)
{
    std::string cfgfile = ExeDirectory+"Config.ini";

    // Parse ini file named config.ini must be in same directory as exe
    QString iniPath=QString::fromUtf8((cfgfile).c_str());
    QSettings settings( iniPath, QSettings::IniFormat );
    parser->Repeat()   =  settings.value( "GLOBALS/Repeat" ).toInt();
    filename          =  (const char *) settings.value( "GLOBALS/Filename" ).toString().toLatin1();
    bOptionWait       =  settings.value( "GLOBALS/Wait" ).toInt();
    portnum           =  settings.value( "GLOBALS/PortNum" ).toInt();
    ip                =  (const char *)settings.value( "GLOBALS/IP" ).toString().toLatin1();
    filename = ExeDirectory+ filename;
    parser->Init(filename);
    backend.Init(ip, portnum, "M1");
    double delay=0;
}
void ParseLoop::run()
{
    int delay=0;
    MyShdrServer::bRunning=true;
    try {
        while(MyShdrServer::bRunning)
        {
             {
                if(delay<0)
                {
                    delay=parser->ProcessStream()*nTimeMultipler;
                    if(delay<0)
                        break; // should only get here if no repeat
                    backend.store_shdr_string(parser->GetLatestBuffer()+"\n");
                }
                else
                    delay-=100;
            }
            QThread::msleep(nSleep);
        }
    }
    catch(std::exception err)
    {
        std::cout << err.what();
        std::cout << StrFormat("Exception in file %s at line %d\n", filename.c_str(),  parser->LineNumber());
    }
    backend.Quit();
    QThread::msleep(2000);
}

void ParseLoop::quit()
{
    backend.Quit();
    QThread::msleep(2000);
}
