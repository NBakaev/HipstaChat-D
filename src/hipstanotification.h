#ifndef HIPSTANOTIFICATION_H
#define HIPSTANOTIFICATION_H
#include <QObject>

class HipstaNotification
{
public:

    bool shown;
    QString type;
    QString details;

    HipstaNotification();
};

#endif // HIPSTANOTIFICATION_H
