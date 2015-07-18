#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>


#include <QtNetwork>
#include <QTcpSocket>

class MyTcpSocket : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = 0);

    void doConnect( QString ip );
    QByteArray data;
    QHostAddress currentPublicInterface;


signals:

public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
//    void setDateToSend(QByteArray data);

private:
    QTcpSocket *socket;

};

#endif // MYTCPSOCKET_H
