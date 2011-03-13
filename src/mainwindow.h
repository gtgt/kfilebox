#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMainWindow>

#include "src/dropboxclient.h"
#include "src/dropboxclientadaptor.h"
#include "src/configuration.h"
#include "src/installerform.h"
#include "src/trayicon.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
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
