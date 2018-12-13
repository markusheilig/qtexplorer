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
#include "fileutils.h"

#define TEXT_SORT_BY_FILE "Dateiupdate"
#define TEXT_SORT_BY_DIR "Ordnerupdate"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    blueBinocularsWithEyes(QIcon(":/binoculars-with-eyes")),
    redBinocularsWithEyes(QIcon(":/binoculars-with-eyes-red"))
{
    ui->setupUi(this);
    setWindowIcon(blueBinocularsWithEyes);

    periodicFileWatcher = new PeriodicFileWatcher(&model);

    ui->tableView->setModel(&model);

    ui->sortType->addItems(QStringList() << TEXT_SORT_BY_FILE << TEXT_SORT_BY_DIR);
    ui->sortType->setCurrentIndex(0);
    connect(ui->sortType, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateSortType(QString)));

    connect(ui->action_about, SIGNAL(triggered(bool)), this, SLOT(showAboutMessageBox()));

    connect(&model, SIGNAL(modelAboutToBeReset()), this, SLOT(onStartedFileLoading()), Qt::QueuedConnection);
    connect(&model, SIGNAL(modelReset()), this, SLOT(onFinishedFileLoading()), Qt::QueuedConnection);
    connect(&model, SIGNAL(fileUpdate(QStringList,QStringList)), this, SLOT(onFileUpdate(QStringList, QStringList)));

    connect(ui->openDirectory, SIGNAL(clicked(bool)), this, SLOT(onOpenDirectoryClicked()));
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(handleDoubleClick(QModelIndex)));

    connect(ui->fileCheckInterval, SIGNAL(valueChanged(int)), periodicFileWatcher, SLOT(setInterval(int)));
    connect(ui->checkForUpdates, SIGNAL(clicked(bool)), this, SLOT(onCheckForUpdatesClicked()));

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->installEventFilter(this);
    trayIcon->setIcon(blueBinocularsWithEyes);
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(showWindowAndBringToFront()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(showWindowAndBringToFront()));
    trayIcon->show();

    loadAndApplySettings();
}

MainWindow::~MainWindow()
{
    delete periodicFileWatcher;
    delete ui;
}

void MainWindow::loadAndApplySettings()
{
    Settings s = SettingsController::parse();
    if (s.valid) {
        move(s.windowPosition);
        resize(s.windowSize);
        ui->fileCheckInterval->setValue(s.fileCheckIntervalInMinutes);
        if (s.sortType == Settings::SortByDir) {
            ui->sortType->setCurrentText(TEXT_SORT_BY_DIR);
            model.setFileSorter(new SortByLastModificationDirectoryDate());
        } else {
            ui->sortType->setCurrentText(TEXT_SORT_BY_FILE);
            model.setFileSorter(new SortByLastModificationFileDate());
        }
        QDir lastOpened = QDir(s.lastOpenedDir);
        if (lastOpened.exists()) {
            QFileInfoList files = FileUtils::getFilesRecursive(lastOpened);
            model.init(lastOpened, files);
        }
    }
    periodicFileWatcher->setInterval(ui->fileCheckInterval->value());
}

void MainWindow::saveSettings()
{
    Settings s;
    if (model.getDir().exists()) {
        s.lastOpenedDir = model.getDir().absolutePath();
    }
    s.windowPosition = pos();
    s.windowSize = size();
    s.fileCheckIntervalInMinutes = ui->fileCheckInterval->value();
    if (ui->sortType->currentText() == TEXT_SORT_BY_DIR) {
        s.sortType = Settings::SortByDir;
    } else {
        s.sortType = Settings::SortByFile;
    }
    SettingsController::save(s);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}

void MainWindow::hideEvent(QHideEvent *event)
{
    hide();
    event->ignore();
}

void MainWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    trayIcon->setIcon(blueBinocularsWithEyes);
}

QString bold(const QString &s) {
    return "<b>" + s + "</b>";
}

void MainWindow::onOpenDirectoryClicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Verzeichnis auswählen"), model.getDir().absolutePath(),
                                                     QFileDialog::ShowDirsOnly | QFileDialog::ReadOnly);
    if (!path.isEmpty()) {
        QDir d = path;
        QFileInfoList files = FileUtils::getFilesRecursive(d);
        model.init(d, files);
    }
}

void MainWindow::onStartedFileLoading()
{
    this->setEnabled(false);
    ui->currentDirectory->setText("Lade Dateien in " + bold(model.getDir().absolutePath()));
    qApp->processEvents();
}

void MainWindow::onFinishedFileLoading()
{
    const QString path = bold(model.getDir().absolutePath());
    const int numberOfFiles = model.rowCount();
    if (numberOfFiles == 0) {
        ui->currentDirectory->setText("Verzeichnis " + path + " enthält keine Dateien");
    } else if (numberOfFiles == 1) {
        ui->currentDirectory->setText("1 Datei in " + path + " gefunden");
    } else {
        ui->currentDirectory->setText(QString::number(numberOfFiles) + " Dateien in " + path + " gefunden");
    }
    this->setEnabled(true);
}

QString buildMessage(const QString &topic, const QStringList &files) {
    if (files.isEmpty()) {
        return "";
    }
    QString message = "> " + topic + ":";
    foreach (const QString &file, files) {
        message += " " + file + ",";
    }
    return message.left(message.length() - 1);
}

void MainWindow::onFileUpdate(const QStringList &newFiles, const QStringList &updatedFiles)
{
    if (!isVisible()) {
        trayIcon->setIcon(redBinocularsWithEyes);
    }

    QString message;
    if (!newFiles.isEmpty()) {
        message = buildMessage("Neue Dateien", newFiles);
    }
    if (!updatedFiles.isEmpty()) {
        if (!message.isEmpty()) {
            message += "\n";
        }
        message += buildMessage("Änderungen", updatedFiles);
    }
    trayIcon->showMessage("Update", message);
}

void MainWindow::onCheckForUpdatesClicked()
{    
    this->setEnabled(false);
    periodicFileWatcher->checkFiles();
    periodicFileWatcher->restart();
}

void MainWindow::showWindowAndBringToFront()
{
    if (!isVisible()) {
        show();
        setWindowState( (windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
#if defined(Q_OS_MAC)
        raise();
#elif defined(Q_OS_WIN)
        activateWindow();
#endif
    }
}

void MainWindow::handleDoubleClick(const QModelIndex &index)
{
    QString path = model.getFileAt(index.row()).absoluteFilePath();
    openFileExplorerAt(path);
}

void MainWindow::openFileExplorerAt(const QString &filePath)
{
#if defined(Q_OS_WIN)
    const QString explorer = "C:/Windows/explorer.exe";
    QString param;
    if (!QFileInfo(filePath).isDir()) {
        param = QLatin1String("/select,");
    }
    param += QDir::toNativeSeparators(filePath);
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

void MainWindow::updateSortType(const QString &sortType)
{
    if (sortType == TEXT_SORT_BY_DIR) {
        model.setFileSorter(new SortByLastModificationDirectoryDate());
    } else {
        model.setFileSorter(new SortByLastModificationFileDate());
    }
}

void MainWindow::showAboutMessageBox()
{
    QString message = "<h3>qtExplorer</h3><hr><div>Icons made by <a href=\"http://www.freepik.com\" title=\"Freepik\">Freepik</a> from <a href=\"https://www.flaticon.com/\" title=\"Flaticon\">www.flaticon.com</a> is licensed by <a href=\"http://creativecommons.org/licenses/by/3.0/\" title=\"Creative Commons BY 3.0\" target=\"_blank\">CC 3.0 BY</a></div>";
    QMessageBox::about(this, "qtExplorer", message);
}


