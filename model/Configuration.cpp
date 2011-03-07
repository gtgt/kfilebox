#include "Configuration.h"

#include <QVariantMap>
//#include <QMapIterator>

Configuration::Configuration()
{
    settings = new KConfig("kfileboxrc");
    generalGroup = new KConfigGroup(settings, "General");

    if(!generalGroup->hasKey("Browser")) {
        initConfigurationFile();
    }

    DB = new ConfigurationDBDriver(this);


//    QVariantMap conf;
//    QVariantMap::const_iterator i = conf.find();
}

Configuration::~Configuration()
{
    delete generalGroup;
    generalGroup = 0;
    delete settings;
    settings = 0;
    delete DB;
    DB = 0;
}

void Configuration::writeSetting(QString key, QString value)
{
    generalGroup->writeEntry(key,value);
    settings->sync();
}

void Configuration::writeSetting(QString key, bool value)
{
    generalGroup->writeEntry(key,value);
    settings->sync();
}

void Configuration::initConfigurationFile()
{
    generalGroup->writeEntry("Browser","firefox");
    generalGroup->writeEntry("FileManager","dolphin");
    generalGroup->writeEntry("IconSet","default");
    generalGroup->writeEntry("ShowNotifications",true);
    generalGroup->writeEntry("AutoStart",true);
    generalGroup->writeEntry("StartDaemon",true);
    settings->sync();

    emit (initializingFile());

}

//! is it good design? {

bool Configuration::hasKey(const QString &key)
{
    return (generalGroup->hasKey(key) || DB->hasKey(key));
}

QString Configuration::getValue(const QString &key)
{
    if(generalGroup->hasKey(key))
        return generalGroup->readEntry(key);
    if(DB->hasKey(key))
        return DB->getValue(key);
    return QString();
}

void Configuration::setValue(const QString &key, const QString &value)
{
    if(DB->hasKey(key))
        DB->setValue(key, value);
    if(!generalGroup->hasKey(key))
        qDebug() << "created new key" << key << "with value" << value;
    generalGroup->writeEntry(key, value);
}

//! }


QString Configuration::getDropboxFolder()
{
    return getValue("dropbox_path");
}
QString Configuration::getBrowser()
{
    return generalGroup->readEntry("Browser");
}

QString Configuration::getFileManager()
{
    return generalGroup->readEntry("FileManager");
}

QString Configuration::getIconSet()
{
    return generalGroup->readEntry("IconSet");
}

bool Configuration::getShowNotifications()
{
    return QVariant(generalGroup->readEntry("ShowNotifications")).toBool();
}

bool Configuration::getStartDaemon()
{
    return QVariant(generalGroup->readEntry("StartDaemon")).toBool();
}

bool Configuration::getAutoStart()
{
    return QVariant(generalGroup->readEntry("AutoStart")).toBool();
}
