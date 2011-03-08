#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>
#include <QDebug>
#include <QVariant>

#include <kconfig.h>
#include <kconfiggroup.h>

#include "configurationdbdriver.h"

//! @todo remove dynamical configuration from MainWindow, leave only local(into function)
class Configuration: public QObject
{
    Q_OBJECT
public:
    Configuration();
    ~Configuration();

    //! @todo remove. But before play with: typename T
    void writeSetting(QString key, QString value) {setValue(key, value);}
    void writeSetting(QString key, bool value){setValue(key, value);}

    QStringList listKeys();
    bool hasKey(const QString &key);
    QVariant getValue(const QString &key);
    void setValue(const QString &key, const QVariant &value);
    void deleteValue(const QString &key);

private:
    void initConfigurationFile();

    KConfig* settings;
    KConfigGroup* generalGroup;

    ConfigurationDBDriver* DB;

signals:
};

#endif // CONFIGURATION_H
