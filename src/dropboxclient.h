#ifndef DROPBOXCLIENT_H
#define DROPBOXCLIENT_H

#include <QDir>
#include <QLocalSocket>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QTextStream>
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
    void static hideGtkUi();
    void static showGtkUi();
    bool static isInstalled();
    QString static getVersion();

private:
    QLocalSocket* m_socket;
    QString m_socketPath;
    QTimer* m_timer;

    DropboxStatus m_status;
    DropboxStatus prev_status;
    QString prev_message;

public slots:
    void start();
    void stop();
    //! DropboxClient::DropboxStatus getStatus();
    //! QString getStatusMessage();

private slots:
    void displayError(QLocalSocket::LocalSocketError socketError);
    void getDropboxStatus();

signals:
    void updateStatus(DropboxClient::DropboxStatus status, const QString &message);

};

#endif //DROPBOXCLIENT_H
