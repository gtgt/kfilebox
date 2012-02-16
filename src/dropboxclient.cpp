#include "dropboxclient.h"

DropboxClient::DropboxClient(QObject *parent) :
	QObject(parent)
{
	m_ps = new QProcess(this);
	m_timer = new QTimer(this);
	dc =new SynchronousDropboxConnection(this);
	dropbox_db = ConfigurationDBDriver::instance();
	prev_status = DropboxUnkown;
	m_message = m_authUrl = "";

	m_dropboxDir = Configuration().getValue("DropboxDir").toString();

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
		m_ps->start(m_dropboxDir.append("dropboxd"));
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
    DropboxStatus m_status = DropboxUnkown;

    //! @todo coment first if{} block(or modify) if you want disable tray icon blinking on startup in green and blue color(default icons scheme)
    if (message.contains("Initializing")||message.contains("Starting")) {
        m_status=DropboxBussy;
    }
    else if (message == "Idle" || message.contains("Connecting")) {
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

    if((prev_status != m_status) || (m_message != message)) {
        prev_status = m_status;
        m_message = message;
        emit updateStatus(m_status, message);
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
        m_authUrl = swap.remove("Please visit ").remove(" to link this machine.");
        if(prevAuthUrl.isEmpty() || prevAuthUrl!=m_authUrl) Notification().send(tr("Please visit <a href=\"%1\">url</a> to link this machine.").arg(m_authUrl));
    }
}

bool DropboxClient::isRunning()
{
    QFile file(QDir::homePath().append("/.dropbox/dropbox.pid"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    int pid = 0;
    QTextStream in(&file);
    in >> pid;
    file.close();
    return QFile::exists(QString("/proc/%1/cmdline").arg(QString::number(pid)));
}

bool DropboxClient::isInstalled()
{
	return QFile(Configuration().getValue("DropboxDir").toString().append("dropbox")).exists();
}

void DropboxClient::hideGtkUi(bool hide)
{
	if(hide && QFile(m_dropboxDir.append("wx._controls_.so")).exists()) {
		QDir().rename(m_dropboxDir.append("wx._controls_.so"), m_dropboxDir.append("wx._controls_orig.so"));
    }
	if(!hide && QFile(m_dropboxDir.append("wx._controls_orig.so")).exists()){
		QDir().rename(m_dropboxDir.append("wx._controls_orig.so"), m_dropboxDir.append("wx._controls_.so"));
    }

}

QString DropboxClient::getVersion()
{
	QFile file(m_dropboxDir.append("VERSION"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();

    QString contents = "";
    QTextStream in(&file);
    in >> contents;

    return contents;
}

/**
  * I assume that inside shared folder you can't share subfolder
  */
QStringList DropboxClient::getSharedFolders()
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
			dir = i.next()+QDir::separator();
			reply = getFolderTag(dir);
			if(reply.isEmpty()) {
				foreach(QString sub_folder, QDir(dir).entryList(QDir::Dirs|QDir::NoDotAndDotDot)) {
					sub_entries.append(dir+sub_folder);
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
//! hope somebody will suggest normal solution:)
// mb replace by regexp?
QString DropboxClient::fixUnicodeChars(const QString &value)
{
	QString humanResult;
	QStringList toHumanable = value.split("\\u");
	if(toHumanable.length()>1) {
		humanResult = toHumanable.first();
		for(int i=1; i<toHumanable.length(); i++ ) {
			if(toHumanable.at(i).length()!=4)
				humanResult.append(QChar(toHumanable.at(i).mid(0, 4).toInt(0, 16))).append(toHumanable.at(i).mid(4));
			else
				humanResult.append(QChar(toHumanable.at(i).toInt(0, 16)));
		}
		return humanResult;
	} else
		return value;
}

QString DropboxClient::resolveFileName(const QString& filename)
{
	QStringList foldersList = this->getSharedFolders();
	foldersList.push_front(dropbox_db->getValue("dropbox_path").toString() + QDir::separator());
	foreach (QString folderPath, foldersList) {
		QString tmpPath = QDir::toNativeSeparators(folderPath+filename);
		if(QFile(tmpPath).exists()) return tmpPath;
	}
	return filename; //! for example, file was deleted
}
