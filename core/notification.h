#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QObject>

//#include "knotification.h"


class Notification : public QObject
{
public:
    Notification();
    static void send(QString msg);

private:
//    KNotification *notificator;
};

#endif // NOTIFICATION_H
