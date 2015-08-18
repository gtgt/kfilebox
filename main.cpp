#include <QApplication>
#include <KAboutData>
#include <KLocalizedString>

#include "src/mainwindow.h"

int main(int argc, char** argv)
{
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    /*
     * Create the application
     */
    QApplication app(argc, argv);

    /*
     * Connect application with translation catalogs
     */
    KLocalizedString::setApplicationDomain("kfilebox");

    /*
     * Construct about data for kfilebox
     */
    KAboutData aboutData(QStringLiteral("kfilebox"),
                         i18n("Kfilebox"),
                         QStringLiteral("0.4.95"),
                         i18n("KDE Dropbox frontend"),
                         KAboutLicense::GPL_V3,
                         i18n("Copyright (C) 2011-2015"),
                         QString(),
                         QStringLiteral("https://github.com/gtgt/kfilebox"));
                         //"nib952051@gmail.com - fixes by GT (https://github.com/gtgt)");

    //aboutData.setOrganizationDomain("kde.org");
    aboutData.setBugAddress("https://github.com/gtgt/kfilebox/issues");
    aboutData.addAuthor(i18n("Guillermo Amat"), QString(), QString(), QString(), QString());

    /*
     * Register about data
     */
    KAboutData::setApplicationData(aboutData);

    /*
     * Set app settings from KAboutData
     */
    app.setApplicationName(aboutData.componentName());
    app.setApplicationDisplayName(aboutData.displayName());
    app.setOrganizationDomain(aboutData.organizationDomain());
    app.setApplicationVersion(aboutData.version());
    app.setQuitOnLastWindowClosed(false);

    /*
     * Create command line parser and handle standard options
     */
    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.setApplicationDescription(aboutData.shortDescription());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);
    aboutData.processCommandLine(&parser);

    MainWindow w;
    Q_UNUSED(w);

    return app.exec();
}
