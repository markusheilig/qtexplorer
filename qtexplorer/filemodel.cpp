#include "filemodel.h"

#include <QDateTime>
#include <QtConcurrent>

FileModel::FileModel()
  : QAbstractTableModel()
{       
    connect(&timer, SIGNAL(timeout()), this, SLOT(checkForFileChanges()), Qt::QueuedConnection);
}

FileModel::~FileModel()
{
    timer.stop();
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


int FileModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return modelList.count();
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
        const QFileInfo &file = modelList.at(index.row());
        if (index.column() == filename) {
            return file.absoluteFilePath().remove(0, dir.absolutePath().size() + 1);
        }
        if (index.column() == lastModified) {
            return file.lastModified();
        }
    }
    return QVariant();
}


QFileInfo FileModel::getFileAt(int row) const
{
    if (row >= 0 && row < modelList.count()) {
        return modelList.at(row);
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

int minutesToMilliseconds(int minutes)
{
    return minutes * 60 * 1000;
}

void FileModel::setFileCheckInterval(int minutes)
{
    const int msec = minutesToMilliseconds(minutes);
    if (msec > 0) {
        timer.start(msec);
    }
}

QPair<QFileInfoList, QFileInfoList> FileModel::getDirectoriesAndFiles(const QDir &dir) const
{
    QFileInfoList dirs;
    QFileInfoList files;
    QDirIterator it(dir,  QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QFileInfo fi = it.next();
        if (fi.isFile()) {
            files.append(fi);
        } else if (fi.isDir()) {
            dirs.append(fi);
        }
    }

    // iterator also gives us the parent directory of 'dir'
    // but we don't want it! so lets remove it:
    QDir parent = dir;
    if (parent.cdUp()) {
        dirs.removeOne(QFileInfo(parent.absolutePath()));
    }

    return QPair<QFileInfoList, QFileInfoList>(dirs, files);
}

QFileInfoList FileModel::toFileInfoList(const QStringSet &filePaths) const
{
    QFileInfoList list;
    foreach (const QString &filePath, filePaths) {
        list.append(filePath);
    }
    return list;
}

QFileInfoList FileModel::sortByLastModified(const QFileInfoList &files) const
{
    QMultiMap<QDateTime, QFileInfo> map;
    foreach (const QFileInfo &fi, files) {
        map.insertMulti(fi.lastModified(), fi);
    }
    return map.values();
}

QStringSet FileModel::getAbsolutePaths(const QFileInfoList &paths) const
{
    QStringSet absolutePaths;
    foreach (const QFileInfo &fi, paths) {
        absolutePaths.insert(fi.absoluteFilePath());
    }
    return absolutePaths;
}

QStringSet FileModel::checkUpdatedFiles(const QStringSet &filesToCheck) const
{
    QStringSet updatedFiles;
    foreach (const QString &file, filesToCheck) {
        QFileInfo fi = file;
        if (fi.lastModified() != getLastKnownModifiedDate(fi)) {
            updatedFiles.insert(file);
        }
    }
    return updatedFiles;
}

QDateTime FileModel::getLastKnownModifiedDate(const QFileInfo &info) const
{
    return getFileInfoByPath(info.absoluteFilePath()).lastModified();
}

QStringList FileModel::getRelativeFilePaths(const QStringList &absolutePaths) const
{
    QStringList relativePaths;
    foreach (const QString &absolutePath, absolutePaths) {
        relativePaths << dir.relativeFilePath(absolutePath);
    }
    return relativePaths;
}

QFileInfo FileModel::getFileInfoByPath(const QString &absoluteFilePath) const
{
    QListIterator<QFileInfo> iter(modelList);
    while (iter.hasNext()) {
        QFileInfo fi = iter.next();
        if (fi.absoluteFilePath() == absoluteFilePath) {
            return fi;
        }
    }
    return QFileInfo();
}


void FileModel::checkForFileChanges()
{    
    emit beginResetModel();

    QPair<QFileInfoList, QFileInfoList> tuple = getDirectoriesAndFiles(dir);
    QStringSet directories = getAbsolutePaths(tuple.first);
    QStringSet files = getAbsolutePaths(tuple.second);    
    QStringSet newFiles = QStringSet(files).subtract(knownFiles);    
    QStringSet removedFiles = QStringSet(knownFiles).subtract(files);
    QStringSet existingFiles = QStringSet(knownFiles).intersect(files);
    QStringSet updatedFiles = checkUpdatedFiles(existingFiles);

    // update internal state

    QStringSet toRemove = removedFiles.unite(updatedFiles);
    foreach (const QFileInfo &file, toRemove) {
        modelList.removeAll(file);
    }

    QFileInfoList toInsert = toFileInfoList(QStringSet(newFiles).unite(updatedFiles));
    QFileInfoList sorted = sortByLastModified(toInsert);
    foreach (const QFileInfo &fileInfo, sorted) {
        modelList.prepend(fileInfo);
    }

    knownDirectories = directories;
    knownFiles = files;

    // send signals
    if (!newFiles.isEmpty() || !updatedFiles.isEmpty()) {
        emit fileUpdate(getRelativeFilePaths(newFiles.toList()),
                        getRelativeFilePaths(updatedFiles.toList()));
    }

    emit endResetModel();
}

void FileModel::restartTimer()
{
    timer.start();
}

void FileModel::loadDirectoryAsync()
{
    beginResetModel();

    QPair<QFileInfoList, QFileInfoList> tuple = getDirectoriesAndFiles(dir);
    QFileInfoList dirs = tuple.first;
    QFileInfoList files = tuple.second;

    modelList = sortByLastModified(files);
    std::reverse(modelList.begin(), modelList.end());
    knownDirectories = getAbsolutePaths(dirs);
    knownFiles = getAbsolutePaths(files);

    endResetModel();
}
