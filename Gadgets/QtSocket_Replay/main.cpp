#include "mainwindow.h"
#include <QApplication>
#include "globals.h"
#include "Timing.h"
int main(int argc, char *argv[])
{
    std::string path(argv[0]);
#ifdef WIN32
    Globals::ExeDirectory() = path.substr(0, path.find_last_of('\\') + 1);
#else
    Globals::ExeDirectory() = path.substr(0, path.find_last_of('/') + 1);
#endif

    QApplication a(argc, argv);
    MainWindow w;
    MainWindow * mainwin;
    mainwin=&w;
    w.show();

    return a.exec();
}
