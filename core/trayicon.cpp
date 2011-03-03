#include "trayicon.h"

#include <QtGui>

#include <QFileInfo>
#include <QFile>
#include <QTimer>
#include <QDebug>
#include "util/SystemCall.h"
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <core/dolphin.h>
#include <core/konqueror.h>

namespace core {

TrayIcon::TrayIcon(Configuration *c)
{
    dc = new DropboxClient();    
    conf=c;
    this->dc->setConfiguration(conf);
    connect(dc, SIGNAL(messageProcessed(QString)), this, SLOT(updateTryIcon(QString)));


    trayIcon = new KStatusNotifierItem();
    loadIcons();
    dStatus = TrayIcon::DropboxUnkown;
    currentMessage="";
    pid=-1;

    //caller= new SystemCall();
    f_journal = new FileJournal();
    createActions();
    createTrayIcon();
}


void TrayIcon::createActions()
{
    openDir = new QAction(tr("Open Dropbox Folder"), this);
    connect(openDir, SIGNAL(triggered()), this, SLOT(openFileBrowser()));

    openHelpCenter = new QAction(tr("Help Center"), this);
    connect(openHelpCenter, SIGNAL(triggered()), this, SLOT(openHelpCenterURL()));

    openDropboxWebsite = new QAction(tr("Launch Dropbox Website"), this);
    connect(openDropboxWebsite, SIGNAL(triggered()), this, SLOT(openDropboxWebsiteURL()));

    openGetMoreSpace = new QAction(tr("Get More Space"), this);
    connect(openGetMoreSpace, SIGNAL(triggered()), this, SLOT(openGetMoreSpaceURL()));

    openTour = new QAction(tr("Tour"), this);
    connect(openTour, SIGNAL(triggered()), this, SLOT(openTourURL()));

    openForums = new QAction(tr("Forums"), this);
    connect(openForums, SIGNAL(triggered()), this, SLOT(openForumsURL()));

    openPrefs = new QAction(tr("Preferences..."), this);
    connect(openPrefs, SIGNAL(triggered()), this, SLOT(openPrefsWindow()));

    startAction = new QAction(tr("Start Dropbox"), this);
    connect(startAction, SIGNAL(triggered()), this, SLOT(startDropboxDaemon()));

    stopAction = new QAction(tr("Stop Dropbox"), this);
    connect(stopAction, SIGNAL(triggered()), this, SLOT(stopDropboxDaemon()));

    //quitAction = new QAction(tr("&Exit"), this);
    //connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void TrayIcon::loadIcons()
{
    QString iconset;
    if (conf->getIconSet().length()==0)
        iconset="default";
    else
        iconset=conf->getIconSet();

    defaultIcon = QIcon(":/icons/img/"+iconset+"/kfilebox.png");
    idleIcon = QIcon(":/icons/img/"+iconset+"/kfilebox_idle.png");
    bussyIcon = QIcon(":/icons/img/"+iconset+"/kfilebox_updating.png");
    errorIcon = QIcon(":/icons/img/"+iconset+"/kfilebox_error.png");

    appIcon = QIcon(":/icons/img/"+iconset+"/kfileboxapp.png");
    if (trayIcon!=NULL)
        trayIcon->setToolTipIconByPixmap(appIcon);
}

void TrayIcon::createTrayIcon()
{

    trayIconMenu = new KMenu(this);

    KMenu *helpMenu= new KMenu(tr("Help"));
    helpMenu->addAction(openHelpCenter);
    helpMenu->addAction(openTour);
    helpMenu->addAction(openForums);

    chFiles= new KMenu(tr("Recently changed files"));
    connect(chFiles, SIGNAL(aboutToShow()), this, SLOT(prepareLastChangedFiles()));

    trayIconMenu->addAction(openDir);
    trayIconMenu->addAction(openDropboxWebsite);
    trayIconMenu->addMenu(chFiles);

    trayIconMenu->addSeparator();
    trayIconMenu->addMenu(helpMenu);
    trayIconMenu->addAction(openGetMoreSpace);
    trayIconMenu->addAction(openPrefs);

    trayIconMenu->addAction(startAction);

    trayIcon->setStatus(KStatusNotifierItem::Active);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIconByPixmap(defaultIcon);
    trayIcon->setToolTipIconByPixmap(appIcon);
    trayIcon->setToolTipTitle("Kfilebox");

    connect(trayIcon, SIGNAL(activateRequested(bool,QPoint)), this,SLOT(trayIconDblClicked(bool,QPoint)));
}

void TrayIcon::openFileBrowser(QString path)
{
    QDir *dir=new QDir(conf->getDropboxFolder());
    if (path.isNull())
        path=dir->toNativeSeparators(dir->path());
    else
        path=dir->toNativeSeparators(dir->path()+dir->separator()+path);

    delete dir;
    dir=0;
    qDebug() << "20: " << conf->getDropboxFolder();

    if (conf->getFileManager().toLower().compare("dolphin")==0)
        fbrowser= new Dolphin();
    else
        fbrowser= new Konqueror(caller);

    if (pid==-1) //Open the window
    {
        //qt_message_output(QtWarningMsg,"Opening a filebrowser...");


        pid=fbrowser->openNewWindow(path);

        //If failed opening the window we assume the program isn't running; we try to run the program
        if (pid==-1){
            //qt_message_output(QtWarningMsg,"It was impossible to open a new window. Trying to launch the filebrowser");
            QString command=conf->getFileManager()+" \""+path+"\"";
            pid=caller->execute(command).toInt();
            if (conf->getFileManager().toLower().compare("dolphin")==0)
                pid=0;
            //qt_message_output(QtWarningMsg,"Execution pid is "+QString::number(pid).toLatin1());

        }
    }
    else //Closing the window
    {
        fbrowser->setPid(pid);
        pid=-1;
        if (fbrowser->closeWindow()==false){ //Already closed: user wants to open
            //qt_message_output(QtWarningMsg,"Already closed");
            openFileBrowser();
        }
    }
}

void TrayIcon::openHelpCenterURL()
{
   caller->openURL("https://www.dropbox.com/help");
}

void TrayIcon::openTourURL()
{
    caller->openURL("https://www.dropbox.com/tour");
}

void TrayIcon::openForumsURL()
{
    caller->openURL(" http://forums.dropbox.com/");
}

void TrayIcon::openDropboxWebsiteURL()
{
    caller->openURL("https://www.dropbox.com/home");
}
void TrayIcon::openGetMoreSpaceURL()
{
    caller->openURL("https://www.dropbox.com/plans");
}

void TrayIcon::startDropboxDaemon()
{
    dc->start();
}

void TrayIcon::stopDropboxDaemon()
{
    dc->stop();
}

void TrayIcon::trayIconDblClicked(bool active, QPoint point){
    if (active)
        openFileBrowser();

}

void TrayIcon::openPrefsWindow()
{
    emit prefsWindowActionTrigered();
}



void TrayIcon::updateTryIcon( QString result)
{


    if (result.trimmed().length()>0){

        if(result.contains("isn't")){
            if (trayIconMenu->actions().contains(stopAction)){
                trayIconMenu->removeAction(startAction);
                startAction = new QAction(tr("Start Dropbox"), this);
                connect(startAction, SIGNAL(triggered()), this, SLOT(startDropboxDaemon()));
                trayIconMenu->insertAction(stopAction,startAction);
                trayIconMenu->removeAction(stopAction);
            }
        }
        else{
            if (trayIconMenu->actions().contains(startAction)){
                trayIconMenu->removeAction(stopAction);
                stopAction = new QAction(tr("Stop Dropbox"), this);
                connect(stopAction, SIGNAL(triggered()), this, SLOT(stopDropboxDaemon()));
                trayIconMenu->insertAction(startAction,stopAction);
                trayIconMenu->removeAction(startAction);
            }
        }

        //Icon update
        if (result.contains("connecting") && dStatus!= TrayIcon::DropboxBussy){
            trayIcon->setIconByPixmap(defaultIcon);
            dStatus=TrayIcon::DropboxBussy;
            trayIcon->setToolTipSubTitle(result);
        }
        else if (result.contains("Idle") && dStatus!= TrayIcon::DropboxIdle)
        {
            trayIcon->setIconByPixmap(idleIcon);
            dStatus=TrayIcon::DropboxIdle;
            trayIcon->setToolTipSubTitle(result);
        }
        else if (result.contains("Up")  && dStatus!= TrayIcon::DropboxUploading){
            if (result.compare(currentMessage)!=0){
                trayIcon->setIconByPixmap(bussyIcon);
                dStatus=TrayIcon::DropboxUploading;
                trayIcon->setToolTipSubTitle(result);
            }
        }
        else if (result.contains("Downloading")) {
            if (result.compare(currentMessage)!=0){
                trayIcon->setIconByPixmap(bussyIcon);
                dStatus=TrayIcon::DropboxDownloading;
                trayIcon->setToolTipSubTitle(result);
            }
        }
        else if (result.contains("Saving")  && dStatus!= TrayIcon::DropboxSaving) {
            trayIcon->setIconByPixmap(bussyIcon);
            dStatus=TrayIcon::DropboxSaving;
            trayIcon->setToolTipSubTitle(result);
        }
        else if (result.contains("Indexing")  && dStatus!= TrayIcon::DropboxIndexing) {
            trayIcon->setIconByPixmap(bussyIcon);
            dStatus=TrayIcon::DropboxIndexing;
            trayIcon->setToolTipSubTitle(result);
        }
        else if(result.contains("isn't") && dStatus!= TrayIcon::DropboxStopped) {
            trayIcon->setIconByPixmap(errorIcon);
            dStatus=TrayIcon::DropboxStopped;
            trayIcon->setToolTipSubTitle(result);
        }
        else if(result.contains("couldn't") && dStatus!= TrayIcon::DropboxDisconnected){
            trayIcon->setIconByPixmap(errorIcon);
            dStatus=TrayIcon::DropboxDisconnected;
            trayIcon->setToolTipSubTitle(result);
        }
        else if(result.contains("dopped") && dStatus!= TrayIcon::DropboxError){
            trayIcon->setIconByPixmap(errorIcon);
            dStatus=TrayIcon::DropboxError;
            trayIcon->setToolTipSubTitle(result);
        }
    }

        //qt_message_output(QtDebugMsg,result.toLatin1());

}


void TrayIcon::prepareLastChangedFiles(){
    QStringList file_name, file_relative_path;
    QString str_path;

    foreach (QAction *a, chFiles->actions()){
        chFiles->removeAction(a);
    }

    QList <QString> files=f_journal->lastChangedFiles();
    QSignalMapper *sm= new QSignalMapper(this);


    for (int i = 0; i < files.size(); ++i) {
        file_relative_path=files.at(i).split(":/");
        file_name=file_relative_path.last().split("/");
        chFiles->addAction(new QAction(file_name.last(), this));
        connect(chFiles->actions().at(i), SIGNAL(triggered()), sm, SLOT(map()));
        file_name.removeLast();
        str_path=QString();
        foreach (QString str,file_name){
            str_path+="/"+str;
        }
        sm->setMapping(chFiles->actions().at(i),str_path);
        //qt_message_output(QtDebugMsg,str_path.toLatin1());
     }
     connect(sm, SIGNAL(mapped(const QString &)), this, SLOT(openFileBrowser(const QString &)));
}

void TrayIcon::setCaller(SystemCall *c){
    this->caller=c;
}

void TrayIcon::testDaemonStart(){
    if (conf->getStartDaemon() && !dc->is_running())
        dc->start();
}


void TrayIcon::getDropboxStatus(){
    if (dc->is_running())
        dc->status();
    else
        dc->processMessage("ok\nstatus\tDropbox isn't running\ndone");

}

} /* End of namespace core */
