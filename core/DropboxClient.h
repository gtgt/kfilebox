#ifndef core_DropboxClient_h
#define core_DropboxClient_h

#include <QDir>
#include <QTextStream>

#include "util/SystemCall.h"
#include "DaemonConnection.h"
#include "notification.h"
#include "model/Configuration.h"

namespace core {

    class DropboxClient: public QObject {
 Q_OBJECT
 public:

    DropboxClient();

    bool start();

    bool stop();

    bool is_running();

    QString status();

 public:
    int pid;

private:
    DaemonConnection *daemon;
    Notification *notif;
    Configuration *conf;

public Q_SLOTS:
    void processMessage(QString);
    void setConfiguration(Configuration *);

Q_SIGNALS:
    void messageProcessed(QString);

};

} /* End of namespace core */

#endif // core_DropboxClient_h
