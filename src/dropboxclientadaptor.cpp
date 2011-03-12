#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

#include "dropboxclientadaptor.h"

DropboxClientAdaptor::DropboxClientAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // I am constructor
    setAutoRelaySignals(true);
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
