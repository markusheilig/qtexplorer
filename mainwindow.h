#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QFileInfo>
#include <QSettings>

#include "filemodel.h"
#include <QFutureWatcher>
#include <QSystemTrayIcon>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;        
    FileModel model;
    QSystemTrayIcon *trayIcon;

private slots:
    void loadAndApplySettings();
    void saveSettings();
    void onOpenDirectoryClicked();
    void onStartedFileLoading();
    void onFinishedFileLoading();
    void onFileUpdate(QStringList newFiles, QStringList updatedFiles);

    void handleDoubleClick(const QModelIndex &index);
    void openFileExplorerAt(const QString &pathIn);    
};

#endif // MAINWINDOW_H
