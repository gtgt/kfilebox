#include <QObject>

#include "kuniqueapplication.h"
#include "kaboutdata.h"
#include "kcmdlineargs.h"
//#include "klocale.h"

//! @todo should I replace all tr() calls to ki18n()?

#include "src/mainwindow.h"


#include <QDebug>
#include <QString>
#include <QRegExp>
int main(int argc, char** argv)
{
	QString needed = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"\
			"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"\
			"p, li { white-space: pre-wrap; }\n"\
			"</style></head><body style=\" font-family:'Ubuntu'; font-size:9pt; font-weight:400; font-style:normal;\">\n"\
			"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">@ubuntard *учеба *дыбр *anime</p>\n"\
			"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Завтра в институте буду торчать с 9 утра до 9 вечера, презентация доклада готова процентов на двадцать, свежескачанный (интересно, а есть еще такие кто сами делают?) реферат про культ личности Сталина даже не прочитан, а я что - я &quot;Genshiken&quot; смотрю.</p>\n"\
			"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">#666777 (2 replies) http://juick.com/666777</p></body></html>\n";


	needed = needed.replace(QRegExp("(@[^ ^<]+)"), "<a href=\"xmpp:juick@juick.com?message;body=\\1\">\\1</a>");
	needed = needed.replace(QRegExp("(\\*[^ ^<]+)"), "<a href=\"xmpp:juick@juick.com?message;body=\\1\">\\1</a>");
	needed = needed.replace(QRegExp("(#[0-9]{5,})"), "<a href=\"xmpp:juick@juick.com?message;body=\\1\">\\1</a> <a href=\"xmpp:juick@juick.com?message;body=\\1\">+</a>");
	qDebug() << needed;



	return 1;


    KAboutData aboutData(
                "kfilebox",
                0,
                ki18n("kfilebox"),
                "0.4.8",
                ki18n("KDE Dropbox frontend"),
                KAboutData::License_GPL_V3,
                ki18n("(c) 2011"),
				ki18n("Kfilebox is a KDE frontend to Dropbox"),
				"http://sourceforge.net/projects/kdropbox/",
                "nib952051@gmail.com");

    KCmdLineArgs::init( argc, argv, &aboutData );
    KUniqueApplication a;
    KUniqueApplication::setQuitOnLastWindowClosed(false);

    MainWindow w;
    Q_UNUSED(w);

    return a.exec();
}
