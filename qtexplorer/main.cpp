#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QMetaType>

int main(int argc, char *argv[])
{
    qRegisterMetaType<QDir>("QDir");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
