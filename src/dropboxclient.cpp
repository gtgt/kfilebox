#include "dropboxclient.h"

DropboxClient::DropboxClient(QObject *parent) :
    QObject(parent)
{
    m_ps = new QProcess(this);
    m_timer = new QTimer(this);
    dc = new SynchronousDropboxConnection(this);
    dropbox_db = Singleton::instance();
    m_prevStatus = DropboxUnknown;
    m_message = m_authUrl = "";
    m_showAuthUrlNotification = true;

    Configuration conf;
    m_distDir.setPath(conf.getValue("DistDir").toString());
    m_configDir.setPath(conf.getValue("ConfigDir").toString());
    m_syncDir.setPath(conf.getValue("SyncDir").toString());

    connect(m_ps, SIGNAL(readyReadStandardOutput()), this, SLOT(readDaemonOutput()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(getDropboxStatus()));
    m_timer->start(500);
}

DropboxClient::~DropboxClient()
{
    if(m_ps->isOpen())
        m_ps->close();
    dropbox_db = 0;
    Singleton::drop();
}

void DropboxClient::start()
{
    if(!isRunning()) {
        m_ps->start(m_distDir.filePath("dropboxd"));
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

    if((m_prevStatus != m_status) || (m_message != message)) {
        m_prevStatus = m_status;
        m_message = message;
        emit updateStatus(m_status, message);
        updateRecentlyChangedFiles();
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
    QFile pid_file(m_configDir.filePath("dropbox.pid"));
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
    return QFile(QDir(Configuration().getValue("DistDir").toString()).filePath("dropbox")).exists();
}

void DropboxClient::hideGtkUi(bool hide)
{
    QString src = m_distDir.filePath("wx._controls_.so"), dst = m_distDir.filePath("wx._controls_orig.so");
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
    QFile file(m_distDir.filePath("VERSION"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();

    QString contents = "";
    QTextStream in(&file);
    in >> contents;
    file.close();

    Q_ASSERT(!contents.isEmpty());
    return contents;
}

void DropboxClient::buildTree(const QDir &root, QStringList &tree)
{
    tree.append(root.absolutePath());

    foreach (const QString &dir, root.entryList(QDir::AllDirs | QDir::NoDotAndDotDot))
    {
        QDir child(root);
        if (child.cd(dir)) buildTree(child, tree);
    }
}

QString DropboxClient::findFile(const QString &file, const QStringList &tree)
{
    QDir dir;
    QString tmpFile;
    foreach (const QString &path, tree) {
        dir.setPath(path);
        tmpFile = dir.filePath(file);
        if (QFile(tmpFile).exists()) return tmpFile;
    }

    return file; //! file not found
}

QStringList DropboxClient::getSharedFolders()
{
    QStringList tree;
    buildTree(m_syncDir, tree);

    QStringList shared;
    QString reply;
    foreach (const QString &dir, tree)
    {
        reply = getFolderTag(dir);
        if (!reply.isEmpty() && reply != "dropbox") {
            shared.append(dir + ": " + reply);
        }
    }

    return shared;
}

void DropboxClient::updateRecentlyChangedFiles() {
    const QByteArray blob = dropbox_db->getValue("recent").toByteArray();
    if (blob == m_recentlyChangedBlob) return;
    m_recentlyChangedBlob = blob;

    bool ok;
    QVariant result = jsonParser.parse(blob, &ok);
    if (!ok) return;

    typedef QPair<double, QString> FilePair;
    QList<FilePair> files;
    foreach (const QVariant &e, result.toList())
    {
        const QVariantMap file = e.toMap();
        if (file.contains("timestamp") && file.contains("server_path")) {
            files.append(qMakePair(file["timestamp"].toDouble(), file["server_path"].toString()));
        }
    }
    if (files.isEmpty()) return;
    qSort(files);

    QStringList tree;
    buildTree(m_syncDir, tree);

    QStringList newRecentlyChanged;
    foreach (const FilePair &pair, files)
    {
        newRecentlyChanged.append(findFile(pair.second.split("/").last(), tree));
    }

    if (!m_recentlyChanged.isEmpty()) {
        Notification notify;
        QFileInfo fileInfo;
        QString reply;
        foreach (const QString &file, newRecentlyChanged) {
            if (!m_recentlyChanged.contains(file)) {
                fileInfo.setFile(file);
                if (fileInfo.exists()) {
                    reply = getFolderTag(fileInfo.dir().absolutePath());
                    if (!reply.isEmpty() && reply != "dropbox") {
                        notify.send(tr("File updated: <a href=\"file://%1\">%1</a>").arg(file));
                        emit newFileAdded(file);
                    }
                }
            }
        }
    }

    m_recentlyChanged = newRecentlyChanged;
}
