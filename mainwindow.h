#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QFileInfo>
#include <QSettings>

#include "filemodel.h"
#include "filecontroller.h"
#include <QFutureWatcher>

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
    FileController *fileController;

private slots:
    void loadAndApplySettings();
    void saveSettings();
    void onOpenDirectoryClicked();
    void onStartedFileLoading(const QDir &dir);
    void onFinishedFileLoading(const QDir &dir);

    void handleDoubleClick(const QModelIndex &index);
    void openFileExplorerAt(const QString &pathIn);
};

#endif // MAINWINDOW_H
