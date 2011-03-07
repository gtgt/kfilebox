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

class ConfigurationDBDriver : public QObject
{
    Q_OBJECT
public:
    explicit ConfigurationDBDriver(QObject *parent = 0);

    bool hasKey(QString key);
    QString getValue(QString key);
    void setValue(QString key, QString value);


signals:

public slots:

};

#endif // CONFIGURATIONDBDRIVER_H
