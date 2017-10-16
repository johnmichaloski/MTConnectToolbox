QT += core
QT -= gui

CONFIG += c++11

TARGET = QtUrAgent
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ../NIST/WinTricks.cpp \
    ../UR_Agent/DevicesXML.cpp \
    ../UR_Agent/ProductVersion.cpp \
    ../UR_Agent/UR_Adapter.cpp \
    ../UR_Agent/UR_Agent.cpp \
    ../UR_Agent/UrInterface.cpp

HEADERS += \
    ../NIST/AppEventLog.h \
    ../NIST/Config.h \
    ../NIST/ErrResult.h \
    ../NIST/File.h \
    ../NIST/Firewall.h \
    ../NIST/hexdump.h \
    ../NIST/InMemLog.h \
    ../NIST/Logger.h \
    ../NIST/macros.h \
    ../NIST/RCSMsgQueue.h \
    ../NIST/RCSMsgQueueThread.h \
    ../NIST/RCSThreadTemplate.h \
    ../NIST/RCSTimer.h \
    ../NIST/StdStringFcn.h \
    ../NIST/WinSingleton.h \
    ../NIST/WinTricks.h \
    ../UR_Agent/DevicesXML.h \
    ../UR_Agent/Globals.h \
    ../UR_Agent/ProductVersion.h \
    ../UR_Agent/UR_Adapter.h \
    ../UR_Agent/UR_Agent.h \
    ../UR_Agent/UrEnums.h \
    ../UR_Agent/UrInterface.h \
    ../UR_Agent/VersionNo.h
