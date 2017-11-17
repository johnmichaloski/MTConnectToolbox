/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLineEdit *socketEdit;
    QLabel *label;
    QLineEdit *fileEdit;
    QLabel *label_2;
    QPushButton *FilePushButton;
    QPushButton *startPushButton;
    QPushButton *stopPushButton;
    QTextEdit *plainTextEdit;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(405, 411);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        socketEdit = new QLineEdit(centralWidget);
        socketEdit->setObjectName(QString::fromUtf8("socketEdit"));
        socketEdit->setGeometry(QRect(20, 30, 113, 22));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 10, 55, 16));
        label->setToolTipDuration(-8);
        fileEdit = new QLineEdit(centralWidget);
        fileEdit->setObjectName(QString::fromUtf8("fileEdit"));
        fileEdit->setGeometry(QRect(20, 90, 351, 22));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 60, 55, 16));
        FilePushButton = new QPushButton(centralWidget);
        FilePushButton->setObjectName(QString::fromUtf8("FilePushButton"));
        FilePushButton->setGeometry(QRect(80, 60, 51, 20));
        startPushButton = new QPushButton(centralWidget);
        startPushButton->setObjectName(QString::fromUtf8("startPushButton"));
        startPushButton->setGeometry(QRect(100, 300, 93, 28));
        stopPushButton = new QPushButton(centralWidget);
        stopPushButton->setObjectName(QString::fromUtf8("stopPushButton"));
        stopPushButton->setGeometry(QRect(210, 300, 93, 28));
        plainTextEdit = new QTextEdit(centralWidget);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setGeometry(QRect(20, 130, 351, 161));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 405, 26));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Socket Replay", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Socket", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Filename", 0, QApplication::UnicodeUTF8));
        FilePushButton->setText(QApplication::translate("MainWindow", "...", 0, QApplication::UnicodeUTF8));
        startPushButton->setText(QApplication::translate("MainWindow", "START", 0, QApplication::UnicodeUTF8));
        stopPushButton->setText(QApplication::translate("MainWindow", "STOP", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
