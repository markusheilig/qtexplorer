#ifndef PERIODICFILEWATCHER_H
#define PERIODICFILEWATCHER_H

#include "filemodel.h"
#include <QObject>
#include <QTimer>

class PeriodicFileWatcher: public QObject
{
    Q_OBJECT

public:
    PeriodicFileWatcher(FileModel *fileModel);
    ~PeriodicFileWatcher();
    void restart();

public slots:
    void setInterval(int minutes);
    void checkFiles();

private:
    QTimer timer;
    FileModel *fileModel;

};

#endif // PERIODICFILEWATCHER_H
