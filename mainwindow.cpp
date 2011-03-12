#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(!DropboxClient::isInstalled()) {

        //! @todo lock MainWindow?
        InstallerForm* di = new InstallerForm(this);
        di->downloadDaemon();

        // waiting for installed
        qDebug() << "too asyncronous";

    }

    dc = new DropboxClient();
    trayIcon= new TrayIcon();

    connect(ui->saveSettings, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->applySettings, SIGNAL(clicked()), this, SLOT(applySettings()));
    connect(ui->cancelSettings, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->moveDropboxFolder, SIGNAL(clicked()), this, SLOT(changeDropboxFolder()));
    connect(ui->cbIconSet, SIGNAL(currentIndexChanged(QString)), this, SLOT(setIcons()));
    connect(ui->unlinkComputer, SIGNAL(clicked()), this, SLOT(unlinkComputer()));


    connect(trayIcon, SIGNAL(prefsWindowActionTrigered()), this, SLOT(show()));
    connect(trayIcon, SIGNAL(startDropbox()), dc, SLOT(start()));
    connect(trayIcon, SIGNAL(stopDropbox()), dc, SLOT(stop()));
    connect(dc, SIGNAL(updateStatus(DropboxClient::DropboxStatus,QString)), trayIcon, SLOT(updateStatus(DropboxClient::DropboxStatus,QString)));

    connect(ui->downloadDontLimitRate, SIGNAL(toggled(bool)), this, SLOT(downloadRadioToggle()));
    connect(ui->downloadLimitRate, SIGNAL(toggled(bool)), this, SLOT(downloadRadioToggle()));

    connect(ui->uploadDontLimitRate, SIGNAL(toggled(bool)), this, SLOT(uploadRadioToggle()));
    connect(ui->uploadAutoLimitRate, SIGNAL(toggled(bool)), this, SLOT(uploadRadioToggle()));
    connect(ui->uploadLimitRate, SIGNAL(toggled(bool)), this, SLOT(uploadRadioToggle()));

    connect(ui->proxyDontUse, SIGNAL(toggled(bool)), this, SLOT(proxyRadioToggle()));
    connect(ui->proxyAutoDetect, SIGNAL(toggled(bool)), this, SLOT(proxyRadioToggle()));
    connect(ui->proxySetManually, SIGNAL(toggled(bool)), this, SLOT(proxyRadioToggle()));

    connect(ui->proxyRequiresAuth, SIGNAL(toggled(bool)), this, SLOT(proxyAuthRadioToggle()));

    loadSettings();


    if(ui->startDaemon->isChecked())
        dc->start();

    initializeDBus();
}

MainWindow::~MainWindow()
{
    delete trayIcon;
    delete adaptor;
    delete dc;
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
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.registerObject("/Kfilebox", dc);
    connection.registerService("org.kde.Kfilebox");

    connect(dc, SIGNAL(updateStatus(DropboxClient::DropboxStatus,QString)), adaptor, SIGNAL(updateStatus(DropboxClient::DropboxStatus,QString)));
    //! @todo a lot of work:)
}

void MainWindow::setIcons(){
    ui->lblBusyIcon->setPixmap(QPixmap(":/icons/img/"+ui->cbIconSet->currentText()+"/kfilebox_updating.png"));
    ui->lblDisconIcon->setPixmap(QPixmap(":/icons/img/"+ui->cbIconSet->currentText()+"/kfilebox.png"));
    ui->lblIdleIcon->setPixmap(QPixmap(":/icons/img/"+ui->cbIconSet->currentText()+"/kfilebox_idle.png"));
    ui->lblStopIcons->setPixmap(QPixmap(":/icons/img/"+ui->cbIconSet->currentText()+"/kfilebox_error.png"));
}

void MainWindow::changeDropboxFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this,tr("Dropbox folder"), ui->dropboxFolder->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if ( (dir.length()!=0) && (ui->dropboxFolder->text() != dir) )
        ui->dropboxFolder->setText(dir);
}

void MainWindow::unlinkComputer()
{
    //! @todo just delete key host_id?
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


void MainWindow::saveSettings()
{
    applySettings();
    hide();
}

//how to get list only of changed elements?
void MainWindow::applySettings()
{
    dc->stop();

    trayIcon->loadIcons(ui->cbIconSet->currentText());

    //! to destroy conf..
    {
        Configuration conf;

        conf.setValue("Browser",ui->browser->text());
        conf.setValue("FileManager",ui->fileManager->currentText());
        conf.setValue("IconSet",ui->cbIconSet->currentText());

        if(ui->dropboxFolder->text() != conf.getValue("dropbox_path").toString()) {

            //! @todo test this
            qDebug() << QDir(conf.getValue("dropbox_path").toString()).rename(conf.getValue("dropbox_path").toString(), ui->dropboxFolder->text());
            conf.setValue("dropbox_path",ui->dropboxFolder->text());
        }

        conf.setValue("ShowNotifications",ui->showNotifications->isChecked());
        conf.setValue("StartDaemon",ui->startDaemon->isChecked());
        conf.setValue("AutoStart",ui->startDaemon->isChecked());
        conf.setValue("GtkUiDisabled",ui->hideGtkUI->isChecked());

        //! @todo add more options to save..

        if(ui->hideGtkUI->isChecked()) {
            dc->hideGtkUi();
        }else {
            dc->showGtkUi();
        }
        conf.setValue("GtkUiDisabled", ui->hideGtkUI->isChecked());
        conf.setValue("p2p_enabled", QVariant(ui->useP2P->isChecked()).toInt());

        // Network
        conf.setValue("throttle_download_style", QVariant(ui->downloadLimitRate->isChecked()).toInt());
        conf.setValue("throttle_download_speed", ui->downloadLimitValue->value());

        int _swap = 0;

        if(ui->uploadAutoLimitRate->isChecked()) {
            _swap = 1;
        } else if (ui->uploadLimitRate->isChecked()) {
            _swap = 2;
        }
        conf.setValue("throttle_upload_style", _swap);
        conf.setValue("throttle_upload_speed", ui->uploadLimitValue->value());

        _swap = 0;
        if(ui->proxyAutoDetect->isChecked()) {
            _swap = 1;
        } else if (ui->proxySetManually->isChecked()) {
            _swap = 2;

            QStringList proxyType;
            proxyType.push_back("HTTP");
            proxyType.push_back("SOCKS4");
            proxyType.push_back("SOCKS5");

            conf.setValue("proxy_type", proxyType.value(ui->proxyType->currentIndex()));
            conf.setValue("proxy_server", ui->proxyServer->text());
            conf.setValue("proxy_port", ui->proxyPort->value());
            conf.setValue("proxy_requires_auth", QVariant(ui->proxyRequiresAuth->isChecked()).toInt());
            conf.setValue("proxy_username", ui->proxyUsername->text());
        }
        conf.setValue("proxy_mode", _swap);
    }

    dc->start();
}

void MainWindow::loadSettings()
{
    Configuration conf;

    QString iconset=conf.getValue("IconSet").toString();
    if (iconset.length()==0)
        iconset="default";

    trayIcon->loadIcons(iconset);


    ui->dropboxFolder->setText(conf.getValue("dropbox_path").toString());
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
    ui->displayAccount->setText(conf.getValue("email").toString());
    ui->useP2P->setChecked(conf.getValue("p2p_enabled").toBool());
    ui->useP2P->setEnabled(conf.hasKey("p2p_enabled"));
    //! @todo Configuration can't know where to save this key (in dropbox db or in self conf file). On first start in dbthis value is missing..
    ui->hideGtkUI->setChecked(conf.getValue("GtkUiDisabled").toBool());

    // Network
    // (0: false, 1: auto, 2: true)
    int _swap = conf.getValue("throttle_download_style").toInt();
    ui->downloadDontLimitRate->setChecked(_swap == 0);
    ui->downloadLimitRate->setChecked(_swap == 1);
    ui->downloadLimitValue->setValue(conf.getValue("throttle_download_speed").toInt());
    ui->downloadLimitValue->setEnabled(ui->downloadLimitRate->isChecked());

    _swap = conf.getValue("throttle_upload_style").toInt();
    ui->uploadAutoLimitRate->setChecked(_swap == 1);
    ui->uploadDontLimitRate->setChecked(_swap == 0);
    ui->uploadLimitRate->setChecked(_swap == 2);
    ui->uploadLimitValue->setValue(conf.getValue("throttle_upload_speed").toInt());
    ui->uploadLimitValue->setEnabled(ui->uploadLimitRate->isChecked());

    _swap = conf.getValue("proxy_mode").toInt();
    ui->proxyAutoDetect->setChecked(_swap == 1);
    ui->proxyDontUse->setChecked(_swap == 0);
    ui->proxySetManually->setChecked(_swap == 2);
    ui->proxyType->setCurrentIndex(ui->proxyType->findText(conf.getValue("proxy_type").toString()));
    ui->proxyType->setEnabled(ui->proxySetManually->isChecked());
    ui->proxyServer->setText(conf.getValue("proxy_server").toString());
    ui->proxyServer->setEnabled(ui->proxySetManually->isChecked());
    ui->proxyPort->setValue(conf.getValue("proxy_port").toInt());
    ui->proxyPort->setEnabled(ui->proxySetManually->isChecked());
    ui->proxyRequiresAuth->setEnabled(ui->proxySetManually->isChecked());
    ui->proxyRequiresAuth->setChecked(conf.getValue("proxy_requires_auth").toBool());
    ui->proxyUsername->setText(conf.getValue("proxy_username").toString());
    ui->proxyUsername->setEnabled(ui->proxyRequiresAuth->isChecked());
    ui->proxyPassword->setEnabled(ui->proxyRequiresAuth->isChecked());

}
