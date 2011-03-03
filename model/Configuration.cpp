#include "Configuration.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <kconfiggroup.h>


Configuration::Configuration()
{
    settings= new KConfig("kfileboxrc");
}

 void Configuration::writeSetting(QString key, QString value, QString group)
 {
     KConfigGroup generalGroup( settings, group );
     generalGroup.writeEntry(key,value);
     settings->sync();
 }
 
 void Configuration::writeSetting(QString key, bool value, QString group)
 {
     KConfigGroup generalGroup( settings, group );
     generalGroup.writeEntry(key,value);
     settings->sync();
 }

 void Configuration::readSettings()
 {
     KConfigGroup generalGroup( settings, "General" );
     browser=generalGroup.readEntry("Browser");
     dropboxFolder=generalGroup.readEntry("DropboxFolder");
     fileManager=generalGroup.readEntry("FileManager");
     iconset=generalGroup.readEntry("IconSet");
     showNotifications=QVariant(generalGroup.readEntry("ShowNotifications")).toBool();
     startDaemon=QVariant(generalGroup.readEntry("StartDaemon")).toBool();
     autoStart=QVariant(generalGroup.readEntry("AutoStart")).toBool();

     if (browser.length()==0)
           initConfigurationFile();

 }

 void Configuration::initConfigurationFile()
 {
     KConfigGroup generalGroup( settings, "General" );
     generalGroup.writeEntry("Browser","firefox");
     generalGroup.writeEntry("FileManager","dolphin");
     generalGroup.writeEntry("IconSet","default");
     generalGroup.writeEntry("DropboxFolder","");
     generalGroup.writeEntry("ShowNotifications",true);
     generalGroup.writeEntry("AutoStart",true);
     generalGroup.writeEntry("StartDaemon",true);
     settings->sync();

     readSettings();

     emit (initializingFile());


 }

 QString Configuration::getDropboxFolder()
 {
    return dropboxFolder;
 }
 QString Configuration::getBrowser()
 {
    return browser;
 }

 QString Configuration::getFileManager()
 {
    return fileManager;
 }

 QString Configuration::getIconSet()
 {
    return iconset;
 }

 bool Configuration::getShowNotifications()
 {
    return showNotifications;
 }

 bool Configuration::getStartDaemon()
 {
    return startDaemon;
 }

 bool Configuration::getAutoStart()
 {
    return autoStart;
 }
