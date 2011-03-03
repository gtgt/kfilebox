#include "dolphin.h"
#include <QDBusInterface>
#include <QDBusReply>

Dolphin::Dolphin(QObject *parent) :
    FileBrowser(parent)
{
        setPid(-1);
}

void Dolphin::execute(){}
bool Dolphin::quit(){return false;}

int Dolphin::openNewWindow(QString path){
    setPid(-1);
    QDBusInterface remoteApp("org.kde.dolphin","/MainApplication","org.kde.dolphin.Application",QDBusConnection::sessionBus());
    if (remoteApp.isValid()){
        QDBusReply<int> reply =remoteApp.call("openWindow",path);
        setPid(reply.value());
        qt_message_output(QtWarningMsg,"Dolphin's window id is "+ QString::number(getPid()).toLatin1());

    }

    return getPid();
}

bool Dolphin::closeWindow(){
    bool returnValue=true;
    QDBusInterface remoteApp("org.kde.dolphin","/dolphin/MainWindow"+QString::number(getPid()),"org.kde.dolphin.MainWindow",QDBusConnection::sessionBus());
    if (remoteApp.connection().isConnected()){
        QDBusReply<int> reply =remoteApp.call("quit");

        if (reply.error().type()==21 || reply.error().type()==4){
            returnValue=false;
        }
        //qt_message_output(QtWarningMsg,"DBus error "+reply.error().errorString(reply.error().type()).toLatin1());

    }
    return returnValue;
}
