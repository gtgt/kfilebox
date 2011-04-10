#include "mainwindow.h"
#include "ui_mainwindow.h"

int pow(int src) {
    if(src==0||src==1)
        return 1;
    return pow(src-1);
}

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

    connect(ui->dialogButtonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(dialogButtonBoxTriggered(QAbstractButton*)));
    connect(ui->moveDropboxFolder, SIGNAL(clicked()), this, SLOT(changeDropboxFolder()));
    connect(ui->cbIconSet, SIGNAL(currentIndexChanged(QString)), this, SLOT(setIcons()));

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

    connect(ui->openDir, SIGNAL(triggered()), this, SLOT(openFileBrowser()));
    connect(ui->openHelpCenter, SIGNAL(triggered()), this, SLOT(openHelpCenterURL()));
    connect(ui->openDropboxWebsite, SIGNAL(triggered()), this, SLOT(openDropboxWebsiteURL()));
    connect(ui->openGetMoreSpace, SIGNAL(triggered()), this, SLOT(openGetMoreSpaceURL()));
    connect(ui->openTour, SIGNAL(triggered()), this, SLOT(openTourURL()));
    connect(ui->openForums, SIGNAL(triggered()), this, SLOT(openForumsURL()));
    connect(ui->openPrefs, SIGNAL(triggered()), this, SLOT(show()));
    connect(ui->startAction, SIGNAL(triggered()), dc, SLOT(start()));
    connect(ui->stopAction, SIGNAL(triggered()), dc, SLOT(stop()));
    connect(ui->chFiles, SIGNAL(aboutToShow()), this, SLOT(prepareLastChangedFiles()));

    ui->startAction->setVisible(false);
    ui->menuBar->setVisible(false);

    loadSettings();

    trayIcon = new KStatusNotifierItem(this);

    sm = new QSignalMapper(this);
    connect(sm, SIGNAL(mapped(const QString &)), this, SLOT(openFileBrowser(const QString &)));

    trayIcon->setStatus(KStatusNotifierItem::Active);
    trayIcon->setContextMenu(static_cast<KMenu*>(ui->trayIconMenu));
    trayIcon->setIconByPixmap(defaultIcon);
    trayIcon->setToolTipIconByPixmap(appIcon);
    trayIcon->setToolTipTitle("Kfilebox");
    trayIcon->setAssociatedWidget(ui->trayIconMenu);

    if(ui->startDaemon->isChecked())
        dc->start();

    initializeDBus();
}

MainWindow::~MainWindow()
{
    delete ui;
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
    connect(dc, SIGNAL(updateStatus(DropboxStatus,QString)), adaptor, SIGNAL(updateStatus(DropboxStatus,QString)));
    //! @todo a lot of work:)
}

void MainWindow::setIcons(){
    ui->lblBusyIcon->setPixmap(QPixmap(QString(":/icons/img/%1/kfilebox_updating.png").arg(ui->cbIconSet->currentText())));
    ui->lblDisconIcon->setPixmap(QPixmap(QString(":/icons/img/%1/kfilebox.png").arg(ui->cbIconSet->currentText())));
    ui->lblIdleIcon->setPixmap(QPixmap(QString(":/icons/img/%1/kfilebox_idle.png").arg(ui->cbIconSet->currentText())));
    ui->lblStopIcons->setPixmap(QPixmap(QString(":/icons/img/%1/kfilebox_error.png").arg(ui->cbIconSet->currentText())));
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
//! @bug when by hand stoping the dropbox status is not changed..
void MainWindow::applySettings()
{
    dc->stop();

    loadIcons(ui->cbIconSet->currentText());

    //! to destroy conf..
    {
        Configuration conf;
        ConfigurationDBDriver db;

        conf.setValue("Browser",ui->browser->text());
        conf.setValue("FileManager",ui->fileManager->currentText());
        conf.setValue("IconSet",ui->cbIconSet->currentText());

        if(ui->dropboxFolder->text() != db.getValue("dropbox_path").toString()) {

            //! @todo test this
            qDebug() << QDir(db.getValue("dropbox_path").toString()).rename(db.getValue("dropbox_path").toString(), ui->dropboxFolder->text());
            db.setValue("dropbox_path",ui->dropboxFolder->text());
        }

        conf.setValue("ShowNotifications",ui->showNotifications->isChecked());
        conf.setValue("StartDaemon",ui->startDaemon->isChecked());
        conf.setValue("AutoStart",ui->startDaemon->isChecked());
        conf.setValue("GtkUiDisabled", ui->hideGtkUI->isChecked());
        db.setValue("p2p_enabled", QVariant(ui->useP2P->isChecked()).toInt());

        //! @todo add more options to save..

        dc->hideGtkUi(ui->hideGtkUI->isChecked());

        // Network
        db.setValue("throttle_download_style", (ui->downloadLimitRate->isChecked()?2:0));
        db.setValue("throttle_download_speed", QString::number(ui->downloadLimitValue->value()).append(".0"));

        int _swap = 0;
        if(ui->uploadAutoLimitRate->isChecked()) {
            _swap = 1;
        } else if (ui->uploadLimitRate->isChecked()) {
            _swap = 2;
        }
        db.setValue("throttle_upload_style", _swap);
        db.setValue("throttle_upload_speed", QString::number(ui->uploadLimitValue->value()).append(".0"));

        _swap = 0;
        if(ui->proxyAutoDetect->isChecked()) {
            _swap = 1;
        } else if (ui->proxySetManually->isChecked()) {
            _swap = 2;

            QStringList proxyType;
            proxyType << "HTTP" << "SOCKS4" << "SOCKS5";

            db.setValue("proxy_type", proxyType.value(ui->proxyType->currentIndex()));
            db.setValue("proxy_server", ui->proxyServer->text());
            db.setValue("proxy_port", ui->proxyPort->value());
            db.setValue("proxy_requires_auth", QVariant(ui->proxyRequiresAuth->isChecked()).toInt());
            db.setValue("proxy_username", ui->proxyUsername->text());
        }
        db.setValue("proxy_mode", _swap);
    }

    //! @todo start not always
    dc->start();
}

void MainWindow::loadSettings()
{
    Configuration conf;
    ConfigurationDBDriver db;

    QString iconset=conf.getValue("IconSet").toString();
    if (iconset.isEmpty())
        iconset="default";

    loadIcons(iconset);


    ui->dropboxFolder->setText(db.getValue("dropbox_path").toString());
    ui->fileManager->setCurrentIndex(ui->fileManager->findText(conf.getValue("FileManager").toString()));
    ui->browser->setText(conf.getValue("Browser").toString());
    ui->showNotifications->setChecked(conf.getValue("ShowNotifications").toBool());
    ui->startDaemon->setChecked(conf.getValue("StartDaemon").toBool());

    if (conf.getValue("IconSet").toString().length()>0)
        ui->cbIconSet->setCurrentIndex(ui->cbIconSet->findText(conf.getValue("IconSet").toString(),Qt::MatchCaseSensitive));
    else
        ui->cbIconSet->setCurrentIndex(ui->cbIconSet->findText("default",Qt::MatchCaseSensitive));
    setIcons();

    ui->displayVersion->setText("Dropbox v" + dc->getVersion());
    ui->displayAccount->setText(db.getValue("email").toString());
    ui->useP2P->setChecked(db.getValue("p2p_enabled", 1).toBool());
    ui->hideGtkUI->setChecked(conf.getValue("GtkUiDisabled").toBool());

    // Network
    // (0: false, 1: auto, 2: true)
    int _swap = db.getValue("throttle_download_style", 0).toInt();
    ui->downloadDontLimitRate->setChecked(_swap == 0);
    ui->downloadLimitRate->setChecked(_swap == 2);
    ui->downloadLimitValue->setValue(db.getValue("throttle_download_speed", 50).toInt());
    ui->downloadLimitValue->setEnabled(ui->downloadLimitRate->isChecked());

    _swap = db.getValue("throttle_upload_style", 1).toInt();
    ui->uploadDontLimitRate->setChecked(_swap == 0);
    ui->uploadAutoLimitRate->setChecked(_swap == 1);
    ui->uploadLimitRate->setChecked(_swap == 2);
    ui->uploadLimitValue->setValue(db.getValue("throttle_upload_speed", 10).toInt());
    ui->uploadLimitValue->setEnabled(ui->uploadLimitRate->isChecked());

    _swap = db.getValue("proxy_mode").toInt();
    ui->proxyDontUse->setChecked(_swap == 0);
    ui->proxyAutoDetect->setChecked(_swap == 1);
    ui->proxySetManually->setChecked(_swap == 2);
    ui->proxyType->setCurrentIndex(ui->proxyType->findText(db.getValue("proxy_type").toString()));
    ui->proxyType->setEnabled(ui->proxySetManually->isChecked());
    ui->proxyServer->setText(db.getValue("proxy_server").toString());
    ui->proxyServer->setEnabled(ui->proxySetManually->isChecked());
    ui->proxyPort->setValue(db.getValue("proxy_port").toInt());
    ui->proxyPort->setEnabled(ui->proxySetManually->isChecked());
    ui->proxyRequiresAuth->setEnabled(ui->proxySetManually->isChecked());
    ui->proxyRequiresAuth->setChecked(db.getValue("proxy_requires_auth").toBool());
    ui->proxyUsername->setText(db.getValue("proxy_username").toString());
    ui->proxyUsername->setEnabled(ui->proxyRequiresAuth->isChecked());
    ui->proxyPassword->setEnabled(ui->proxyRequiresAuth->isChecked());

}


//! cutted from TrayIcon keep watching

void MainWindow::loadIcons(const QString &iconset)
{
    defaultIcon = QIcon(QString(":/icons/img/%1/kfilebox.png").arg(iconset));
    idleIcon = QIcon(QString(":/icons/img/%1/kfilebox_idle.png").arg(iconset));
    bussyIcon = QIcon(QString(":/icons/img/%1/kfilebox_updating.png").arg(iconset));
    errorIcon = QIcon(QString(":/icons/img/%1/kfilebox_error.png").arg(iconset));
    appIcon = QIcon(QString(":/icons/img/%1/kfileboxapp.png").arg(iconset));

    if(trayIcon!=NULL)
        trayIcon->setToolTipIconByPixmap(appIcon);
}

void MainWindow::openFileBrowser(const QString &path)
{
    //! if file - open parent folder for it
    QString dirName = QDir::toNativeSeparators(ui->dropboxFolder->text().append(path));
    QFileInfo fileInfo(dirName);
    if(fileInfo.isFile())
        dirName = fileInfo.dir().path();

    QDesktopServices::openUrl(QUrl(dirName));
}

//! @todo get this urls from dropbox daemon

//! @todo
//helper::runner::perform(QUrl url) {
//    if(options[useNative])
//        QDesktopServices::openUrl(url);
//    else
//        QProcess::startDetached(options[browser] + options[browserOptions] + url);
//}

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
        if(ui->stopAction->isVisible() || !ui->startAction->isVisible()) {
            ui->startAction->setVisible(true);
            ui->stopAction->setVisible(false);
        }
    } else{
        if (!ui->stopAction->isVisible() || ui->startAction->isVisible()) {
            ui->startAction->setVisible(false);
            ui->stopAction->setVisible(true);
        }
    }

    ui->statusAction->setText(message);
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

//! @todo recent files from shared folders
//! in db '/gp/lacrimoza.gp5'
//! absolute path is '~/Dropbox/shared-folder/' + that file
void MainWindow::prepareLastChangedFiles(){
    QStringList files;
    ConfigurationDBDriver conf;
    QString recentlyChanged = conf.getValue("recently_changed3").toString();

    if(recentlyChanged.isEmpty()) return;
    foreach (QString elem, recentlyChanged.split("\n")) {
        QStringList list = elem.split(":");
        if(list.length()>1)
            files.push_back(fixUnicodeChars(list.value(1)));
    }

    for (int i = 0; i < 5; ++i) {
        disconnect(ui->chFiles->actions().at(i), SIGNAL(triggered()), sm, SLOT(map()));
        sm->removeMappings(ui->chFiles->actions().at(i));
        ui->chFiles->actions().at(i)->setText(files.at(i).split("/").last());
        ui->chFiles->actions().at(i)->setEnabled(QFile(ui->dropboxFolder->text()+files.at(i)).exists());

        connect(ui->chFiles->actions().at(i), SIGNAL(triggered()), sm, SLOT(map()));
        sm->setMapping(ui->chFiles->actions().at(i), files.at(i));
    }
}

//! `\u0441\u043D\u0438\u043C\u043E\u043A38.png'
//! convert to `снимок38.png'
//! hope somebody will find normal solution)
QString MainWindow::fixUnicodeChars(QString value)
{
    QString humanResult;
    QStringList toHumanable = value.split("\\u");
    if(toHumanable.length()>1) {
        humanResult = toHumanable.first();
        for(int i=1; i<toHumanable.length(); i++ ) {
            if(toHumanable.at(i).length()!=4)
                humanResult.append(QChar(toHumanable.at(i).mid(0, 4).toInt(0, 16))).append(toHumanable.at(i).mid(4));
            else
                humanResult.append(QChar(toHumanable.at(i).toInt(0, 16)));
        }
        return humanResult;
    } else
        return value;
}
