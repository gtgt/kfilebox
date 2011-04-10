#ifndef DROPBOXCLIENT_H
#define DROPBOXCLIENT_H

#include <QDir>
#include <QLocalSocket>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QTimer>

#include "src/notification.h"

// for memory
// ("", "shared", "", "", "", "photos", "public", "", "")

//! @todo provide 2 low level classes asynchronous(to get status in loop) and syncronous(to query commands like get_folder_tag)

enum DropboxStatus {DropboxUnkown, DropboxIdle, DropboxBussy, DropboxError, DropboxUploading, DropboxDownloading,
                    DropboxSaving, DropboxIndexing, DropboxStopped, DropboxDisconnected};

class DropboxClient : public QObject
{
    Q_OBJECT
public:
    explicit DropboxClient(QObject* parent = 0);
    ~DropboxClient();

    void sendCommand(const QString& command);

    //! This functions not strongly related to this class..
    bool isRunning();
    void static hideGtkUi(bool v);
    bool static isInstalled();
    QString static getVersion();

    inline QString getAuthUrl() const {return m_authUrl;}

private:
    void getSharedFolders(const QString& to);

    QTimer* m_timer;
    QLocalSocket* m_socket;
    QProcess* m_ps;
    QString m_message, m_authUrl, m_socketPath;
    DropboxStatus m_status, prev_status;
    QStringList* m_sharedFolders; //! @todo move to syncronouse model..


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
    void updateStatus(DropboxStatus status, const QString& message);

};

#endif //DROPBOXCLIENT_H
