#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QDebug>
#include <QObject>
#include <QVariant>

#include <kconfig.h>
#include <kconfiggroup.h>

#include "configurationdbdriver.h"


class Configuration : public QObject
{
    Q_OBJECT
public:
    explicit Configuration(QObject *parent = 0);
    ~Configuration();

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

};

#endif // CONFIGURATION_H
