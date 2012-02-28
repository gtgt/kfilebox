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

enum DROPBOX_DB_VERSION {DROPBOX_DB, CONFIG_DB, UNKNOWN};

class ConfigurationDBDriver: public QObject
{
	Q_OBJECT
	Q_ENUMS(DROPBOX_DB_VERSION)
	QSqlDatabase* db;
	DROPBOX_DB_VERSION dbVersion;
public:
	explicit ConfigurationDBDriver(QObject *parent = 0);

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
};

class Singleton
{
	static ConfigurationDBDriver* d;

	Singleton() {}
	Singleton(const Singleton &);
	Singleton& operator=(const Singleton &);
public:
	static ConfigurationDBDriver* instance() {
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		if (!d) {
			d = new ConfigurationDBDriver();
		}
		return d;
	}

	static void drop() {
		static QMutex mutex;
		QMutexLocker locker(&mutex);
		delete d;
		d = 0;
	}
};

#endif // CONFIGURATIONDBDRIVER_H
