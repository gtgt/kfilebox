#include <QSqlDatabase>
#include <QDir>
#include <QSqlRecord>
#include <QList>
#include <QString>
#include <QStringList>
#include <QSqlQuery>
#include <QVariant>

class FileJournal {
    /* {src_lang=cpp}*/


 public:

    FileJournal();

    QList <QString> lastChangedFiles();

    ~FileJournal();


 private:
    QSqlDatabase db;
};

