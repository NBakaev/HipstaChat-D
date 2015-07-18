#include "angularclient.h"
#include "ui_angularclient.h"

AngularClient::AngularClient(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AngularClient)
{
    ui->setupUi(this);
}

AngularClient::~AngularClient()
{
    delete ui;
}
