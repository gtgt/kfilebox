#include "configuration.h"

Configuration::Configuration(QObject *parent) :
    QObject(parent)
{
    settings = new KConfig("kfileboxrc");
    generalGroup = new KConfigGroup(settings, "General");

    if(!generalGroup->hasKey("DistDir") || !generalGroup->hasKey("ConfigDir") || !generalGroup->hasKey("SyncDir")) {
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
    generalGroup->writeEntry("Browser", "rekonq"); //! @todo do nothing
    generalGroup->writeEntry("FileManager", "dolphin"); //! @todo do nothing
    generalGroup->writeEntry("IconSet", "default");
    generalGroup->writeEntry("ShowNotifications", true);
    generalGroup->writeEntry("StartDaemon", true);
    generalGroup->writeEntry("GtkUiDisabled", true);
    generalGroup->writeEntry("P2PEnabled", true);
    generalGroup->writeEntry("DistDir", QDir::cleanPath(QDir::homePath() + QDir::separator() + ".dropbox-dist"));
    generalGroup->writeEntry("ConfigDir", QDir::cleanPath(QDir::homePath() + QDir::separator() + ".dropbox"));
    generalGroup->writeEntry("SyncDir", QDir::cleanPath(QDir::homePath() + QDir::separator() + "Dropbox"));

    settings->sync();
}
