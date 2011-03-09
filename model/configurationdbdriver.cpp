#include "configurationdbdriver.h"

#include <QSqlError>

//! @todo play with old dropbox if need
ConfigurationDBDriver::ConfigurationDBDriver(QObject *parent) :
    QObject(parent)
{
    dbFilename = QDir::toNativeSeparators(QDir::homePath()+"/.dropbox/config.db");
    if(QFile(dbFilename).exists())
    {
        dbVersion = CONFIG_DB;
        // config.db, can be upgraded, lets check schema
        db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE","DROPBOX_CONF"));
        db->setDatabaseName(dbFilename);
        if (!db->open())
            return;

        QSqlQuery query = db->exec("SELECT value FROM `config` WHERE `key`='config_schema_version'");
        if (query.next()) {
            if(query.value(0).toInt() != 1)
            {
                dbVersion = DROPBOX_DB;
                //! destroy this connection :(
                //! create new connection to old-style db
            }
        }

    } else if (QFile(QDir::toNativeSeparators(QDir::homePath()+"/.dropbox/dropbox.db")).exists())
    {
        // dropbox.db, old-style
        dbFilename = QDir::toNativeSeparators(QDir::homePath()+"/.dropbox/dropbox.db");
        dbVersion = DROPBOX_DB;
    } else
    {
        dbFilename = "";
        dbVersion = UNKNOWN;
        qDebug() << "~/.dropbox/[config.db,dropbox.db] not founded";
    }
}


ConfigurationDBDriver::~ConfigurationDBDriver()
{
    QString connectionName = db->connectionName();
    db->close();
    delete db;
    db=0;
    QSqlDatabase::removeDatabase(connectionName);
}

bool ConfigurationDBDriver::hasKey(const QString &key)
{
    if(dbVersion != CONFIG_DB)
        return false;

    QSqlQuery query = db->exec("SELECT COUNT(`key`) FROM `config` WHERE `key`='"+key+"'");
    if (query.next()) {
        if(query.value(0).toInt() == 1)
            return true;
    }
    return false;
}

QVariant ConfigurationDBDriver::getValue(const QString &key)
{
    if(dbVersion != CONFIG_DB)
        return QVariant();

    QSqlQuery query = db->exec("SELECT `value` FROM `config` WHERE `key`='"+key+"' LIMIT 1");
    if (query.next()) {
        return query.value(0);
    }
    return QVariant();
}

//! This function should only be called when dropbox is stopped
//! was not properly tested yeat
void ConfigurationDBDriver::setValue(const QString &key, const QVariant &value)
{
    if(dbVersion != CONFIG_DB)
        return;

    db->exec("INSERT INTO `config` (`key`, `value`) VALUES ('"+key+"', '"+value.toString()+"')");
    if(db->lastError().isValid())
        qDebug() << db->lastError();

}
