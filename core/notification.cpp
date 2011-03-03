#include "notification.h"
#include <QIcon>
#include <QPixmap>

Notification::Notification()
{
   //notificator->setPixmap(QIcon(":/img/kfilebox.png").pixmap(48,48));
}

void Notification::notify(QString msg)
{
    if (conf->getShowNotifications()==true){
        notificator= new KNotification("notify",KNotification::Persistent);
        notificator->setTitle("Kfilebox");
        //notificator->setPixmap(QIcon(":/img/white/kfileboxapp.png").pixmap(48,48));
        notificator->setText(msg);
        //notificator->event(notificator->eventId(),"Kfilebox",msg,QPixmap(":/img/white/kfileboxapp.png"));
        notificator->sendEvent();
    }
}


void Notification::setConfiguration(Configuration *c){
    this->conf=c;
}
