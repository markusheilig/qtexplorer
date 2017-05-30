#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QAbstractTableModel>
#include <QFileInfoList>
#include <QDir>

enum Columns {
    lastModified,
    filename,

    columns_len
};

class FileModel : public QAbstractTableModel
{
public:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;    
    QFileInfo getFileAt(int row) const;

public slots:        
    void clearFiles();
    void setFiles(const QFileInfoList &files, const QDir &dir);

private:
    QFileInfoList files;
    QDir dir;
};

#endif // FILEMODEL_H
