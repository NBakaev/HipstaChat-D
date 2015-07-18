#include "debuginfo.h"
#include "ui_debuginfo.h"

DebugInfo::DebugInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugInfo)
{
    ui->setupUi(this);
}

DebugInfo::~DebugInfo()
{
    delete ui;
}
