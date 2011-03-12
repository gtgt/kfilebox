#ifndef DROPBOXCLIENTADAPTOR_H
#define DROPBOXCLIENTADAPTOR_H

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QtDBus>
#include <QVariant>

#include "src/dropboxclient.h" //! for DropboxClient::DropboxStatus

QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

class DropboxClientAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.Kfilebox")
    Q_CLASSINFO("D-Bus Introspection", ""
                "  <interface name=\"org.kde.Kfilebox\">\n"
                "    <method name=\"start\"/>\n"
                "    <method name=\"stop\"/>\n"
                "    <signal name=\"updateStatus\"/>\n"
                "  </interface>\n"
                "")
public:
    DropboxClientAdaptor(QObject *parent);
    virtual ~DropboxClientAdaptor();

public:
public Q_SLOTS:
    void start();
    void stop();

Q_SIGNALS:
    void updateStatus(DropboxClient::DropboxStatus, QString);
};

#endif // DROPBOXCLIENTADAPTOR_H
