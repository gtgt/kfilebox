#ifndef DROPBOXCLIENT_H
#define DROPBOXCLIENT_H

#include <QDir>
#include <QLocalSocket>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QTimer>

#include "src/notification.h"

//! @todo provide 2 low level classes asynchronous(to get status in loop) and syncronous(to query commands like get_folder_tag)

enum DropboxStatus {DropboxUnkown, DropboxIdle, DropboxBussy, DropboxError, DropboxUploading, DropboxDownloading,
                    DropboxSaving, DropboxIndexing, DropboxStopped, DropboxDisconnected};

class DropboxClient : public QObject
{
    Q_OBJECT
public:
    explicit DropboxClient(QObject *parent = 0);
    ~DropboxClient();
    void sendCommand(const QString &command);

    bool isRunning();

    //! This functions not strongly related to this class..
    QString getAuthUrl() const;
    void static hideGtkUi(bool v);
    bool static isInstalled();
    QString static getVersion();
    //    void getSharedFolders(const QString& to);

private:
    QTimer *m_timer;
    QLocalSocket *m_socket;
    QString m_socketPath;
    QProcess *m_ps;

    QString prev_message;
    DropboxStatus m_status;
    DropboxStatus prev_status;
    //    QStringList* m_sharedFolders;

    QString authUrl;

public slots:
    void start();
    void stop();
    //! DropboxClient::DropboxStatus getStatus();
    //! QString getStatusMessage();

private slots:
    void readDaemonOutput();
    void receiveReply();
    void processReply(const QString &message);
    void displayError(QLocalSocket::LocalSocketError socketError);
    void getDropboxStatus();

signals:
    void updateStatus(DropboxStatus status, const QString &message);

};

#endif //DROPBOXCLIENT_H
