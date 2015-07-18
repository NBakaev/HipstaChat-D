#include <QObject>
#include <QDebug>
#include "QDateTime"
#include <QtSql>
#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "useraccount.h"
#include "serializeservice.h"
#include "requestdispatcher.h"
#include "dbconnection.h"
#include "hipstahttprequest.h"

#include "message.h"
#include "mytcpsocket.h"

namespace RequestDispatcher {

///////////// COMPARE URL AND HEADER FOR CONTROLLER /////////////
bool isMathController (QString method, QString url, HipstaHTTPRequest *parsedRequest ){
    if (method.compare( parsedRequest->method ) == 0 && url.compare( parsedRequest->url ) == 0)
        return true;
    return false;
}
/////////////////////////////////////////////////////////////////

 // RETURN - [ responseBody, responseCode ]
  QStringList dispatchAndGetResponseBodyAndCode( HipstaHTTPRequest *parsedRequest, Ui::MainWindow *ui, MainWindow *mainWindow ){

     QSqlQuery a_query;

     QString statusCode = QString("500");
     QString responseBody;
     QStringList toReturn;

     QJsonDocument jsonPayloadDocument = QJsonDocument::fromJson(parsedRequest->data.toUtf8());
     QJsonObject jsonObj = jsonPayloadDocument.object();

     qDebug() << jsonObj["name"].toString();

////////////////////////

     if (! mainWindow->hideHTTPRequestBar ){
         ui->textinfo->append( " ---------");
         ui->textinfo->append( "METHOD: <" + parsedRequest->method +">" );
         ui->textinfo->append( "URL: <" + parsedRequest->url +">"  );
         ui->textinfo->append( "PAYLOAD: <" + parsedRequest->data +">"  );
         ui->textinfo->append( QDateTime::currentDateTime().toString());
         ui->textinfo->append( " --------- \n\r");
     }


//////////////////////////////////////////////////////////////////////////////////////////////////////
//                         JUST TEST METHOD                                                         //
//                        'GET /debug HTTP/1.1'                                                     //
//////////////////////////////////////////////////////////////////////////////////////////////////////
    if ( isMathController( QString ("GET"), QString ("/debug"), parsedRequest  ) ){
         responseBody = QString("THIS IS <b>GET</b> METHOD AND <b>DEBUG</b> ;D ");
         statusCode = QString ("500");
         toReturn << responseBody << statusCode;
         return toReturn;
     }

    if ( isMathController( QString ("GET"), QString ("/alive"), parsedRequest  ) ){
         responseBody =  QString("{\"status\": \"OK\"}");
         statusCode = QString ("200");
         toReturn << responseBody << statusCode;
         return toReturn;
     }


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //                         When someone send us message                                             //
    //                        'POST /message HTTP/1.1'                                                  //
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    if (parsedRequest->method.compare( QString ("POST") ) == 0 && parsedRequest->url.compare( QString ("/message") ) == 0){

        QJsonDocument JsonMessgeDoc= QJsonDocument::fromJson ( parsedRequest->data.toUtf8() );

        qDebug() << QString ("-----").toUtf8();
        qDebug() << QString ("Recieved new message ").toUtf8();
//        qDebug() << parsedRequest->data;
        qDebug() << QString ("-----").toUtf8();


        QJsonObject jsonObjMess = JsonMessgeDoc.object();

        HipstaMessage *message = new HipstaMessage();
        message->text =jsonObjMess["text"].toString();
        message->name =jsonObjMess["name"].toString();
        message->receiverID =QString::number(0);
        message->sendDate = QDateTime::currentDateTime().toString();

        a_query.prepare("SELECT * FROM userAccounts where ip = :ipValue " );
        a_query.bindValue(":ipValue", parsedRequest->requestedIP ) ;

        if (!a_query.exec()) {
            qDebug() << "Даже селект не получается, я пас.";
        }

        QSqlRecord rec = a_query.record();

        QString senderFirstName;
        QString senderLastName;
        QString senderNickName;

        int resultCount = 0;
        while (a_query.next()) {
            message->sender = a_query.value(rec.indexOf("id")).toString();
            senderFirstName = a_query.value(rec.indexOf("firstName")).toString();
            senderLastName = a_query.value(rec.indexOf("lastName")).toString();
            senderNickName = a_query.value(rec.indexOf("nickName")).toString();

            resultCount ++;
        };

        if (resultCount == 0){
            // we shoud create new user account
            // because we have not sender in our database :(

            UserAccount *accountToAddNew =  new UserAccount ();

             a_query.prepare("INSERT INTO userAccounts (firstName, ip, port)"
                                           "VALUES (:firstNameValue, :ipValue, :portValue);");
             a_query.bindValue(":firstNameValue", parsedRequest->requestedIP);
             a_query.bindValue(":ipValue", parsedRequest->requestedIP);
             a_query.bindValue(":portValue", parsedRequest->requestedPort);

            if (!a_query.exec()) {
                qDebug() << "Даже ADD NEW ACCOUNT не получается, я пас.";
                qDebug() <<  a_query.lastError().text();
            }


            /////////

            a_query.prepare("SELECT * FROM userAccounts where ip = :ipValue " );
            a_query.bindValue(":ipValue", parsedRequest->requestedIP ) ;

            if (!a_query.exec()) {
                qDebug() << "Даже селект не получается, я пас.";
            }

            rec = a_query.record();

            while (a_query.next()) {
                message->sender = a_query.value(rec.indexOf("id")).toString();
            };

//             qDebug() << a_query.value(rec.indexOf("id")).toString();

        }

        /// save message to localDB and response success

        QString title;
        title = senderFirstName.append(QString (" ") ).append( senderLastName).append( QString (" ( " )).append( senderNickName ).append( QString (" )" ) );

        QJsonDocument JsonMessgeDocMessageText= QJsonDocument::fromJson ( message->text.toUtf8() );
        QJsonObject JsonMessgeDocMessageTextObject = JsonMessgeDocMessageText.object();

        QMessageBox::information(0, title ,JsonMessgeDocMessageTextObject["textMsg"].toString(), QMessageBox::Ok);


        if (SerializeService::saveMessage (message)){
            responseBody =  QString("{\"result\": \"OK\"}");
             statusCode = QString ("200");
        }else{
            responseBody =  QString("{\"result\": \"FAILED\"}");
             statusCode = QString ("500");
        }

        toReturn << responseBody << statusCode;
        return toReturn;
    }



//////////////////////////////////////////////////////////////////////////////////////////////////////
//                         JUST TEST METHODS ( NOT GET )                                            //
//                        '{GET|PUT|POST... } /debug HTTP/1.1'                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////

     if ( parsedRequest->url.compare( QString ("/debug")) == 0 ){
         responseBody += " <br>Requested Method is: <b>" + parsedRequest->method + "</b>";
         responseBody += " <br>Requested Url is: <b>" + parsedRequest->url + "</b> <br><br>";
         responseBody += parsedRequest->headers.join("\n <br>");

         statusCode = QString ("500");
         toReturn << responseBody << statusCode;
         return toReturn;
     }

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                         GET CURRENT USER PROFILE                                                 //
//////////////////////////////////////////////////////////////////////////////////////////////////////
     if ( isMathController( QString ("GET"), QString ("/myaccount"), parsedRequest  ) ){
         UserAccount *myAccount = new UserAccount ();

         if (!a_query.exec("SELECT * FROM currentUser")) {
             qDebug() << "Даже селект не получается, я пас.";
         }
         QSqlRecord rec = a_query.record();


         while (a_query.next()) {

          myAccount->firstName =a_query.value(rec.indexOf("firstName")).toString();
          myAccount->lastName = a_query.value(rec.indexOf("lastName")).toString();
          myAccount->nickName = a_query.value(rec.indexOf("nickName")).toString();
          myAccount->email = a_query.value(rec.indexOf("email")).toString();
          myAccount->avatarUrl = a_query.value(rec.indexOf("avatarUrl")).toString();
          myAccount->id = a_query.value(rec.indexOf("id")).toString();
          myAccount->ip = mainWindow->currentPublicInterface.toString();
          myAccount->port = a_query.value(rec.indexOf("port")).toString();
          myAccount->lastOnlineDate = jsonObj["lastOnlineDate"].toString();
          }

         statusCode = QString ("200");

         responseBody = SerializeService::serializeUserAccount( myAccount );

         toReturn << responseBody << statusCode;
         return toReturn;
     }





//////////////////////////////////////////////////////////////////////////////////////////////////////
//                         GET CURRENT USER PROFILE                                                 //
//////////////////////////////////////////////////////////////////////////////////////////////////////
          if ( isMathController( QString ("GET"), QString ("/room"), parsedRequest  ) ){
              HipstaMessage *messageUnic = new HipstaMessage ();


//              if (!a_query.exec("select distinct on sender * from messages order by max(id);")) {
               if (!a_query.exec("select distinct sender,text  from messages order by id;")) {
                  qDebug() << "Даже селект не получается, я пас.";
              }
              QSqlRecord rec = a_query.record();

              int count = 0;

              responseBody  = " [ ";
              while (a_query.next()) {

               messageUnic->text =a_query.value(rec.indexOf("text")).toString();
               messageUnic->sender = a_query.value(rec.indexOf("sender")).toString();
               messageUnic->receiverID = a_query.value(rec.indexOf("receiverID")).toString();

               if (count > 0) responseBody += ",";
                QJsonObject jjr ;

               jjr["text"] = messageUnic->text;
               jjr["sender"] = messageUnic->sender;
               jjr["receiverID"] = messageUnic->receiverID;

               QJsonDocument docText(jjr);

               responseBody += docText.toJson();

               count ++;
               }

              statusCode = QString ("200");
              responseBody  += " ] ";

            toReturn << responseBody << statusCode;
                return toReturn;
          }




          //////////////////////////////////////////////////////////////////////////////////////////////////////
          //                         GET CURRENT USER PROFILE                                                 //
          //////////////////////////////////////////////////////////////////////////////////////////////////////
                    if ( isMathController( QString ("POST"), QString ("/roomDetails"), parsedRequest  ) ){
                        HipstaMessage *messageUnic = new HipstaMessage ();

                        QJsonDocument des2= QJsonDocument::fromJson (parsedRequest->data.toUtf8());
                        QJsonObject jsonObj2 = des2.object();

                        a_query.prepare("SELECT * from messages where (sender = :senderValue AND receiverID = 0) or (receiverID = :senderValue AND sender = 0) ");
                        a_query.bindValue(":senderValue", jsonObj2["id"].toString() ) ;
                         if (!a_query.exec()) {

                            qDebug() << "Даже селект MESSAGES не получается, я пас.";
                        }
                        QSqlRecord rec = a_query.record();

                        int count = 0;

                        responseBody  = " [ ";
                        while (a_query.next()) {

                         messageUnic->text =a_query.value(rec.indexOf("text")).toString();
                         messageUnic->sender = a_query.value(rec.indexOf("sender")).toString();
                         messageUnic->receiverID = a_query.value(rec.indexOf("receiverID")).toString();
                         messageUnic->sendDate = a_query.value(rec.indexOf("sendDate")).toString();

                         if (count > 0) responseBody += ",";
                         QJsonObject jjr ;

                         jjr["text"] = messageUnic->text;
                         jjr["sender"] = messageUnic->sender;
                         jjr["receiverID"] = messageUnic->receiverID;
                         jjr["sendDate"] = messageUnic->sendDate;

                         QJsonDocument docText(jjr);
                         responseBody += docText.toJson();
                         count ++;
                         }

                        statusCode = QString ("200");
                        responseBody  += " ] ";

                      toReturn << responseBody << statusCode;
                          return toReturn;
                    }

//////////////////////////////////////////////////////////////////////////////////////////////////
     if ( isMathController( QString ("GET"), QString ("/accountSettings"), parsedRequest  ) ){

         if (!a_query.exec("SELECT * FROM accountSettings")) {
             qDebug() << "Даже селект не получается, я пас.";
         }
         QSqlRecord rec = a_query.record();

        QString accountSettingsData;
         while (a_query.next()) {
          accountSettingsData = a_query.value(rec.indexOf("data")).toString();
            }

         statusCode = QString ("200");

          QJsonObject jj ;
          jj["data"] = accountSettingsData;
          QJsonDocument doc(jj);
          QByteArray bytes = doc.toJson();
          responseBody  = bytes;

         toReturn << responseBody << statusCode;
         return toReturn;
     }




     if ( isMathController( QString ("GET"), QString ("/contactList"), parsedRequest  ) ){
         QList<UserAccount> userAccountList;

         if (!a_query.exec("SELECT * FROM userAccounts")) {
             qDebug() << "Даже селект не получается, я пас.";
         }
         QSqlRecord rec = a_query.record();

         UserAccount *userAccount = new UserAccount();
         int count = 0;

         responseBody  = " [ ";

        QString accountSettingsData;
         while (a_query.next()) {
             userAccount = new UserAccount();

             userAccount->firstName =a_query.value(rec.indexOf("firstName")).toString();
             userAccount->lastName = a_query.value(rec.indexOf("lastName")).toString();
             userAccount->nickName = a_query.value(rec.indexOf("nickName")).toString();
             userAccount->email = a_query.value(rec.indexOf("email")).toString();
             userAccount->avatarUrl = a_query.value(rec.indexOf("avatarUrl")).toString();
             userAccount->id = a_query.value(rec.indexOf("id")).toString();
             userAccount->ip = a_query.value(rec.indexOf("ip")).toString();
             userAccount->port = a_query.value(rec.indexOf("port")).toString();
             userAccount->lastOnlineDate = jsonObj["lastOnlineDate"].toString();

             if (count > 0) responseBody += ",";
                responseBody += SerializeService::serializeUserAccount( userAccount );
                count ++;
            }

         statusCode = QString ("200");
                 responseBody  += " ] ";

         toReturn << responseBody << statusCode;
         return toReturn;
     }



     //////////////////////////////////////////////////////////////////////////////////////////////////////
     //                         UPDATE CURRENT accountSettings                                              //
     //////////////////////////////////////////////////////////////////////////////////////////////////////
          if ( isMathController( QString ("POST"), QString ("/accountSettings"), parsedRequest  ) ){

              QJsonDocument des2= QJsonDocument::fromJson (parsedRequest->data.toUtf8());
              QJsonObject jsonObj2 = des2.object();

              a_query.prepare("UPDATE accountSettings SET data = :dataValue ");
              a_query.bindValue(":dataValue", jsonObj2["data"].toString() ) ;

              if (!a_query.exec()) {
                  qDebug() << "Даже UPDATE не получается, я пас.";
                  qDebug() <<  a_query.lastError().text();
              }
              statusCode = QString ("200");
              responseBody = "{}";
              toReturn << responseBody << statusCode;
              return toReturn;
          }


//////////////////////////////////////////////////////////////////////////////////////////////////////
//                         UPDATE CURRENT USER PROFILE                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////////
     if ( isMathController( QString ("PUT"), QString ("/myaccount"), parsedRequest  ) ){
         UserAccount *myAccount =  SerializeService::deserializeUserAccount (   parsedRequest->data.toUtf8() );

         a_query.prepare("UPDATE currentUser SET "
                         "firstName = :firstNameValue, "
                         "lastName = :lastNameValue,"
                         "email = :emailValue,"
                         "avatarUrl = :avatarUrlValue,"
                         "nickName = :nickNameValue");

         a_query.bindValue(":firstNameValue", myAccount->firstName);
         a_query.bindValue(":lastNameValue", myAccount->lastName);
         a_query.bindValue(":emailValue", myAccount->email);
         a_query.bindValue(":avatarUrlValue", myAccount->avatarUrl);
         a_query.bindValue(":nickNameValue", myAccount->nickName);

//         https://pp.vk.me/c323528/v323528025/4e78/MqmvWqNrBIg.jpg

         if (!a_query.exec()) {
             qDebug() << "Даже UPDATE не получается, я пас.";
             qDebug() <<  a_query.lastError().text();
         }
         statusCode = QString ("200");
         responseBody = SerializeService::serializeUserAccount( myAccount );
         toReturn << responseBody << statusCode;
         return toReturn;
     }

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                         MANUALLY ADD ACCOUNT WITH IP... PORT                                     //
//////////////////////////////////////////////////////////////////////////////////////////////////////
               if ( isMathController( QString ("POST"), QString ("/accountList"), parsedRequest  ) ){

                   UserAccount *accountToAdd =  SerializeService::deserializeUserAccount (   parsedRequest->data.toUtf8() );


                    a_query.prepare("INSERT INTO userAccounts (email, nickName, firstName, lastName , avatarUrl, ip, port)"
                                                  "VALUES (:emailValue, :nickNameValue, :firstNameValue, :lastNameValue , :avatarUrlValue, :ipValue, :portValue);");
                    a_query.bindValue(":emailValue", accountToAdd->email);
                    a_query.bindValue(":nickNameValue", accountToAdd->nickName);
                    a_query.bindValue(":firstNameValue", accountToAdd->firstName);
                    a_query.bindValue(":lastNameValue", accountToAdd->lastName);
                    a_query.bindValue(":avatarUrlValue", accountToAdd->avatarUrl);
                    a_query.bindValue(":ipValue", accountToAdd->ip);
                    a_query.bindValue(":portValue", accountToAdd->port);

                   if (!a_query.exec()) {
                       qDebug() << "Даже UPDATE не получается, я пас.";
                       qDebug() <<  a_query.lastError().text();
                   }
                   statusCode = QString ("200");
                   responseBody = SerializeService::serializeUserAccount( accountToAdd );
                   toReturn << responseBody << statusCode;
                   return toReturn;
               }


//////////////////////////////////////////////////////////////////////////////////////////////////////
//                         THAT'S PRIVATE API  WHEN !WE! Want send to another man mesage            //
//////////////////////////////////////////////////////////////////////////////////////////////////////
          if ( isMathController( QString ("POST"), QString ("/sendMessage"), parsedRequest  ) ){

              QJsonDocument des23= QJsonDocument::fromJson (parsedRequest->data.toUtf8());
              QJsonObject jsonObj23 = des23.object();

               HipstaMessage *message = new HipstaMessage();
               message->text = jsonObj23["text"].toString();
               message->sender = jsonObj23["sender"].toString();
               message->name = jsonObj23["name"].toString();
               message->receiverID = jsonObj23["receiverID"].toString();
               message->sendDate = QDateTime::currentDateTime().toString();

               QString ip;

               a_query.prepare("SELECT * FROM userAccounts where id = :idValue " );
               a_query.bindValue(":idValue", message->receiverID ) ;

               if (!a_query.exec()) {
                   qDebug() << "Даже селект не получается, я пас.";
               }

               QSqlRecord rec = a_query.record();

               while (a_query.next()) {
                   ip = QString ( a_query.value(rec.indexOf("ip")).toString() );
               };

//               QString data("{\"text\": \"OK\"}");
               QJsonObject jj ;
               jj["text"] = message->text;
               jj["sender"] = message->sender;
               jj["name"] = message->name;
               jj["receiverID"] = message->receiverID;

               QJsonDocument doc(jj);
               QByteArray data = doc.toJson();

               MyTcpSocket socket;
               QByteArray toSend = QString ("POST /message HTTP/1.1\r\n\r\n").toUtf8() + data + QString ("\r\n\r\n").toUtf8();
               // set data that will be sent
               socket.data = toSend;
               socket.currentPublicInterface = mainWindow->currentPublicInterface;

               // send data to another host
               socket.doConnect(ip);

               a_query.prepare("INSERT INTO messages (text, sender, name, receiverID , sendDate)"
                                             "VALUES (:textValue, :senderValue, :nameValue, :receiverIDValue , :sendDateValue);");
               a_query.bindValue(":textValue", message->text);
               a_query.bindValue(":senderValue", message->sender);
               a_query.bindValue(":nameValue", message->name);
               a_query.bindValue(":receiverIDValue", message->receiverID);
               a_query.bindValue(":sendDateValue", message->sendDate);

              if (!a_query.exec()) {
                  qDebug() << "Даже UPDATE не получается, я пас.";
                  qDebug() <<  a_query.lastError().text();
              }
              statusCode = QString ("200");
              responseBody  = des23.toJson();
              toReturn << responseBody << statusCode;
              return toReturn;
          }



    // wrong request - no such method
     toReturn << QString("{\"error\": \"UNRESOLVED_CONTROLLER_URL\"}") << QString("404");;
     return toReturn;


 }


}
