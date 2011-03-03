#include "konqueror.h"
#include <QDBusInterface>
#include <QDBusReply>

Konqueror::Konqueror(QObject *parent) :
    FileBrowser(parent)
{
}

Konqueror::Konqueror(SystemCall *sysc,QObject *parent) :
    FileBrowser(parent)
{
    sc=sysc;
}

void Konqueror::execute(){

}

bool Konqueror::quit(){
    //qt_message_output(QtWarningMsg,tr("Closing the window"));
    sc->close();

    return true;
}

int Konqueror::openNewWindow(QString path){
    setPid(-1);
    setPid(sc->execute("konqueror "+path).toInt());
    //qt_message_output(QtWarningMsg,tr("Konqueror's pid is' "+QString::number(getPid()).toLatin1()));

    return getPid();
}

bool Konqueror::closeWindow(){
    return quit();
}
