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
    //if (conf->getValue("ShowNotifications").toBool()==true){
    notification->showMessage("Kfilebox", message, "Kfilebox");
}

