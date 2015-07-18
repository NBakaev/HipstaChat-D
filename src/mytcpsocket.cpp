#include "mytcpsocket.h"

#include <QNetworkProxy>


MyTcpSocket::MyTcpSocket(QObject *parent) :
    QObject(parent)
{
}

void MyTcpSocket::doConnect(QString ip )
{
    socket = new QTcpSocket(this);
    socket->bind( currentPublicInterface  );

//    socket->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, "192.168.1.198",8888 ));
//    socket->setProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy, "192.168.1.198",8889 ));

    connect(socket, SIGNAL(connected()),this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(socket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
    connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));

    qDebug() << "connecting...";

    qDebug () << ip;
    // this is not blocking call
    socket->QAbstractSocket::connectToHost( ip , 33335);

    // we need to wait...
    if(!socket->waitForConnected(5000))
    {
        qDebug() << "Error: " << socket->errorString();
    }
}
//Use qint32 to ensure that the number have 4 bytes
QByteArray IntToArray(qint32 source)
{
    //Avoid use of cast, this is the Qt way to serialize objects
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}


void MyTcpSocket::connected()
{
    qDebug() << "connected...";
    qDebug() << this->data;

    if(socket->state() == QAbstractSocket::ConnectedState)
       {
           socket->write(data); //write the data itself
           socket->waitForBytesWritten();
       }
}

void MyTcpSocket::disconnected()
{
    qDebug() << "disconnected...";
}

void MyTcpSocket::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
    socket->close();
}

void MyTcpSocket::readyRead()
{
    qDebug() << "reading...";

    // read the data from the socket
    qDebug() << socket->readAll();
}
