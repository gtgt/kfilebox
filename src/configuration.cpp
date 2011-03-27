#include "configuration.h"
#include <QPointer>
Configuration::Configuration(QObject *parent) :
    QObject(parent)
{
    settings = new KConfig("kfileboxrc");
    generalGroup = new KConfigGroup(settings, "General");

    if(!generalGroup->hasKey("Browser")) {
        initConfigurationFile();
    }
}

Configuration::~Configuration()
{
    settings->sync();

    delete generalGroup;
    generalGroup = 0;
    delete settings;
    settings = 0;
}

void Configuration::initConfigurationFile()
{
    generalGroup->writeEntry("Browser", "rekonq");
    generalGroup->writeEntry("FileManager", "dolphin");
    generalGroup->writeEntry("IconSet", "default");
    generalGroup->writeEntry("ShowNotifications", true);            //! @todo use from config.db
    generalGroup->writeEntry("AutoStart", true);                    //! @todo use from config.db
    generalGroup->writeEntry("StartDaemon", true);                  //! @todo use from config.db
    generalGroup->writeEntry("GtkUiDisabled", true);

    //! @todo add more pairs

    settings->sync();
}

bool Configuration::hasKey(const QString &key) const
{
    return generalGroup->hasKey(key);
}

//! @todo inline
QVariant Configuration::getValue(const QString &key) const
{
    return generalGroup->readEntry(key);
}

void Configuration::setValue(const QString &key, const QVariant &value)
{
    if(getValue(key)==value)
        return;

    generalGroup->writeEntry(key, value);
}

void Configuration::deleteValue(const QString &key)
{
    generalGroup->deleteEntry(key);
}
