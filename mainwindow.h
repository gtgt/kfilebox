#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QFileInfo>
#include "model/Configuration.h"


#include "core/trayicon.h"

//! @todo Configuration may be created and used locally only in two functions[loadSettings, applySettings], REMOVE TrayIcon relations to it

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
    void setIcons();
    void downloadRadioToggle();
    void uploadRadioToggle();
    void proxyRadioToggle();
    void proxyAuthRadioToggle();
};

#endif // MAINWINDOW_H
