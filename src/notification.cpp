#include "notification.h"

Notification::Notification(QObject *parent) :
    QObject(parent)
{
    Configuration conf;
    ShowNotifications = conf.getValue("ShowNotifications").toBool();
}

void Notification::send(const QString &message)
{
	if(!ShowNotifications) {
		qDebug() << "Notification: " << message;
		return;
	}

	QString service = "org.freedesktop.Notifications";
	QString path = "/org/freedesktop/Notifications";
	QString interface = "org.freedesktop.Notifications";
	QString method = "Notify";

	QList<QVariant> arguments;
	arguments.append(QVariant(QString("Kfilebox")));
	arguments.append(QVariant(quint32(0)));
	arguments.append(QVariant(QString("kfilebox")));
	arguments.append(QVariant("Kfilebox"));
	arguments.append(QVariant(message));
	arguments.append(QVariant(QStringList()));
	arguments.append(QVariant(QVariantMap()));
	arguments.append(QVariant((int)0));

	QDBusMessage msg = QDBusMessage::createMethodCall(service, path, interface, method);
	msg.setArguments(arguments);
	QDBusConnection::sessionBus().call(msg);
}
