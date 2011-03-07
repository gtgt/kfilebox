#ifndef core_DropboxClient_h
#define core_DropboxClient_h

#include <QDir>
#include <QLocalSocket>
#include <QProcess>
#include <QString>
#include <QTextStream>
#include <QTimer>

#include "notification.h"


namespace core {

class DropboxClient: public QObject {
    Q_OBJECT
public:
    DropboxClient();
    ~DropboxClient();
    void sendCommand(QString command);

    void start();
    void stop();

    bool is_running();

    // /*proxy from strings to inline functions*/ void getStatus(){sendCommand("get_dropbox_status");}

private:
    QLocalSocket* m_socket;
    QString m_socketPath;
    QTimer* timer;

private slots:
    void readMsg();
    void displayError(QLocalSocket::LocalSocketError socketError);
    void getDropboxStatus();

signals:
    void messageProcessed(QString);

};

} /* End of namespace core */

#endif // core_DropboxClient_h
