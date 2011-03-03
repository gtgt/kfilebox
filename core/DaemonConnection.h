#ifndef core_DaemonConnection_h
#define core_DaemonConnection_h

#include <QDir>
#include <QFile>
#include <QLocalSocket>
#include <QString>
#include <QMessageBox>



namespace core {

class DaemonConnection : public QObject {

 Q_OBJECT
 public:

    DaemonConnection();

    virtual ~DaemonConnection();

    QString send_command(QString command, QString args);


 private:
    QLocalSocket *s;
    QFile *f;
    quint16 blockSize;



private Q_SLOTS:
    void readMsg();
    void displayError(QLocalSocket::LocalSocketError socketError);

Q_SIGNALS:
    void dataReceived(QString data);

};

} /* End of namespace core */

#endif // core_DaemonConnection_h
