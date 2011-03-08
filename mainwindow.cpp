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

    caller= new SystemCall();
    ui->setupUi(this);

    if (!QFile(QDir::homePath().append("/.dropbox-dist")).exists()){
        //        using namespace installer;
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

        connect(trayIcon, SIGNAL(prefsWindowActionTrigered()), this, SLOT(show()));
        //connect(conf)

        ui->leDropboxFolferLocation->setText(conf->getDropboxFolder());
        ui->cmbFileManager->setCurrentIndex(ui->cmbFileManager->findText(conf->getFileManager())); //leFileManager->setText(conf->getFileManager());
        ui->leBrowser->setText(conf->getBrowser());
        ui->chkNotifications->setChecked(conf->getShowNotifications());
        ui->chkStartDropbox->setChecked(conf->getStartDaemon());        
        if (conf->getIconSet().length()>0)
            ui->cbIconSet->setCurrentIndex(ui->cbIconSet->findText(conf->getIconSet(),Qt::MatchCaseSensitive));
        else
            ui->cbIconSet->setCurrentIndex(ui->cbIconSet->findText("default",Qt::MatchCaseSensitive));
        setIcons();
        caller->set_browser(conf->getBrowser());
        trayIcon->setCaller(caller);

//        ui->displayVersion->setText("Dropbox v1.0.20"); // searching
        ui->displayAccount->setText(conf->getValue("email"));

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
    //! dropbox.stop()
    //! dropbox.settings.folder = newer location
    //! FS move dropbox folder
    //! dropbox.start()

    QString fileName = QFileDialog::getExistingDirectory(this,tr("Dropbox folder"), ui->leDropboxFolferLocation->text());
    if (fileName.length()>0)
        ui->leDropboxFolferLocation->setText(fileName);
}

void MainWindow::saveSettings()
{
    qDebug() << "saveSettings called";
    return;
    applySettings();
    hide();
}

void MainWindow::applySettings()
{
    qDebug() << "applySettings called";
    return;

    conf->writeSetting("Browser",ui->leBrowser->text());
    conf->writeSetting("FileManager",ui->cmbFileManager->currentText());// leFileManager->text());
    conf->writeSetting("IconSet",ui->cbIconSet->currentText());
    conf->writeSetting("DropboxFolder",ui->leDropboxFolferLocation->text());
    conf->writeSetting("ShowNotifications",ui->chkNotifications->isChecked());
    conf->writeSetting("StartDaemon",ui->chkStartDropbox->isChecked());
    conf->writeSetting("AutoStart",ui->chkStartDropbox->isChecked());

    trayIcon->loadIcons();

    caller->set_browser(conf->getBrowser());
}
