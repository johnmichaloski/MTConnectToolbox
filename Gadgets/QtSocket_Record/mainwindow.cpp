#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include "Globals.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->IPtextbox->setText("129.6.32.69");
    ui->socketEdit->setText("30002"); // universal robot status
    ui->fileEdit->setText((Globals::ExeDirectory() + "port.log").c_str());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startButton_clicked()
{

    if(recorder.mRunThread.get())
    {
        QMessageBox::question(this,
                              tr("Error"),
                              "Socket Recorder Already Running",
                              QMessageBox::Ok);
        return;
    }
    QString ip = ui->IPtextbox->text();
    QString socket = ui->socketEdit->text();
    QString fileName = ui->fileEdit->text();;
    if(fileName.isEmpty()||
            ip.isEmpty()||
            socket.isEmpty())
    {
        QMessageBox::question(this,
                              tr("Error"),
                              "Missing IP/Socket/Filename - can't process",
                              QMessageBox::Ok);
        return;
    }
    std::string err= recorder.init(ip.toStdString(), socket.toStdString(), fileName.toStdString());
    if(!err.empty())
    {
        QMessageBox::question(this,
                              tr("Error"),
                              err.c_str(),
                              QMessageBox::Ok);
        return;

    }
    recorder.startRecording();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateCapture()));
    timer->start(1000);
}
void MainWindow::updateCapture()
{
    while(buffer_queue.SizeMsgQueue() > 0)
    {
        ui->plainTextEdit->appendPlainText(buffer_queue.PopFrontMsgQueue().c_str());
    }
}

void MainWindow::on_stopButton_clicked()
{
    timer->stop();
    buffer_queue.ClearMsgQueue();
    ui->plainTextEdit->clear();
    recorder.halt();
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    recorder.halt();
    QThread::sleep(2);
}
void MainWindow::on_pushButton_clicked()
{
    QString fileName;
    fileName=QFileDialog::getSaveFileName(this,
                                          tr("Save Log File"),
                                          QDir::currentPath(),
                                          "Log Files (*.log );; Text Files (*.txt)",
                                          new QString("Text files (*.txt)"));

    ui->fileEdit-> setText(fileName);

}
