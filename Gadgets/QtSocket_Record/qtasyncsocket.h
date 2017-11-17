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
#include <fstream>

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

class MySocketClient : public QThread
{
    Q_OBJECT
public:
    explicit MySocketClient(QThread *parent = 0);
    ~MySocketClient();
    void halt ( );
    void run();
    std::string  init (std::string host, std::string port, std::string filename);
    void  startRecording();

    std::string mHost;
    std::string mPort;
    std::string mFilename;
    std::ofstream rawSocketFile;
    MutexVariable<int>  mRunThread;    /**< flag with thread mutex */

signals:

protected:
    bool mConnected;
    bool _bInited;
    bool bRunning;
public slots:
    void                            connected();
    void                            disconnected();
    void                            ready_read(); /**< ready to read socket slot */
private:
    QTcpSocket *                    socket;        /**< client socket to read/write  */
    QHostAddress address;
    //Socket_Record_Dlg * mParent;
};


#endif // QTASYNCSOCKET_H
