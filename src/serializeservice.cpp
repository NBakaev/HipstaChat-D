
#include "serializeservice.h"
#include <QJsonObject>
#include "QJsonDocument"

#include "useraccount.h"

#include "message.h"
#include <QtSql>


namespace SerializeService
{

    bool saveMessage (HipstaMessage *message){
        QSqlQuery a_query;
         a_query.prepare("INSERT INTO messages (text, sender, name , receiverID, sendDate )"
                                       "VALUES (:textValue, :senderValue, :nameValue, :receiverIDValue, :sendDateValue  );");

         a_query.bindValue(":textValue", message->text);
         a_query.bindValue(":senderValue", message->sender);
         a_query.bindValue(":nameValue", message->name);
         a_query.bindValue(":receiverIDValue", message->receiverID);
         a_query.bindValue(":sendDateValue", message->sendDate);


        if (!a_query.exec()) {
            qDebug() << "Даже ADD NEW MESSAGE не получается, я пас.";
            qDebug() <<  a_query.lastError().text();
            return false;
        }
        return true;
    }


    QByteArray serializeUserAccount (UserAccount *account){

        QJsonObject jj ;
        jj["id"] = account->id;
        jj["email"] = account->email;
        jj["nickName"] = account->nickName;
        jj["firstName"] = account->firstName;
        jj["lastName"] = account->lastName;
        jj["avatarUrl"] = account->avatarUrl;
        jj["createdDate"] = account->createdDate;
        jj["lastOnlineDate"] = account->lastOnlineDate;

        jj["ip"] = account->ip;
        jj["port"] = account->port;


        QJsonDocument doc(jj);
        QByteArray bytes = doc.toJson();

        return bytes;
    }


    UserAccount* deserializeUserAccount (QByteArray  s){
        UserAccount *userAccount = new UserAccount();

         QJsonDocument des= QJsonDocument::fromJson (s);
         QJsonObject jsonObj = des.object();

         userAccount->firstName =jsonObj["firstName"].toString();
         userAccount->lastName = jsonObj["lastName"].toString();
         userAccount->nickName = jsonObj["nickName"].toString();
         userAccount->email = jsonObj["email"].toString();
         userAccount->avatarUrl = jsonObj["avatarUrl"].toString();
         userAccount->id = jsonObj["id"].toString();
         userAccount->ip = jsonObj["ip"].toString();
         userAccount->port = jsonObj["port"].toString();
         userAccount->lastOnlineDate = jsonObj["lastOnlineDate"].toString();


        return userAccount;
    };

}

