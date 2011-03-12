#ifndef NOTIFICATION_H
#define NOTIFICATION_H

//#define USE_KSTATUSNOTIFIERITEM

#ifdef USE_KSTATUSNOTIFIERITEM
#include "kstatusnotifieritem.h"
#else
#include <QDebug>
#endif

#include <QObject>

class Notification : public QObject
{
#ifdef USE_KSTATUSNOTIFIERITEM
    KStatusNotifierItem* notification;
#endif

public:
    Notification();
    ~Notification();
    void send(const QString &message);
};

#endif // NOTIFICATION_H

//static Notification notify;
