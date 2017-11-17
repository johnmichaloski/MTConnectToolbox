#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include "Logger.h"
#include "Globals.h"
MainWindow * mainwin;
int main(int argc, char *argv[])
{
    std::string path(argv[0]);
#ifdef WIN32
    Globals::ExeDirectory() = path.substr(0, path.find_last_of('\\') + 1);
#else
    Globals::ExeDirectory() = path.substr(0, path.find_last_of('/') + 1);
#endif
    QApplication a(argc, argv);

    // http://doc.qt.io/qt-4.8/qt-linguist-hellotr-example.html
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    //     myappTranslator.load("myapp_" + QLocale::system().name());
    //     app.installTranslator(&myappTranslator);

    MainWindow w;
    mainwin=&w;
    w.show();

    return a.exec();
}
