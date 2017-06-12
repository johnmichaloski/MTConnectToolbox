//
// main.cpp
//

/*
 * DISCLAIMER:
 * This software was produced by the National Institute of Standards
 * and Technology (NIST), an agency of the U.S. government, and by statute is
 * not subject to copyright in the United States.  Recipients of this software
 * assume all responsibility associated with its operation, modification,
 * maintenance, and subsequent redistribution.
 *
 * See NIST Administration Manual 4.09.07 b and Appendix I.
 */

#define OutputDebugString(X)

#include <QCoreApplication>
#include <QThread>
#include <QSettings>
#include <QTimer>

#include "ShdrParser.h"
//#include "ShdrBackEnd.h"
#include <iostream>
#include <signal.h>

#include "Timing.h"
#include "globals.h"
//#include "shdrloop.h"

ParseLoop loop;


int main(int argc, char *argv[])
{
    std::string ExeDirectory;
    std::string path(argv[0]);
#ifdef WIN32
    ExeDirectory = path.substr(0, path.find_last_of('\\') + 1);
#else
    ExeDirectory = path.substr(0, path.find_last_of('/') + 1);
#endif
    std::cout << "Welcome to the XYZIJK Shdr echo - echo contents of out.txt\n";


    QCoreApplication * a= new QCoreApplication(argc, argv);
    loop.configure(ExeDirectory);

    QObject::connect(a, SIGNAL(aboutToQuit()), &loop, SLOT(quit()));

    loop.start();
    return a->exec();
}
