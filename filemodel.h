#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QAbstractTableModel>
#include <QFileInfoList>
#include <QDir>
#include <QFileSystemWatcher>

enum Columns {
    lastModified,
    filename,

    columns_len
};

class FileModel : public QAbstractTableModel
{
public:
    FileModel();
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;    
    QFileInfo getFileAt(int row) const;

    void loadDirectory(const QDir &dir);
    QDir getDir() const;

private:
    QFileInfoList files;
    QDir dir;
    QFileSystemWatcher watcher;

    void loadDirectoryAsync();
    int getRow(const QFileInfo &info) const;
    bool contains(const QFileInfo &info) const;
    //bool removeFile(const QFileInfo &info);
    QDateTime getLastModified(const QFileInfo &info);

private slots:
    void onDirectoryChanged(const QString &path);
    void onFileChanged(const QString &path);
};

#endif // FILEMODEL_H
