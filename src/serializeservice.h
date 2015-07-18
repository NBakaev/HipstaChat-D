#ifndef SERIALIZESERVICE
#define SERIALIZESERVICE

#include "useraccount.h"
#include <QObject>
#include "message.h";
namespace SerializeService
{
    QByteArray serializeUserAccount (UserAccount *account);

     bool saveMessage (HipstaMessage *message);

    UserAccount* deserializeUserAccount (QByteArray  s);
}



#endif // SERIALIZESERVICE

