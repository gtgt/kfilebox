#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QFileInfo>
#include "model/Configuration.h"
#include "util/SystemCall.h"

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
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    Configuration *conf;
    SystemCall *caller;
    TrayIcon *trayIcon;

private slots:
    void applySettings();
    void saveSettings();
    void moveDropboxFolder();
    void setIcons(); //! who is it
};

#endif // MAINWINDOW_H
