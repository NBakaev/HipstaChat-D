#ifndef DEBUGINFO_H
#define DEBUGINFO_H

#include <QDialog>

namespace Ui {
class DebugInfo;
}

class DebugInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DebugInfo(QWidget *parent = 0);
    ~DebugInfo();

private:
    Ui::DebugInfo *ui;
};

#endif // DEBUGINFO_H
