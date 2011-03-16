#include "trayicon.h"

TrayIcon::TrayIcon(QWidget *parent) :
    QWidget(parent)
{
    trayIcon = new KStatusNotifierItem(this);

    sm = new QSignalMapper(this);
    connect(sm, SIGNAL(mapped(const QString &)), this, SLOT(openFileBrowser(const QString &)));


    createActions();
    createTrayIcon();

    chFiles->addAction(new QAction("empty", this));
    chFiles->addAction(new QAction("empty", this));
    chFiles->addAction(new QAction("empty", this));
    chFiles->addAction(new QAction("empty", this));
    chFiles->addAction(new QAction("empty", this));

}

TrayIcon::~TrayIcon()
{
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
    startAction->setVisible(false);

    stopAction = new QAction(tr("Stop Dropbox"), this);
    connect(stopAction, SIGNAL(triggered()), this, SIGNAL(stopDropbox()));

    statusAction = new QAction("connecting", this);
    statusAction->setEnabled(false);

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

    KMenu *helpMenu= new KMenu(tr("Help"), this);
    helpMenu->addAction(openHelpCenter);
    helpMenu->addAction(openTour);
    helpMenu->addAction(openForums);

    chFiles= new KMenu(tr("Recently changed files"), this);
    connect(chFiles, SIGNAL(aboutToShow()), this, SLOT(prepareLastChangedFiles()));

    trayIconMenu->addAction(openDir);
    trayIconMenu->addAction(openDropboxWebsite);
    trayIconMenu->addMenu(chFiles);

    trayIconMenu->addSeparator();
    trayIconMenu->addAction(statusAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addMenu(helpMenu);
    trayIconMenu->addAction(openGetMoreSpace);
    trayIconMenu->addAction(openPrefs);

    trayIconMenu->addAction(stopAction);
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
    QDesktopServices::openUrl(QUrl(QDir::toNativeSeparators(conf.getValue("dropbox_path").toString().append(path))));

}

//! @todo get this urls from dropbox daemon
void TrayIcon::openHelpCenterURL()
{
    QDesktopServices::openUrl(QUrl("https://www.dropbox.com/help"));
}

void TrayIcon::openTourURL()
{
    QDesktopServices::openUrl(QUrl("https://www.dropbox.com/tour"));
}

void TrayIcon::openForumsURL()
{
    QDesktopServices::openUrl(QUrl("http://forums.dropbox.com/"));
}

void TrayIcon::openDropboxWebsiteURL()
{
    QDesktopServices::openUrl(QUrl("https://www.dropbox.com/home"));
}
void TrayIcon::openGetMoreSpaceURL()
{
    QDesktopServices::openUrl(QUrl("https://www.dropbox.com/plans"));
}

void TrayIcon::updateStatus(DropboxClient::DropboxStatus newStatus, const QString &message)
{
    if(newStatus == DropboxClient::DropboxStopped){
        if(stopAction->isVisible() || !startAction->isVisible()) {
            startAction->setVisible(true);
            stopAction->setVisible(false);
        }
    } else{
        if (!stopAction->isVisible() || startAction->isVisible()) {
            startAction->setVisible(false);
            stopAction->setVisible(true);
        }
    }

    statusAction->setText(message);
    trayIcon->setToolTipSubTitle(message);

    switch(newStatus) {
    case DropboxClient::DropboxBussy:
        trayIcon->setIconByPixmap(defaultIcon);
        break;
    case DropboxClient::DropboxIdle:
        trayIcon->setIconByPixmap(idleIcon);
        break;
    case DropboxClient::DropboxUploading:
        trayIcon->setIconByPixmap(bussyIcon);
        break;
    case DropboxClient::DropboxDownloading:
        trayIcon->setIconByPixmap(bussyIcon);
        break;
    case DropboxClient::DropboxSaving:
        trayIcon->setIconByPixmap(bussyIcon);
        break;
    case  DropboxClient::DropboxIndexing:
        trayIcon->setIconByPixmap(bussyIcon);
        break;
    case DropboxClient::DropboxStopped:
        trayIcon->setIconByPixmap(errorIcon);
        break;
    case DropboxClient::DropboxDisconnected:
        trayIcon->setIconByPixmap(errorIcon);
        break;
    case DropboxClient::DropboxError:
        trayIcon->setIconByPixmap(errorIcon);
        break;
    case DropboxClient::DropboxUnkown:
        break;
    }
}

//! @todo recent files from shared folders
//! in db '/gp/lacrimoza.gp5'
//! absolute path is '~/Dropbox/shared-folder/' + that file
void TrayIcon::prepareLastChangedFiles(){

    QStringList files;
    Configuration conf;
    QString dropboxPath = conf.getValue("dropbox_path").toString();
    QString recentlyChanged = conf.getValue("recently_changed3").toString();
    if(recentlyChanged.isEmpty()) return;
    foreach (QString elem, recentlyChanged.split("\n")) {
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
            files.push_back(humanResult);
        } else
            files.push_back(list.value(1));
    }

    for (int i = 0; i < 5; ++i) {
        disconnect(chFiles->actions().at(i), SIGNAL(triggered()), sm, SLOT(map()));
        sm->removeMappings(chFiles->actions().at(i));
        chFiles->actions().at(i)->setText(files.at(i).split("/").last());
        chFiles->actions().at(i)->setEnabled(QFile(dropboxPath+files.at(i)).exists());

        connect(chFiles->actions().at(i), SIGNAL(triggered()), sm, SLOT(map()));
        sm->setMapping(chFiles->actions().at(i), QString(files.at(i)));
    }
}
