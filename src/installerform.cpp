#include "installerform.h"
#include "ui_installerform.h"

InstallerForm::InstallerForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstallerForm)
{
    ui->setupUi(this);
    setModal(true);
    setWindowModality(Qt::ApplicationModal);

    //! @todo download to temp file name
    downloadPath=QDir::toNativeSeparators(QDir::homePath().append("/daemon.tar.gz"));

    if(QSysInfo::WordSize==64)
        daemonUrl="http://www.dropbox.com/download?plat=lnx.x86_64";
    else
        daemonUrl="http://www.dropbox.com/download?plat=lnx.x86";

    downloadDaemon();

}

InstallerForm::~InstallerForm()
{
    delete request;
    delete manager;
    delete ui;
}

void InstallerForm::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void InstallerForm::downloadDaemon()
{
    show();

    manager = new QNetworkAccessManager(this);

    file.setFileName(downloadPath);
    file.open(QIODevice::WriteOnly);

    request = new QNetworkRequest(QUrl(daemonUrl));
    request->setRawHeader("User-Agent", "Kfilebox");

    reply = manager->get(*request);

    connect(reply, SIGNAL(finished()), SLOT(downloadFinished()));
    connect(reply, SIGNAL(readyRead()), SLOT(downloadReadyRead()));

    connect(reply,SIGNAL(downloadProgress(qint64,qint64)), SLOT(setProgressValue(qint64,qint64)));
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)), SLOT(displayError(QNetworkReply::NetworkError)));

}

void InstallerForm::setProgressValue(qint64 bytesReceived, qint64 bytesTotal){
    ui->progressBar->setValue(100*bytesReceived/bytesTotal);
}

void InstallerForm::processFile()
{
    QProcess sc;
    sc.execute("tar -xf "+downloadPath+" -C "+QDir::homePath());
    sc.waitForFinished();

    hide();

    sc.startDetached("rm -f "+ downloadPath);
}


void InstallerForm::downloadFinished()
{
    file.close();
    QVariant possible_redirect=reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!possible_redirect.toString().isEmpty() && possible_redirect.toString()!=daemonUrl) {
        daemonUrl=possible_redirect.toUrl().toString();
        downloadDaemon();
    } else {
        reply->close();
        delete reply;
        processFile();
    }
}

void InstallerForm::downloadReadyRead()
{
    file.write(reply->readAll());
}

void InstallerForm::displayError(QNetworkReply::NetworkError err){
    qDebug() << err << tr("Error downloading file");
}