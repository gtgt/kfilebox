#include <QObject>

#include "kuniqueapplication.h"
#include "kaboutdata.h"
#include "kcmdlineargs.h"
//#include "klocale.h"

//! @todo should I replace all tr() calls to ki18n()?

#include "src/mainwindow.h"


int main(int argc, char** argv)
{
    KAboutData aboutData(
                "kfilebox",
                0,
                ki18n("kfilebox"),
                "0.4.9",
                ki18n("KDE Dropbox frontend"),
                KAboutData::License_GPL_V3,
                ki18n("(c) 2011"),
				ki18n("Kfilebox is a KDE frontend to Dropbox"),
				"http://sourceforge.net/projects/kdropbox/",
                "nib952051@gmail.com");

    aboutData.setBugAddress("http://sourceforge.net/apps/trac/kdropbox/report/1");
    KCmdLineArgs::init( argc, argv, &aboutData );
    KUniqueApplication a;
    KUniqueApplication::setQuitOnLastWindowClosed(false);

    MainWindow w;
    Q_UNUSED(w);

    return a.exec();
}
