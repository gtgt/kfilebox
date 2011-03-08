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
    connect(dc, SIGNAL(messageProcessed(QString)), this, SLOT(updateTryIcon(QString)));
    if(conf->getStartDaemon() && !dc->is_running()) dc->start();

    trayIcon = new KStatusNotifierItem();
    loadIcons();
    dStatus = TrayIcon::DropboxUnkown;
    currentMessage="";
    pid=-1;

    //caller= new SystemCall();
    createActions();
    createTrayIcon();
}

TrayIcon::~TrayIcon()
{
    //! delete openDir..
    delete dc;
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

    trayIconMenu->addAction(stopAction);
    startAction->setVisible(false);
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
    //! path = dropbox.getPathTo(path)
    QString needed;
    if (path.isNull())
        needed=QDir::toNativeSeparators(conf->getDropboxFolder());
    else
        needed=QDir::toNativeSeparators(conf->getDropboxFolder().append(QDir::separator()).append(path));

    /*if (conf->getFileManager().toLower().compare("dolphin")==0)
        fbrowser= new Dolphin();
    else
        fbrowser= new Konqueror(caller);*/

    //        pid=fbrowser->openNewWindow(path);
    QString command=conf->getFileManager()+" \""+needed+"\"";
    QProcess::startDetached(command);

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
    Q_UNUSED(point);
    if (active)
        openFileBrowser();

}

//! @FIXME
void TrayIcon::openPrefsWindow()
{
    emit prefsWindowActionTrigered();
}



void TrayIcon::updateTryIcon( QString result)
{
    if (result.trimmed().length()>0){
        if(result.contains("isn't")){
            if(stopAction->isVisible()) {
                startAction->setVisible(true);
                stopAction->setVisible(false);
            }
        }
        else{
            if (startAction->isVisible()){
                startAction->setVisible(false);
                stopAction->setVisible(true);
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


    QStringList files;
    foreach (QString elem, conf->getValue("recently_changed3").split("\n")) {
        QStringList list = elem.split(":");
        if(list.length()>1)
        {
            //! `\u0441\u043D\u0438\u043C\u043E\u043A38.png'
            //! convert to `снимок38.png'
            //! hope somebody will find normal solution)
            QString humanResult;
            QStringList toHumanable = list.value(1).split("\\u");
            if(toHumanable.length()>1)
            {
                humanResult = toHumanable.first();
                for(int i=1; i<toHumanable.length(); i++ )
                {
                    if(toHumanable.at(i).length()!=4)
                        humanResult.append(QChar(toHumanable.at(i).mid(0, 4).toInt(0, 16))).append(toHumanable.at(i).mid(4));
                    else
                        humanResult.append(QChar(toHumanable.at(i).toInt(0, 16)));
                }
                files.append(humanResult);
            } else
                files.append(list.value(1).trimmed());
        }
    }
    if( files.size() == 0 )
        files.append("File list is empty:(");


    //! @bug memory leak
    QSignalMapper *sm= new QSignalMapper(this);

    for (int i = 0; i < files.size(); ++i) {
        file_relative_path=files.at(i).split(":/");
        file_name=file_relative_path.last().split("/");
        //! @bug memory leak
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

//! stub - to remove
void TrayIcon::setCaller(SystemCall *c){
    this->caller=c;
}

} /* End of namespace core */
