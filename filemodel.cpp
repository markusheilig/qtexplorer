#include "filemodel.h"

#include <QDateTime>

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

void FileModel::clearFiles()
{    
    beginResetModel();
    files.clear();
    endResetModel();
}

void FileModel::setFiles(const QFileInfoList &files, const QDir &dir)
{
    beginResetModel();
    this->files = files;
    this->dir = dir;
    endResetModel();
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
