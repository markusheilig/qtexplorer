#include "filecontroller.h"

#include <QtConcurrent>
#include <QListIterator>

FileController::FileController(QObject *parent)
    : QObject(parent)
{
    connect(&watcher, SIGNAL(fileChanged(QString)), this, SLOT(onFileChanged(QString)));
}

FileController::~FileController()
{

}

QDir FileController::getDir() const
{
    return dir;
}

int FileController::numberOfFiles() const
{
    return model.rowCount();
}

void FileController::addObserver(QAbstractItemView *view)
{
    view->setModel(&model);
}

QFileInfoList sortDescendingByLastModifiedDate(const QFileInfoList &unsortedFiles)
{
    QFileInfoList sortedFiles;
    QListIterator<QFileInfo> iterator(unsortedFiles);
    iterator.toBack();
    while (iterator.hasPrevious()) {
        sortedFiles.append(iterator.previous());
    }
    return sortedFiles;
}

void FileController::loadDirectoryAsync()
{
    model.clearFiles();

    QMultiMap<QDateTime, QFileInfo> map;
    QDirIterator iterator(dir, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        const QFileInfo &fi = iterator.next();
        if (fi.isFile()) {
            map.insertMulti(fi.lastModified(), fi);
        }
    }

    QFileInfoList sortedFiles;
    QListIterator<QFileInfo> iter(map.values());
    iter.toBack();
    while (iter.hasPrevious()) {
        sortedFiles.append(iter.previous());
    }
    model.setFiles(sortedFiles, dir);

    emit finishedFileLoading(dir);
}

void FileController::loadDirectory(const QDir &dir)
{
    watcher.removePath(dir.absolutePath());
    if (dir.exists()) {
        this->dir = dir;
        qDebug() << watcher.addPath(dir.absolutePath());
        emit startedFileLoading(dir);
        QtConcurrent::run(this, &FileController::loadDirectoryAsync);
    }
}

QFileInfo FileController::getFileAt(int row) const
{
    return model.getFileAt(row);
}

void FileController::onFileChanged(const QString &filePath)
{
    qDebug() << "changed: " << filePath;
}
