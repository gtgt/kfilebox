#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>
#include <QDebug>
#include <QVariant>

#include <kconfig.h>
#include <kconfiggroup.h>

#include "configurationdbdriver.h"

//! I bad people cause I keeping connection to configuration files: users can loose their data..
//! I hope will fix this issue
//! @todo create QVariantMap of conf::values - increase memory usage, increase safety. If i understand - need to create ConfigurationFSDriver (
class Configuration: public QObject
{
    Q_OBJECT
public:
    Configuration();
    ~Configuration();

    //! @todo remove. But before play with: typename T
    void writeSetting(QString key, QString value);
    void writeSetting(QString key, bool value);

    //! key type is QString, value type is QVariant - as we know we cant create 2 functions with only different return type
    bool hasKey(const QString &key);
    QString getValue(const QString &key);
    void setValue(const QString &key, const QString &value);

    //! @todo replace in code with getValue("key")
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
