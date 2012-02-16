#include "dropboxclientadaptor.h"

DropboxClientAdaptor::DropboxClientAdaptor(DropboxClient *parent)
	: QDBusAbstractAdaptor(parent)
{
    setAutoRelaySignals(true);
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.registerObject("/Kfilebox", parent);
    connection.registerService("org.kde.Kfilebox");

	connect(parent, SIGNAL(updateStatus(DropboxStatus,QString)), this, SIGNAL(update_status(DropboxStatus,QString)));
}

DropboxClientAdaptor::~DropboxClientAdaptor()
{
}

void DropboxClientAdaptor::start()
{
	DropboxClient* dc = qobject_cast<DropboxClient*>(parent());
	dc->start();
}

void DropboxClientAdaptor::stop()
{
	DropboxClient* dc = qobject_cast<DropboxClient*>(parent());
	dc->stop();
}

QString DropboxClientAdaptor::get_public_link(const QString& filename) {
	DropboxClient* dc = qobject_cast<DropboxClient*>(parent());
	return dc->getPublicLink(filename);
}

QString DropboxClientAdaptor::send_command(const QString& arg) {
	DropboxClient* dc = qobject_cast<DropboxClient*>(parent());
	return dc->sendCommand(arg);
}

QString DropboxClientAdaptor::get_status_message() {
	DropboxClient* dc = qobject_cast<DropboxClient*>(parent());
	return dc->getStatusMessage();
}

QString DropboxClientAdaptor::get_version() {
	DropboxClient* dc = qobject_cast<DropboxClient*>(parent());
	return dc->getVersion();
}

QStringList DropboxClientAdaptor::get_shared_folders() {
	DropboxClient* dc = qobject_cast<DropboxClient*>(parent());
	return dc->getSharedFolders();
}

QStringList DropboxClientAdaptor::get_recently_changed() {
	DropboxClient* dc = qobject_cast<DropboxClient*>(parent());
	return dc->getRecentlyChangedFiles();
}

QString DropboxClientAdaptor::get_auth_url() {
	DropboxClient* dc = qobject_cast<DropboxClient*>(parent());
	return dc->getAuthUrl();
}

QString DropboxClientAdaptor::get_folder_tag(const QString &filename) {
	DropboxClient* dc = qobject_cast<DropboxClient*>(parent());
	return dc->getFolderTag(filename);
}

