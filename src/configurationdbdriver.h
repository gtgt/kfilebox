#ifndef CONFIGURATIONDBDRIVER_H
#define CONFIGURATIONDBDRIVER_H

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
#include <QVariant>

// Different ways to get list of recently changed files
//        if(dbVersion == DROPBOX_DB)
//            query = db->exec("SELECT active_server_path FROM file_journal ORDER BY active_sjid DESC LIMIT 5");
//        else if (dbVersion == CONFIG_DB)
//            query = db->exec("SELECT value FROM config WHERE key='recently_changed3'");

class ConfigurationDBDriver : public QObject
{
    Q_OBJECT
public:
    explicit ConfigurationDBDriver(QObject *parent = 0);
    ~ConfigurationDBDriver();

    QStringList listKeys() const;

    inline bool hasKey(const QString& key) {
        QSqlQuery query = justQuery(QString("SELECT COUNT(`key`) FROM `config` WHERE `key`='%1'").arg(key));
        if (query.next()) {
            if(query.value(0).toInt() == 1)
                return true;
        }
        return false;
    }

    inline QVariant getValue(const QString& key, QVariant defaultValue=QVariant()) {
        QSqlQuery query = justQuery(QString("SELECT `value` FROM `config` WHERE `key`='%1' LIMIT 1").arg(key));
        if (query.next()) {
            return query.value(0);
        }
        return defaultValue;
    }

    inline void setValue(const QString &key, const QVariant& value) {
        justQuery(QString("REPLACE INTO `config` (`key`, `value`) VALUES ('%1', %2)").arg(key).arg(value.toString()));
    }

    inline void deleteValue(const QString& key) {
        justQuery(QString("DELETE FROM `config` WHERE `key`='%1'").arg(key));
    }

private:
    inline QSqlQuery justQuery(const QString& query)
    {
        if(dbVersion != CONFIG_DB)
            return QSqlQuery();

        QSqlQuery result = db->exec(query);
        if(db->lastError().isValid())
            qDebug() << db->lastError() << " query:\n" << db->exec().lastQuery();
        return result;
    }
    enum DROPBOX_DB_VERSION {DROPBOX_DB, CONFIG_DB, UNKNOWN} dbVersion;
    QString dbFilename;
    QSqlDatabase* db;

};

#endif // CONFIGURATIONDBDRIVER_H
