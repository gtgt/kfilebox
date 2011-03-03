#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QObject>

#include "knotification.h"
#include "model/Configuration.h"

class Notification : public QObject
{
public:
    Notification();
    void notify(QString msg);
    void setConfiguration(Configuration *);

private:
    KNotification *notificator;
    Configuration *conf;
};

#endif // NOTIFICATION_H
