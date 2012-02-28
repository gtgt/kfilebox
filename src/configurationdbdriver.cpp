#include "configurationdbdriver.h"

ConfigurationDBDriver* Singleton::d = 0;

//! @todo play with old dropbox if need
ConfigurationDBDriver::ConfigurationDBDriver(QObject *parent) :
    QObject(parent)
{
    QString dbFilename = QDir::toNativeSeparators(QDir::homePath()+"/.dropbox/config.db");
    if(QFile(dbFilename).exists())
    {
        dbVersion = CONFIG_DB;
        // config.db, can be upgraded, lets check schema
        db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE","DROPBOX_CONF"));
        db->setDatabaseName(dbFilename);
        if (!db->open())
            return;

        QSqlQuery query = justQuery("SELECT value FROM `config` WHERE `key`='config_schema_version'");
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
        qDebug() << "~/.dropbox/[config.db,dropbox.db] was not found";
    }
}


ConfigurationDBDriver::~ConfigurationDBDriver()
{
    if(dbVersion != CONFIG_DB)
        return;

    QString connectionName = db->connectionName();
    db->close();
    delete db;
    db=0;
    QSqlDatabase::removeDatabase(connectionName);
}

bool ConfigurationDBDriver::hasKey(const QString& key) {
    QSqlQuery query = justQuery(QString("SELECT COUNT(`key`) FROM `config` WHERE `key`='%1'").arg(key));
    if (query.next()) {
        if(query.value(0).toInt() == 1)
            return true;
    }
    return false;
}

QVariant ConfigurationDBDriver::getValue(const QString& key, QVariant defaultValue) {
    QSqlQuery query = justQuery(QString("SELECT `value` FROM `config` WHERE `key`='%1' LIMIT 1").arg(key));
    if (query.next()) {
        return query.value(0);
    }
    return defaultValue;
}

void ConfigurationDBDriver::setValue(const QString &key, const QVariant& value) {
    justQuery(QString("REPLACE INTO `config` (`key`, `value`) VALUES ('%1', %2)").arg(key).arg(value.toString()));
}

void ConfigurationDBDriver::deleteValue(const QString& key) {
    justQuery(QString("DELETE FROM `config` WHERE `key`='%1'").arg(key));
}
