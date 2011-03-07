#include "DropboxClient.h"

namespace core {

DropboxClient::DropboxClient(){
    m_socket = new QLocalSocket(this);
    connect(m_socket, SIGNAL(error(QLocalSocket::LocalSocketError)),this, SLOT(displayError(QLocalSocket::LocalSocketError)));

    m_socketPath = QDir::toNativeSeparators(QDir::homePath().append("/.dropbox/command_socket"));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(getDropboxStatus()));
    timer->start(500);
}

DropboxClient::~DropboxClient()
{
    m_socket->close();
    delete m_socket;
    m_socket = 0;
    delete timer;
    timer = 0;
}

/**
  * change algorithm to
  * if(currentStatus == "stoped" && newStatus == "running") { Notification::send(tr("Dropbox daemon started")); }
  * and hardcode it in trayIcon .. :(
  */
void DropboxClient::start()
{
    QProcess::startDetached(QDir::toNativeSeparators(QDir::homePath().append("/.dropbox-dist/dropboxd")));
    m_socket->connectToServer(m_socketPath);
    m_socket->waitForConnected(5000);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readMsg()));

}

void DropboxClient::stop()
{
    sendCommand("tray_action_hard_exit");
}

bool DropboxClient::is_running()
{
    QFile file(QDir::toNativeSeparators(QDir::homePath().append("/.dropbox/dropbox.pid")));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    int pid = 0;
    QTextStream in(&file);
    in >> pid;


    if (QFile(QString("/proc/"+QString::number(pid)+"/cmdline")).open(QIODevice::ReadOnly | QIODevice::Text))
        return true;

    return false;

}

void DropboxClient::getDropboxStatus(){
    if (is_running())
        sendCommand("get_dropbox_status");
    else
        emit messageProcessed("Dropbox isn't running");
}


void DropboxClient::sendCommand(QString command)
{
    int waitTime = 100;

    if(!m_socket->isOpen())
    {
        //        qDebug() << "DropboxClient::sendCommand()";
        m_socket->connectToServer(m_socketPath);
        if(!m_socket->waitForConnected(waitTime))
            return;
    }

    m_socket->write(command.toUtf8());
    m_socket->write(QString("\n").toUtf8());
    m_socket->write(QString("done\n").toUtf8());
    m_socket->flush();

    return;
}

void DropboxClient::readMsg()
{
    QString msg = m_socket->readAll();
    QString message = "?";
    foreach (QString elem, msg.split("\n")) {
        if(elem.length()>0 && elem!="ok" && elem !="done")
        {
            QStringList list = elem.split("\t");
            if (list.length()==1 && list[0].trimmed()=="status")
                message = "Idle";
            else if (list.length()>1 && list[0].trimmed()=="status")
                message = list[1];

            emit messageProcessed(message);
        }
    }
}

void DropboxClient::displayError(QLocalSocket::LocalSocketError socketError)
{
    switch (socketError) {
    case QLocalSocket::ServerNotFoundError:
        //! socket isn't exists > is dropbox exists? can I run installer?
        //         qt_message_output(QtWarningMsg, tr("The host was not found. Please check Dropbox daemon installation.").toLatin1());
        Notification::send(tr("The host was not found. Please check Dropbox daemon installation."));
        break;
    case QLocalSocket::ConnectionRefusedError:
        //! this is not an error
        //         qt_message_output(QtWarningMsg,tr("The connection was refused by the peer. "
        //                                     "Make sure the Dropbox daemon is running, "
        //                                     "and check that the settings are correct.").toLatin1());
        //        Notification::send(tr("The connection was refused by the peer.\nMake sure the Dropbox daemon is running, and check that the settings are correct."));
        break;
    case QLocalSocket::PeerClosedError:
        Notification::send(tr("Dropbox daemon stoped"));
        break;
    default:
        Notification::send(tr("The following error occurred: %1.").arg(m_socket->errorString()));
        //         qt_message_output(QtWarningMsg,tr("The following error occurred: %1.")
        //                                  .arg(s->errorString()).toLatin1());
    }

}

} /* End of namespace core */
