#ifndef DOLPHIN_H
#define DOLPHIN_H

#include <core/filebrowser.h>
#include <QObject>

class Dolphin : public FileBrowser
{
Q_OBJECT
public:
    explicit Dolphin(QObject *parent = 0);
    void execute();
    bool quit();
    int openNewWindow(QString path);
    bool closeWindow();

signals:

public slots:

};

#endif // DOLPHIN_H
