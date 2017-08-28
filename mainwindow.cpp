#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDirIterator>
#include <QDir>
#include <QFileInfoList>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSystemTrayIcon>

#include "settingscontroller.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);    

    fileController = new FileController(this);
    fileController->addObserver(ui->tableView);

    connect(fileController, SIGNAL(startedFileLoading(QDir)), this, SLOT(onStartedFileLoading(QDir)));
    connect(fileController, SIGNAL(finishedFileLoading(QDir)), this, SLOT(onFinishedFileLoading(QDir)));        
    connect(fileController, SIGNAL(directoryChanged(QFileInfo)), this, SLOT(onDirectoryChanged(QFileInfo)));

    connect(ui->openDirectory, SIGNAL(clicked(bool)), this, SLOT(onOpenDirectoryClicked()));
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(handleDoubleClick(QModelIndex)));

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);    

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/panda.jpg"));
    trayIcon->show();

    loadAndApplySettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadAndApplySettings()
{
    Settings s = SettingsController::parse();
    if (s.valid) {
        move(s.windowPosition);
        resize(s.windowSize);
        fileController->loadDirectoryAsync(QDir(s.lastOpenedDir));        
    }
}

void MainWindow::saveSettings()
{
    Settings s;
    s.lastOpenedDir = fileController->getDir().absolutePath();
    s.windowPosition = pos();
    s.windowSize = size();
    SettingsController::save(s);
}

void MainWindow::closeEvent(QCloseEvent *event)
{    
    saveSettings();
    event->accept();
}

QString italic(const QString &s) {
    return "<i>" + s + "</i>";
}

void MainWindow::onOpenDirectoryClicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Verzeichnis auswählen"), fileController->getDir().absolutePath(),
                                                     QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);
    if (!path.isEmpty()) {
        fileController->loadDirectoryAsync(QDir(path));
    }
}

void MainWindow::onStartedFileLoading(const QDir &dir)
{
    ui->openDirectory->setEnabled(false);
    ui->currentDirectory->setText("Lade Dateien in " + italic(dir.absolutePath()));
}

void MainWindow::onFinishedFileLoading(const QDir &dir)
{
    const QString path = dir.absolutePath();
    const int numberOfFiles = fileController->numberOfFiles();
    if (numberOfFiles == 0) {
        ui->currentDirectory->setText("Verzeichnis " + italic(path) + " enthält keine Dateien");
    } else if (numberOfFiles == 1) {
        ui->currentDirectory->setText("1 Datei in " + italic(path) + " gefunden");
    } else {
        ui->currentDirectory->setText(QString::number(numberOfFiles) + " Dateien in " + italic(path) + "</i> gefunden");
    }
    ui->openDirectory->setEnabled(true);
}

void MainWindow::onFileChanged(const QString &file)
{
    trayIcon->showMessage("New File", file);
}

void MainWindow::handleDoubleClick(const QModelIndex &index)
{
    QString selectedFile = fileController->getFileAt(index.row()).absoluteFilePath();
    openFileExplorerAt(selectedFile);
}

void MainWindow::openFileExplorerAt(const QString &filePath)
{
#if defined(Q_OS_WIN)
    const QString explorer = "C:/Windows/explorer.exe";
    QString param;
    if (!QFileInfo(pathIn).isDir()) {
        param = QLatin1String("/select,");
    }
    param += QDir::toNativeSeparators(pathIn);
    QString command = explorer + " " + param;
    QProcess::startDetached(command);
#elif defined(Q_OS_MAC)
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e") << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(filePath);
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e") << QLatin1String("tell application \"Finder\" to activate");
    QProcess::execute("/usr/bin/osascript", scriptArgs);
#endif
}

void MainWindow::onDirectoryChanged(const QFileInfo &info)
{
    trayIcon->showMessage("dir removed", info.baseName());
}

