#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(!DropboxClient::isInstalled()) {
        InstallerForm di(this);
        di.exec();
    }

    dc = new DropboxClient(this);
	db = Singleton::instance();
    if(Configuration().getValue("StartDaemon").toBool())
        dc->start();

    //! []
    openDropboxWebsite = new QAction("Launch Dropbox Website", this);
    action0 = new QAction(this);
    action1 = new QAction(this);
    action2 = new QAction(this);
    action3 = new QAction(this);
    action4 = new QAction(this);
    statusAction = new QAction("connecting", this);
    statusAction->setEnabled(false);
    openHelpCenter = new QAction("Help Center", this);
    openTour = new QAction("Tour", this);
    openForums = new QAction("Forums", this);
    openGetMoreSpace = new QAction("Get More Space", this);
    openPrefs = new QAction("Preferences", this);
    startAction = new QAction("Start Dropbox", this);
    stopAction = new QAction("Stop Dropbox", this);
    openDir = new QAction("Open Dropbox Folder", this);
    trayIconMenu = new KMenu("tray menu", this);
    chFiles = new QMenu("Recently changed files", this);
    helpMenu = new QMenu("Help", this);

    trayIconMenu->addAction(openDir);
    trayIconMenu->addAction(openDropboxWebsite);
    trayIconMenu->addAction(chFiles->menuAction());
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(statusAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(helpMenu->menuAction());
    trayIconMenu->addAction(openGetMoreSpace);
    trayIconMenu->addAction(openPrefs);
    trayIconMenu->addAction(startAction);
    trayIconMenu->addAction(stopAction);
    chFiles->addAction(action0);
    chFiles->addAction(action1);
    chFiles->addAction(action2);
    chFiles->addAction(action3);
    chFiles->addAction(action4);
    helpMenu->addAction(openHelpCenter);
    helpMenu->addAction(openTour);
    helpMenu->addAction(openForums);

    //! [/] and +


    connect(ui->dialogButtonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(dialogButtonBoxTriggered(QAbstractButton*)));
    
    
    connect(ui->moveDropboxFolder, SIGNAL(clicked()), this, SLOT(changeDropboxFolder()));
    connect(ui->cbIconSet, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(loadIcons())); //! not loadIcons(const QString&) cause contents is translated

    connect(dc, SIGNAL(updateStatus(DropboxStatus,QString)), this, SLOT(updateStatus(DropboxStatus,QString)));

    connect(ui->downloadDontLimitRate, SIGNAL(toggled(bool)), this, SLOT(downloadRadioToggle()));
    connect(ui->downloadLimitRate, SIGNAL(toggled(bool)), this, SLOT(downloadRadioToggle()));

    connect(ui->uploadDontLimitRate, SIGNAL(toggled(bool)), this, SLOT(uploadRadioToggle()));
    connect(ui->uploadAutoLimitRate, SIGNAL(toggled(bool)), this, SLOT(uploadRadioToggle()));
    connect(ui->uploadLimitRate, SIGNAL(toggled(bool)), this, SLOT(uploadRadioToggle()));

    connect(ui->proxyDontUse, SIGNAL(toggled(bool)), this, SLOT(proxyRadioToggle()));
    connect(ui->proxyAutoDetect, SIGNAL(toggled(bool)), this, SLOT(proxyRadioToggle()));
    connect(ui->proxySetManually, SIGNAL(toggled(bool)), this, SLOT(proxyRadioToggle()));

    connect(ui->proxyRequiresAuth, SIGNAL(toggled(bool)), this, SLOT(proxyAuthRadioToggle()));

    connect(openDir, SIGNAL(triggered()), this, SLOT(openFileBrowser()));
    connect(openHelpCenter, SIGNAL(triggered()), this, SLOT(openHelpCenterURL()));
    connect(openDropboxWebsite, SIGNAL(triggered()), this, SLOT(openDropboxWebsiteURL()));
    connect(openGetMoreSpace, SIGNAL(triggered()), this, SLOT(openGetMoreSpaceURL()));
    connect(openTour, SIGNAL(triggered()), this, SLOT(openTourURL()));
    connect(openForums, SIGNAL(triggered()), this, SLOT(openForumsURL()));
    connect(openPrefs, SIGNAL(triggered()), this, SLOT(show()));
    connect(startAction, SIGNAL(triggered()), dc, SLOT(start()));
    connect(stopAction, SIGNAL(triggered()), dc, SLOT(stop()));
    connect(chFiles, SIGNAL(aboutToShow()), this, SLOT(prepareLastChangedFiles()));

    startAction->setVisible(false);

    trayIcon = new KStatusNotifierItem(this);
    iconsetList =  new QStringList();
    iconsetList->push_back("default");
    iconsetList->push_back("monochrome");
    iconsetList->push_back("white");

    loadSettings();

    sm = new QSignalMapper(this);
    connect(sm, SIGNAL(mapped(const QString &)), this, SLOT(openFileBrowser(const QString &)));

    initializeDBus();
}

MainWindow::~MainWindow()
{
	db = 0;
    delete ui;
	delete iconsetList;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::initializeDBus()
{
    adaptor = new DropboxClientAdaptor(dc);
}

void MainWindow::changeDropboxFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this,tr("Dropbox folder"), ui->dropboxFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if ( (dir.length()!=0) && (ui->dropboxFolder->text() != dir) )
        ui->dropboxFolder->setText(dir);
}

void MainWindow::downloadRadioToggle()
{
    ui->downloadLimitValue->setEnabled(ui->downloadLimitRate->isChecked());
}

void MainWindow::uploadRadioToggle()
{
    ui->uploadLimitValue->setEnabled(ui->uploadLimitRate->isChecked());
}

void MainWindow::proxyRadioToggle()
{
    ui->proxyType->setEnabled(ui->proxySetManually->isChecked());
    ui->proxyServer->setEnabled(ui->proxySetManually->isChecked());
    ui->proxyPort->setEnabled(ui->proxySetManually->isChecked());
    ui->proxyRequiresAuth->setEnabled(ui->proxySetManually->isChecked());

    if(ui->proxySetManually->isChecked() == false) ui->proxyRequiresAuth->setChecked(false);
}

void MainWindow::proxyAuthRadioToggle()
{
    ui->proxyUsername->setEnabled(ui->proxyRequiresAuth->isChecked());
    ui->proxyPassword->setEnabled(ui->proxyRequiresAuth->isChecked());
}


void MainWindow::dialogButtonBoxTriggered(QAbstractButton* button)
{
    QDialogButtonBox::StandardButton m_type = ui->dialogButtonBox->standardButton(button);
    if (m_type == QDialogButtonBox::Ok) {
        applySettings();
        hide();
    } else if (m_type == QDialogButtonBox::Cancel) {
        loadSettings();
        hide();
    } else if (m_type == QDialogButtonBox::Apply) {
        applySettings();
    }
}

void MainWindow::applySettings()
{
    dc->stop();
    loadIcons();

    //! to destroy conf..
    {
        Configuration conf;

        conf.setValue("Browser",ui->browser->text());
        conf.setValue("FileManager",ui->fileManager->currentText());
        conf.setValue("IconSet",iconsetList->at(ui->cbIconSet->currentIndex()));

		if(ui->dropboxFolder->text() != db->getValue("dropbox_path").toString()) {

            //! @todo test this
			qDebug() << QDir(db->getValue("dropbox_path").toString()).rename(db->getValue("dropbox_path").toString(), ui->dropboxFolder->text());
			db->setValue("dropbox_path",ui->dropboxFolder->text());
        }

        conf.setValue("ShowNotifications",ui->showNotifications->isChecked());
        conf.setValue("StartDaemon",ui->startDaemon->isChecked());
        conf.setValue("AutoStart",ui->startDaemon->isChecked());
        conf.setValue("GtkUiDisabled", ui->hideGtkUI->isChecked());
		db->setValue("p2p_enabled", QVariant(ui->useP2P->isChecked()).toInt());

        dc->hideGtkUi(ui->hideGtkUI->isChecked());

        // Network
		db->setValue("throttle_download_style", (ui->downloadLimitRate->isChecked()?2:0));
		db->setValue("throttle_download_speed", QString::number(ui->downloadLimitValue->value()).append(".0"));

        int _swap = 0;
        if(ui->uploadAutoLimitRate->isChecked()) {
            _swap = 1;
        } else if (ui->uploadLimitRate->isChecked()) {
            _swap = 2;
        }
		db->setValue("throttle_upload_style", _swap);
		db->setValue("throttle_upload_speed", QString::number(ui->uploadLimitValue->value()).append(".0"));

        _swap = 0;
        if(ui->proxyAutoDetect->isChecked()) {
            _swap = 1;
        } else if (ui->proxySetManually->isChecked()) {
            _swap = 2;

            QStringList proxyType;
            proxyType << "HTTP" << "SOCKS4" << "SOCKS5";

			db->setValue("proxy_type", "'"+proxyType.value(ui->proxyType->currentIndex())+"'");
			db->setValue("proxy_server", "'"+ui->proxyServer->text()+"'");
			db->setValue("proxy_port", ui->proxyPort->value());
			db->setValue("proxy_requires_auth", QVariant(ui->proxyRequiresAuth->isChecked()).toInt());
			db->setValue("proxy_username", "'"+ui->proxyUsername->text()+"'");
        }
		db->setValue("proxy_mode", _swap);
    }

    dc->start();
}

void MainWindow::loadSettings()
{
    Configuration conf;

    QString iconset=conf.getValue("IconSet").toString();
    if (iconset.isEmpty())
        iconset="default";

    ui->cbIconSet->setCurrentIndex(iconsetList->indexOf(iconset));

    loadIcons(iconset);

    //    trayIcon->setStatus(KStatusNotifierItem::Active);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTipTitle("Kfilebox");
    trayIcon->setAssociatedWidget(trayIconMenu);

	ui->dropboxFolder->setText(db->getValue("dropbox_path").toString());
    ui->fileManager->setCurrentIndex(ui->fileManager->findText(conf.getValue("FileManager").toString()));
    ui->browser->setText(conf.getValue("Browser").toString());
    ui->showNotifications->setChecked(conf.getValue("ShowNotifications").toBool());
    ui->startDaemon->setChecked(conf.getValue("StartDaemon").toBool());

    ui->displayVersion->setText("Dropbox v" + dc->getVersion());
	ui->displayAccount->setText(db->getValue("email").toString());
	ui->useP2P->setChecked(db->getValue("p2p_enabled", 1).toBool());
    ui->hideGtkUI->setChecked(conf.getValue("GtkUiDisabled").toBool());

    // Network
    // (0: false, 1: auto, 2: true)
	int _swap = db->getValue("throttle_download_style", 0).toInt();
    ui->downloadDontLimitRate->setChecked(_swap == 0);
    ui->downloadLimitRate->setChecked(_swap == 2);
	ui->downloadLimitValue->setValue(db->getValue("throttle_download_speed", 50).toInt());
    ui->downloadLimitValue->setEnabled(ui->downloadLimitRate->isChecked());

	_swap = db->getValue("throttle_upload_style", 1).toInt();
    ui->uploadDontLimitRate->setChecked(_swap == 0);
    ui->uploadAutoLimitRate->setChecked(_swap == 1);
    ui->uploadLimitRate->setChecked(_swap == 2);
	ui->uploadLimitValue->setValue(db->getValue("throttle_upload_speed", 10).toInt());
    ui->uploadLimitValue->setEnabled(ui->uploadLimitRate->isChecked());

	_swap = db->getValue("proxy_mode").toInt();
    ui->proxyDontUse->setChecked(_swap == 0);
    ui->proxyAutoDetect->setChecked(_swap == 1);
    ui->proxySetManually->setChecked(_swap == 2);
	ui->proxyType->setCurrentIndex(ui->proxyType->findText(db->getValue("proxy_type").toString()));
    ui->proxyType->setEnabled(ui->proxySetManually->isChecked());
	ui->proxyServer->setText(db->getValue("proxy_server").toString());
    ui->proxyServer->setEnabled(ui->proxySetManually->isChecked());
	ui->proxyPort->setValue(db->getValue("proxy_port").toInt());
    ui->proxyPort->setEnabled(ui->proxySetManually->isChecked());
    ui->proxyRequiresAuth->setEnabled(ui->proxySetManually->isChecked());
	ui->proxyRequiresAuth->setChecked(db->getValue("proxy_requires_auth").toBool());
	ui->proxyUsername->setText(db->getValue("proxy_username").toString());
    ui->proxyUsername->setEnabled(ui->proxyRequiresAuth->isChecked());
    ui->proxyPassword->setEnabled(ui->proxyRequiresAuth->isChecked());

}


//! cutted from TrayIcon keep watching

void MainWindow::loadIcons(const QString &iconset)
{
    //! due to translated Iconset
    QString iconset_;
    if(iconset.isEmpty())
        iconset_ = iconsetList->at(ui->cbIconSet->currentIndex());
    else
        iconset_ = iconset;

    defaultIcon = QIcon(QString(":/icons/img/%1/kfilebox.png").arg(iconset_));
    idleIcon = QIcon(QString(":/icons/img/%1/kfilebox_idle.png").arg(iconset_));
    bussyIcon = QIcon(QString(":/icons/img/%1/kfilebox_updating.png").arg(iconset_));
    errorIcon = QIcon(QString(":/icons/img/%1/kfilebox_error.png").arg(iconset_));
    appIcon = QIcon(QString(":/icons/img/%1/kfileboxapp.png").arg(iconset_));

    ui->lblBusyIcon->setPixmap(QPixmap(QString(":/icons/img/%1/kfilebox_updating.png").arg(iconset_)));
    ui->lblDisconIcon->setPixmap(QPixmap(QString(":/icons/img/%1/kfilebox.png").arg(iconset_)));
    ui->lblIdleIcon->setPixmap(QPixmap(QString(":/icons/img/%1/kfilebox_idle.png").arg(iconset_)));
    ui->lblStopIcons->setPixmap(QPixmap(QString(":/icons/img/%1/kfilebox_error.png").arg(iconset_)));

    trayIcon->setIconByPixmap(defaultIcon);
    //    trayIcon->setIconByName(QString(":/icons/img/%1/kfilebox.png").arg(iconset_));
    trayIcon->setToolTipIconByPixmap(appIcon);
}

//! if path is file - open parent folder for it; if is empty - open Dropbox location
void MainWindow::openFileBrowser(const QString &path)
{
    QString dirName = path;
    if(path.isEmpty())
        dirName = ui->dropboxFolder->text();

    QFileInfo fileInfo(dirName);
    if(fileInfo.isFile())
        dirName = fileInfo.dir().path();

    QDesktopServices::openUrl(QUrl(dirName));
}

//! @todo create layer: to use system wide or predeffined
void MainWindow::openHelpCenterURL()
{
    QDesktopServices::openUrl(QUrl("https://www.dropbox.com/help"));
}

void MainWindow::openTourURL()
{
    QDesktopServices::openUrl(QUrl("https://www.dropbox.com/tour"));
}

void MainWindow::openForumsURL()
{
    QDesktopServices::openUrl(QUrl("http://forums.dropbox.com/"));
}

void MainWindow::openDropboxWebsiteURL()
{
    QDesktopServices::openUrl(QUrl("https://www.dropbox.com/home"));
}
void MainWindow::openGetMoreSpaceURL()
{
    QDesktopServices::openUrl(QUrl("https://www.dropbox.com/plans"));
}

void MainWindow::updateStatus(DropboxStatus newStatus, const QString &message)
{
    if(newStatus == DropboxStopped){
        startAction->setVisible(true);
        stopAction->setVisible(false);
    } else if (!stopAction->isVisible() || startAction->isVisible()) {
            startAction->setVisible(false);
            stopAction->setVisible(true);
    }

    statusAction->setText(message);
    trayIcon->setToolTipSubTitle(message);

    switch(newStatus) {
    case DropboxIdle:
        trayIcon->setIconByPixmap(idleIcon);
        break;
    case DropboxBussy:
    case DropboxUploading:
    case DropboxDownloading:
    case DropboxSaving:
    case DropboxIndexing:
        trayIcon->setIconByPixmap(bussyIcon);
        break;
    case DropboxUnkown:
    case DropboxStopped:
    case DropboxDisconnected:
    case DropboxError:
        trayIcon->setIconByPixmap(errorIcon);
        break;
    }
}

void MainWindow::prepareLastChangedFiles(){
	QStringList files = dc->getRecentlyChangedFiles();
	for (int i = 0; i < files.count(); ++i) {
		disconnect(chFiles->actions().at(i), SIGNAL(triggered()), sm, SLOT(map()));
		sm->removeMappings(chFiles->actions().at(i));
		chFiles->actions().at(i)->setText(files.at(i).split("/").last());
		chFiles->actions().at(i)->setEnabled(QFile(files.at(i)).exists());

		connect(chFiles->actions().at(i), SIGNAL(triggered()), sm, SLOT(map()));
		sm->setMapping(chFiles->actions().at(i), files.at(i));
	}
}
