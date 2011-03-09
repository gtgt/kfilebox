#ifndef TRAYICON_H
#define TRAYICON_H

#include <QObject>
#include <QAction>
#include <QProcess>
#include <QSignalMapper>

#include <kstatusnotifieritem.h>
#include <kmenu.h>
#include "kicon.h"

#include "core/notification.h"
#include "model/Configuration.h"

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

    QSignalMapper* sm;

public slots:
    void openFileBrowser(const QString &path="");
    void openHelpCenterURL();
    void openTourURL();
    void openForumsURL();
    void updateTrayIcon(const QString &result);
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
