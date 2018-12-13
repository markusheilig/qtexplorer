#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QDir>
#include <QSet>
#include <QStringList>

class FileUtils
{
public:
    static QStringList getRelativeFilePaths(const QStringList &absolutePaths, const QDir &dir);
    static QPair<QFileInfoList, QFileInfoList> getDirectoriesAndFiles(const QDir &dir);
    static QFileInfoList toFileInfoList(const QSet<QString> &filePaths);
    static QSet<QString> getAbsolutePaths(const QFileInfoList &paths);
    static QFileInfoList getFiles(const QDir &dir);

private:
    FileUtils();
};

#endif // FILEUTILS_H
