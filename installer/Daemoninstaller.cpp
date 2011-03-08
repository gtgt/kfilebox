#include "Daemoninstaller.h"

namespace installer {

using namespace util;

Daemoninstaller::Daemoninstaller()
{
    downloadPath=QDir::home().path().append(QDir::separator());
    //downloadPath.append("tmp").append(QDir::separator());

    if(QSysInfo::WordSize==64)
        daemonUrl="http://www.dropbox.com/download?plat=lnx.x86_64";
    else
        daemonUrl="http://www.dropbox.com/download?plat=lnx.x86";

}

void Daemoninstaller::downloadDaemon()
{
    Downloader *d= new Downloader(this->daemonUrl,"daemon.tar.gz");
    d->set_downloadpath(downloadPath);
    connect(d,SIGNAL(fileDownloaded()),this,SLOT(onFileDownloaded()));
    form = new InstallerForm();
    form->show();
    connect(d,SIGNAL(downloadProgress(int)),this,SLOT(onDownloadProgressChange(int)));
    d->download();
}

void Daemoninstaller::extract()
{
    QProcess sc;

    sc.setWorkingDirectory(downloadPath);
    downloadPath.append("daemon.tar.gz");

    sc.execute("tar -xf "+downloadPath);
    sc.waitForFinished();
    sc.close();
    executeWizzard();

}

void Daemoninstaller::executeWizzard()
{
    QProcess sc;
    sc.startDetached(QDir::toNativeSeparators(QDir::homePath().append("/.dropbox-dist/dropboxd")));
    sc.waitForStarted();
    preventGtkGuiExecution();
    //sc->close();
}

void Daemoninstaller::preventGtkGuiExecution()
{
    form->hide();
    delete form;
    form = 0;

    sleep(5);
    QProcess sc;
    sc.startDetached("mv "+QDir::homePath()+"/.dropbox-dist/wx._controls_.so "+QDir::homePath()+"/.dropbox-dist/wx._controls_orig.so");
    sc.startDetached("rm "+ downloadPath);
    //sc->close();
    quick_exit(1);
}

void Daemoninstaller::onFileDownloaded(){
    extract();
}

void Daemoninstaller::onFileExtracted(int i){
    qt_message_output(QtWarningMsg,tr("File extracted").toLatin1());
}

void Daemoninstaller::onDownloadProgressChange(int i){
    form->setProgressValue(i);

}

} /* End of namespace installer */
