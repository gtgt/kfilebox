#ifndef KONQUEROR_H
#define KONQUEROR_H

#include <core/filebrowser.h>
#include <QObject>
#include "util/SystemCall.h"

class Konqueror : public FileBrowser
{
Q_OBJECT
public:
    explicit Konqueror(QObject *parent = 0);
    Konqueror(SystemCall *sc,QObject *parent = 0);
    void execute();
    bool quit();
    int openNewWindow(QString path);
    bool closeWindow();

private:
    SystemCall *sc;

signals:

public slots:

};

#endif // KONQUEROR_H
