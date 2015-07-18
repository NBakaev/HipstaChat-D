#ifndef ANGULARCLIENT_H
#define ANGULARCLIENT_H

#include <QDialog>

namespace Ui {
class AngularClient;
}

class AngularClient : public QDialog
{
    Q_OBJECT

public:
    explicit AngularClient(QWidget *parent = 0);
    ~AngularClient();

private:
    Ui::AngularClient *ui;
};

#endif // ANGULARCLIENT_H
