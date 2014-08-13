#include "dropboxclient.h"

DropboxClient::DropboxClient(QObject *parent) :
    QObject(parent),
    m_showAuthUrlNotification(true),
    m_status(DropboxUnknown)
{
    m_ps = new QProcess(this);
    m_timer = new QTimer(this);
    dc = new SynchronousDropboxConnection(this);
    dropbox_db = Singleton::instance();

    const Configuration conf;
    m_distDir.setPath(conf.getValue("DistDir").toString());
    m_configDir.setPath(conf.getValue("ConfigDir").toString());

    connect(m_ps, SIGNAL(readyReadStandardOutput()), this, SLOT(readDaemonOutput()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(getDropboxStatus()));
    m_timer->start(500);
}

DropboxClient::~DropboxClient()
{
    if (m_ps->isOpen())
        m_ps->close();
    dropbox_db = 0;
    Singleton::drop();
}

void DropboxClient::start()
{
    if (!isRunning()) {
        m_ps->start(m_distDir.filePath("dropboxd"));
    }
}

void DropboxClient::stop()
{
    sendCommand("tray_action_hard_exit");

    if (m_ps->state() != QProcess::NotRunning) {
        m_ps->waitForFinished();
    } else {
        // if process was self-restarted
        QMutex dummy;
        QElapsedTimer timer;
        dummy.lock();
        timer.start();
        while (isRunning() && timer.elapsed() < 30000) {
            QWaitCondition().wait(&dummy, 500);
        }
    }
    //! @todo should we kill a process if it still running?
}

void DropboxClient::getDropboxStatus()
{
    QString message = sendCommand("get_dropbox_status");
    DropboxStatus status = DropboxUnknown;

    if (message.isEmpty()) {
        message = "Dropbox daemon isn't running";
        status = DropboxStopped;
    }
    else if (message == "Idle" || message.contains("Up to date")) {
        status = DropboxIdle;
    }
    else if (message.contains("dopped")) {
        status = DropboxError;
    }
    else if (message.contains("isn't") || message.contains("Syncing paused") || message.contains("Connecting") || message.contains("Waiting to be linked") || message.contains("couldn't")) {
        status = DropboxDisconnected;
    }
    else if (message.contains("Indexing") || message.contains("Initializing") || message.contains("Starting")) {
        status = DropboxIndexing;
    }
    else if (message.contains("Saving")) {
        status = DropboxSaving;
    }
    else if (message.contains("Downloading")) {
        status = DropboxDownloading;
    }
    else if (message.contains("Updating") || message.contains("Uploading")) {
        status = DropboxUploading;
    }

    if (m_status != status && m_status == DropboxStopped && m_ps->state() == QProcess::NotRunning) {
        // process was self-restarted
        stop();
        hideGtkUi(Configuration().getValue("GtkUiDisabled").toBool());
        start();
    }

    if (m_status != status || m_message != message) {
        m_status = status;
        m_message = message;

        emit updateStatus(status, message);
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
        const QString prevAuthUrl = m_authUrl;
        m_authUrl = swap.remove("Please visit ").remove(" to link this machine.").remove("This client is not linked to any account...").trimmed();
        if (m_showAuthUrlNotification && (prevAuthUrl.isEmpty() || prevAuthUrl != m_authUrl))
            Notification().send(tr("Please visit <a href=\"%1\">url</a> to link this machine.").arg(m_authUrl));
    }
}

bool DropboxClient::isRunning() const
{
    QFile pid_file(m_configDir.filePath("dropbox.pid"));
    if (!pid_file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    int pid = 0;
    QTextStream pid_in(&pid_file);
    pid_in >> pid;
    pid_file.close();

    QFile stat_file(QString("/proc/%1/stat").arg(QString::number(pid)));
    if (!stat_file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream stat_in(&stat_file);
    const QString stat = stat_in.readAll();
    stat_file.close();

    return stat.contains("(dropbox)");
}

bool DropboxClient::isInstalled()
{
    return QFileInfo(QDir(Configuration().getValue("DistDir").toString()), "dropboxd").exists();
}

void DropboxClient::hideGtkUi(bool hide)
{
    const QString src = m_distDir.filePath("wx._controls_.so"),
            dst = m_distDir.filePath("wx._controls_orig.so");

    if (hide && QFileInfo(src).exists()) {
        if (QFileInfo(dst).exists())
            QFile::remove(dst);
        QFile::rename(src, dst);
        return;
    }

    if (!hide && QFileInfo(dst).exists()) {
        if (QFileInfo(src).exists())
            QFile::remove(dst);
        else
            QFile::rename(dst, src);
    }
}

void DropboxClient::setP2PEnabled(bool enabled)
{
    sendCommand(QString("set_lan_sync\nlansync\t%1").arg(enabled ? "enabled" : "disabled"));
}

QString DropboxClient::getVersion() const
{
    QFile file(m_distDir.filePath("VERSION"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();

    QString contents;
    QTextStream in(&file);
    in >> contents;
    file.close();

    Q_ASSERT(!contents.isEmpty());
    return contents;
}

/**
  * I assume that inside shared folder you can't share subfolder
  */
QStringList DropboxClient::getSharedFolders()
{
    // reply: (shared, dropbox, public, photos, "")
    QString reply, dir;
    QStringList shared_folders, entries, sub_entries;

    sub_entries.append(Configuration().getValue("SyncDir").toString());
    while(!sub_entries.isEmpty()){
        entries = sub_entries;
        sub_entries.clear();
        QMutableStringListIterator i(entries);
        while(i.hasNext()) {
            dir = i.next() + QDir::separator();
            reply = getFolderTag(dir);
            if(reply.isEmpty()) {
                foreach(QString sub_folder, QDir(dir).entryList(QDir::AllDirs|QDir::NoDotAndDotDot)) {
                    sub_entries.append(QDir::cleanPath(dir + sub_folder));
                }
            } else if(reply!="dropbox") {
                shared_folders.push_back(dir);
            }
        }
    }

    return shared_folders;
}

void DropboxClient::buildFileTree(const QDir &root, QStringList &tree) const
{
    foreach (const QString &file, root.entryList(QDir::Files))
    {
        tree.append(root.filePath(file));
    }

    foreach (const QString &dir, root.entryList(QDir::AllDirs | QDir::NoDotAndDotDot))
    {
        QDir child(root);
        if (child.cd(dir)) buildFileTree(child, tree);
    }
}

void DropboxClient::updateRecentlyChangedFiles() {
    const QByteArray blob = dropbox_db->getValue("recent").toByteArray();
    if (blob == m_recentlyChangedBlob) return;
    m_recentlyChangedBlob = blob;

    bool ok;
    const QVariant result = jsonParser.parse(blob, &ok);
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
    buildFileTree(QDir(Configuration().getValue("SyncDir").toString()), tree);

    QStringList newRecentlyChanged;
    QString cleanPath;
    static const QRegExp reRemoveId("^\\d+:");
    foreach (const FilePair &pair, files)
    {
        cleanPath = QString(pair.second).remove(reRemoveId);

        foreach (const QString &file, tree) {
            if (file.endsWith(cleanPath, Qt::CaseInsensitive)) {
                newRecentlyChanged.append(file);
                break;
            }
        }
    }

    if (!m_recentlyChanged.isEmpty()) {
        const Notification notify;
        QFileInfo fileInfo;
        QString reply;
        foreach (const QString &file, newRecentlyChanged) {
            if (!m_recentlyChanged.contains(file)) {
                fileInfo.setFile(file);
                reply = getFolderTag(fileInfo.path());
                if (reply == "shared") {
                    notify.send(tr("File updated: <a href=\"file://%1\">%1</a>").arg(file));
                    emit newFileAdded(file);
                }
            }
        }
    }

    m_recentlyChanged = newRecentlyChanged;
}
