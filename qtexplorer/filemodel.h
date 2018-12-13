#ifndef FILEMODEL_H
#define FILEMODEL_H

#include "filesorter.h"

#include <QAbstractTableModel>
#include <QFileInfoList>
#include <QDir>
#include <QSet>
#include <QPair>
#include <QTimer>

enum Columns {
    lastModified,
    filename,

    columns_len
};

class FileModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    FileModel();
    ~FileModel();
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;    
    QFileInfo getFileAt(int row) const;

    QDir getDir() const;    

    void init(const QDir &dir, const QFileInfoList &files);
    void update(const QFileInfoList &files);
    void setFileSorter(FileSorter *fileSorter);

signals:
    void fileUpdate(const QStringList &newFiles, const QStringList &updatedFiles);

private:
    QFileInfoList files;
    QFileInfoList modelList;    
    QDir dir;
    FileSorter *fileSorter;

    QSet<QString> getUpdatedFiles(const QSet<QString> &filesToCheck) const;
};

#endif // FILEMODEL_H
