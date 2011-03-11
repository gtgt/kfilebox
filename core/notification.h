#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QObject>
#include <QProcess>

//#include "knotification.h"


class Notification : public QObject
{
public:
    Notification();
    static void send(const QString &message);

private:
//    KNotification *notificator;
};

#endif // NOTIFICATION_H
