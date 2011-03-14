#include "configuration.h"

Configuration::Configuration(QObject *parent) :
    QObject(parent)
{
    settings = new KConfig("kfileboxrc");
    generalGroup = new KConfigGroup(settings, "General");

    if(!generalGroup->hasKey("Browser")) {
        initConfigurationFile();
    }

    DB = new ConfigurationDBDriver(this);
}

Configuration::~Configuration()
{
    settings->sync();

    delete generalGroup;
    generalGroup = 0;
    delete settings;
    settings = 0;
    delete DB;
    DB = 0;
}

void Configuration::initConfigurationFile()
{
    generalGroup->writeEntry("Browser", "rekonq");
    generalGroup->writeEntry("FileManager", "dolphin");
    generalGroup->writeEntry("IconSet", "default");
    generalGroup->writeEntry("ShowNotifications", true);
    generalGroup->writeEntry("AutoStart", true);
    generalGroup->writeEntry("StartDaemon", true);
    generalGroup->writeEntry("GtkUiDisabled", true);

    //! @todo add more pairs

    settings->sync();
}

bool Configuration::hasKey(const QString &key) const
{
    return (generalGroup->hasKey(key) || DB->hasKey(key));
}

QVariant Configuration::getValue(const QString &key) const
{
    if(DB->hasKey(key))
        return DB->getValue(key);
    return generalGroup->readEntry(key);
}

void Configuration::setValue(const QString &key, const QVariant &value)
{
    if(getValue(key)==value)
        return;

    if(DB->hasKey(key))
        DB->setValue(key, value);
    else
        generalGroup->writeEntry(key, value);
}

void Configuration::deleteValue(const QString &key)
{
    if(DB->hasKey(key))
        DB->deleteValue(key);
    else
        generalGroup->deleteEntry(key);
}
