#ifndef CONFIGURATIONDBDRIVER_H
#define CONFIGURATIONDBDRIVER_H

#include <QObject>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
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

    bool hasKey(const QString &key);
    QString getValue(const QString &key);
    void setValue(const QString &key, const QString &value);
    void deleteValue(const QString &key);

private:
    enum DROPBOX_DB_VERSION {DROPBOX_DB, CONFIG_DB, UNKNOWN} dbVersion;
    QString dbFilename;
signals:

public slots:

};

#endif // CONFIGURATIONDBDRIVER_H
