#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QDebug>
#include <QIcon>
#include <QObject>
#include <QPixmap>

//#include "knotification.h"
#include "kstatusnotifieritem.h"

class Notification : public QObject
{
public:
    Notification();
    ~Notification();
    void send(const QString &message);
private:

    KStatusNotifierItem* notification;

};

#endif // NOTIFICATION_H
