#-------------------------------------------------
#
# Project created by QtCreator 2017-11-15T19:24:09
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtSocket_Record
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qtasyncsocket.cpp

HEADERS  += mainwindow.h \
    hexdump.h \
    Logger.h \
    Timing.h \
    qtasyncsocket.h \
    globals.h \
    RCSMsgQueue.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -Wno-unused-variable
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -fpermissive
QMAKE_CXXFLAGS +=-Wno-deprecated
QMAKE_CXXFLAGS +=-Wno-sign-compare
QMAKE_CXXFLAGS +=-Wno-missing-field-initializers


DISTFILES += \
    Notes
