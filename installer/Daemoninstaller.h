#ifndef installer_Daemoninstaller_h
#define installer_Daemoninstaller_h

#include <QString>
#include <QObject>
#include "installerform.h"



namespace installer {

    class Daemoninstaller: public QObject {
 Q_OBJECT
 public:

    Daemoninstaller();

    void downloadDaemon();

    void extract();

    void executeWizzard();

    void preventGtkGuiExecution();

public Q_SLOTS:
    void onFileDownloaded();
    void onFileExtracted(int);
    void onDownloadProgressChange(int);

 private:
    QString daemonUrl;
    QString downloadPath;
    InstallerForm *form;
};

} /* End of namespace installer */

#endif // installer_Daemoninstaller_h
