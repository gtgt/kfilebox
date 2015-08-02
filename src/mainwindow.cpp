#include <KLocalizedString>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    status(DropboxUnknown)
{
    ui->setupUi(this);
    move(QApplication::desktop()->screenGeometry().center() - rect().center());

    if (!DropboxClient::isInstalled()) {
        InstallerForm di(this);
        di.exec();
    }

    dc = new DropboxClient(this);
    if (Configuration().getValue("StartDaemon").toBool())
        dc->start();

    //! []
    trayIconMenu = new QMenu(this);

    openDropboxWebsite = new QAction(tr("Launch Dropbox Website"), trayIconMenu);
    statusAction = new QAction("connecting", trayIconMenu);
    statusAction->setEnabled(false);
    openGetMoreSpace = new QAction(tr("Get More Space"), trayIconMenu);
    openPrefs = new QAction(tr("Preferences"), trayIconMenu);
    startAction = new QAction(tr("Start Dropbox"), trayIconMenu);
    stopAction = new QAction(tr("Stop Dropbox"), trayIconMenu);
    openDir = new QAction(tr("Open Dropbox Folder"), trayIconMenu);
    chFiles = new QMenu(tr("Recently changed files"), trayIconMenu);

    helpMenu = new QMenu(tr("Help"), trayIconMenu);
    openHelpCenter = new QAction(tr("Help Center"), helpMenu);
    openTour = new QAction(tr("Tour"), helpMenu);
    openForums = new QAction(tr("Forums"), helpMenu);

    helpMenu->addAction(openHelpCenter);
    helpMenu->addAction(openTour);
    helpMenu->addAction(openForums);

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

    actionMapper = new QSignalMapper(chFiles);
    connect(actionMapper, SIGNAL(mapped(const QString &)), this, SLOT(openFileBrowser(const QString &)));
    //! [/] and +

    connect(ui->dialogButtonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(dialogButtonBoxTriggered(QAbstractButton*)));
    
    connect(ui->moveDropboxFolder, SIGNAL(clicked()), this, SLOT(changeDropboxFolder()));
    connect(ui->cbIconSet, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(loadIcons())); //! not loadIcons(const QString&) cause contents is translated

    connect(dc, SIGNAL(updateStatus(DropboxStatus,QString)), this, SLOT(updateStatus(DropboxStatus,QString)));

    connect(ui->useP2P, SIGNAL(toggled(bool)), this, SLOT(useP2PToggle(bool)));

    /*connect(ui->downloadDontLimitRate, SIGNAL(toggled(bool)), this, SLOT(downloadRadioToggle()));
    connect(ui->downloadLimitRate, SIGNAL(toggled(bool)), this, SLOT(downloadRadioToggle()));

    connect(ui->uploadDontLimitRate, SIGNAL(toggled(bool)), this, SLOT(uploadRadioToggle()));
    connect(ui->uploadAutoLimitRate, SIGNAL(toggled(bool)), this, SLOT(uploadRadioToggle()));
    connect(ui->uploadLimitRate, SIGNAL(toggled(bool)), this, SLOT(uploadRadioToggle()));

    connect(ui->proxyDontUse, SIGNAL(toggled(bool)), this, SLOT(proxyRadioToggle()));
    connect(ui->proxyAutoDetect, SIGNAL(toggled(bool)), this, SLOT(proxyRadioToggle()));
    connect(ui->proxySetManually, SIGNAL(toggled(bool)), this, SLOT(proxyRadioToggle()));

    connect(ui->proxyRequiresAuth, SIGNAL(toggled(bool)), this, SLOT(proxyAuthRadioToggle()));*/

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
    iconsetList = new QStringList();
    iconsetList->push_back("default");
    iconsetList->push_back("monochrome");
    iconsetList->push_back("white");

    loadSettings();

    initializeDBus();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete iconsetList;
}

void MainWindow::show()
{
    ui->displayVersion->setText("Dropbox v" + dc->getVersion());
    QMainWindow::show();
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
    const QString dir = QFileDialog::getExistingDirectory(this, tr("Dropbox folder"), ui->dropboxFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty() && ui->dropboxFolder->text() != dir)
        ui->dropboxFolder->setText(dir);
}

void MainWindow::useP2PToggle(bool checked)
{
    dc->setP2PEnabled(checked);
}

/*void MainWindow::downloadRadioToggle()
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
}*/

void MainWindow::dialogButtonBoxTriggered(QAbstractButton* button)
{
    const QDialogButtonBox::StandardButton m_type = ui->dialogButtonBox->standardButton(button);
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
    Configuration conf;

    if (conf.getValue("GtkUiDisabled").toBool() != ui->hideGtkUI->isChecked()) {
        conf.setValue("GtkUiDisabled", ui->hideGtkUI->isChecked());

        dc->stop();
        dc->hideGtkUi(ui->hideGtkUI->isChecked());
        dc->start();
    }

    if (conf.getValue("SyncDir").toString() != ui->dropboxFolder->text()) {
        conf.setValue("SyncDir", ui->dropboxFolder->text());
    }

    conf.setValue("ShowNotifications", ui->showNotifications->isChecked());
    conf.setValue("StartDaemon", ui->startDaemon->isChecked());
    conf.setValue("AutoStart", ui->startDaemon->isChecked());
    conf.setValue("P2PEnabled", ui->useP2P->isChecked());
    conf.setValue("Browser", ui->browser->text());
    conf.setValue("FileManager", ui->fileManager->currentText());
    conf.setValue("IconSet", iconsetList->at(ui->cbIconSet->currentIndex()));

    //! @todo doesn't work with newer dropbox daemon
    // Network
    /*db->setValue("throttle_download_style", (ui->downloadLimitRate->isChecked()?2:0));
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
    db->setValue("proxy_mode", _swap);*/
}

void MainWindow::loadSettings()
{
    const Configuration conf;

    QString iconset = conf.getValue("IconSet").toString();
    if (iconset.isEmpty())
        iconset = "default";

    ui->cbIconSet->setCurrentIndex(iconsetList->indexOf(iconset));

    loadIcons(iconset);

    //    trayIcon->setStatus(KStatusNotifierItem::Active);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTipTitle("Kfilebox");
    trayIcon->setAssociatedWidget(trayIconMenu);

    ui->dropboxFolder->setText(conf.getValue("SyncDir").toString());
    ui->fileManager->setCurrentIndex(ui->fileManager->findText(conf.getValue("FileManager").toString()));
    ui->browser->setText(conf.getValue("Browser").toString());
    ui->showNotifications->setChecked(conf.getValue("ShowNotifications").toBool());
    ui->startDaemon->setChecked(conf.getValue("StartDaemon").toBool());

    ui->displayAccount->setText("");
    ui->useP2P->setChecked(conf.getValue("P2PEnabled").toBool());
    dc->setP2PEnabled(ui->useP2P->isChecked());
    ui->hideGtkUI->setChecked(conf.getValue("GtkUiDisabled").toBool());

    //! @todo doesn't work with newer dropbox daemon
    // Network
    // (0: false, 1: auto, 2: true)
    /*int _swap = db->getValue("throttle_download_style", 0).toInt();
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
    ui->proxyPassword->setEnabled(ui->proxyRequiresAuth->isChecked());*/
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

    trayIcon->setToolTipIconByPixmap(appIcon);
    updateTrayIcon();
}

//! if path is file - open parent folder for it; if is empty - open Dropbox location
void MainWindow::openFileBrowser(const QString &path) const
{
    QString dirName = path;
    if (path.isEmpty())
        dirName = ui->dropboxFolder->text();

    const QFileInfo fileInfo(dirName);
    if (fileInfo.isFile())
        dirName = fileInfo.path();

    QDesktopServices::openUrl(QUrl(dirName));
}

//! @todo create layer: to use system wide or predeffined
void MainWindow::openHelpCenterURL() const
{
    QDesktopServices::openUrl(QUrl("https://www.dropbox.com/help"));
}

void MainWindow::openTourURL() const
{
    QDesktopServices::openUrl(QUrl("https://www.dropbox.com/tour"));
}

void MainWindow::openForumsURL() const
{
    QDesktopServices::openUrl(QUrl("https://forums.dropbox.com/"));
}

void MainWindow::openDropboxWebsiteURL() const
{
    QDesktopServices::openUrl(QUrl("https://www.dropbox.com/home"));
}

void MainWindow::openGetMoreSpaceURL() const
{
    QDesktopServices::openUrl(QUrl("https://www.dropbox.com/plans"));
}

void MainWindow::updateStatus(DropboxStatus newStatus, const QString &message)
{
    status = newStatus;

    if (status == DropboxStopped) {
        startAction->setVisible(true);
        stopAction->setVisible(false);
    } else if (!stopAction->isVisible() || startAction->isVisible()) {
        startAction->setVisible(false);
        stopAction->setVisible(true);
    }

    static const QRegExp reClear("[\\n\\r\\t]");
    statusAction->setText(message.section(reClear, 0, 0));
    trayIcon->setToolTipSubTitle(message);

    updateTrayIcon();
}

void MainWindow::updateTrayIcon()
{
    switch (status) {
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
    case DropboxError:
    case DropboxUnknown:
    case DropboxDisconnected:
        trayIcon->setIconByPixmap(errorIcon);
        break;
    case DropboxStopped:
    default:
        trayIcon->setIconByPixmap(defaultIcon);
    }
}

void MainWindow::prepareLastChangedFiles()
{
    QAction *action;
    foreach (action, chFiles->actions())
    {
        disconnect(action, SIGNAL(triggered()), actionMapper, SLOT(map()));
        actionMapper->removeMappings(action);
        delete action;
    }
    chFiles->clear();

    const QStringList files = dc->getRecentlyChangedFiles();
    QFileInfo fileInfo;
    QString name;
    for (QStringList::size_type i = files.size() - 1, j = 0; i >= 0 && j < 3; --i, ++j)
    {
        fileInfo.setFile(files.at(i));
        if (fileInfo.fileName().length() > 30) {
            name = fileInfo.fileName().left(17) + "…" + fileInfo.fileName().right(10);
        }
        else {
            name = fileInfo.fileName();
        }

        action = new QAction(name, chFiles);
        connect(action, SIGNAL(triggered()), actionMapper, SLOT(map()));
        actionMapper->setMapping(action, fileInfo.path());
        chFiles->addAction(action);
    }
}
