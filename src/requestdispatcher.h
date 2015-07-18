#ifndef REQUESTDISPATCHER_H
#define REQUESTDISPATCHER_H
#include <QStringList>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "hipstahttprequest.h"

namespace RequestDispatcher
{
    QStringList dispatchAndGetResponseBodyAndCode (HipstaHTTPRequest *, Ui::MainWindow *ui, MainWindow *w );
    bool isMathController (QString method, QString url, HipstaHTTPRequest *parsedRequest );
}




#endif // REQUESTDISPATCHER_H
