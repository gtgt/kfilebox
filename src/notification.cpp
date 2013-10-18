#include "notification.h"

Notification::Notification(QObject *parent) :
    QObject(parent)
{
    Configuration conf;
    ShowNotifications = conf.getValue("ShowNotifications").toBool();
}

void Notification::send(const QString &message) const
{
	if(!ShowNotifications) {
		qDebug() << "Notification: " << message;
		return;
	}

    static const QString service = "org.freedesktop.Notifications",
            path = "/org/freedesktop/Notifications",
            interface = "org.freedesktop.Notifications",
            method = "Notify";

    const QVariantList arguments = QVariantList()
            << QVariant(QString("Kfilebox"))
            << QVariant(quint32(0))
            << QVariant(QString("kfilebox"))
            << QVariant("Kfilebox")
            << QVariant(message)
            << QVariant(QStringList())
            << QVariant(QVariantMap())
            << QVariant((int)0);

	QDBusMessage msg = QDBusMessage::createMethodCall(service, path, interface, method);
	msg.setArguments(arguments);
	QDBusConnection::sessionBus().call(msg);
}
