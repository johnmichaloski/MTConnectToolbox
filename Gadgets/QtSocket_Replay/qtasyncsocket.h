#ifndef QTASYNCSOCKET_H
#define QTASYNCSOCKET_H

#include <QObject>
#include <QtGlobal>
#include <QThread>
#include <QMutex>
#include <QThread>
#include <QVector>
#include <QString>
#include <QMutexLocker>
#include <QWaitCondition>
#include <QTcpServer>
#include <QAbstractSocket>
#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QThreadPool>


#include <functional>
#include <memory>
#include <assert.h>
#include <ctype.h>
#include <string>
#include <sstream>

#include "SocketParser.h"
// std bind
using namespace std::placeholders;
class MainWindow;

template <typename T=bool>
class MutexVariable
{
public:
    MutexVariable() { mVar=0; }

    void set(T val=1)
    {
        QMutexLocker lock( &mMutex );
        mVar=val;
    }
    void clear()
    {
        QMutexLocker lock( &mMutex );
        mVar=0;
    }
    T get()
    {
        QMutexLocker lock( &mMutex );
        T tmp;
        tmp=mVar;
        return tmp;
    }

protected:
    QMutex mMutex;
    T mVar;
};



class MySocketClient;
typedef std::vector<uint8_t>                              raw_message_t;

class MySocketServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MySocketServer(QObject *parent = 0);
    ~MySocketServer();
    void                            Init(long portnumber, std::string devicename);
    void                            Reset(void);
    void                            Quit(void);

    void                            store_socket_string(raw_message_t msg);
    void                            remove_client( MySocketClient *client);
    int                             client_count(){ return clients.size(); }

    uint32_t &                      Port(){ return _portnumber; }
    std::string &                   Device(){ return _deviceName; }
    int Count();
    MutexVariable<int>              mRunThread;    /**< thread mutex */

    std::vector<raw_message_t>	    _allmsgssofar;   /**<  vector of parsed shdr output */
    std::vector<MySocketClient * >    clients;         /**<  vector of client threads */
    int                             aHeartbeatFreq;  /**<  heartbeat max timer each client thread */
    static bool                     bRunning;        /**<  static running flag all threads */
protected:
    void incomingConnection(qintptr socketDescriptor);

    std::string						_deviceName; /**< device name -unused */
    uint32_t						_portnumber; /**<  TCP/IP socket port number */
    bool							_bInited;

signals:
    void startClient(MySocketClient *);

public slots:
    void                            startmServer(); /**<  start listening */

};

class MySocketClient : public QThread
{
    Q_OBJECT
public:
    explicit MySocketClient(MySocketServer * server, QThread *parent = 0);
    void                            set_socket(qintptr Descriptor);
    virtual void                    run();
    void                            stop_thread();
    void                            reset();

signals:

protected:
    MySocketServer *                  mServer;        /**< parent shdr server */
    MutexVariable<int>              mRunThread;    /**< thread mutex */
    int                             aHeartbeatCnt; /**< timer countdown of heartbeats */
    size_t                          _nmsgs;       /**< at shdr string */
    std::stringstream in_buffer;
public slots:
    void                            connected();
    void                            disconnected();
    void                            ready_read(); /**< ready to read socket slot */
    void                            start_thread(); /**< ready to start thread slot */
private:
    QTcpSocket *                    socket;        /**< client socket to read/write  */

};

class SocketParser;

class ParseLoop : public QThread {
    Q_OBJECT
public:
    ParseLoop (QObject *parent = 0) ;
    ~ParseLoop() ;

    void configure(std::string ExeDirectory);

    std::string init(MainWindow * ,
                     SocketParser *parser,
                     std::string filename,
                     std::string sportnum);
    virtual void run();

    ///////////////////////////////////////
    SocketParser *mParser;    /**< pointer to shdr parser from file  */
    MySocketServer mBackend;  /**< back end socket manager  */
    MainWindow * mMainWindow;

protected:
    int nSleep ;            /**< each loop sleep amount */
    int nTimeMultipler ;
    std::string mFilename  ; /**< file path containing SHDR to echo */
    long mPortnum;           /**< ip port number */
    bool bOptionWait;       /**< wait to start streaming - skipped now */
    int mCountdown;


public slots:
    void quit();

};
#endif // QTASYNCSOCKET_H
