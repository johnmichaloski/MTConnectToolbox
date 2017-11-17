#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "qtasyncsocket.h"
#include "RCSMsgQueue.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    MySocketClient recorder;
    void closeEvent(QCloseEvent *event);
    RCS::CMessageQueue<std::string> buffer_queue;

private slots:
    void on_startButton_clicked();

    void on_stopButton_clicked();

    void on_pushButton_clicked();
    void updateCapture();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    int mTimerId;

};

#endif // MAINWINDOW_H
