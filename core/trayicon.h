#ifndef TRAYICON_H
#define TRAYICON_H

#include <QAction>
#include <QDesktopServices>
#include <QObject>
#include <QProcess>
#include <QSignalMapper>
#include <QUrl>

#include <kstatusnotifieritem.h>
#include <kmenu.h>
#include "kicon.h"

//#include "core/notification.h"
#include "core/DropboxClient.h" // for enum DropboxClient
#include "model/Configuration.h"

namespace core {

class TrayIcon : public QWidget
{
    Q_OBJECT
public:
    TrayIcon();
    ~TrayIcon();
    void loadIcons(const QString &iconset);

private:


    void createActions();
    void createTrayIcon();

    KStatusNotifierItem *trayIcon;
    KMenu *trayIconMenu;
    KMenu *chFiles;


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
    void updateStatus(DropboxClient::DropboxStatus newStatus, const QString &message);

    void openFileBrowser(const QString &path="");
    void openHelpCenterURL();
    void openTourURL();
    void openForumsURL();
    void openDropboxWebsiteURL();
    void openGetMoreSpaceURL();
    void prepareLastChangedFiles();

signals:
    void prefsWindowActionTrigered();
    void startDropbox();
    void stopDropbox();
};
} /* End of namespace core */
#endif // TRAYICON_H
