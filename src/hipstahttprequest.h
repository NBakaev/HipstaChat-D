#ifndef HIPSTAHTTPREQUEST_H
#define HIPSTAHTTPREQUEST_H

#include <QObject>
#include <QStringList>

class HipstaHTTPRequest
{
public:
    HipstaHTTPRequest();

    QString method;
    QString url;
    QStringList headers;
    QString data;

    QString requestedIP;
    QString requestedPort;

};

#endif // HIPSTAHTTPREQUEST_H
