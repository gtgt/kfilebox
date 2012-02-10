#include "configuration.h"

Configuration::Configuration(QObject *parent) :
    QObject(parent)
{
    settings = new KConfig("kfileboxrc");
    generalGroup = new KConfigGroup(settings, "General");

    if(!generalGroup->hasKey("DropboxDir")) {
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
    generalGroup->writeEntry("AutoStart", true);
    generalGroup->writeEntry("StartDaemon", true);
    generalGroup->writeEntry("GtkUiDisabled", true);
    generalGroup->writeEntry("DropboxDir", QDir::homePath().append("/.dropbox-dist/"));
    //! @todo add more pairs

    settings->sync();
}
