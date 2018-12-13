#include "file.h"


File::File(const QFileInfo &fileInfo)
{
    absolutePath = fileInfo.absoluteFilePath();
    lastModificationDate = fileInfo.lastModified();
}

File::File(const QString &absolutePath, const QDateTime &lastModificationDate)
{
    this->absolutePath = absolutePath;
    this->lastModificationDate =lastModificationDate;
}
