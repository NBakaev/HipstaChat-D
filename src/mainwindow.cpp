#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonObject>
#include <iostream>
#include <sstream>
#include <QStringList>
#include <QFileInfo>
#include <iostream>
#include <fstream>
#include <QMimeDatabase>
#include <QtSql>
#include <QDesktopServices>


#include "message.h"
#include "requestdispatcher.h"
#include "serializeservice.h"
#include "hipstahttprequest.h"
#include "angularclient.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    this->allInterfaces= QNetworkInterface::allAddresses();
    QStringList interf;

    interf.append( QString ("LISTEN ALL INTERFACES") );

    for (QHostAddress interfaceCurrent : allInterfaces){
        interf.append( interfaceCurrent.toString() );
    }

    ui->listWidget->addItems( interf );

    currentPublicInterface = QHostAddress::Any;


    // start private server
    this->on_starting_clicked();
    // start static server
    this->on_starting_static_clicked ();
    // start public server
    this->on_starting_public_clicked ();


}

MainWindow::~MainWindow(){
    delete ui;

    // private api server started
    server_status=0;
    // static (AngularJS) server started
    static_server_status=0;
    // public ( other users ) server started
    public_server_status=0;
    // open static server page
    // in default external browser
    startBrowserOnInit = true;
}

void MainWindow::on_starting_clicked(){
    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newuser()));
    if (!tcpServer->listen(QHostAddress::Any, 33333) && server_status==0) {
        qDebug() <<  QObject::tr("Unable to start the PRIVATE API server: %1.").arg(tcpServer->errorString());
        ui->textinfo->append(tcpServer->errorString());
    } else {
        server_status=1;
        qDebug() << tcpServer->isListening() << "TCPSocket listen on port";
        ui->textinfo->append(QString::fromUtf8("Сервер PRIVATE API запущен!"   ));
    }
}


void MainWindow::on_starting_static_clicked(){
    tcpServerStatic = new QTcpServer(this);
    connect(tcpServerStatic, SIGNAL(newConnection()), this, SLOT(newUserStatic()));
    if (!tcpServerStatic->listen(QHostAddress::Any, 33334) && static_server_status==0) {
        qDebug() <<  QObject::tr("Unable to start the STATIC API server: %1.").arg(tcpServerStatic->errorString());
        ui->textinfo->append(tcpServerStatic->errorString());
    } else {
        static_server_status=1;
        qDebug() << tcpServerStatic->isListening() << "TCPSocket listen on port";
        ui->textinfo->append(QString::fromUtf8("Сервер STATIC API запущен!"   ));
        if (startBrowserOnInit){
            QDesktopServices::openUrl(QUrl("http://localhost:33334", QUrl::TolerantMode));
        }


    }
}


void MainWindow::on_starting_public_clicked(){
    tcpServerPublic = new QTcpServer(this);
    connect(tcpServerPublic, SIGNAL(newConnection()), this, SLOT(newUserPublic()));

    if (!tcpServerPublic->listen(currentPublicInterface, 33335) && public_server_status==0) {
        qDebug() <<  QObject::tr("Unable to start the PUBLIC API server: %1.").arg(tcpServerPublic->errorString());
        ui->textinfo->append(tcpServerPublic->errorString());
    } else {
        public_server_status=1;
        qDebug() << tcpServerPublic->isListening() << "TCPSocket listen on port";
        ui->textinfo->append(QString::fromUtf8("Сервер PUBLIC API запущен: "   ) + currentPublicInterface.toString() );

    }
}





void MainWindow::on_stoping_clicked(){
    if(server_status==1){
        foreach(int i,SClients.keys()){
            QTextStream os(SClients[i]);
            os.setAutoDetectUnicode(true);
            os << QDateTime::currentDateTime().toString() << "\n";
            SClients[i]->close();
            SClients.remove(i);
        }
        tcpServer->close();
        ui->textinfo->append(QString::fromUtf8("Сервер  PRIVATE API остановлен!"));
        qDebug() << QString::fromUtf8("Сервер PRIVATE API остановлен!");
        server_status=0;
    }
}



/////////////// STOP PUBLIC SERVER ////////////

void MainWindow::on_stoping_public_clicked(){
    if(public_server_status==1){
        foreach(int i,SClients.keys()){
            QTextStream os(SClients[i]);
            os.setAutoDetectUnicode(true);
            os << QDateTime::currentDateTime().toString() << "\n";
            SClients[i]->close();
            SClients.remove(i);
        }
        tcpServerPublic->close();
        ui->textinfo->append(QString::fromUtf8("Сервер  PUBLIC API остановлен!"));
        qDebug() << QString::fromUtf8("Сервер PUBLIC API остановлен!");
        public_server_status=0;
    }
}




//////////////////////////////////////////////////////////////////////////////////////////////////////
//                         CALLBACK, WHEN WE HAVE                                                   //
//                         NEW CONNECTION PRIVATE API REQUEST                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::newuser(){
    if(server_status==1){
        ui->textinfo->append(QString::fromUtf8("У нас новое соединение PRIVATE API!"));
        QTcpSocket* clientSocket=tcpServer->nextPendingConnection();
        ui->textinfo->append( QString( clientSocket->peerAddress().toString() +
                                       " :" + QString::number(clientSocket->peerPort() ) +
                                       " ( " + clientSocket->peerName() + " )") );

        int idusersocs=clientSocket->socketDescriptor();
        SClients[idusersocs]=clientSocket;
        connect(SClients[idusersocs],SIGNAL(readyRead()),this, SLOT(slotReadClient()));
    }
}


void MainWindow::newUserStatic(){
    if(static_server_status==1){
//        ui->textinfo->append(QString::fromUtf8("У нас новое соединение STATIC API!"));
        QTcpSocket* clientSocket=tcpServerStatic->nextPendingConnection();
        ui->textinfo->append( QString( clientSocket->peerAddress().toString() +
                                       " :" + QString::number(clientSocket->peerPort() ) +
                                       " ( " + clientSocket->peerName() + " )") );

        int idusersocs=clientSocket->socketDescriptor();
        SClients[idusersocs]=clientSocket;
        connect(SClients[idusersocs],SIGNAL(readyRead()),this, SLOT(slotReadClientStaticServer()));
    }
}



void MainWindow::newUserPublic(){
    if(public_server_status==1){
        ui->textinfo->append(QString::fromUtf8("У нас новое соединение PUBLIC API!"));
        QTcpSocket* clientSocket=tcpServerPublic->nextPendingConnection();
        ui->textinfo->append( QString( clientSocket->peerAddress().toString() +
                                       " :" + QString::number(clientSocket->peerPort() ) +
                                       " ( " + clientSocket->peerName() + " )") );

        int idusersocs=clientSocket->socketDescriptor();
        SClients[idusersocs]=clientSocket;
        connect(SClients[idusersocs],SIGNAL(readyRead()),this, SLOT(slotReadClientPublicServer()));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                         PARSE HTTP PROTOCOL REQUEST                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////////

HipstaHTTPRequest* MainWindow::parseRequestBytes ( QString bytes ){
    // split request HTTP headers by line
    QStringList lines = bytes.split( "\r\n");
    // lines number in request
    int stringNum = 0;
    // does clien sent payload with request
    bool isPayloadHere = false;

    // ('GET / HTTP/1.1')
    QStringList firstHeader;
    // 'HOST', Cookie ...
    QStringList secondaryHeades;
    // payload string
    QString requestPaylod;

    foreach( QString line, lines ) {
        if (stringNum == 0){
            firstHeader = line.split( " " );
        }else{
            // check for empty line as separator between headers and payload
            if (line.compare( QString("") ) == 0 || line.compare( QString(" ") ) == 0  ){
                isPayloadHere = true;
                continue;
            }
            // add line to payload string
            if (isPayloadHere){
              requestPaylod += line;
              continue;
            }
            //if we here - that's
            // mean we are in secondary headers now
            // just add to List
            secondaryHeades << line;
        }
        stringNum++;
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                         BELOW WE HAVE PARSED REQUEST                                             //
//                        AND WILL DISPATCH AND WRITE TO SOCKET                                     //
//////////////////////////////////////////////////////////////////////////////////////////////////////

    HipstaHTTPRequest *parsedRequest = new HipstaHTTPRequest();

    // GET / POST / PUT / OPTIONS / DELETE etc
    parsedRequest->method = firstHeader.value(0);

    // myaccount / deleteById{id} etc
    parsedRequest->url = firstHeader.value(1);

    // additional header - 'Cache-Control', 'Accept', 'Accept-Language'...
    parsedRequest->headers = secondaryHeades;

    // paylod ( with POST, PUT...)
    parsedRequest->data = requestPaylod;

    return parsedRequest;
}


void MainWindow::slotReadClient(){

    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    int idusersocs=clientSocket->socketDescriptor();
    QTextStream os(clientSocket);
    os.setAutoDetectUnicode(true);

    // read all data from TCP connection
    // that was sent
    QString myString(clientSocket->readAll());
    // parse reqiest headers
    HipstaHTTPRequest* parsedRequest = this->parseRequestBytes(myString);

    QStringList dispatchered = RequestDispatcher::dispatchAndGetResponseBodyAndCode (  parsedRequest, ui, this  );

 int codeResponse = 500;

 if (dispatchered.value(1).compare( QString("200") ) == 0  ) codeResponse = 200;
 if (dispatchered.value(1).compare( QString("404") ) == 0  ) codeResponse = 404;
 if (dispatchered.value(1).compare( QString("500") ) == 0  ) codeResponse = 500;

// 'OPTIONS' - Ajax request that make browser auto
 if (parsedRequest->method.compare( QString ("OPTIONS")) == 0){ codeResponse = 200; }

 QString headers("HTTP/1.1 "+ QString::number(codeResponse)+" OK\r\n");

 if (codeResponse == 200){
     headers += "Content-Type: application/json; charset=\"utf-8\"\r\n";
 }else{
     headers += "Content-Type: text/html; charset=\"utf-8\"\r\n";
 }

  headers += "Access-Control-Allow-Headers: X-PINGOTHER, Origin, X-Requested-With, Content-Type, Accept, Authorization\r\n";
  headers += "Access-Control-Allow-Origin: *\r\n";
  headers += "Server: HipstaChat-D\r\n";
  headers += "Access-Control-Allow-Methods: GET, POST, OPTIONS, PUT, DELETE\r\n";

 // HTTP: end of headers
 headers += "\r\n";

 QString responseBody = dispatchered.value(0);
 QString reponseToSend = headers + responseBody;
    // SEND RESPONSE
    os << reponseToSend;

    // CLOSE SOCKET - WE ARE REST
    clientSocket->close();
    SClients.remove(idusersocs);
}


// static HTTP server
void MainWindow::slotReadClientStaticServer(){

    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    int idusersocs=clientSocket->socketDescriptor();
    QTextStream  os(clientSocket);
    os.setAutoDetectUnicode(true);

    QString myString(clientSocket->readAll());

    HipstaHTTPRequest* parsedRequest = this->parseRequestBytes(myString);

    if (parsedRequest->url.compare( QString (" ") ) == 0 || parsedRequest->url.compare( QString ("/") ) == 0   ){
        parsedRequest->url = QString ("index.html");
    }

    QString file = QDir::currentPath() + QString ("/app/" ) + parsedRequest->url ;

      streampos size;
      char * memblock;
      QByteArray responseBody  ;

      ifstream file2( file.toStdString().c_str() , ios::in|ios::binary|ios::ate);
      if (file2.is_open())
      {
        size = file2.tellg();
        memblock = new char [size];
        file2.seekg (0, ios::beg);
        file2.read (memblock, size);
        file2.close();

        responseBody = QByteArray( memblock , size);

      }

        QMimeDatabase db;
           QMimeType type = db.mimeTypeForFile(file);

           int codeResponse = 200;



 if (parsedRequest->method.compare( QString ("OPTIONS")) == 0){ codeResponse = 200; }

 QByteArray  headers = QString("HTTP/1.1 "+ QString::number(codeResponse)+" OK\r\n").toUtf8();
 headers += "Content-Type: " + type.name() + ";\r\n";


 // HTTP: end of headers
 headers += "\r\n";

 QByteArray reponseToSend = headers + responseBody;

    // SEND RESPONSE
    os << reponseToSend;

    // CLOSE SOCKET - WE ARE REST
    clientSocket->close();
    SClients.remove(idusersocs);
}





// public server request
void MainWindow::slotReadClientPublicServer(){

    QTcpSocket* clientSocket = (QTcpSocket*)sender();
    int idusersocs=clientSocket->socketDescriptor();
    QTextStream os(clientSocket);
    os.setAutoDetectUnicode(true);

    QString myString(clientSocket->readAll());

    HipstaHTTPRequest* parsedRequest = this->parseRequestBytes(myString);

    parsedRequest->requestedIP = clientSocket->peerAddress().toString() ;
    parsedRequest->requestedPort  = QString::number(clientSocket->peerPort() );

    bool allowedRequest = false;

    int codeResponse= 500;
    QString responseBody ;


    /// if another user want to see our profile - just dispatch as
    /// we request it information
    if (parsedRequest->method.compare( QString ("GET") ) == 0 && parsedRequest->url.compare( QString ("/profile") ) == 0){
      parsedRequest->url = QString ("/myaccount");
      allowedRequest = true;
    }

    // just some heartbeat
    if (parsedRequest->method.compare( QString ("GET") ) == 0 && parsedRequest->url.compare( QString ("/alive") ) == 0){
      allowedRequest = true;
    }

    // send to this user message
    if (parsedRequest->method.compare( QString ("POST") ) == 0 && parsedRequest->url.compare( QString ("/message") ) == 0){
      allowedRequest = true;
    }


  QStringList dispatchered;
    if ( allowedRequest ){
       dispatchered  = RequestDispatcher::dispatchAndGetResponseBodyAndCode (  parsedRequest, ui, this  );


 if (dispatchered.value(1).compare( QString("200") ) == 0  ) codeResponse = 200;
 if (dispatchered.value(1).compare( QString("404") ) == 0  ) codeResponse = 404;
 if (dispatchered.value(1).compare( QString("500") ) == 0  ) codeResponse = 500;

 responseBody = dispatchered.value(0);

    }else{
        codeResponse = 403;
        responseBody =  QString("{\"status\": \"NOT_ALLOWED_METHOD\"}");
    }

 if (parsedRequest->method.compare( QString ("OPTIONS")) == 0){ codeResponse = 200; }

 QString headers("HTTP/1.1 "+ QString::number(codeResponse)+" OK\r\n");

 if (codeResponse == 200){
     headers += "Content-Type: application/json; charset=\"utf-8\"\r\n";
 }else{
     headers += "Content-Type: text/html; charset=\"utf-8\"\r\n";
 }

  headers += "Access-Control-Allow-Headers: X-PINGOTHER, Origin, X-Requested-With, Content-Type, Accept, Authorization\r\n";
  headers += "Access-Control-Allow-Origin: *\r\n";
  headers += "Server: HipstaChat-D\r\n";
  headers += "Access-Control-Allow-Methods: GET, POST, OPTIONS, PUT, DELETE\r\n";

 // HTTP: end of headers
 headers += "\r\n";

 QString reponseToSend = headers + responseBody;
    // SEND RESPONSE !!!
    os << reponseToSend;

    // CLOSE SOCKET - WE ARE REST
    clientSocket->close();
    SClients.remove(idusersocs);
}





void MainWindow::on_pushButton_clicked()
{
    ui->textinfo->clear();
}


void MainWindow::on_pushButton_2_clicked()
{
//   qDebug() <<  this->allInterfaces;
}

void MainWindow::on_listWidget_activated(const QModelIndex &index)
{

    qDebug() << index.row();
    // stop public server
    this->on_stoping_public_clicked();

    // get clicked interface by clicked index
    if ( index.row() == 0 ){
        this->currentPublicInterface = QHostAddress::Any;
    }else{
        this->currentPublicInterface = allInterfaces.at(index.row() - 1) ;
    }


    // start public server
    this->on_starting_public_clicked();
//    qDebug() << index;
}
