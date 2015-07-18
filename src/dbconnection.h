#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QtSql>

class DBconnection
{
    public:
        static QSqlDatabase getInstance();

    private:
        DBconnection();
};

#endif // DBCONNECTION_H
