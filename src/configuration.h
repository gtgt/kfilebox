#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QDebug>
#include <QObject>
#include <QVariant>

#include <kconfig.h>
#include <kconfiggroup.h>

class Configuration : public QObject
{
    Q_OBJECT
public:
    explicit Configuration(QObject *parent = 0);
    ~Configuration();

    QStringList listKeys() const;
    bool hasKey(const QString &key) const;
    QVariant getValue(const QString &key) const;
    void setValue(const QString &key, const QVariant &value);
    void deleteValue(const QString &key);

private:
    void initConfigurationFile();

    KConfig* settings;
    KConfigGroup* generalGroup;

};

#endif // CONFIGURATION_H
