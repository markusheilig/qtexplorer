#ifndef FILECONTROLLER_H
#define FILECONTROLLER_H

#include <QObject>
#include <QDir>
#include <QFuture>
#include <QAbstractItemView>
#include <QFileSystemWatcher>
#include "filemodel.h"

class FileController : public QObject
{
    Q_OBJECT

public:
    FileController(QObject *parent = 0);
    ~FileController();

    void addObserver(QAbstractItemView *view);
    void loadDirectory(const QDir &dir);
    int numberOfFiles() const;
    QDir getDir() const;
    QFileInfo getFileAt(int row) const;

signals:
    void startedFileLoading(const QDir &dir);
    void finishedFileLoading(const QDir &dir);

private:
    FileModel model;
    QDir dir;
    QFileSystemWatcher watcher;

    void loadDirectoryAsync();

private slots:
    void onFileChanged(const QString &filePath);
};

#endif // FILECONTROLLER_H
