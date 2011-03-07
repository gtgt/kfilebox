#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>
#include <QDebug>
#include <QVariant>

#include <kconfig.h>
#include <kconfiggroup.h>

#include "configurationdbdriver.h"

//! I bad people cause I keep connection to configuration files: users can loose their data
//! I hope will fix this issue
//! And I droping connection to sqlite db every time - crazy..
class Configuration: public QObject
{
    Q_OBJECT
public:
    Configuration();
    ~Configuration();

    void writeSetting(QString key, QString value);
    void writeSetting(QString key, bool value);

    bool hasKey(const QString &key);
    QString getValue(const QString &key);
    void setValue(const QString &key, const QString &value);

    QString getDropboxFolder();
    QString getBrowser();
    QString getFileManager();
    QString getIconSet();
    bool getStartDaemon();
    bool getAutoStart();
    bool getShowNotifications();

private:
    void initConfigurationFile();

    KConfig* settings;
    KConfigGroup* generalGroup;

    ConfigurationDBDriver* DB;

signals:
    void initializingFile();

};

#endif // CONFIGURATION_H
