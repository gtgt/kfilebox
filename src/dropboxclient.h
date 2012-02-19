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

#include "notification.h"
#include "configuration.h"
#include "configurationdbdriver.h"

class SynchronousDropboxConnection;

enum DropboxStatus {DropboxUnkown, DropboxIdle, DropboxBussy, DropboxError, DropboxUploading, DropboxDownloading,
                    DropboxSaving, DropboxIndexing, DropboxStopped, DropboxDisconnected};
class DropboxClient : public QObject
{
    Q_OBJECT
    Q_ENUMS(DropboxStatus)
public:
    explicit DropboxClient(QObject* parent = 0);
    ~DropboxClient();

    void updateSharedFolders(const QString& to);
    QStringList getSharedFolders();

    //! This functions not strongly related to this class..
	bool isRunning();
	void hideGtkUi(bool hide);
    bool static isInstalled();
	QString getVersion();

    inline QString getAuthUrl() const {return m_authUrl;}
	QStringList getRecentlyChangedFiles();

private:
    QTimer* m_timer;
    QProcess* m_ps;
	QString m_message;
	QString m_authUrl;
	QString m_dropboxDir; // from kfilebox config
    DropboxStatus prev_status;
	QStringList recently_changed;

    SynchronousDropboxConnection* dc;
	ConfigurationDBDriver* dropbox_db;

	QString fixUnicodeChars(const QString &value);
	QString resolveFileName(const QString& filename);
	void updateRecentlyChangedFiles();

public slots:
    void start();
    void stop();

    QString sendCommand(const QString& command);

	QString getPublicLink(const QString& file) {
        return sendCommand(QString("get_public_link\npath\t%1").arg(file)).remove("link\t");
    }

	QString getFolderTag(const QString& command) {
		return sendCommand(QString("get_folder_tag\npath\t%1").arg(command)).remove("tag\t");
	}

    DropboxStatus getStatus() const {return prev_status;}

	QString getStatusMessage() const {return m_message;}

private slots:
    void readDaemonOutput();
    void getDropboxStatus();

signals:
    void updateStatus(DropboxStatus status, const QString& message);
	void newFileAdded(const QString filename);

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
        reply = reply.remove("notok\n");
        reply = reply.remove("ok\n");

        if (reply == "status")
            reply = "Idle";
        else if (reply.startsWith("status"))
            reply = reply.mid(7);

        return reply;
    }

private:
    QLocalSocket* m_socket;
    QString m_socketpath;

};

#endif //DROPBOXCLIENT_H
