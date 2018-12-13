#include "periodicfilewatcher.h"

#include "fileutils.h"
#include <QtConcurrent>

PeriodicFileWatcher::PeriodicFileWatcher(FileModel *fileModel)
    : QObject()
{
    this->fileModel = fileModel;
    connect(&timer, SIGNAL(timeout()), this, SLOT(checkFiles()));
}

PeriodicFileWatcher::~PeriodicFileWatcher()
{
    timer.stop();
}

void PeriodicFileWatcher::setInterval(int minutes)
{
    const int msec = minutes * 60 * 1000;
    if (msec > 0) {
        timer.start(msec);
    }
}

void PeriodicFileWatcher::restart()
{
    timer.start();
}

void PeriodicFileWatcher::checkFiles()
{
    QtConcurrent::run([=]() {
        QFileInfoList files = FileUtils::getFilesRecursive(fileModel->getDir());
        fileModel->update(files);
    });
}
