#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QFileInfo>
#include <QSettings>

#include "filemodel.h"
#include "periodicfilewatcher.h"
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
    void hideEvent(QHideEvent *event);    
    void showEvent(QShowEvent *event);

private:
    Ui::MainWindow *ui;
    FileModel model;
    QSystemTrayIcon *trayIcon;
    PeriodicFileWatcher *periodicFileWatcher;
    const QIcon blueBinocularsWithEyes;
    const QIcon redBinocularsWithEyes;


private slots:
    void loadAndApplySettings();
    void saveSettings();
    void onOpenDirectoryClicked();
    void onStartedFileLoading();
    void onFinishedFileLoading();
    void onFileUpdate(const QStringList &newFiles, const QStringList &updatedFiles);
    void onCheckForUpdatesClicked();    
    void showWindowAndBringToFront();

    void handleDoubleClick(const QModelIndex &index);
    void openFileExplorerAt(const QString &pathIn);
    void updateSortType(const QString &sortType);

    void showAboutMessageBox();
};

#endif // MAINWINDOW_H
