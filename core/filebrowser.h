#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QObject>
#include <QString>

class FileBrowser : public QObject
{
public:
    FileBrowser(QObject *parent = 0);
    void setPid(int);
    int getPid();
    virtual void execute()=0;
    virtual bool quit()=0;
    virtual int openNewWindow(QString path)=0;
    virtual bool closeWindow()=0;

private:
    int pid;
    QString name;

};

#endif // FILEBROWSER_H
