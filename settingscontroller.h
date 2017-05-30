#ifndef SETTINGSCONTROLLER_H
#define SETTINGSCONTROLLER_H

#include <QString>
#include <QSize>
#include <QPoint>

struct Settings;

class SettingsController
{
public:
    SettingsController();        
    static Settings parse();
    static void save(const Settings &settings);

private:
    static QString getSettingsFilePath();
};

struct Settings {
    QSize windowSize;
    QPoint windowPosition;
    QString lastOpenedDir;
    bool valid = false;
};


#endif // SETTINGSCONTROLLER_H
