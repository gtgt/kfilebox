#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>
#include <QDebug>
#include <QVariant>

#include <kconfig.h>
#include <kconfiggroup.h>

#include "configurationdbdriver.h"

//! @todo remove dynamical configuration from MainWindow, leave only local(into function)
//! another way: create QVariantMap and store configuration there for fragmentated get configuration TrayIcon
//! OR in TrayIcon::prepareLastChangedFiles() use ConfigurationDBDriver, another places are called less frequently
//! ..
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
};

#endif // CONFIGURATION_H
