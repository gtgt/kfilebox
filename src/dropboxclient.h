#ifndef DROPBOXCLIENT_H
#define DROPBOXCLIENT_H

#include <QDir>
#include <QLocalSocket>
#include <QMap>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QTimer>

#include "src/notification.h"

//! @todo provide 2 low level classes asynchronous(to get status in loop) and syncronous(to query commands like get_folder_tag)

class SynchronousDropboxClient;

enum DropboxStatus {DropboxUnkown, DropboxIdle, DropboxBussy, DropboxError, DropboxUploading, DropboxDownloading,
                    DropboxSaving, DropboxIndexing, DropboxStopped, DropboxDisconnected};

class DropboxClient : public QObject
{
    Q_OBJECT
public:
    explicit DropboxClient(QObject* parent = 0);
    ~DropboxClient();

    void sendCommand(const QString& command);
    void updateSharedFolders(const QString& to);
    QStringList getSharedFolders();

    //! This functions not strongly related to this class..
    bool isRunning();
    void static hideGtkUi(bool v);
    bool static isInstalled();
    QString static getVersion();

    inline QString getAuthUrl() const {return m_authUrl;}

private:
    QTimer* m_timer;
    QLocalSocket* m_socket;
    QProcess* m_ps;
    QString m_message, m_authUrl, m_socketPath;
    DropboxStatus m_status, prev_status;
    QMap<QString,QString>* m_sharedFolders;


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




class SynchronousDropboxConnection : public QObject
{
    Q_OBJECT
public:
    explicit SynchronousDropboxConnection(QObject* parent = 0) :
        QObject(parent),
        m_socket(new QLocalSocket(this))
    {
        m_socketpath = QDir::toNativeSeparators(QDir::homePath().append("/.dropbox/command_socket"));
        m_socket->connectToServer(m_socketpath);
    }
    virtual ~SynchronousDropboxConnection() {}

    //Re-entrant
    QString sendCommand(const QString& command)
    {
        int waitTime = 100;

        if(!m_socket->isOpen())
        {
            m_socket->connectToServer(m_socketpath);
            if(!m_socket->waitForConnected(waitTime))
                return QString();
        }

        m_socket->write(command.toUtf8());
        m_socket->write(QString("\ndone\n").toUtf8());
        m_socket->flush();

        QString reply;
        while(true)
        {
            if(!m_socket->waitForReadyRead(waitTime))
            {
                //If we have to wait this long, the m_socket probably isn't open anymore (dropbox died or closed)
                m_socket->close();
                return QString();
            }

            reply.append(m_socket->readAll());

            if(reply.endsWith("done\n")) break;
        }

        //! @todo if(reply.stripTONotEmptyStrings().count()==3) {reply.stripped().remove(1) AND remove(3)}
        //Strip out \ndone\n and ok\n
        reply = reply.remove("\ndone\n");
        reply = reply.remove("ok\n");

        return reply;
    }

private:
    QLocalSocket* m_socket;
    QString m_socketpath;

};







#endif //DROPBOXCLIENT_H
