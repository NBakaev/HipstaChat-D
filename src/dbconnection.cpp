#include "dbconnection.h"
#include "fileservice.h"

#include <QtSql>

#include <QFileInfo>
 QSqlDatabase DBconnection::getInstance(){
             QSqlDatabase    instance; // Guaranteed to be destroyed.

            instance = QSqlDatabase::addDatabase("QSQLITE");
            QString dbName ("my_db.db");

//            QString dbName ("/mnt/hgfs/test/QTcpServer01-master/my_db.db");
            instance.setDatabaseName(dbName);

            if (!instance.open()) {
                 qDebug()  << instance.lastError().text();
                   qDebug() << "Что-то пошло не так!";
               }

            return instance;
        }
    DBconnection::DBconnection() {};


