#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QDBusMessage>
//#include <QDBusPendingReply>
#include <QDBusConnection>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>

#include "configuration.h"

class Notification : public QObject
{
    Q_OBJECT
    bool ShowNotifications;
public:
    explicit Notification(QObject *parent = 0);
	~Notification() {}
    void send(const QString &message);

signals:

public slots:

};

#endif // NOTIFICATION_H

//static Notification notify;
