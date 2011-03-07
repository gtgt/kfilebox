#include "configurationdbdriver.h"

//#include <QMap>

ConfigurationDBDriver::ConfigurationDBDriver(QObject *parent) :
    QObject(parent)
{
//    yeap it was a local test
//    QMap<QString, QString> map;
//    map.insert("config", "config.db");

}

/*QString getPath(QString key)
{
     return key; //map.at(key);
}
*/
bool ConfigurationDBDriver::hasKey(QString key)
{
    bool ret = false;
    {
        //! синхронизировать код с FileManager
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","DROPBOX_CONF");

        db.setDatabaseName(QDir::toNativeSeparators(QDir::homePath()+"/.dropbox/config.db"));
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

QString ConfigurationDBDriver::getValue(QString key)
{
    QString ret = "";
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","DROPBOX_CONF");
        db.setDatabaseName(QDir::toNativeSeparators(QDir::homePath()+"/.dropbox/config.db"));
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

void ConfigurationDBDriver::setValue(QString key, QString value)
{
    // "oops: this function was not tested yeat";
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","DROPBOX_CONF");
        db.setDatabaseName(QDir::toNativeSeparators(QDir::homePath()+"/.dropbox/config.db"));
        if (!db.open())
            return;

        db.exec("INSERT INTO `config` (`value`) VALUES ('"+value+"') WHERE `key`='"+key+"'");
    }
    QSqlDatabase::removeDatabase("DROPBOX_CONF");
}
