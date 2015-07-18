#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>

class HipstaMessage : public QObject
{
    Q_OBJECT
public:

    QString text;
    // UserAccount->id
    QString sender;
    QString name;
    // UserAccount->id

    // if zero - that's we
    QString receiverID;
    QString sendDate;


    explicit HipstaMessage(QObject *parent = 0);

private:

signals:

public slots:
};

#endif // MESSAGE_H
