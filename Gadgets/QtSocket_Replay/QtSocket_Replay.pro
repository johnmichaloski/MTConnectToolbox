#-------------------------------------------------
#
# Project created by QtCreator 2017-11-16T14:11:18
#
#-------------------------------------------------

QT       += core gui network widgets
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11

TARGET = QtSocket_Replay
TEMPLATE = app

release: DESTDIR = release
debug:   DESTDIR = debug

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

QMAKE_CXXFLAGS += -Wno-unused-variable
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -fpermissive
QMAKE_CXXFLAGS +=-Wno-deprecated
QMAKE_CXXFLAGS +=-Wno-sign-compare
QMAKE_CXXFLAGS +=-Wno-missing-field-initializers
QMAKE_CXXFLAGS +=-std=gnu++11

SOURCES += main.cpp\
        mainwindow.cpp \
    qtasyncsocket.cpp

HEADERS  += mainwindow.h \
    globals.h \
    hexdump.h \
    Logger.h \
    RCSMsgQueue.h \
    SocketParser.h \
    Timing.h \
    qtasyncsocket.h

FORMS    += mainwindow.ui

DISTFILES += \
    Notes
