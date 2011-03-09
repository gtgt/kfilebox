#ifndef installer_Daemoninstaller_h
#define installer_Daemoninstaller_h

#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QObject>
#include <QString>


#include "installerform.h"

namespace installer {

class Daemoninstaller: public QObject {
    Q_OBJECT
public:

    Daemoninstaller();
    ~Daemoninstaller();

    void downloadDaemon();

    void executeWizzard();

    void preventGtkGuiExecution();

public slots:
    void extract();

    void downloadFinished();
    void downloadReadyRead();
    void displayError(QNetworkReply::NetworkError);

private:
    QString daemonUrl;
    QString downloadPath;
    InstallerForm *form;

    QFile file;
    QNetworkReply *reply;
    QUrl url;


};

} /* End of namespace installer */

#endif // installer_Daemoninstaller_h
