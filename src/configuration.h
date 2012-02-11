#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QDebug>
#include <QDir>
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
    inline bool hasKey(const QString& key) const {return generalGroup->hasKey(key);}
    inline QVariant getValue(const QString& key) const {return generalGroup->readEntry(key);}
    inline void setValue(const QString& key, const QVariant& value) {generalGroup->writeEntry(key, value);}
    inline void deleteValue(const QString &key) {generalGroup->deleteEntry(key);}

private:
    void initConfigurationFile();

    KConfig* settings;
    KConfigGroup* generalGroup;

};

#endif // CONFIGURATION_H
