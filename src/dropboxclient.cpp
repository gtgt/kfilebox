#include "dropboxclient.h"

DropboxClient::DropboxClient(QObject *parent) :
    QObject(parent)
{
    m_status = DropboxClient::DropboxUnkown;

    authUrl = "";

    m_socket = new QLocalSocket(this);
    m_socketPath = QDir::toNativeSeparators(QDir::homePath().append("/.dropbox/command_socket"));

    connect(m_socket, SIGNAL(error(QLocalSocket::LocalSocketError)),this, SLOT(displayError(QLocalSocket::LocalSocketError)));
    connect(m_socket, SIGNAL(readyRead()), SLOT(receiveReply()));

    m_ps = new QProcess(this);
    connect(m_ps, SIGNAL(readyReadStandardOutput()), this, SLOT(readDaemonOutput()));

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(getDropboxStatus()));
    m_timer->start(500);
}

DropboxClient::~DropboxClient()
{
    if(m_ps->isOpen())
        m_ps->close();
}

void DropboxClient::start()
{
    m_status=DropboxClient::DropboxUnkown;
    if(!isRunning()) {
        m_ps->start(QDir::toNativeSeparators(QDir::homePath().append("/.dropbox-dist/dropboxd")));
        m_ps->waitForStarted(500);
    }
    m_socket->connectToServer(m_socketPath);
}

void DropboxClient::stop()
{
    sendCommand("tray_action_hard_exit");
    processReply("Dropbox isn't running");
    //    m_status = DropboxClient::DropboxStopped;
    //    emit updateStatus(m_status, "Dropbox isn't running");
}

bool DropboxClient::isRunning()
{
    //    return (m_status!=DropboxClient::DropboxStopped);
    QFile file(QDir::toNativeSeparators(QDir::homePath().append("/.dropbox/dropbox.pid")));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    int pid = 0;
    QTextStream in(&file);
    in >> pid;

    return QFile(QString("/proc/"+QString::number(pid)+"/cmdline")).open(QIODevice::ReadOnly | QIODevice::Text);
}

/** Loop. I don't know what is worth(every m_timer->interval() ):
  * - get dropbox pid from file, try to read file in /proc/PID/some_file
  * - if m_socket isn't open - try to connect, got error
  */
void DropboxClient::getDropboxStatus()
{
    sendCommand("get_dropbox_status");
}

//Re-entrant
void DropboxClient::sendCommand(const QString &command)
{
    if(!m_socket->isOpen())
    {
        m_socket->connectToServer(m_socketPath);
        //        if(!m_socket->waitForConnected(waitTime)) {
        //            qDebug() << m_socket->errorString();
        //            m_status = DropboxClient::DropboxStopped;
        return; // "Dropbox isn't running";
        //        }
    }

    m_socket->write(command.toUtf8());
    m_socket->write(QString("\n").toUtf8());
    m_socket->write(QString("done\n").toUtf8());
    m_socket->flush();

}

void DropboxClient::receiveReply()
{
    QString reply = m_socket->readAll();

    reply = reply.remove("\ndone\n");
    reply = reply.remove("done\n"); //! @todo
    reply = reply.remove("ok\n");

    //Remove status\t or Replace status to Idle
    QStringList list = reply.split("\t");
    if (list.length()==1 && list[0].trimmed()=="status")
        reply = "Idle";
    else if (list.length()>1 && list[0].trimmed()=="status")
        reply = list[1];

    processReply(reply);
}

void DropboxClient::processReply(const QString &message)
{
    if(message.isEmpty()) return;

    if (message.contains("connecting")) {
        m_status=DropboxClient::DropboxBussy;
    }
    else if ( message.contains("Idle")) {
        m_status=DropboxClient::DropboxIdle;
    }
    else if (message.contains("Up")) {
        m_status=DropboxClient::DropboxUploading;
    }
    else if (message.contains("Downloading")) {
        m_status=DropboxClient::DropboxDownloading;
    }
    else if (message.contains("Saving")) {
        m_status=DropboxClient::DropboxSaving;
    }
    else if (message.contains("Indexing")) {
        m_status=DropboxClient::DropboxIndexing;
    }
    else if(message.contains("isn't")) {
        m_status=DropboxClient::DropboxStopped;
    }
    else if(message.contains("couldn't")){
        m_status=DropboxClient::DropboxDisconnected;
    }
    else if(message.contains("dopped")){
        m_status=DropboxClient::DropboxError;
    }

    if((prev_status != m_status) || (prev_message != message)) {
        prev_status = m_status;
        prev_message = message;
        emit updateStatus(m_status, message);
    }
}


void DropboxClient::readDaemonOutput()
{
    QString swap = m_ps->readAllStandardOutput();
    if (swap.contains("https://www.dropbox.com/cli_link?host_id=")) {
        authUrl = swap.remove("Please visit ").remove(" to link this machine.");
    }
}

void DropboxClient::displayError(QLocalSocket::LocalSocketError socketError)
{
    Notification notify;

    switch (socketError) {
    case QLocalSocket::ServerNotFoundError:
        //! socket isn't exists > is dropbox exists? can I run installer?
        //         qt_message_output(QtWarningMsg, tr("The host was not found. Please check Dropbox daemon installation.").toLatin1());
        //        notify.send(tr("The host was not found. Please check Dropbox daemon installation."));
        break;
    case QLocalSocket::ConnectionRefusedError:
        //! this is not an error
        //         qt_message_output(QtWarningMsg,tr("The connection was refused by the peer. "
        //                                     "Make sure the Dropbox daemon is running, "
        //                                     "and check that the settings are correct.").toLatin1());
        //        notify.send(tr("The connection was refused by the peer.\nMake sure the Dropbox daemon is running, and check that the settings are correct."));
        break;
    case QLocalSocket::SocketTimeoutError:
    case QLocalSocket::PeerClosedError:
        //        m_status = DropboxClient::DropboxStopped;
        //        emit updateStatus(m_status, "Dropbox isn't running");
        //        notify.send(tr("Dropbox daemon stoped"));
        break;
    default:
        notify.send(tr("The following error occurred: %1.").arg(m_socket->errorString()));
        //         qt_message_output(QtWarningMsg,tr("The following error occurred: %1.")
        //                                  .arg(s->errorString()).toLatin1());
    }

}

// ..

QString DropboxClient::getAuthUrl() const
{
    return authUrl;
}

bool DropboxClient::isInstalled()
{
    return QFile(QDir::toNativeSeparators(QDir::homePath().append("/.dropbox-dist/dropbox"))).exists();
}

void DropboxClient::hideGtkUi(bool v)
{
    if(v) {
        if(QFile(QDir::homePath().append("/.dropbox-dist/wx._controls_.so")).exists())
            QDir().rename(QDir::homePath().append("/.dropbox-dist/wx._controls_.so"), QDir::homePath().append("/.dropbox-dist/wx._controls_orig.so"));
        //        else
        //            qDebug() << "HideGtkUi: Failed to move /.dropbox-dist/wx._controls_.so";
    } else {
        if(QFile(QDir::homePath().append("/.dropbox-dist/wx._controls_orig.so")).exists())
            QDir().rename(QDir::homePath().append("/.dropbox-dist/wx._controls_orig.so"), QDir::homePath().append("/.dropbox-dist/wx._controls_.so"));
        //        else
        //            qDebug() << "ShowGtkUi: Failed to move /.dropbox-dist/wx._controls_orig.so";
    }

}

QString DropboxClient::getVersion()
{
    QFile file(QDir::toNativeSeparators(QDir::homePath().append("/.dropbox-dist/VERSION")));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();

    QString contents = "" ;
    QTextStream in(&file);
    in >> contents;

    return contents;
}
