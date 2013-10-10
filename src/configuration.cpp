#include "configuration.h"

Configuration::Configuration(QObject *parent) :
    QObject(parent)
{
    settings = new KConfig("kfileboxrc");
    generalGroup = new KConfigGroup(settings, "General");

    if(!generalGroup->hasKey("DropboxDir") || !generalGroup->hasKey("SyncDir")) {
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
    generalGroup->writeEntry("Browser", "rekonq"); //! @fixme do nothing
    generalGroup->writeEntry("FileManager", "dolphin"); //! @fixme do nothing
    generalGroup->writeEntry("IconSet", "default");
    generalGroup->writeEntry("ShowNotifications", true);
    generalGroup->writeEntry("AutoStart", true); //! @fixme do nothing
    generalGroup->writeEntry("StartDaemon", true);
    generalGroup->writeEntry("GtkUiDisabled", true);
    generalGroup->writeEntry("P2PEnabled", true);
    generalGroup->writeEntry("DropboxDir", QDir::cleanPath(QDir::homePath() + QDir::separator() + ".dropbox-dist")); //! @todo rename for clarity
    generalGroup->writeEntry("SyncDir", QDir::cleanPath(QDir::homePath() + QDir::separator() + "Dropbox"));

    settings->sync();
}
