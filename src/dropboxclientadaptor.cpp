#include "dropboxclientadaptor.h"

DropboxClientAdaptor::DropboxClientAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    setAutoRelaySignals(true);
    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.registerObject("/Kfilebox", parent);
    connection.registerService("org.kde.Kfilebox");
}

DropboxClientAdaptor::~DropboxClientAdaptor()
{
}

void DropboxClientAdaptor::start()
{
    QMetaObject::invokeMethod(parent(), "start");
}

void DropboxClientAdaptor::stop()
{
    QMetaObject::invokeMethod(parent(), "stop");
}
