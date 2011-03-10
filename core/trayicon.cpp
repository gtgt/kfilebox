#include "trayicon.h"

namespace core {

TrayIcon::TrayIcon()
{
    trayIcon = new KStatusNotifierItem();
    dStatus = TrayIcon::DropboxUnkown;
    currentMessage="";

    sm= new QSignalMapper(this);

    createActions();
    createTrayIcon();
}

TrayIcon::~TrayIcon()
{
    //! @todo delete all elements
    delete sm;
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
    connect(openPrefs, SIGNAL(triggered()), this, SIGNAL(prefsWindowActionTrigered()));

    startAction = new QAction(tr("Start Dropbox"), this);
    connect(startAction, SIGNAL(triggered()), this, SIGNAL(startDropbox()));

    stopAction = new QAction(tr("Stop Dropbox"), this);
    connect(stopAction, SIGNAL(triggered()), this, SIGNAL(stopDropbox()));

    //quitAction = new QAction(tr("&Exit"), this);
    //connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void TrayIcon::loadIcons(const QString &iconset)
{
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

    connect(trayIcon, SIGNAL(activateRequested(bool,QPoint)), this,SLOT(openFileBrowser()));
}

void TrayIcon::openFileBrowser(const QString &path)
{
    //! @todo pathToFile = dropbox.getPathTo(file)
    //! get variables[FileManager, DropboxPath] from ui->dropboxPath

    Configuration conf;
    QProcess::startDetached(conf.getValue("FileManager").toString().append(" \"")
                            .append(QDir::toNativeSeparators(conf.getValue("dropbox_path").toString().append(path)))
                            .append("\""));

}

//! @todo get this urls from dropbox daemon
void TrayIcon::openHelpCenterURL()
{
    Configuration conf;
    QProcess::startDetached(conf.getValue("Browser").toString().append(" https://www.dropbox.com/help"));
}

void TrayIcon::openTourURL()
{
    Configuration conf;
    QProcess::startDetached(conf.getValue("Browser").toString().append(" https://www.dropbox.com/tour"));
}

void TrayIcon::openForumsURL()
{
    Configuration conf;
    QProcess::startDetached(conf.getValue("Browser").toString().append(" http://forums.dropbox.com/"));
}

void TrayIcon::openDropboxWebsiteURL()
{
    Configuration conf;
    QProcess::startDetached(conf.getValue("Browser").toString().append(" https://www.dropbox.com/home"));
}
void TrayIcon::openGetMoreSpaceURL()
{
    Configuration conf;
    QProcess::startDetached(conf.getValue("Browser").toString().append(" https://www.dropbox.com/plans"));
}

//! @todo state may be stored in DropboxClient
//! on state change better call SLOT
void TrayIcon::updateTrayIcon(const QString &result)
{
    if (result.trimmed().length()==0)
        qDebug() << "(TrayIcon::updateTrayIcon) Error: empty message received";

    if(result.contains("isn't")){
        if(stopAction->isVisible() || !startAction->isVisible()) {
            startAction->setVisible(true);
            stopAction->setVisible(false);
        }
    }
    else{
        if (!stopAction->isVisible() || startAction->isVisible()){
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

//! @bug recent files from shared folders
//! in db '/gp/lacrimoza.gp5'
//! absolute path is '~/Dropbox/shared-folder/' + that file
void TrayIcon::prepareLastChangedFiles(){

    foreach (QAction *a, chFiles->actions()){
        chFiles->removeAction(a);
    }

    QString file;
    Configuration conf;
    QString dropboxPath = conf.getValue("dropbox_path").toString();
    foreach (QString elem, conf.getValue("recently_changed3").toString().split("\n")) {
        QStringList list = elem.split(":");
        if(list.length()<=1) continue;

        //! `\u0441\u043D\u0438\u043C\u043E\u043A38.png'
        //! convert to `снимок38.png'
        //! hope somebody will find normal solution)
        QString humanResult;
        QStringList toHumanable = list.value(1).split("\\u");
        if(toHumanable.length()>1) {
            humanResult = toHumanable.first();
            for(int i=1; i<toHumanable.length(); i++ ) {
                if(toHumanable.at(i).length()!=4)
                    humanResult.append(QChar(toHumanable.at(i).mid(0, 4).toInt(0, 16))).append(toHumanable.at(i).mid(4));
                else
                    humanResult.append(QChar(toHumanable.at(i).toInt(0, 16)));
            }
            file = humanResult;
        } else
            file = list.value(1);

        //! @bug memory leak?
        chFiles->addAction(new QAction(file.split("/").last(), this));
        chFiles->actions().last()->setEnabled(QFile(dropboxPath+file).exists());

        connect(chFiles->actions().last(), SIGNAL(triggered()), sm, SLOT(map()));

        sm->setMapping(chFiles->actions().last(), file);


    }
    connect(sm, SIGNAL(mapped(const QString &)), this, SLOT(openFileBrowser(const QString &)));
}

} /* End of namespace core */
