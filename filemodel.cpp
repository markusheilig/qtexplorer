#include "filemodel.h"

#include <QDateTime>
#include <QtConcurrent>

FileModel::FileModel()
  : QAbstractTableModel()
{
    connect(&watcher, &QFileSystemWatcher::directoryChanged, this, &FileModel::onDirectoryChanged);
    connect(&watcher, &QFileSystemWatcher::fileChanged, this, &FileModel::onFileChanged);
}

QVariant FileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == filename) {
            return "Dateipfad";
        }
        if (section == lastModified) {
            return "zuletzt geändert am";
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

QFileInfo FileModel::getFileAt(int row) const
{
    if (row >= 0 && row < files.count()) {
        return files.at(row);
    }
    return QFileInfo();
}

void FileModel::loadDirectory(const QDir &dir)
{
    if (dir.exists()) {
        this->dir = dir;
        QtConcurrent::run(this, &FileModel::loadDirectoryAsync);
    }
}

QDir FileModel::getDir() const
{
    return dir;
}

bool FileModel::contains(const QFileInfo &info) const
{
    return getRow(info) != -1;
}

QDateTime FileModel::getLastModified(const QFileInfo &info)
{
    for (int i = 0; i < files.size(); i++) {
        QFileInfo fileInfo = files.at(i);
        if (fileInfo.absoluteFilePath() == info.absoluteFilePath()) {
            return fileInfo.lastModified();
        }
    }
    return QDateTime();
}

/*
bool FileModel::removeFile(const QFileInfo &info)
{
    int row = getRow(info);
    bool exists = row != -1;
    if (exists) {
        beginResetModel();
        files.removeAt(row);
        endResetModel();
    }
    return exists;
}
*/
void FileModel::onDirectoryChanged(const QString &path)
{
    // wird das auch bei subdirectories aufgerufen?
    // wenn eine datei im subdirectory erstellt wird?

    QMultiMap<QDateTime, QFileInfo> map;
    QDirIterator iterator(QDir(path), QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        const QFileInfo &fi = iterator.next();
        if (fi.isFile()) {
            int row = getRow(fi);
            if (row == -1) {
                // new file
                watcher.addPath(fi.absoluteFilePath());
                map.insertMulti(fi.lastModified(), fi);
            } else if (fi.lastModified() != getLastModified(fi)) {
                // updated file
                files.removeAt(row);
                map.insertMulti(fi.lastModified(), fi);
            }
        }
    }

    if (!map.isEmpty()) {
        emit beginResetModel();
        QListIterator<QFileInfo> iter(map.values());
        while (iter.hasNext()) {
            files.prepend(iter.next());
        }
        emit endResetModel();
    }
}

void FileModel::onFileChanged(const QString &path)
{
    QFileInfo fileInfo(path);
    int row = getRow(fileInfo);
    bool changedFile = false;
    beginResetModel();
    if (row != -1) {
        files.removeAt(row);
    }
    if (fileInfo.exists()) {
        changedFile = true;
        files.prepend(fileInfo);
    }
    endResetModel();

    if (changedFile) {
        // todo: emit signal 'changed file'
    }
}

void FileModel::loadDirectoryAsync()
{
    beginResetModel();

    // remove watcher
    QStringList paths;
    paths << watcher.directories() << watcher.files();
    if (!paths.isEmpty()) {
        watcher.removePaths(paths);
    }

    // load files
    QMultiMap<QDateTime, QFileInfo> map;
    QDirIterator iterator(dir, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        const QFileInfo &fi = iterator.next();
        if (fi.isFile()) {
            map.insertMulti(fi.lastModified(), fi);
        }
        watcher.addPath(fi.absoluteFilePath());
    }

    // sort
    QFileInfoList sortedFiles;
    QListIterator<QFileInfo> iter(map.values());
    iter.toBack();
    while (iter.hasPrevious()) {
        QFileInfo fileInfo = iter.previous();
        sortedFiles.append(fileInfo);
    }
    this->files = sortedFiles;

    endResetModel();
}

int FileModel::getRow(const QFileInfo &info) const
{
    for (int i = 0; i < files.size(); i++) {
        QFileInfo fileInfo = files.at(i);
        if (fileInfo.absoluteFilePath() == info.absoluteFilePath()) {
            return i;
        }
    }
    return -1;
}

int FileModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return files.count();
}

int FileModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns_len;
}

QVariant FileModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);

    if (role == Qt::DisplayRole) {
        const QFileInfo &file = files.at(index.row());
        if (index.column() == filename) {            
            return file.absoluteFilePath().remove(0, dir.absolutePath().size() + 1);
        }
        if (index.column() == lastModified) {
            return file.lastModified();
        }
    }
    return QVariant();
}
