#include <QObject>

#include "kuniqueapplication.h"
/* for debugging...
#include "kapplication.h"
*/

#include "kaboutdata.h"
#include "kcmdlineargs.h"
//#include "klocale.h"
//! @todo should I replace all tr() calls to ki18n()?

#include "src/mainwindow.h"


int main(int argc, char** argv)
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    KAboutData aboutData("kfilebox",
                         0,
                         ki18n("kfilebox"),
                         "0.4.10",
                         ki18n("KDE Dropbox frontend"),
                         KAboutData::License_GPL_V3,
                         ki18n("(c) 2011"),
                         ki18n("Kfilebox is a KDE frontend to Dropbox"),
                         "http://sourceforge.net/projects/kdropbox/",
                         "nib952051@gmail.com - fixes by GT (https://github.com/gtgt)");

    aboutData.setBugAddress("https://github.com/gtgt/kfilebox/issues");
    KCmdLineArgs::init( argc, argv, &aboutData );
    KUniqueApplication a;
    KUniqueApplication::setQuitOnLastWindowClosed(false);

    /* for debugging...
     KApplication a;
     KApplication::setQuitOnLastWindowClosed(false);
    */


    MainWindow w;
    Q_UNUSED(w);

    return a.exec();
}
