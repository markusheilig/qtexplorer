#include "settingscontroller.h"

#include <QDir>
#include <QApplication>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

const QString windowPosition = "windowPosition";
const QString x = "x";
const QString y = "y";
const QString windowSize = "windowSize";
const QString width = "width";
const QString height = "height";
const QString lastDir = "lastDir";
const QString fileCheckInterval = "fileCheckInterval";

SettingsController::SettingsController()
{

}

QString SettingsController::getSettingsFilePath()
{
    return QApplication::applicationDirPath() + QDir::separator() + "settings.json";
}

Settings SettingsController::parse()
{
    Settings settings;

    QFile file(getSettingsFilePath());
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject json = doc.object();
        settings.windowPosition = QPoint(json[windowPosition].toObject()[x].toInt(), json[windowPosition].toObject()[y].toInt());
        settings.windowSize = QSize(json[windowSize].toObject()[width].toInt(), json[windowSize].toObject()[height].toInt());
        settings.lastOpenedDir = json[lastDir].toString();
        settings.fileCheckInterval = json[fileCheckInterval].toInt();
        settings.valid = true;
    }

    return settings;
}

void SettingsController::save(const Settings &settings)
{
    QJsonObject json;

    json[lastDir] = settings.lastOpenedDir;
    json[fileCheckInterval] = settings.fileCheckInterval;

    QJsonObject wPos;
    wPos[x] = settings.windowPosition.x();
    wPos[y] = settings.windowPosition.y();
    json[windowPosition] = wPos;

    QJsonObject wSize;
    wSize[width] = settings.windowSize.width();
    wSize[height] = settings.windowSize.height();
    json[windowSize] = wSize;

    QFile file(getSettingsFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(json);
        file.write(doc.toJson());
    }
}
