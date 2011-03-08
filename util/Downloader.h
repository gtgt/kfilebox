#ifndef util_Downloader_h
#define util_Downloader_h

#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>



namespace util {

class Downloader: public QObject {
    Q_OBJECT
public:

    /** simple access function to set the attribute url by function
      * @param value value to set for the attribute url
      */
    void set_url( QUrl value ) { url = value; };

    /** simple access function to get the attribute url by function */
    QUrl get_url( void ) const { return url; };

    /** simple access function to set the attribute filename by function
      * @param value value to set for the attribute filename
      */
    void set_filename( QString value ) { filename = value; };

    /** simple access function to get the attribute filename by function */
    QString get_filename( void ) const { return filename; };

    /** simple access function to set the attribute filename by function
      * @param value value to set for the attribute filename
      */
    void set_downloadpath( QString value ) { downloadpath = value; };

    /** simple access function to get the attribute filename by function */
    QString get_downloadpath( void ) const { return downloadpath; };

    Downloader(QString url, QString file);

    void download();


    Downloader();

Q_SIGNALS:
    void fileDownloaded();
    void downloadProgress(int);

private:
    QFile file;
    QNetworkReply *reply;
    QUrl url;

    QString filename;


    QString downloadpath;

public Q_SLOTS:
    void downloadFinished();
    void downloadReadyRead();
    void downloadProgress(qint64,qint64);
    void displayError(QNetworkReply::NetworkError);

};

} /* End of namespace util */

#endif // util_Downloader_h
