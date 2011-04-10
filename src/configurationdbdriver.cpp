#include "configurationdbdriver.h"

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
    if(dbVersion != CONFIG_DB)
        return;

    QString connectionName = db->connectionName();
    db->close();
    delete db;
    db=0;
    QSqlDatabase::removeDatabase(connectionName);
}
