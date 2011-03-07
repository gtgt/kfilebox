#ifndef TRAYICON_H
#define TRAYICON_H

#include <QObject>
#include <kstatusnotifieritem.h>
#include <QAction>
#include <kmenu.h>
#include "util/SystemCall.h"
#include "DropboxClient.h"
#include "kicon.h"
#include "model/Configuration.h"
#include "core/notification.h"
#include <core/filebrowser.h>

namespace core {
class TrayIcon : public QWidget
{
    Q_OBJECT
public:
    TrayIcon(Configuration *);
    ~TrayIcon();
    void setCaller(SystemCall *);
    void loadIcons();

protected:
    //void changeEvent(QEvent *e);

private:
    void createActions();
    void createTrayIcon();

    KStatusNotifierItem *trayIcon;
    KMenu *trayIconMenu;
    KMenu *chFiles;
    Configuration *conf;
    FileBrowser *fbrowser;

    enum DropboxStatus {DropboxUnkown, DropboxIdle, DropboxBussy, DropboxError, DropboxUploading, DropboxDownloading, DropboxSaving, DropboxIndexing, DropboxStopped, DropboxDisconnected};
    DropboxStatus dStatus;
    //bool idle;
    QString currentMessage;

    QIcon defaultIcon;
    QIcon idleIcon;
    QIcon bussyIcon;
    QIcon errorIcon;
    QIcon appIcon;

    QAction *openDir;
    QAction *openDropboxWebsite;
    QAction *openGetMoreSpace;
    QAction *openPrefs;
    QAction *openHelpCenter;
    QAction *openTour;
    QAction *openForums;
    QAction *startAction;
    QAction *stopAction;
    QAction *quitAction;

    SystemCall *caller;
    int pid;
    DropboxClient *dc;

public slots:
    void openFileBrowser(QString path="");
    void openPrefsWindow();
    void openHelpCenterURL();
    void openTourURL();
    void openForumsURL();
    void startDropboxDaemon();
    void stopDropboxDaemon();
    void updateTryIcon(QString);
    void trayIconDblClicked(bool, QPoint);
    void prepareLastChangedFiles();
    void openDropboxWebsiteURL();
    void openGetMoreSpaceURL();

 signals:
    void prefsWindowActionTrigered();
};
} /* End of namespace core */
#endif // TRAYICON_H
