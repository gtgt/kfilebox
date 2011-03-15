#ifndef DROPBOXCLIENT_H
#define DROPBOXCLIENT_H

#include <QDir>
#include <QLocalSocket>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QTimer>

#include "notification.h"

class DropboxClient : public QObject
{
    Q_OBJECT
public:
    enum DropboxStatus {DropboxUnkown, DropboxIdle, DropboxBussy, DropboxError, DropboxUploading, DropboxDownloading,
                        DropboxSaving, DropboxIndexing, DropboxStopped, DropboxDisconnected};

    explicit DropboxClient(QObject *parent = 0);
    ~DropboxClient();
    QString sendCommand(const QString &command);

    bool isRunning();

    //! This functions not strongly related to this class..
    QString getAuthUrl() const;
    void static hideGtkUi();
    void static showGtkUi();
    bool static isInstalled();
    QString static getVersion();

private:
    QTimer *m_timer;
    QLocalSocket *m_socket;
    QString m_socketPath;
    QProcess *m_ps;

    QString prev_message;
    DropboxStatus m_status;
    DropboxStatus prev_status;

    QString authUrl;

public slots:
    void start();
    void stop();
    //! DropboxClient::DropboxStatus getStatus();
    //! QString getStatusMessage();

private slots:
    void readDaemonOutput();
    void displayError(QLocalSocket::LocalSocketError socketError);
    void getDropboxStatus();

signals:
    void updateStatus(DropboxClient::DropboxStatus status, const QString &message);

};

#endif //DROPBOXCLIENT_H
