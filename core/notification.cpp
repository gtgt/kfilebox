#include "notification.h"

Notification::Notification()
{
    notification = new KStatusNotifierItem("Kfilebox");

}

Notification::~Notification()
{
    delete notification;
}


void Notification::send(const QString &message)
{
    qDebug() << message;

    notification->showMessage("Kfilebox", message, "Kfilebox");

    /*if (conf->getValue("ShowNotifications").toBool()==true){
        notificator= new KNotification("notify",KNotification::Persistent);
        notificator->setTitle("Kfilebox");
        //notificator->setPixmap(QIcon(":/img/white/kfileboxapp.png").pixmap(48,48));
        notificator->setText(msg);
        //notificator->event(notificator->eventId(),"Kfilebox",msg,QPixmap(":/img/white/kfileboxapp.png"));
        notificator->sendEvent();
    }*/
}

