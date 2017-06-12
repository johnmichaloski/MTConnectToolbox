

QT += core
QT       += network
#QT -= gui
TARGET = ShdrEcho
CONFIG += console
CONFIG -= app_bundle
#CONFIG += static

TEMPLATE = app

release: DESTDIR = release
debug:   DESTDIR = debug

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui


#INCLUDEPATH  += C:\boost\boost_1_61_0
#LIBS += -L"C:\boost\boost_1_61_0\stage\lib"
#LIBS += "C:\boost\boost_1_61_0\stage\lib\libboost_system-mgw49-d-1_61.a"
#LIBS += "C:\boost\boost_1_61_0\stage\lib\libboost_thread-mgw49-mt-d-1_61.a"
##LIBS += "C:\boost\boost_1_61_0\stage\lib\libboost_chrono-mgw49-d-1_61.a"
#LIBS += -L"C:\Qt\Tools\mingw492_32\i686-w64-mingw32\lib" -lmswsock -llibws2_32
#LIBS += "C:\Qt\5.6\mingw49_32\lib\Qt5Core.a"

QMAKE_CXXFLAGS += -DBOOST_THREAD_NO_LIB \
  -Wunused-local-typedefs -Wno-unused-but-set-variable

win32
{
QMAKE_CXXFLAGS += -DWIN32
}
QMAKE_CXXFLAGS += -Wno-unused-variable
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -fpermissive
QMAKE_CXXFLAGS +=-Wno-deprecated
QMAKE_CXXFLAGS +=-DLINUXSOCKETS=1
QMAKE_CXXFLAGS +=-Wno-sign-compare
#QMAKE_CXXFLAGS +=-std=gnu++11

SOURCES += main.cpp \
    qtasyncsocket.cpp

HEADERS += \
    Timing.h \
    ShdrParser.h \
    globals.h \
    qtasyncsocket.h

DISTFILES += \
    Notes
