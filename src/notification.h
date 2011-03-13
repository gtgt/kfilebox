#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QDBusMessage>
#include <QDBusPendingReply>
#include <QDBusConnection>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>

class Notification : public QObject
{
    Q_OBJECT

public:
    explicit Notification(QObject *parent = 0);
    ~Notification();
    void static send(const QString &message);

signals:

public slots:

};

#endif // NOTIFICATION_H

//static Notification notify;
