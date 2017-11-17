#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qtasyncsocket.h"
#include <QTimer>
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
    ParseLoop loop;
    SocketParser myparser;

    void closeEvent(QCloseEvent *event);
    RCS::CMessageQueue<std::string> buffer_queue;

private slots:

    void on_FilePushButton_clicked();

    void on_startPushButton_clicked();

    void on_stopPushButton_clicked();

    void updateReplay();

private:
    Ui::MainWindow *ui;
    QTimer * timer;
};

#endif // MAINWINDOW_H
