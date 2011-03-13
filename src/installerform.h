#ifndef INSTALLERFORM_H
#define INSTALLERFORM_H

#include <QDebug>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QString>
#include <QUrl>

namespace Ui {
class InstallerForm;
}

class InstallerForm : public QDialog
{
    Q_OBJECT
private:
    Ui::InstallerForm *ui;
    QString daemonUrl;
    QString downloadPath;

    QFile file;
    QNetworkReply* reply;
    QNetworkRequest* request;
    QNetworkAccessManager* manager;

protected:
    void changeEvent(QEvent *e);

public:
    explicit InstallerForm(QWidget *parent = 0);
    ~InstallerForm();

public slots:
    void downloadDaemon();
    void processFile();

    void setProgressValue(qint64, qint64);
    void downloadFinished();
    void downloadReadyRead();
    void displayError(QNetworkReply::NetworkError);

};

#endif // INSTALLERFORM_H
