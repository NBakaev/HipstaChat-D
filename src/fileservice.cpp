#include "fileservice.h"
#include <QObject>
#include <QFileInfo>


FileService::FileService()
{

}


   bool FileService::fileExists(QString path) {
    QFileInfo checkFile(path);
    // check if file exists and if yes: Is it really a file and no directory?
    if (checkFile.exists() && checkFile.isFile()) {
        return true;
    } else {
        return false;
    }
}

