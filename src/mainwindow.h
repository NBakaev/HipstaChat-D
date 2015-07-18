#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QTcpSocket>
#include <QObject>
#include <QByteArray>
#include <QDebug>
#include "hipstahttprequest.h"

namespace Ui {
    class MainWindow;
}

class QTcpServer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QString text;
    // interface from which
    // will be send request to another hosts
    // and we will recieve messages
    QHostAddress currentPublicInterface;
    // don't show http parsed requests
    bool hideHTTPRequestBar = true;

HipstaHTTPRequest* parseRequestBytes ( QString bytes );

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_starting_clicked();
    void on_stoping_clicked();

    void on_stoping_public_clicked();



    void slotReadClient();
    void slotReadClientStaticServer();
    void slotReadClientPublicServer();

    void newuser();
    void newUserStatic();
    void newUserPublic();



    void on_starting_static_clicked();
    void on_starting_public_clicked();


    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_listWidget_activated(const QModelIndex &index);

private:
    Ui::MainWindow *ui;

    QTcpServer *tcpServer;
    QTcpServer *tcpServerStatic;
    QTcpServer *tcpServerPublic;


    int server_status;
    int static_server_status;
    int public_server_status;

    QList<QHostAddress> allInterfaces;

    bool startBrowserOnInit = false;



    QMap<int,QTcpSocket *> SClients;
};

#endif
// MAINWINDOW_H
