#include "fileutils.h"

#include <QDirIterator>

QStringList FileUtils::getRelativeFilePaths(const QStringList &absolutePaths, const QDir &dir)
{
    QStringList relativePaths;
    foreach (const QString &absolutePath, absolutePaths) {
        relativePaths << dir.relativeFilePath(absolutePath);
    }

    return relativePaths;
}

QFileInfoList FileUtils::getFilesRecursive(const QDir &dir)
{
    QFileInfoList files;
    QDirIterator it(dir,  QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo fi = it.next();
        if (fi.isFile()) {
            files.append(fi);
        }
    }

    return files;
}

QFileInfoList FileUtils::toFileInfoList(const QSet<QString> &filePaths)
{
    QFileInfoList list;
    foreach (const QString &filePath, filePaths) {
        list.append(filePath);
    }
    return list;
}

QSet<QString> FileUtils::getAbsolutePaths(const QFileInfoList &paths)
{
    QSet<QString> absolutePaths;
    foreach (const QFileInfo &fi, paths) {
        absolutePaths.insert(fi.absoluteFilePath());
    }
    return absolutePaths;
}

QFileInfoList FileUtils::getFiles(const QDir &dir)
{
    QFileInfoList files;
    QDirIterator it(dir);
    while (it.hasNext()) {
        QFileInfo fi = it.next();
        if (fi.isFile()) {
            files.append(fi);
        }
    }
    return files;
}

FileUtils::FileUtils()
{

}
