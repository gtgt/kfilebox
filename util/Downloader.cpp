#include "Downloader.h"

namespace util {
Downloader::Downloader(QString url, QString file)
{
    //this->url=new QUrl();
    this->url.setUrl(url);
    this->filename=file;
}

void Downloader::download()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QString file_path=filename;
    if (downloadpath.length()>0)
        file_path=downloadpath+filename;
    file.setFileName(file_path);
    file.open(QIODevice::WriteOnly);
    QNetworkRequest *request= new QNetworkRequest(QUrl(url));
    request->setRawHeader("User-Agent", "Kfilebox");
    reply = manager->get(*request);
    connect(reply, SIGNAL(finished()), SLOT(downloadFinished()));
    connect(reply, SIGNAL(readyRead()), SLOT(downloadReadyRead()));
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),SLOT(downloadProgress(qint64,qint64)));
    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),SLOT(displayError(QNetworkReply::NetworkError)));

    //qt_message_output(QtWarningMsg,"Descargando archivo desde "+ url.toString().toLatin1());

}

void Downloader::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit (downloadProgress(100*bytesReceived/bytesTotal));

}

void Downloader::downloadFinished()
{
    file.close();
    QVariant possible_redirect=reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!possible_redirect.toUrl().toString().isEmpty() && possible_redirect.toUrl()!=url){
        url=possible_redirect.toUrl().toString();
        download();
    }
    else
    {
        reply->close();
        delete reply;
        emit (fileDownloaded());
    }
}

void Downloader::downloadReadyRead()
{
    file.write(reply->readAll());
}

void Downloader::displayError(QNetworkReply::NetworkError err){
    qt_message_output(QtWarningMsg,tr("Error downloading file").toLatin1());
}

} /* End of namespace util */
