#include "dropboxclient.h"

DropboxClient::DropboxClient(QObject *parent) :
    QObject(parent),
    //    m_sharedFolders(new QStringList()),
    m_socket(new QLocalSocket(this)),
    m_ps(new QProcess(this)),
    m_timer(new QTimer(this))
{
    m_status = prev_status = DropboxUnkown;
    prev_message = "";
    authUrl = "";

    m_socketPath = QDir::toNativeSeparators(QDir::homePath().append("/.dropbox/command_socket"));

    connect(m_socket, SIGNAL(error(QLocalSocket::LocalSocketError)),this, SLOT(displayError(QLocalSocket::LocalSocketError)));
    connect(m_socket, SIGNAL(readyRead()), SLOT(receiveReply()));

    connect(m_ps, SIGNAL(readyReadStandardOutput()), this, SLOT(readDaemonOutput()));

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
    if(!isRunning()) {
        m_ps->start(QDir::toNativeSeparators(QDir::homePath().append("/.dropbox-dist/dropboxd")));
        m_ps->waitForStarted(500);
    }
    m_socket->connectToServer(m_socketPath);
}

void DropboxClient::stop()
{
    sendCommand("tray_action_hard_exit");
    //    int count=0; while(true){sleep(200); count++; qDebug() << "iteration " << count; if((!isRunning())||(count>=12)) break;}
    processReply("Dropbox isn't running");
}

bool DropboxClient::isRunning()
{
    //    return (m_status!=DropboxStopped);
    QFile file(QDir::toNativeSeparators(QDir::homePath().append("/.dropbox/dropbox.pid")));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    int pid = 0;
    QTextStream in(&file);
    in >> pid;

    return QFile(QString("/proc/%1/cmdline").arg(QString::number(pid))).open(QIODevice::ReadOnly | QIODevice::Text);
}

/** Loop. I don't know what is worth(every m_timer->interval() ):
  * - get dropbox pid from file, try to read file in /proc/PID/some_file
  * - if m_socket isn't open - try to connect, got error
  */
void DropboxClient::getDropboxStatus()
{
    sendCommand("get_dropbox_status");
}

void DropboxClient::sendCommand(const QString &command)
{
    if(!m_socket->isOpen())
    {
        m_socket->connectToServer(m_socketPath);
        //        if(!m_socket->waitForConnected(waitTime)) {
        //            qDebug() << m_socket->errorString();
        //            m_status = DropboxStopped;
        return; // "Dropbox isn't running";
        //        }
    }

    m_socket->write(command.toUtf8());
    m_socket->write(QString("\ndone\n").toUtf8());
    m_socket->flush();

}

void DropboxClient::receiveReply()
{
    QString reply = m_socket->readAll();

    reply = reply.remove("\ndone\n");
    reply = reply.remove("done\n"); //! @todo
    reply = reply.remove("ok\n");

    //    if(reply.startsWith("tag")) {
    //        qDebug() << reply.split("\t");
    //        m_sharedFolders->push_back(reply);
    //        return;
    //    }

    //Remove status\t or Replace status to Idle
    QStringList list = reply.split("\t");
    if (list.length()==1 && list[0].trimmed()=="status")
        reply = "Idle";
    else if (list.length()>1 && list[0].trimmed()=="status")
        reply = list[1];

    processReply(reply);
}

//! @bug fix if dropbox is stoped
void DropboxClient::processReply(const QString &message)
{
    if(message.isEmpty()) return;

    if (message.contains("connecting")) {
        m_status=DropboxBussy;
    }
    else if ( message.contains("Idle")) {
        m_status=DropboxIdle;
    }
    else if (message.contains("Up")) {
        m_status=DropboxUploading;
    }
    else if (message.contains("Downloading")) {
        m_status=DropboxDownloading;
    }
    else if (message.contains("Saving")) {
        m_status=DropboxSaving;
    }
    else if (message.contains("Indexing")) {
        m_status=DropboxIndexing;
    }
    else if(message.contains("isn't")) {
        m_status=DropboxStopped;
    }
    else if(message.contains("couldn't")||message.contains("Syncing paused")||message.contains("Waiting to be linked")){
        m_status=DropboxDisconnected;
    }
    else if(message.contains("dopped")){
        m_status=DropboxError;
    }

    if((prev_status != m_status) || (prev_message != message)) {
        prev_status = m_status;
        prev_message = message;
        emit updateStatus(m_status, message);
    }

    //    if((m_status == DropboxIdle) && (m_sharedFolders->isEmpty())) {
    //        getSharedFolders("/home/nib/Dropbox"); //! hard coded yeat
    //    }
}


void DropboxClient::readDaemonOutput()
{
    QString swap = m_ps->readAllStandardOutput();
    if (swap.contains("https://www.dropbox.com/cli_link?host_id=")) {
        QString prevAuthUrl = authUrl;
        authUrl = swap.remove("Please visit ").remove(" to link this machine.");
        if(prevAuthUrl.isEmpty() || prevAuthUrl!=authUrl) Notification().send(QString(tr("Please visit <a href=\"%1\">url</a> to link this machine.")).arg(authUrl));
    }
}

void DropboxClient::displayError(QLocalSocket::LocalSocketError socketError)
{
    switch (socketError) {
    case QLocalSocket::ServerNotFoundError:
        //! socket isn't exists > is dropbox exists? can I run installer?
        //        notify.send(tr("The host was not found. Please check Dropbox daemon installation."));
        break;
    case QLocalSocket::ConnectionRefusedError:
        //! this is not an error
        //        notify.send(tr("The connection was refused by the peer.\nMake sure the Dropbox daemon is running, and check that the settings are correct."));
        break;
    case QLocalSocket::SocketTimeoutError:
    case QLocalSocket::PeerClosedError:
        //        m_status = DropboxStopped;
        //        emit updateStatus(m_status, "Dropbox isn't running");
        processReply("Dropbox isn't running");
        //        notify.send(tr("Dropbox daemon stoped"));
        break;
    default:
        Notification().send(tr("The following error occurred: %1.").arg(m_socket->errorString()));
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


/**
  * @todo Dropbox doesn't providing
  * How to get shared folders?
  * manually get %tag% of the every folder insede of %dropbox_folder%
  * where needed %tag% must be like shared
  *
  * I assume that inside shared folder you can't share subfolder
  *
  * getSharedFolders(QString path)
  * foreach(subPath, getSubfolders(path))
  *     if(folderTag(subPath) == "shared")
  *         add to shared
  *     else
  *         getSharedFolders(subPath)
  */
//void DropboxClient::getSharedFolders(const QString& to)
//{

//    //    <- get_folder_tag /some/folder/
//    //    -> tag    shared  # this folder is shared
//    //       tag    dropbox # this is your dropbox's root folder
//    //       tag    public  # this is your public folder
//    //       tag    photos  # this is your photos folder
//    //       tag            # otherwise

//    //    sendCommand("get_folder_tag\npath\t/home/nib/Dropbox/Audio/");

//    foreach (QString filename, QDir(to).entryList(QDir::Dirs|QDir::NoDotAndDotDot)) {
//        sendCommand(QString("get_folder_tag\npath\t%1").arg(to+QDir::separator()+filename+QDir::separator()));
//        getSharedFolders(to+QDir::separator()+filename);
//    }
//}
