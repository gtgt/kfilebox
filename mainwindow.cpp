#include <QtGui>

#include <QFileInfo>
#include <QFile>
#include <QTimer>
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

    QDir *dropbox_dist= new QDir(QDir::homePath().append(QDir::separator()).append(".dropbox-dist"));
    if (!dropbox_dist->exists(dropbox_dist->path())){
        using namespace installer;
        Daemoninstaller *di=new Daemoninstaller();
        di->downloadDaemon();
    }
    else {
        conf=new Configuration();
        conf->readSettings();
        trayIcon= new TrayIcon(conf);
        connect(trayIcon,SIGNAL(prefsWindowActionTrigered()),this,SLOT(openPrefsWindow()));
        connect(conf,SIGNAL(initializingFile()),trayIcon,SLOT(openPrefsWindow()));
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
        trayIcon->testDaemonStart();

        QTimer *timer= new QTimer(this);
        connect(timer, SIGNAL(timeout()), trayIcon, SLOT(getDropboxStatus()));

        timer->start(500);
    }
}

MainWindow::~MainWindow()
{
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


void MainWindow::on_toolButton_clicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this,tr("Dropbox folder"), ui->leDropboxFolferLocation->text());
    if (fileName.length()>0)
        ui->leDropboxFolferLocation->setText(fileName);
}

void MainWindow::on_pbSavePrefs_clicked()
{
    on_pbApplyPrefs_clicked();
    this->close();
}

void MainWindow::on_pbApplyPrefs_clicked()
{
    conf->writeSetting("Browser",ui->leBrowser->text());
    conf->writeSetting("FileManager",ui->cmbFileManager->currentText());// leFileManager->text());
    conf->writeSetting("IconSet",ui->cbIconSet->currentText());
    conf->writeSetting("DropboxFolder",ui->leDropboxFolferLocation->text());
    conf->writeSetting("ShowNotifications",ui->chkNotifications->isChecked());
    conf->writeSetting("StartDaemon",ui->chkStartDropbox->isChecked());
    conf->writeSetting("AutoStart",ui->chkStartDropbox->isChecked());
    conf->readSettings();
    trayIcon->loadIcons();

    caller->set_browser(conf->getBrowser());
}

void MainWindow::openPrefsWindow()
{
    this->show();
}
