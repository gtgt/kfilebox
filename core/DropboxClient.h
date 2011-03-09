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
    bool isRunning();

    //! This functions not strongly related to client..
    void static hideGtkUi();
    void static showGtkUi();
    bool isInstalled();

private:
    QLocalSocket* m_socket;
    QString m_socketPath;
    QTimer* timer;

public slots:
    void start();
    void stop();

private slots:
    void readMsg();
    void displayError(QLocalSocket::LocalSocketError socketError);
    void getDropboxStatus();

signals:
    void messageProcessed(QString);

};

} /* End of namespace core */

#endif // core_DropboxClient_h
