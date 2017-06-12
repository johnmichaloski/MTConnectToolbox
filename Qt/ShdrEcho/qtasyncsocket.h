#ifndef QTASYNCSOCKET_H
#define QTASYNCSOCKET_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QThread>
#include <QVector>
#include <QString>
#include <QTcpSocket>
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

// std bind
using namespace std::placeholders;


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



class MyShdrClient;

class MyShdrServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MyShdrServer(QObject *parent = 0);
    void                            Init(std::string domain, long portnumber, std::string devicename);
    void                            Reset(void);
    void                            Quit(void);

    void                            store_shdr_string(std::string str);
    void                            remove_client( MyShdrClient *client);
    int                             client_count(){ return clients.size(); }

    std::string &                   Ip(){ return _domainname; }
    uint32_t &                      Port(){ return _portnumber; }
    std::string &                   Device(){ return _deviceName; }

    std::vector<std::string>	    _allshdrsofar;   /**<  vector of parsed shdr output */
    std::vector<MyShdrClient * >    clients;         /**<  vector of client threads */
    int                             aHeartbeatFreq;  /**<  heartbeat max timer each client thread */
    static bool                     bRunning;        /**<  static running flag all threads */
protected:
    void incomingConnection(qintptr socketDescriptor);

    std::string						_deviceName; /**< device name -unused */
    std::string						_domainname; /**< TCP/IP ip or domain name */
    uint32_t						_portnumber; /**<  TCP/IP socket port number */
    bool							_bInited;

signals:
    void startClient(MyShdrClient *);

public slots:
    void                            start_server(); /**<  start listening */

};

class MyShdrClient : public QThread
{
    Q_OBJECT
public:
    explicit MyShdrClient(MyShdrServer * server, QThread *parent = 0);
    void                            set_socket(qintptr Descriptor);
    virtual void                    run();
    void                            stop_thread();
    void                            reset();

signals:

protected:
    MyShdrServer *                  _server;        /**< parent shdr server */
    MutexVariable<int>              mRunThread;    /**< thread mutex */
    int                             aHeartbeatCnt; /**< timer countdown of heartbeats */
    size_t                          _nshdrs;       /**< at shdr string */
    std::stringstream in_buffer;
public slots:
    void                            connected();
    void                            disconnected();
    void                            ready_read(); /**< ready to read socket slot */
    void                            start_thread(); /**< ready to start thread slot */
private:
    QTcpSocket *                    socket;        /**< client socket to read/write  */

};

class ShdrParser;

class ParseLoop : public QThread {
    Q_OBJECT
public:
    ParseLoop (QObject *parent = 0) ;
    ~ParseLoop() {}
    void configure(std::string ExeDirectory);
    virtual void run();

    ///////////////////////////////////////
    ShdrParser *parser;    /**< pointer to shdr parser from file  */
    MyShdrServer backend;  /**< back end socket manager  */

protected:
    int nSleep ;            /**< each loop sleep amount */
    int nTimeMultipler ;
    // Option handling
    std::string filename  ; /**< file path containing SHDR to echo */
    long portnum;           /**< ip port number */
    std::string ip ;        /**< ip host name */
    bool bOptionWait;       /**< wait to start streaming - skipped now */


public slots:
    void quit();

};
#endif // QTASYNCSOCKET_H
