#include <QApplication>
#include "mainwindow.h"
#include "dbconnection.h"
#include <QtSql>
//#include "qwebview.h"

#include "debuginfo.h"

#include "angularclient.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    // Page with Ksenia chat
//    DebugInfo d;
//    d.show();

//    AngularClient dd;
//    dd.show();


    QSqlDatabase db = DBconnection::getInstance();

    return a.exec();
}
