#ifndef TRAYICON_H
#define TRAYICON_H

#include <QObject>
#include <kstatusnotifieritem.h>
#include <QAction>
#include <kmenu.h>
#include "DropboxClient.h"
#include "kicon.h"
#include "model/Configuration.h"
#include "core/notification.h"

#include <QProcess>

namespace core {
class TrayIcon : public QWidget
{
    Q_OBJECT
public:
    TrayIcon();
    ~TrayIcon();
    void loadIcons(const QString &iconset);

protected:
    //void changeEvent(QEvent *e);

private:
    void createActions();
    void createTrayIcon();

    KStatusNotifierItem *trayIcon;
    KMenu *trayIconMenu;
    KMenu *chFiles;
    Configuration *conf;

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
    void startDropbox();
    void stopDropbox();
};
} /* End of namespace core */
#endif // TRAYICON_H
