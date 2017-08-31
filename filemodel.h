#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QAbstractTableModel>
#include <QFileInfoList>
#include <QDir>
#include <QFileSystemWatcher>
#include <QSet>
#include <QPair>

enum Columns {
    lastModified,
    filename,

    columns_len
};

typedef QSet<QString> QStringSet;

class FileModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    FileModel();
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;    
    QFileInfo getFileAt(int row) const;

    void loadDirectory(const QDir &dir);
    QDir getDir() const;

signals:
    void fileUpdate(const QStringList &newFiles, const QStringList &updatedFiles);

private:
    QFileInfoList modelList;    
    QStringSet knownFiles;
    QStringSet knownDirectories;
    QDir dir;
    QFileSystemWatcher watcher;

    QPair<QFileInfoList, QFileInfoList> getDirectoriesAndFiles(const QDir &dir) const;
    QFileInfoList toFileInfoList(const QStringSet &filePaths) const;
    QFileInfoList sortByLastModified(const QFileInfoList &files) const;
    QFileInfo getFileInfoByPath(const QString &absoluteFilePath) const;
    QStringSet getAbsolutePaths(const QFileInfoList &paths) const;
    QStringSet checkUpdatedFiles(const QStringSet &filesToCheck) const;
    QDateTime getLastKnownModifiedDate(const QFileInfo &info) const;
    QStringList getRelativeFilePaths(const QStringList &absolutePaths) const;
    void loadDirectoryAsync();    

private slots:
    void onDirectoryChanged(const QString &path);    
};

#endif // FILEMODEL_H
