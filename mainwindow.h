#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QFileInfo>
#include <QtGui>
#include <QFileInfo>
#include <QFile>
#include <QDebug>

#include "model/Configuration.h"
#include "installer/Daemoninstaller.h"
#include "core/DropboxClient.h"
#include "core/trayicon.h"

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

    Ui::MainWindow* ui;
    TrayIcon* trayIcon;
    DropboxClient* dc;

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
