#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QTimer>
#include <QFileDialog>
#include <QDialog>

#include "Globals.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->socketEdit->setText("7878"); // universal robot status
    ui->fileEdit->setText((Globals::ExeDirectory() + "raw_status_dump.txt").c_str());
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateReplay()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_FilePushButton_clicked()
{
    QString fileName;
    fileName=QFileDialog::getOpenFileName(this,
                                          tr("Save Log File"),
                                          QDir::currentPath(),
                                          "Log Files (*.log );; Text Files (*.txt)",
                                          new QString("Text files (*.txt)"));

    ui->fileEdit-> setText(fileName);
}

void MainWindow::on_startPushButton_clicked()
{
    if(MySocketServer::bRunning)
    {
        QMessageBox::question(this,
                              tr("Error"),
                              "Socket Recorder Already Running",
                              QMessageBox::Ok);
        return;
    }
    QString socket = ui->socketEdit->text();
    QString fileName = ui->fileEdit->text();;
    if(fileName.isEmpty()||
             socket.isEmpty())
    {
        QMessageBox::question(this,
                              tr("Error"),
                              "Missing IP/Socket/Filename - can't process",
                              QMessageBox::Ok);
        return;
    }

    std::string err = loop.init(this, &myparser, fileName.toStdString(), socket.toStdString() );

    if(!err.empty())
    {
        QMessageBox::question(this,
                              tr("Error initialization"),
                              err.c_str(),
                              QMessageBox::Ok);
        return;

    }

    loop.start();

    timer->start(1000);
}

void MainWindow::on_stopPushButton_clicked()
{
    if(timer)
        timer->stop();
    buffer_queue.ClearMsgQueue();
    ui->plainTextEdit->clear();
    loop.quit();
}
void MainWindow::closeEvent(QCloseEvent *event)
{
//    QDialog *msgDlg= new QDialog(this);
//    msgDlg->setWindowTitle("Closing");
//    msgDlg->setModal(false);
//    msgDlg->show();

    loop.quit();
    QThread::sleep(2);
//    delete(msgDlg);
}
void MainWindow::updateReplay()
{
    while(buffer_queue.SizeMsgQueue() > 0)
    {
        ui->plainTextEdit->append(buffer_queue.PopFrontMsgQueue().c_str());
    }
}
