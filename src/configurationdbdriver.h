#ifndef CONFIGURATIONDBDRIVER_H
#define CONFIGURATIONDBDRIVER_H

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
#include <QVariant>

class ConfigurationDBDriver : public QObject
{
    Q_OBJECT
	static ConfigurationDBDriver* m_Instance;

    explicit ConfigurationDBDriver(QObject *parent = 0);
public:
	static ConfigurationDBDriver* instance() {
		static QMutex mutex;
		if (!m_Instance) {
			mutex.lock();
			if (!m_Instance)
				m_Instance = new ConfigurationDBDriver();
			mutex.unlock();
		}
		return m_Instance;
	}

	static void drop() {
		static QMutex mutex;
		mutex.lock();
		delete m_Instance;
		m_Instance = 0;
		mutex.unlock();
	}

    ~ConfigurationDBDriver();

    QStringList listKeys() const;

    bool hasKey(const QString& key);
    QVariant getValue(const QString& key, QVariant defaultValue=QVariant());
    void setValue(const QString &key, const QVariant& value);
    void deleteValue(const QString& key);

private:
    inline QSqlQuery justQuery(const QString& query)
    {
        if(dbVersion != CONFIG_DB)
            return QSqlQuery();

        QSqlQuery result = db->exec(query);
        if(db->lastError().isValid())
            qDebug() << db->lastError() << "\nquery:" << query;
        return result;
    }
    enum DROPBOX_DB_VERSION {DROPBOX_DB, CONFIG_DB, UNKNOWN} dbVersion;
    QSqlDatabase* db;

};

#endif // CONFIGURATIONDBDRIVER_H
