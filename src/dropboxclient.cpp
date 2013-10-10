#include "dropboxclient.h"

DropboxClient::DropboxClient(QObject *parent) :
    QObject(parent)
{
    m_ps = new QProcess(this);
    m_timer = new QTimer(this);
    dc = new SynchronousDropboxConnection(this);
    // dropbox_db = Singleton::instance();
    prev_status = DropboxUnknown;
    m_message = m_authUrl = "";
    m_showAuthUrlNotification = true;
    m_dropboxDir.setPath(Configuration().getValue("DropboxDir").toString());
    m_syncDir.setPath(Configuration().getValue("SyncDir").toString());

    connect(m_ps, SIGNAL(readyReadStandardOutput()), this, SLOT(readDaemonOutput()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(getDropboxStatus()));
    m_timer->start(500);
}

DropboxClient::~DropboxClient()
{
    if(m_ps->isOpen())
        m_ps->close();
    // dropbox_db = 0;
    // Singleton::drop();
}

void DropboxClient::start()
{
    if(!isRunning()) {
        m_ps->start(m_dropboxDir.filePath("dropboxd"));
    }
}

void DropboxClient::stop()
{
    sendCommand("tray_action_hard_exit");
    m_ps->waitForFinished();
}

void DropboxClient::getDropboxStatus()
{
    QString message = sendCommand("get_dropbox_status");

    if(message.isEmpty()) {
        message = "Dropbox daemon isn't running";
    }
    DropboxStatus m_status = DropboxUnknown;

    //! @todo coment first if{} block(or modify) if you want disable tray icon blinking on startup in green and blue color(default icons scheme)
    if (message == "Idle") {
        m_status = DropboxIdle;
    }
    else if (message.contains("dopped")) {
        m_status = DropboxError;
    }
    else if (message.contains("Initializing") || message.contains("Starting") || message.contains("isn't")) {
        m_status = DropboxStopped;
    }
    else if (message.contains("Syncing paused") || message.contains("Connecting") || message.contains("Waiting to be linked") || message.contains("couldn't")) {
        m_status = DropboxDisconnected;
    }
    else if (message.contains("Indexing")) {
        m_status = DropboxIndexing;
    }
    else if (message.contains("Saving")) {
        m_status = DropboxSaving;
    }
    else if (message.contains("Downloading")) {
        m_status = DropboxDownloading;
    }
    else if (message.contains("Updating") || message.contains("Uploading")) {
        m_status = DropboxUploading;
    }

    if((prev_status != m_status) || (m_message != message)) {
        prev_status = m_status;
        m_message = message;
        emit updateStatus(m_status, message);
        //updateRecentlyChangedFiles(); // FIXME: doesn't work
    }
}

QString DropboxClient::sendCommand(const QString &command)
{
    return dc->sendCommand(command);
}

void DropboxClient::readDaemonOutput()
{
    QString swap = m_ps->readAllStandardOutput();
    if (swap.contains("https://www.dropbox.com/cli_link?host_id=")) {
        QString prevAuthUrl = m_authUrl;
        m_authUrl = swap.remove("Please visit ").remove(" to link this machine.").remove("This client is not linked to any account...").trimmed();
        if(m_showAuthUrlNotification && (prevAuthUrl.isEmpty() || prevAuthUrl!=m_authUrl)) Notification().send(tr("Please visit <a href=\"%1\">url</a> to link this machine.").arg(m_authUrl));
    }
}

bool DropboxClient::isRunning()
{
    QFile pid_file(QDir(QDir::homePath()).filePath(".dropbox/dropbox.pid"));
    if (!pid_file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    int pid = 0;
    QTextStream pid_in(&pid_file);
    pid_in >> pid;
    pid_file.close();

    QString stat_path = QString("/proc/%1/stat").arg(QString::number(pid));
    if (!QFile::exists(stat_path))
        return false;

    QFile stat_file(stat_path);
    if (!stat_file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream stat_in(&stat_file);
    QString stat = stat_in.readAll();
    stat_file.close();

    return stat.contains("(dropbox)");
}

bool DropboxClient::isInstalled()
{
    return QFile(QDir(Configuration().getValue("DropboxDir").toString()).filePath("dropbox")).exists();
}

void DropboxClient::hideGtkUi(bool hide)
{
    QString src = m_dropboxDir.filePath("wx._controls_.so"), dst = m_dropboxDir.filePath("wx._controls_orig.so");
    if(hide && QFile(src).exists()) {
        QDir().rename(src, dst);
        return;
    }
    if(!hide && QFile(dst).exists()){
        QDir().rename(dst, src);
    }
}

void DropboxClient::setP2PEnabled(bool enabled)
{
    sendCommand(QString("set_lan_sync\nlansync\t%1").arg(enabled ? "enabled" : "disabled"));
}

QString DropboxClient::getVersion()
{
    QFile file(m_dropboxDir.filePath("VERSION"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();

    QString contents = "";
    QTextStream in(&file);
    in >> contents;
    file.close();

    Q_ASSERT(!contents.isEmpty());
    return contents;
}

/**
  * I assume that inside shared folder you can't share subfolder
  */
/*QStringList DropboxClient::getSharedFolders()
{
    //reply: (shared, dropbox, public, photos, "")
    QString reply;
    QStringList shared_folders;
    QStringList entries;
    QStringList sub_entries;
    sub_entries.append(dropbox_db->getValue("dropbox_path").toString());
    QString dir;

    while(!sub_entries.isEmpty()){
        entries = sub_entries;
        sub_entries.clear();
        QMutableStringListIterator i(entries);
        while(i.hasNext()) {
            dir = i.next() + QDir::separator();
            reply = getFolderTag(dir);
            if(reply.isEmpty()) {
                foreach(QString sub_folder, QDir(dir).entryList(QDir::Dirs|QDir::NoDotAndDotDot)) {
                    sub_entries.append(QDir::cleanPath(dir + sub_folder));
                }
            } else if(reply!="dropbox") {
                shared_folders.push_back(dir);
            }
        }
    }

    return shared_folders;
}

//! recent files from shared folders
//! in db '/gp/lacrimoza.gp5'
//! absolute path is '~/Dropbox/shared-folder/' + that file
//! take a look resolveFileName()
QStringList DropboxClient::getRecentlyChangedFiles(){
    QStringList files;
    QString recentlyChanged = dropbox_db->getValue("recently_changed3").toString();

    QString fixed;
    foreach (QString elem, recentlyChanged.split("\n")) {
        QStringList list = elem.split(":");
        if(list.length()>1) {
            fixed = fixUnicodeChars(list.value(1));
            fixed = fixed.right(fixed.length() - 1);
            files.push_back(resolveFileName(fixed));
        }
    }
    return files;
}

//! `\u0441\u043D\u0438\u043C\u043E\u043A38.png'
//! convert to `снимок38.png'
QString DropboxClient::fixUnicodeChars(const QString &value)
{
    //    :( wanna value.replace(rx, function(m){ m = .., return m; });
    //    return value.replace(QRegExp("\\\\u([0-9A-F]{4})"), QChar(QString("\\1").toInt(0, 16)));

    QString result = value;
    QRegExp rx("\\\\u([0-9A-F]{4})");
    while (rx.indexIn(result) != -1) {
        result = result.replace(rx.cap(0), QChar(rx.cap(1).toInt(0,16)));
    }
    return result;
}

QString DropboxClient::resolveFileName(const QString& filename)
{
    QStringList foldersList = this->getSharedFolders();
    foldersList.push_front(dropbox_db->getValue("dropbox_path").toString());
    foreach (QString folderPath, foldersList) {
        QString tmpPath = QDir::cleanPath(folderPath + QDir::separator() + filename);
        if(QFile(tmpPath).exists()) return tmpPath;
    }
    return filename; //! for example, file was deleted
}

void DropboxClient::updateRecentlyChangedFiles() {
    QStringList list = getRecentlyChangedFiles();
    if(!recently_changed.isEmpty()) {
        foreach (QString item, list) {
            if(!recently_changed.contains(item)) {
                Notification n;
                n.send(tr("File updated: <a href=\"file://%1\">%1</a>").arg(item));
                emit newFileAdded(item);
            }
        }
    }
    recently_changed = list;
}*/
