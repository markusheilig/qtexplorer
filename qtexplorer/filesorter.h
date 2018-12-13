#ifndef FILESORTER_H
#define FILESORTER_H

#include "fileutils.h"
#include <QDateTime>
#include <QDirIterator>
#include <QFileInfoList>
#include <QMultiMap>
#include <QPair>
#include <QDebug>

class FileSorter {
public:
    virtual QFileInfoList sort(const QFileInfoList &filesToSort) = 0;
    virtual ~FileSorter() {}
};

/**
 * Sort files descending by last file modification date.
 */
class SortByLastModificationFileDate : public FileSorter {
public:
    QFileInfoList sort(const QFileInfoList &filesToSort)
    {
        QMultiMap<QDateTime, QFileInfo> map;
        foreach (const QFileInfo &fi, filesToSort) {
            map.insertMulti(fi.lastModified(), fi);
        }

        // reverse order to sort descending
        QFileInfoList sortedFiles = map.values();
        std::reverse(sortedFiles.begin(), sortedFiles.end());
        return sortedFiles;
    }
};

/**
 * Sort files descending by last directory modification date.
 */
class SortByLastModificationDirectoryDate : public FileSorter {
public:
    QFileInfoList sort(const QFileInfoList &filesToSort)
    {
        SortByLastModificationFileDate fileSorter;
        QFileInfoList sortedFiles = fileSorter.sort(filesToSort);

        QStringList dirPaths;
        Q_FOREACH (const QFileInfo &fi, sortedFiles) {
            QString dirPath = fi.absolutePath();
            if (!dirPaths.contains(dirPath)) {
                dirPaths.append(dirPath);
            }
        }

        QFileInfoList result;
        foreach (const QString &dirPath, dirPaths) {
            QDir dir = dirPath;
            const QFileInfoList &files = FileUtils::getFiles(dir);
            const QFileInfoList &sortedFiles = fileSorter.sort(files);
            Q_FOREACH (const QFileInfo &fi, sortedFiles) {
                result.append(fi);
            }
        }

        return result;
    }

};

#endif // FILESORTER_H
