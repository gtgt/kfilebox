#include <QtGui>

#include <QFileInfo>
#include <QFile>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "util/SystemCall.h"
#include "installer/Daemoninstaller.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    caller= new SystemCall();

    if (!QFile(QDir::homePath().append("/.dropbox-dist")).exists()){
        installer::Daemoninstaller *di=new installer::Daemoninstaller();
        di->downloadDaemon();
        delete di;
    }
    else {
        conf=new Configuration();
        trayIcon= new TrayIcon(conf);
        connect(ui->saveSettings, SIGNAL(clicked()), this, SLOT(saveSettings()));
        connect(ui->applySettings, SIGNAL(clicked()), this, SLOT(applySettings()));
        connect(ui->cancelSettings, SIGNAL(clicked()), this, SLOT(hide()));
        connect(ui->moveDropboxFolder, SIGNAL(clicked()), this, SLOT(moveDropboxFolder()));
        connect(ui->cbIconSet, SIGNAL(currentIndexChanged(QString)), this, SLOT(setIcons()));
        connect(ui->unlinkComputer, SIGNAL(clicked()), this, SLOT(unlinkComputer()));


        connect(trayIcon, SIGNAL(prefsWindowActionTrigered()), this, SLOT(show()));

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
    }
}

MainWindow::~MainWindow()
{
    delete conf;
    delete caller;
    delete trayIcon;
    delete ui;
}

void MainWindow::setIcons(){
    ui->lblBusyIcon->setPixmap(QPixmap(":/icons/img/"+ui->cbIconSet->currentText()+"/kfilebox_updating.png"));
    ui->lblDisconIcon->setPixmap(QPixmap(":/icons/img/"+ui->cbIconSet->currentText()+"/kfilebox.png"));
    ui->lblIdleIcon->setPixmap(QPixmap(":/icons/img/"+ui->cbIconSet->currentText()+"/kfilebox_idle.png"));
    ui->lblStopIcons->setPixmap(QPixmap(":/icons/img/"+ui->cbIconSet->currentText()+"/kfilebox_error.png"));
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

void MainWindow::moveDropboxFolder()
{
    QString fileName = QFileDialog::getExistingDirectory(this,tr("Dropbox folder"), ui->dropboxFolder->text());

    // may be.. sed '\s\conf->getValue("dropbox_path")\ui->dropboxFolder->text()\g\' ?
    if ( (fileName.length()==0) || (conf->getValue("dropbox_path") == fileName) )
        return;

    //     dropbox.stop();
    //    QDir::rename(conf->getValue("dropbox_path"), fileName);
    conf->setValue("dropbox_path", fileName);
    //        dropbox.start();


}

void MainWindow::unlinkComputer()
{
    // comming soon
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
    loadSettings();
}

void MainWindow::applySettings()
{
    qDebug() << "applySettings called";
    return;

    conf->writeSetting("Browser",ui->browser->text());
    conf->writeSetting("FileManager",ui->fileManager->currentText());
    conf->writeSetting("IconSet",ui->cbIconSet->currentText());
    conf->writeSetting("DropboxFolder",ui->dropboxFolder->text()); //! wrong
    conf->writeSetting("ShowNotifications",ui->showNotifications->isChecked());
    conf->writeSetting("StartDaemon",ui->startDaemon->isChecked());
    conf->writeSetting("AutoStart",ui->startDaemon->isChecked());

    trayIcon->loadIcons();

    caller->set_browser(conf->getBrowser());
}

void MainWindow::loadSettings()
{
    caller->set_browser(conf->getBrowser());
    trayIcon->setCaller(caller);

    ui->dropboxFolder->setText(conf->getDropboxFolder());
    ui->fileManager->setCurrentIndex(ui->fileManager->findText(conf->getFileManager()));
    ui->browser->setText(conf->getBrowser());
    ui->showNotifications->setChecked(conf->getShowNotifications());
    ui->startDaemon->setChecked(conf->getStartDaemon());
    if (conf->getIconSet().length()>0)
        ui->cbIconSet->setCurrentIndex(ui->cbIconSet->findText(conf->getIconSet(),Qt::MatchCaseSensitive));
    else
        ui->cbIconSet->setCurrentIndex(ui->cbIconSet->findText("default",Qt::MatchCaseSensitive));
    setIcons();

    //    ui->displayVersion->setText("Dropbox v1.0.20"); // I'll find you :)
    ui->displayAccount->setText(conf->getValue("email"));
    ui->useP2P->setChecked(QVariant(conf->getValue("p2p_enabled")).toBool());
    ui->hideGtkUI->setChecked(QVariant(conf->getValue("GtkUiDisabled")).toBool());

    // Network
    // (0: false, 1: auto, ?: true)
    int _swap = QVariant(conf->getValue("throttle_download_style")).toInt();
    ui->downloadDontLimitRate->setChecked(_swap == 0);
    ui->downloadLimitRate->setChecked(_swap == 1);
    ui->downloadLimitValue->setValue(QVariant(conf->getValue("throttle_download_speed")).toInt());
    ui->downloadLimitValue->setEnabled(ui->downloadLimitRate->isChecked());

    _swap = QVariant(conf->getValue("throttle_upload_style")).toInt();
    ui->uploadAutoLimitRate->setChecked(_swap == 1);
    ui->uploadDontLimitRate->setChecked(_swap == 0);
    ui->uploadLimitRate->setChecked(_swap == 13);
    ui->uploadLimitValue->setValue(QVariant(conf->getValue("throttle_upload_speed")).toInt());
    ui->uploadLimitValue->setEnabled(ui->uploadLimitRate->isChecked());

    _swap = QVariant(conf->getValue("proxy_mode")).toInt();
    ui->proxyAutoDetect->setChecked(_swap == 1);
    ui->proxyDontUse->setChecked(_swap == 0);
    ui->proxySetManually->setChecked(_swap == 13);
    ui->proxyType->setCurrentIndex(ui->proxyType->findText(conf->getValue("proxy_type")));
    ui->proxyType->setEnabled(ui->proxySetManually->isChecked());
    ui->proxyServer->setText(conf->getValue("proxy_server"));
    ui->proxyServer->setEnabled(ui->proxySetManually->isChecked());
    ui->proxyPort->setValue(QVariant(conf->getValue("proxy_port")).toInt());
    ui->proxyPort->setEnabled(ui->proxySetManually->isChecked());
    ui->proxyRequiresAuth->setEnabled(ui->proxySetManually->isChecked());
    ui->proxyRequiresAuth->setChecked(QVariant(conf->getValue("proxy_requires_auth")).toBool());
    ui->proxyUsername->setText(conf->getValue("proxy_username"));
    ui->proxyUsername->setEnabled(ui->proxyRequiresAuth->isChecked());
    ui->proxyPassword->setEnabled(ui->proxyRequiresAuth->isChecked());

}
