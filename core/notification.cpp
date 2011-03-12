#include "notification.h"

#include <QDebug>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusError>

#include <QString>
#include <QVariant>



Notification::Notification()
{
#ifdef USE_KSTATUSNOTIFIERITEM
    notification = new KStatusNotifierItem("Kfilebox");
#endif
}

Notification::~Notification()
{
#ifdef USE_KSTATUSNOTIFIERITEM
    delete notification;
#endif
}


void Notification::send(const QString &message)
{

    QDBusInterface remoteApp("org.freedesktop.Notifications","/org/freedesktop/Notifications","org.freedesktop.Notifications.Notify",QDBusConnection::sessionBus());
    if (remoteApp.isValid()){

        QDBusReply<uint> reply = remoteApp.call(QString("org.freedesktop.Notifications.Notify"), QVariant("kfilebox"), QVariant(115), QVariant("kfilebox"), QVariant("summary"), QVariant("body"));
        qDebug() << reply;
    } else {
        qDebug() << "remote call is invalid" << remoteApp.lastError();
    }

    // uint org.freedesktop.Notifications.Notify(QString app_name, uint replaces_id, QString app_icon, QString summary, QString body, QStringList actions, QVariantMap hints, int timeout)


#ifdef USE_KSTATUSNOTIFIERITEM
    //if (conf->getValue("ShowNotifications").toBool()==true){
    notification->showMessage("Kfilebox", message, "Kfilebox");
#else
    qDebug() << "Notification: " << message;
#endif
}

