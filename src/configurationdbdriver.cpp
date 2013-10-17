#include "configurationdbdriver.h"

ConfigurationDBDriver* Singleton::d = 0;

//! @todo play with old dropbox if need
ConfigurationDBDriver::ConfigurationDBDriver(QObject *parent) :
    QObject(parent),
    dbVersion(DropboxDBUnknown)
{
    QString dbFilename = QDir(Configuration().getValue("ConfigDir").toString()).filePath("aggregation.dbx");
    if (QFileInfo(dbFilename).exists())
    {
        db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "DROPBOX_CONF"));
        db->setDatabaseName(dbFilename);
        if (!db->open())
            return;

        dbVersion = DropboxDBAggregation;
    }
    else {
        qDebug() << QString("\"%1\" was not found").arg(dbFilename);
    }
}


ConfigurationDBDriver::~ConfigurationDBDriver()
{
    if (dbVersion != DropboxDBAggregation)
        return;

    QString connectionName = db->connectionName();
    db->close();
    delete db;
    db = 0;
    QSqlDatabase::removeDatabase(connectionName);
}

bool ConfigurationDBDriver::hasKey(const QString& key) {
    QSqlQuery query = justQuery(QString("SELECT COUNT(`key`) FROM `snapshot` WHERE `key`='%1'").arg(key));
    if (query.next()) {
        if (query.value(0).toInt() == 1)
            return true;
    }
    return false;
}

QVariant ConfigurationDBDriver::getValue(const QString& key, QVariant defaultValue) {
    QSqlQuery query = justQuery(QString("SELECT `value` FROM `snapshot` WHERE `key`='%1' LIMIT 1").arg(key));
    if (query.next()) {
        return query.value(0);
    }
    return defaultValue;
}

/*void ConfigurationDBDriver::setValue(const QString &key, const QVariant& value) {
    justQuery(QString("REPLACE INTO `snapshot` (`key`, `value`) VALUES ('%1', %2)").arg(key).arg(value.toString()));
}

void ConfigurationDBDriver::deleteValue(const QString& key) {
    justQuery(QString("DELETE FROM `snapshot` WHERE `key`='%1'").arg(key));
}*/
