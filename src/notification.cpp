#include "notification.h"

Notification::Notification(QObject *parent) :
    QObject(parent)
{
}

Notification::~Notification()
{
}


void Notification::send(const QString &message)
{

    // Notify(QString app_name, uint replaces_id, QString app_icon, QString summary, QString body, QStringList actions, QVariantMap hints, int timeout)

    QString service   = "org.freedesktop.Notifications";
    QString path      = "/org/freedesktop/Notifications";
    QString interface = "org.freedesktop.Notifications";
    QString method    = "Notify";

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
    QDBusPendingReply<quint32> reply;

    msg.setArguments(arguments);

    reply = QDBusConnection::sessionBus().call(msg);

}
