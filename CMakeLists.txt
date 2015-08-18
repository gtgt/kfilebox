cmake_minimum_required(VERSION 2.8.12)

project(kfilebox)

# Require extra-cmake-modules
find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Automatically handle the usual Qt5 tasks
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Helper modules provided by extra-cmake-modules
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
    Network
    DBus
    Sql
)

find_package(KF5 REQUIRED COMPONENTS
    Config
    CoreAddons
    I18n
    Notifications
)

set(kfilebox_sources
    main.cpp
    DropboxIcons.qrc
    src/configuration.cpp
    src/configurationdbdriver.cpp
    src/dropboxclientadaptor.cpp
    src/installerform.cpp
    src/notification.cpp
    src/dropboxclient.cpp
    src/mainwindow.cpp
)

option(ENABLE_DBUS "Add dbus communication layer" ON)

add_subdirectory(po)
add_subdirectory(icons)

add_executable(kfilebox ${kfilebox_sources})

target_link_libraries(kfilebox 
    Qt5::Widgets
    Qt5::Network
    Qt5::DBus
    Qt5::Sql
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::CoreAddons
    KF5::I18n
    KF5::Notifications)

install(TARGETS kfilebox ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS kfilebox.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES kfilebox.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFY5RCDIR})
