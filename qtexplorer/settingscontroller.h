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

    enum SortType { SortByFile, SortByDir };

    QSize windowSize;
    QPoint windowPosition;
    QString lastOpenedDir;
    int fileCheckIntervalInMinutes;
    SortType sortType = SortByFile;
    bool valid = false;

    QString sortTypeAsString() const
    {
        if (sortType == SortByDir) {
            return "dir";
        }
        return "file";
    }

    void updateSortType(const QString &sortType)
    {
        if (sortType == "dir") {
            this->sortType = SortByDir;
        } else {
            this->sortType = SortByFile;
        }
    }

};


#endif // SETTINGSCONTROLLER_H
