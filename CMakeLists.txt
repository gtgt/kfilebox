PROJECT(kfilebox)
FIND_PACKAGE(KDE4 REQUIRED)
include_directories( ${QT_INCLUDES} ${KDE4_INCLUDES})

SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTSQL TRUE)
SET(QT_USE_QTDBUS TRUE)

SET(kfilebox_cpp main.cpp src/configuration.cpp src/dropboxclientadaptor.cpp src/installerform.cpp src/notification.cpp src/configurationdbdriver.cpp src/dropboxclient.cpp src/mainwindow.cpp)
SET(kfilebox_h src/configurationdbdriver.h src/dropboxclientadaptor.h src/installerform.h src/notification.h src/configuration.h src/dropboxclient.h src/mainwindow.h)
SET(kfilebox_ui src/mainwindow.ui src/installerform.ui)
SET(kfilebox_qrc DropboxIcons.qrc)

INCLUDE(${QT_USE_FILE})

OPTION(ENABLE_DBUS "Add dbus communication layer" ON)

#CONFIGURE_FILE(config.h.in config.h)

QT4_WRAP_CPP(kfilebox_moc_cpp ${kfilebox_h})
QT4_WRAP_UI(kfilebox_ui_h ${kfilebox_ui})
QT4_ADD_RESOURCES(kfilebox_resources_rcc ${kfilebox_qrc})

ADD_SUBDIRECTORY(locale)

ADD_EXECUTABLE(kfilebox ${kfilebox_cpp} ${kfilebox_moc_cpp} ${kfilebox_ui_h} ${kfilebox_resources_rcc})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

TARGET_LINK_LIBRARIES(kfilebox ${QT_LIBRARIES} ${KDE4_KDEUI_LIBS} ${KDE4_KDECORE_LIBS} )

INSTALL(TARGETS kfilebox DESTINATION bin)

# install icons and some stuff
#translations
