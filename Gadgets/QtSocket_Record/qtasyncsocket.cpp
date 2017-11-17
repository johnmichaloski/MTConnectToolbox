

#include "qtasyncsocket.h"
#include <QProcess>
#include <QFuture>
#include <QHostInfo>


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


#include "MainWindow.h"
extern MainWindow * mainwin;

#include "hexdump.h"
#include "globals.h"
#include "Logger.h"
using namespace Globals;


/////////////////////////////////////////////////////////
MySocketClient::MySocketClient( QThread *parent) :
    QThread(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(ready_read()));
    bRunning=false;

}
MySocketClient::~MySocketClient()
{
    delete socket;
}

std::string MySocketClient::init (std::string host, std::string port, std::string filename)
{

    mPort     = port;
    mHost     = host;
    mFilename = filename;
    mConnected = false;
    if ( filename.find_last_of('\\') == std::string::npos )
    {
        filename = Globals::ExeDirectory( ) + filename;
    }

    // Open socket recording file

    rawSocketFile.open(filename.c_str( ), std::fstream::out);

    if ( !rawSocketFile )
    {
        return Globals::StrFormat("ERROR, couldn't open %s\n", mFilename.c_str( ));
    }
    _bInited=true;
    return "";
}
void MySocketClient::halt ( )
{
    mRunThread.set(0);
    while(bRunning)
        QThread::sleep(1);
}

void MySocketClient::startRecording()
{
    if(!_bInited)
        return;
    start();
}

// asynchronous - runs separately from the thread we created
void MySocketClient::connected()
{
    mConnected=true;
    qDebug() << "Client connected event";
}

// asynchronous
void MySocketClient::disconnected()
{
    mConnected=false;
}

void MySocketClient::ready_read()
{
    if(mRunThread.get() == 0)
        return;

    try {
        int bytes = socket->bytesAvailable();

        //First lets check that whole line of data can be read from socket
        if(bytes < 1)
        {
            //Data not ready, return to wait next readReady() signal
            return;
        }

        QByteArray buffer = socket->readAll();

        std::stringstream ss;
        ss << Logging::CLogger::Timestamp( ) << "|";
        ss << Nist::RawDump(buffer.data(), buffer.count(), "%02X,");
        rawSocketFile << ss.str( );
        rawSocketFile.flush( );
        ss << "\r\n";
        mainwin->buffer_queue.AddMsgQueue(ss.str());
    }
    catch(...)
    {
        mConnected=false;
    }

    //mParent->buffer_queue.AddMsgQueue(ss.str( ));

}
void MySocketClient::run()
{
    mRunThread.set(1);
    mConnected=false;
    bRunning=true;

    while(mRunThread.get())
    {
        //bool connected = (socket->state() == QTcpSocket::ConnectedState);
        while(!mConnected)
        {
            socket->connectToHost(mHost.c_str(), QString(mPort.c_str()).toInt());

            if(socket->waitForConnected(3000))
            {
                mConnected=true;
            }
            else
            {
                mConnected=false;
                QThread::sleep ( 5 );
            }
        }

        //Blocks until new data is available for reading and the readyRead() signal
        // has been emitted, or until msecs milliseconds have passed.
        // If msecs is -1, this function will not time out.
        if(!socket->waitForReadyRead())
        {
            mConnected=false;
            QThread::sleep (1 );
            continue;
        }
        int bytes = socket->bytesAvailable();
        //First lets check that whole line of data can be read from socket
        if(bytes < 1)
        {
            //Data not ready, return to wait next readReady() signal
            QThread::sleep ( 1 );
            continue;
        }

        // QThread::sleep ( unsigned long secs )
        // QThread::usleep ( unsigned long usecs )
        // QThread::msleep ( unsigned long msecs )
        QThread::sleep ( 1 );
    }
    rawSocketFile.close( );
    socket->disconnectFromHost();
    bRunning=false;

}

