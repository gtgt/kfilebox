#include "configurationdbdriver.h"

ConfigurationDBDriver::ConfigurationDBDriver(QObject *parent) :
    QObject(parent)
{
    dbFilename = QDir::toNativeSeparators(QDir::homePath()+"/.dropbox/config.db");
    if(QFile().exists(dbFilename))
    {
        dbVersion = DROPBOX_DB;
        // config.db, can be upgraded, lets check schema
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","DROPBOX_CONF");
            db.setDatabaseName(dbFilename);
            if (!db.open())
                return;

            QSqlQuery query = db.exec("SELECT value FROM `config` WHERE `key`='config_schema_version'");
            if (query.next()) {
                if(query.value(0).toInt() == 1)
                    dbVersion = CONFIG_DB;
            }
        }
        QSqlDatabase::removeDatabase("DROPBOX_CONF");


    } else if (QFile(QDir::toNativeSeparators(QDir::homePath()+"/.dropbox/dropbox.db")).exists())
    {
        // dropbox.db, old-style
        dbFilename = QDir::toNativeSeparators(QDir::homePath()+"/.dropbox/dropbox.db");
        dbVersion = DROPBOX_DB;
    } else
    {
        dbFilename = "";
        dbVersion = UNKNOWN;
        qDebug() << "no one version is installed";
    }
}

bool ConfigurationDBDriver::hasKey(const QString &key)
{
    bool ret = false;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","DROPBOX_CONF");
        db.setDatabaseName(dbFilename);
        if (!db.open())
            return false;

        QSqlQuery query = db.exec("SELECT COUNT(`key`) FROM `config` WHERE `key`='"+key+"'");
        if (query.next()) {
            if(query.value(0).toInt() == 1)
                ret = true;
        }
    }
    QSqlDatabase::removeDatabase("DROPBOX_CONF");
    return ret;
}

QString ConfigurationDBDriver::getValue(const QString &key)
{
    QString ret = "";
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","DROPBOX_CONF");
        db.setDatabaseName(dbFilename);
        if (!db.open())
            return ret;

        QSqlQuery query = db.exec("SELECT `value` FROM `config` WHERE `key`='"+key+"' LIMIT 1");
        if (query.next()) {
            ret = query.value(0).toString();
        }
    }
    QSqlDatabase::removeDatabase("DROPBOX_CONF");
    return ret;
}

// This function should only be called when dropbox is stopped
void ConfigurationDBDriver::setValue(const QString &key, const QString &value)
{
    // "oops: this function was not tested yeat";
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","DROPBOX_CONF");
        db.setDatabaseName(dbFilename);
        if (!db.open())
            return;

        db.exec("UPDATE `config` SET `value`='"+value+"' WHERE `key`='"+key+"'");
    }
    QSqlDatabase::removeDatabase("DROPBOX_CONF");
}
