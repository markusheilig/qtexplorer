#include "filemodel.h"

#include <QDateTime>
#include <QtConcurrent>
#include "fileutils.h"

FileModel::FileModel()
  : QAbstractTableModel()
{       
    fileSorter = new SortByLastModificationFileDate();
}

FileModel::~FileModel()
{    
    delete fileSorter;
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

QDir FileModel::getDir() const
{
    return dir;
}

void FileModel::init(const QDir &dir, const QPair<QFileInfoList, QFileInfoList> &dirsAndFiles)
{
    emit beginResetModel();

    this->dir = dir;
    this->dirsAndFiles = dirsAndFiles;
    modelList = fileSorter->sort(dirsAndFiles);

    emit endResetModel();
}

void FileModel::update(const QPair<QFileInfoList, QFileInfoList> &dirsAndFiles)
{
    emit beginResetModel();

    QSet<QString> knownFiles = FileUtils::getAbsolutePaths(this->dirsAndFiles.second);

    QSet<QString> files = FileUtils::getAbsolutePaths(dirsAndFiles.second);
    QSet<QString> newFiles = QSet<QString>(files).subtract(knownFiles);
    QSet<QString> existingFiles = QSet<QString>(knownFiles).intersect(files);
    QSet<QString> updatedFiles = getUpdatedFiles(existingFiles);

    if (!newFiles.isEmpty() || !updatedFiles.isEmpty()) {
        emit fileUpdate(FileUtils::getRelativeFilePaths(newFiles.toList(), dir),
                        FileUtils::getRelativeFilePaths(updatedFiles.toList(), dir));
    }

    this->dirsAndFiles = dirsAndFiles;
    modelList = fileSorter->sort(dirsAndFiles);

    emit endResetModel();
}

void FileModel::setFileSorter(FileSorter *fileSorter)
{
    emit beginResetModel();

    delete this->fileSorter;
    this->fileSorter = fileSorter;
    modelList = fileSorter->sort(dirsAndFiles);

    emit endResetModel();
}

QSet<QString> FileModel::getUpdatedFiles(const QSet<QString> &filesToCheck) const
{
    QMap<QString, QDateTime> lastKnownModificationDates;
    foreach (const QFileInfo &fi, modelList) {
        lastKnownModificationDates[fi.absoluteFilePath()] = fi.lastModified();
    }

    QSet<QString> updatedFiles;
    foreach (const QString &file, filesToCheck) {
        QFileInfo fi = file;
        if (fi.lastModified() != lastKnownModificationDates[file]) {
            updatedFiles.insert(file);
        }
    }
    return updatedFiles;
}
