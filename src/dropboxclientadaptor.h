#ifndef DROPBOXCLIENTADAPTOR_H
#define DROPBOXCLIENTADAPTOR_H

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QtDBus>
#include <QVariant>

#include "dropboxclient.h" //! for DropboxClient::DropboxStatus

class DropboxClientAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.kde.Kfilebox")
	Q_CLASSINFO("D-Bus Introspection", ""
				"  <interface name=\"org.kde.Kfilebox\">\n"
				"    <method name=\"start\"/>\n"
				"    <method name=\"stop\"/>\n"
				"    <signal name=\"update_status\">\n"
				"        <arg name=\"state\" type=\"i\" direction=\"out\"/>\n"
				"        <arg name=\"message\" type=\"s\" direction=\"out\"/>\n"
				"    </signal>\n"
				"    <signal name=\"new_file_added\">\n"
				"        <arg type=\"s\" direction=\"out\"/>\n"
				"    </signal>\n"
				"    <method name=\"get_public_link\">\n"
				"        <arg name=\"filename\" type=\"s\" direction=\"in\" />\n"
				"        <arg type=\"s\" direction=\"out\" />\n"
				"    </method>\n"
				"    <method name=\"send_command\">\n"
				"        <arg name=\"arg\" type=\"s\" direction=\"in\" />\n"
				"        <arg type=\"s\" direction=\"out\" />\n"
				"    </method>\n"
				"    <method name=\"get_status_message\">\n"
				"        <arg type=\"s\" direction=\"out\" />\n"
				"    </method>\n"
				"    <method name=\"get_version\">\n"
				"        <arg type=\"s\" direction=\"out\" />\n"
				"    </method>\n"
				"    <method name=\"get_auth_url\">\n"
				"        <arg type=\"s\" direction=\"out\" />\n"
				"    </method>\n"
				"    <method name=\"get_folder_tag\">\n"
				"        <arg name=\"filename\" type=\"s\" direction=\"in\" />\n"
				"        <arg type=\"s\" direction=\"out\" />\n"
				"    </method>\n"
				"    <method name=\"get_shared_folders\">\n"
				"        <arg type=\"as\" direction=\"out\" />\n"
				"    </method>\n"
				"    <method name=\"get_recently_changed\">\n"
				"        <arg type=\"as\" direction=\"out\" />\n"
				"    </method>\n"
				"  </interface>\n"
				"")
public:
	DropboxClientAdaptor(DropboxClient *parent);
    virtual ~DropboxClientAdaptor();
public slots:
    void start();
    void stop();
	QString get_public_link(const QString& filename);
	QString send_command(const QString& arg);
	QString get_status_message();
	QString get_version();
	QStringList get_shared_folders();
	QStringList get_recently_changed();
	QString get_auth_url();
	QString get_folder_tag(const QString& filename);

private slots:
	void i_update_status(DropboxStatus s, const QString& m);

signals:
	void update_status(int, QString);
	void new_file_added(const QString&);
};

#endif // DROPBOXCLIENTADAPTOR_H
