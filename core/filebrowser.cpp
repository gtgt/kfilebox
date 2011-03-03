#include "filebrowser.h"

FileBrowser::FileBrowser(QObject *parent )

{
}

void FileBrowser::setPid(int p){
    pid =p;

}
int FileBrowser::getPid(){
    return pid;
}
