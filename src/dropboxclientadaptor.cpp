#include "dropboxclientadaptor.h"

DropboxClientAdaptor::DropboxClientAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // I am constructor
    setAutoRelaySignals(true);
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.registerObject("/Kfilebox", parent);
    connection.registerService("org.kde.Kfilebox");
}

DropboxClientAdaptor::~DropboxClientAdaptor()
{
    // I am destructor
}

void DropboxClientAdaptor::start()
{
    QMetaObject::invokeMethod(parent(), "start");
}

void DropboxClientAdaptor::stop()
{
    QMetaObject::invokeMethod(parent(), "stop");
}
