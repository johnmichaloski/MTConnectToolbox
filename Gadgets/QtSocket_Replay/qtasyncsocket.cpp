

#include "qtasyncsocket.h"
#include <QProcess>
#include <QFuture>

#include <stddef.h>		// NULL, sizeof
#include <stdio.h>		// printf
#include <stdlib.h>		// atoi, strtof
#include <ctype.h>		// isspace
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <algorithm>
#include <string>
#include <sstream>
#include <fcntl.h>
#include <iostream>

#include "mainwindow.h"


#include "globals.h"
#include "SocketParser.h"

using namespace Globals;


////////////////////////////////////////////////////////
bool MySocketServer::bRunning=false;
MySocketServer::MySocketServer(QObject *parent) :
    QTcpServer(parent)
{
    Reset();
    aHeartbeatFreq = 10000;

}
MySocketServer::~MySocketServer()
{
    for(size_t i=0; i< clients.size(); i++)
        delete clients[i];
}

void MySocketServer::Init(long portnumber, std::string devicename)
{
    _bInited=true;
    _portnumber=portnumber;
    _deviceName=devicename;
    startmServer();
}

void MySocketServer::Quit()
{
    bRunning=false;
    for(int i=0; i< clients.size(); i++)
        QThread::msleep(1000);
}

void MySocketServer::Reset()
{
    _bInited=false;
    _allmsgssofar.clear();
    for(size_t i=0; i< clients.size(); i++)
        clients[i]->reset();
}
void MySocketServer::store_socket_string(raw_message_t msg )
{
    if(!_bInited)
    {
        _bInited=true;
        _allmsgssofar.clear();
    }
    _allmsgssofar.push_back(msg);
}

void MySocketServer::startmServer()
{
    bRunning=true;
    if(listen(QHostAddress::Any, Port()))
    {
        qDebug() << "Server: started\n";
    }
    else
    {
        qDebug() << "Server: not started!\n";
    }
}

void MySocketServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "MySocketServer::incomingConnection!\n";
    // At the incoming connection, make a client
    // and set the socket
    MySocketClient *client = new MySocketClient(this);
    client->set_socket(socketDescriptor);
    clients.push_back(client);
    connect(
        this, SIGNAL( startClient( MySocketClient *  ) ),
        client, SLOT( start_thread() )
    );

    //client->start_thread();
    emit startClient(client);
}
int MySocketServer::Count()
{
    return clients.size();
}

void MySocketServer::remove_client( MySocketClient *client)
{
    std::vector<MySocketClient * >::iterator it;
    if((it=std::find(clients.begin(), clients.end(), client))!= clients.end())
         clients.erase(it);
}

/////////////////////////////////////////////////////////
MySocketClient::MySocketClient(MySocketServer * server, QThread *parent) :
    QThread(parent)
{
    mServer= server;
}
void MySocketClient::reset()
{
     _nmsgs=0;
}

void MySocketClient::set_socket(qintptr descriptor)
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
void MySocketClient::start_thread()
{
    mRunThread.set(1);
    start();
}

void MySocketClient::stop_thread()
{
    mRunThread.set(0);
}

// asynchronous - runs separately from the thread we created
void MySocketClient::connected()
{
    qDebug() << "Client connected event";
}

// asynchronous
void MySocketClient::disconnected()
{
    stop_thread();
    qDebug() << "Client disconnected";
}

void MySocketClient::ready_read()
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
            std::string mPong= Timing::StrFormat( "* PONG %d\n", mServer->aHeartbeatFreq);
            socket->write(mPong.c_str(), mPong.size());
            QThread::msleep(50);
        }
    }
}

void MySocketClient::run()
{
    // FIXME need mutex  - cant add while we are consuming
    // This is set to the maximum shdr - which will be streamed at once to listener
    _nmsgs=mServer->_allmsgssofar.size();

    // This is catch up for any new connection
    // For raw connection - skip
#if 0
    for(size_t i=0; i< mServer->_allmsgssofar.size(); i++)
    {
        socket->write(mServer->_allmsgssofar[i].data(), mServer->_allmsgssofar[i].size());
        QThread::msleep(50);
    }
#else
    if(_nmsgs>0)
        _nmsgs--;
#endif
     aHeartbeatCnt=mServer->aHeartbeatFreq;

    for (; MySocketServer::bRunning && mRunThread.get();)
    {
        aHeartbeatCnt-=100;  // heartbeat countdown
        QThread::msleep(100);

        while(_nmsgs<mServer->_allmsgssofar.size())
        {
            // fixme: this should be mutex copy
            raw_message_t _msg=mServer->_allmsgssofar[_nmsgs++];
            if(_msg.size() < 1)
                continue;
            aHeartbeatCnt=mServer->aHeartbeatFreq;

            if(socket->state() == QAbstractSocket::ConnectedState)
            {
                socket->write(_msg.data(), _msg.size());
                socket->flush(); socket->flush();
            }
            else
                mRunThread.set(0);
            QThread::msleep(50);
        }
    }
    mServer->remove_client(this);
}
////////////////////////////////////////////////////////////////


ParseLoop::ParseLoop (QObject *parent) : QThread(parent)
{
    mPortnum=7878;
//    ip = "127.0.0.1";
    bOptionWait=false;
    nSleep = 100;
    nTimeMultipler = 1;
    mParser= new SocketParser(); // SocketParser(backend);
    mParser->Repeat()=true;
}
ParseLoop::~ParseLoop()
{
    delete mParser;
}

void ParseLoop::configure(std::string ExeDirectory)
{
    std::string cfgfile = ExeDirectory+"Config.ini";

    // Parse ini file named config.ini must be in same directory as exe
    QString iniPath=QString::fromUtf8((cfgfile).c_str());
    QSettings settings( iniPath, QSettings::IniFormat );
    mParser->Repeat()   =  settings.value( "GLOBALS/Repeat" ).toInt();
    mFilename          =  (const char *) settings.value( "GLOBALS/Filename" ).toString().toLatin1();
    bOptionWait       =  settings.value( "GLOBALS/Wait" ).toInt();
    mPortnum           =  settings.value( "GLOBALS/PortNum" ).toInt();
    mFilename = ExeDirectory+ mFilename;
    mParser->Init(mFilename);
    mBackend.Init(mPortnum, "M1");
    double delay=0;
}
void ParseLoop::run()
{
    int delay=0;
    MySocketServer::bRunning=true;
    try {
        while(MySocketServer::bRunning)
        {
            if ( ( mBackend.Count( ) < 1 ) && bOptionWait )
             {
                 nSleep = mCountdown;
             }
             else
             {
                 if ( delay < 0 )
                 {
                     delay = (long) mParser->ProcessStream( ) * nTimeMultipler;

                     if ( delay < 0 )
                     {
                         break;                             // should only get here if no repeat
                     }
                     mBackend.store_socket_string(mParser->GetLatestMsg( ));

                     // this->screen_msg(mParser.GetLatestBuffer()+"\r\n"); // GUI slow down
                     mMainWindow->buffer_queue.AddMsgQueue(mParser->GetLatestBuffer( ) + "\r\n");
                 }
                 else
                 {
                     delay -= mCountdown;
                 }
             }
            QThread::msleep(nSleep);
        }
    }
    catch(std::exception err)
    {
        std::cout << err.what();
        std::cout << Globals::StrFormat("Exception in file %s at line %d\n", mFilename.c_str(),  mParser->LineNumber());
    }
    mBackend.Quit();
    QThread::msleep(2000);
}
std::string ParseLoop::init(MainWindow * pMainWindow, SocketParser *parser, std::string filename, std::string sportnum)
{
    std::string err;
    try
    {
        mMainWindow=           pMainWindow;
        mParser->Repeat()   =  1;
        mFilename          =  filename;
        bOptionWait       =  1;
        mPortnum           =  QString(sportnum.c_str()).toInt();
        mParser->Init(filename);
        mBackend.Init(mPortnum, "M1");
        mCountdown=100;
    }
    catch(std::exception &e)
    {
        err=e.what();
    }
    return err;
}

void ParseLoop::quit()
{
    MySocketServer::bRunning=false;
    mBackend.Quit();
    QThread::msleep(2000);
}
