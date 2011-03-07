#include "FileJournal.h"

FileJournal::FileJournal()
{
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE","DROPBOX"));

    if(false)
        db->setDatabaseName(QDir::toNativeSeparators(QDir::homePath().append("/.dropbox/dropbox.db")));
    else if (true)
        db->setDatabaseName(QDir::toNativeSeparators(QDir::homePath().append("/.dropbox/config.db")));


    if (!db->open())
        qt_message_output(QtDebugMsg,"Error connecting to dropbox configuration db");
}

FileJournal::~FileJournal()
{
    QString db_name = db->connectionName();
    db->close();
    delete db;
    db=0;
    QSqlDatabase::removeDatabase(db_name);

}

QStringList FileJournal::lastChangedFiles()
{

    QSqlQuery query;
    // if version == 0
    if(false)
        query = db->exec("SELECT active_server_path FROM file_journal ORDER BY active_sjid DESC LIMIT 5");
    else if (true) // version == 1
        query = db->exec("SELECT value FROM config WHERE key='recently_changed3'");

    QStringList paths;
    while (query.next()) {

        foreach (QString elem, query.value(0).toString().split("\n")) {
            QStringList list = elem.split(":");
            if(list.length()>1)
            {
                paths.append(list.value(1).trimmed());
            }
        }

    }

    if( (query.size() == -1) && (paths.size() == 0) )
        paths.append("File list is empty:(");

    return paths;
}

