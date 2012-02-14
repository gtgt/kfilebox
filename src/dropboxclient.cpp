#include "dropboxclient.h"

DropboxClient::DropboxClient(QObject *parent) :
    QObject(parent),
    m_sharedFolders(new QMap<QString,QString>()),
    m_ps(new QProcess(this)),
    m_timer(new QTimer(this)),
	dc(new SynchronousDropboxConnection(this)),
	prev_status(DropboxUnkown),
	m_message(""),
	m_authUrl("")
{
	m_dropboxDir = Configuration().getValue("DropboxDir").toString();
	m_dropbox_path = ConfigurationDBDriver().getValue("dropbox_path").toString();

    connect(m_ps, SIGNAL(readyReadStandardOutput()), this, SLOT(readDaemonOutput()));
    connect(m_timer, SIGNAL(timeout()), this, SLOT(getDropboxStatus()));
    m_timer->start(500);
}

DropboxClient::~DropboxClient()
{
    if(m_ps->isOpen())
        m_ps->close();
    delete m_sharedFolders;
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

    if((m_status == DropboxIdle )) { //&& (m_sharedFolders->isEmpty())
		updateSharedFolders(m_dropbox_path);
    }

}

QString DropboxClient::sendCommand(const QString &command)
{
//    QString message = ;
//    if(command=="tray_action_hard_exit") {
//        message = "Dropbox daemon isn't running";
//    }
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
void DropboxClient::updateSharedFolders(const QString& to)
{
    //! (shared, dropbox, public, photos, "")
    QString reply;

    foreach (QString filename, QDir(to).entryList(QDir::Dirs|QDir::NoDotAndDotDot)) {
        QString tmpPath = to+QDir::separator()+filename+QDir::separator();
        reply = dc->sendCommand(QString("get_folder_tag\npath\t%1").arg(tmpPath));
        reply = reply.remove("tag\t");

        if(reply.isEmpty())
            updateSharedFolders(to+QDir::separator()+filename);
        else
            m_sharedFolders->insert(tmpPath, reply);
    }
}

QStringList DropboxClient::getSharedFolders()
{
    QStringList retVal;
    QMap<QString,QString>::const_iterator i = m_sharedFolders->constBegin();
    while (i != m_sharedFolders->constEnd()) {
        if(i.value() != "dropbox")
            retVal.push_back(i.key());
        ++i;
    }
    return retVal;
}

//! recent files from shared folders
//! in db '/gp/lacrimoza.gp5'
//! absolute path is '~/Dropbox/shared-folder/' + that file
//! take a look resolveFileName()
QStringList DropboxClient::prepareLastChangedFiles(){
	QStringList files;
	ConfigurationDBDriver conf;
	QString recentlyChanged = conf.getValue("recently_changed3").toString();

//	if(recentlyChanged.isEmpty())
//		return files;
	foreach (QString elem, recentlyChanged.split("\n")) {
		QStringList list = elem.split(":");
		if(list.length()>1) {
			files.push_back(resolveFileName(fixUnicodeChars(list.value(1))));
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
	foldersList.push_front(m_dropbox_path + QDir::separator());
	foreach (QString folderPath, foldersList) {
		QString tmpPath = QDir::toNativeSeparators(folderPath+filename);
		if(QFile(tmpPath).exists()) return tmpPath;
	}
	return filename; //! for example, file was deleted
}
