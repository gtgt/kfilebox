#ifndef TRAYICON_H
#define TRAYICON_H

#include <QAction>
#include <QDesktopServices>
#include <QMenu>
#include <QSignalMapper>
#include <QUrl>
#include <QWidget>

#include <kstatusnotifieritem.h>
#include <kmenu.h>
#include <kicon.h>

#include "src/dropboxclient.h" // for enum DropboxClient
#include "src/configuration.h"
#include "src/configurationdbdriver.h"


class TrayIcon : public QWidget
{
    Q_OBJECT
public:
    explicit TrayIcon(QWidget *parent = 0);
    ~TrayIcon();
    void loadIcons(const QString &iconset);

private:

    QString fixUnicodeChars(QString value);

    KStatusNotifierItem* trayIcon;
    QMenu* trayIconMenu;
    QMenu* chFiles;
    QMenu* helpMenu;

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
    QAction *statusAction;

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

#endif // TRAYICON_H
