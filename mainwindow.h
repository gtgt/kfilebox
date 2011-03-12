#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QFileInfo>
#include <QtGui>
#include <QFileInfo>
#include <QFile>
#include <QDebug>

#include <QtDBus/QDBusConnection>

#include "model/Configuration.h"
#include "installer/installerform.h"
#include "core/DropboxClient.h"
#include "core/trayicon.h"

#include "dropboxclientadaptor.h"

using namespace core;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent* e);

private:
    void loadSettings();
    void initializeDBus();

    Ui::MainWindow* ui;
    TrayIcon* trayIcon;
    DropboxClient* dc;
    DropboxClientAdaptor* adaptor;

private slots:
    void applySettings();
    void saveSettings();
    void changeDropboxFolder();
    void unlinkComputer();
    void setIcons();    // select icon themes
    void downloadRadioToggle();
    void uploadRadioToggle();
    void proxyRadioToggle();
    void proxyAuthRadioToggle();
};

#endif // MAINWINDOW_H
