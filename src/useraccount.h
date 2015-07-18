#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include <QObject>

class UserAccount : public QObject
{
    Q_OBJECT
public:

    QString id;
    QString email;
    QString nickName;
    QString firstName;
    QString lastName;
    QString avatarUrl;
    QString createdDate;
    QString lastOnlineDate;

    QString ip;
    QString port;

    explicit UserAccount(QObject *parent = 0);

signals:

public slots:
};

#endif // USERACCOUNT_H
