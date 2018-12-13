#ifndef FILE_H
#define FILE_H

#include <QFileInfo>
#include <QDateTime>

class File
{
public:
    File(const QFileInfo &fileInfo);
    File(const QString &absolutePath, const QDateTime &lastModificationDate);

    QString getAbsolutePath() const { return absolutePath; }
    QDateTime getLastModificationDate() const { return lastModificationDate; }

private:
    QString absolutePath;
    QDateTime lastModificationDate;
};

#endif // FILE_H
