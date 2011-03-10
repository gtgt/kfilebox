#include "Daemoninstaller.h"

#include <QDebug>

namespace installer {

//using namespace util;

Daemoninstaller::Daemoninstaller()
{
    //! @todo download to temp file name
    downloadPath=QDir::toNativeSeparators(QDir::homePath().append("/daemon.tar.gz"));

    if(QSysInfo::WordSize==64)
        daemonUrl="http://www.dropbox.com/download?plat=lnx.x86_64";
    else
        daemonUrl="http://www.dropbox.com/download?plat=lnx.x86";

}

Daemoninstaller::~Daemoninstaller()
{
    delete form;
    form = 0;
}

void Daemoninstaller::downloadDaemon()
{
    qDebug() << "installer 0";

    form = new InstallerForm();
    form->show();

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    file.setFileName(downloadPath);
    file.open(QIODevice::WriteOnly);
    QNetworkRequest* request = new QNetworkRequest(QUrl(daemonUrl));
    request->setRawHeader("User-Agent", "Kfilebox");
    reply = manager->get(*request);


    connect(reply, SIGNAL(finished()), SLOT(downloadFinished()));
    connect(reply, SIGNAL(readyRead()), SLOT(downloadReadyRead()));

    connect(reply,SIGNAL(downloadProgress(qint64,qint64)), form, SLOT(setProgressValue(qint64,qint64)));
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)), SLOT(displayError(QNetworkReply::NetworkError)));

qDebug() << "installer 1";
}

void Daemoninstaller::extract()
{
    QProcess sc;

    sc.setWorkingDirectory(QFileInfo(downloadPath).dir().path());

    sc.execute("tar -xf "+downloadPath);
    sc.waitForFinished();
    executeWizzard();

}

void Daemoninstaller::executeWizzard()
{
//    QProcess sc;
//    sc.startDetached(QDir::toNativeSeparators(QDir::homePath().append("/.dropbox-dist/dropboxd")));
//    sc.waitForStarted();
    preventGtkGuiExecution();
}

void Daemoninstaller::preventGtkGuiExecution()
{
    form->hide();

//    sleep(5);
    QProcess sc;
//    sc.startDetached("mv "+QDir::homePath()+"/.dropbox-dist/wx._controls_.so "+QDir::homePath()+"/.dropbox-dist/wx._controls_orig.so");
    sc.startDetached("rm -f "+ downloadPath);
//    quick_exit(1); //! ??
}



void Daemoninstaller::downloadFinished()
{
    file.close();
    QVariant possible_redirect=reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!possible_redirect.toString().isEmpty() && possible_redirect.toString()!=daemonUrl){
        daemonUrl=possible_redirect.toUrl().toString();
        downloadDaemon();
    }
    else
    {
        reply->close();
        delete reply;
        extract();
    }
}

void Daemoninstaller::downloadReadyRead()
{
    file.write(reply->readAll());
}

void Daemoninstaller::displayError(QNetworkReply::NetworkError err){
    qt_message_output(QtWarningMsg,tr("Error downloading file").toLatin1());
}






} /* End of namespace installer */
