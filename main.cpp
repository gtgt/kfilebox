#include "kuniqueapplication.h"
#include "kaboutdata.h"
#include "kcmdlineargs.h"
#include "klocale.h"

#include "src/mainwindow.h"

int main(int argc, char *argv[])
{
    KAboutData aboutData(
                "kfilebox",
                0,
                ki18n("kfilebox"),
                "0.4.8",
                ki18n("KDE dropbox client and gui"),
                KAboutData::License_GPL_V3,
                ki18n("(c) 2010"),
                // Optional text shown in the About box.
                // Can contain any information desired.
                ki18n("Kfilebox is a KDE dropbox client and gui"),
                "http://kfilebox.deuteros.es/",
                "kfilebox@gmail.com");

    KCmdLineArgs::init( argc, argv, &aboutData );
    KUniqueApplication a;
    KUniqueApplication::setQuitOnLastWindowClosed(false);


    MainWindow w;
    Q_UNUSED(w);
    return a.exec();
}
