#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QFileInfo>
#include "model/Configuration.h"
#include "util/SystemCall.h"

#include "core/trayicon.h"

//! @todo remove SystemCaller

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
    void changeEvent(QEvent *e);

private:
    void loadSettings();

    Ui::MainWindow *ui;
    Configuration *conf;
    SystemCall *caller;
    TrayIcon *trayIcon;

private slots:
    void applySettings();
    void saveSettings();
    void moveDropboxFolder();
    void unlinkComputer();
    void setIcons();
    void downloadRadioToggle();
    void uploadRadioToggle();
    void proxyRadioToggle();
    void proxyAuthRadioToggle();
};

#endif // MAINWINDOW_H
