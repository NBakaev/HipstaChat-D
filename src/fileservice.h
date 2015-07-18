#ifndef FILESERVICE_H
#define FILESERVICE_H

#include <QObject>
#include <QFileInfo>

class FileService
{
public:
    FileService();

 static   bool fileExists(QString path) ;
};

#endif // FILESERVICE_H
