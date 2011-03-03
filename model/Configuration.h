#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>
#include <kconfig.h>

class Configuration: public QObject
{
    Q_OBJECT
public:
    KConfig *settings;
    Configuration();
    void initConfigurationFile();
    void writeSetting(QString key, QString value, QString group="General");
    void writeSetting(QString key, bool value, QString group="General");
    void readSettings();
    QString getDropboxFolder();
    QString getBrowser();
    QString getFileManager();
    QString getIconSet();
    bool getStartDaemon();
    bool getAutoStart();
    bool getShowNotifications();

private:
    QString organization;
    QString application;

    QString dropboxFolder;
    QString browser;
    QString fileManager;
    QString iconset;
    bool showNotifications;
    bool startDaemon;
    bool autoStart;
    //QString dropboxPyLocation;

Q_SIGNALS:
    void initializingFile();

};

#endif // CONFIGURATION_H
