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
    Q_OBJECT
#ifdef USE_KSTATUSNOTIFIERITEM
    KStatusNotifierItem* notification;
#endif

public:
    explicit Notification(QObject *parent = 0);
    ~Notification();
    void send(const QString &message);

signals:

public slots:

};

#endif // NOTIFICATION_H

//static Notification notify;
