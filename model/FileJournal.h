#include <QDir>
#include <QString>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QVariant>

class FileJournal: public QObject{
    Q_OBJECT
public:
    FileJournal();
    ~FileJournal();
    QStringList lastChangedFiles();

private:
    QSqlDatabase* db;
};

