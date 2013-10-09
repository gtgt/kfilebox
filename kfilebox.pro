# -------------------------------------------------
# Project created by QtCreator 2010-01-28T13:45:03
# -------------------------------------------------


QT += sql network dbus
LIBS += -lkdeui -lkdecore
TARGET = bin/kfilebox
TEMPLATE = app
INCLUDEPATH += /usr/include/kde4
SOURCES += main.cpp \
    src/dropboxclient.cpp \
    src/dropboxclientadaptor.cpp \
    src/notification.cpp \
    src/configuration.cpp \
    src/configurationdbdriver.cpp \
    src/installerform.cpp \
    src/mainwindow.cpp
HEADERS += \
    src/dropboxclient.h \
    src/dropboxclientadaptor.h \
    src/notification.h \
    src/configuration.h \
    src/configurationdbdriver.h \
    src/installerform.h \
    src/mainwindow.h
RESOURCES += DropboxIcons.qrc

FORMS += \
    src/installerform.ui \
    src/mainwindow.ui

TRANSLIST = ar\
    br \
    cs \
    de \
    el \
    en \
    es \
    fr \
    gl \
    it \
    lt \
    nl \
    pl \
    ru \
    si \
    tr \
    tw \
    zh

for(language, TRANSLIST):TRANSLATIONS += locale/$${language}/kfilebox.po
FORMS +=
!isEmpty(TRANSLATIONS) {
    isEmpty(QMAKE_LRELEASE) {
        win32:QMAKE_LRELEASE = msgfmt.exe
        else:QMAKE_LRELEASE = msgfmt
    }
    TSQM.name = msgfmt \
        ${QMAKE_FILE_IN}
    TSQM.input = TRANSLATIONS
    TSQM.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.mo
    TSQM.commands = $$QMAKE_LRELEASE \
        -o \
        $$TSQM.output \
        ${QMAKE_FILE_IN}
    TSQM.CONFIG = no_link
    QMAKE_EXTRA_COMPILERS += TSQM
    PRE_TARGETDEPS += compiler_TSQM_make_all
}
else:message(No translation files in project)
