# SPDX-FileCopyrightText: 2014-2020 Marco Martin <mart@kde.org>
# SPDX-FileCopyrightText: 2017-2021 Bhushan Shah <bshah@kde.org>
# SPDX-FileCopyrightText: 2019-2021 Nicolas Fella <nicolas.fella@kde.org>
# SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.24)

project(plasma-mobile)
set(PROJECT_VERSION "6.8.2")

set(PROJECT_DEP_VERSION "6.8.0")
set(QT_MIN_VERSION "6.11.0")
set(KF6_MIN_VERSION "6.30.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(INSTALL_SYSTEMD_SERVICE "Install a systemd service file to start the session" OFF)

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

set(KF_IGNORE_PLATFORM_CHECK ON CACHE BOOL "Don't check platform that is being built on")

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMOptionalAddSubdirectory)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMMarkNonGuiExecutable)
include(ECMGenerateHeaders)
include(ECMFindQmlModule)
include(ECMQmlModule)
include(ECMConfiguredInstall)
include(ECMFindQmlModule)
include(ECMQtDeclareLoggingCategory)
include(GenerateExportHeader)
include(KDEGitCommitHooks)
include(KDEClangFormat)
include(FeatureSummary)

add_definitions(-DQT_NO_URL_CAST_FROM_STRING)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX PLASMA_MOBILE
    VERSION_HEADER ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED
    Core
    Qml
    Quick
    Gui
    Sensors
    Multimedia
)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    I18n
    GlobalAccel
    KIO
    Config
    DBusAddons
    ItemModels
    Service
    Notifications
    ModemManagerQt
    NetworkManagerQt
    KCMUtils
    Package
    JobWidgets
    WindowSystem
)

find_package(Plasma CONFIG REQUIRED)
find_package(PlasmaQuick CONFIG REQUIRED)
find_package(PlasmaActivities CONFIG REQUIRED)
find_package(KF6Screen CONFIG REQUIRED)
find_package(SonicLogging CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)

find_package(Qt6QmlPrivate REQUIRED) # workaround for Qcoro
find_package(QCoro6 REQUIRED COMPONENTS DBus Qml)
qcoro_enable_coroutines()
kde_enable_exceptions()

find_package(KF6KirigamiAddons 0.6 REQUIRED)
find_package(epoxy REQUIRED)
find_package(XCB REQUIRED COMPONENTS XCB SHAPE RANDR)
find_package(KWinX11 ${PROJECT_DEP_VERSION} REQUIRED COMPONENTS
    kwin
)

find_package(LibKWorkspace CONFIG REQUIRED)
find_package(Libudev REQUIRED)

find_package(KWinX11DBusInterface)
set_package_properties(KWinX11DBusInterface PROPERTIES DESCRIPTION "KWin DBus interface"
    TYPE REQUIRED
    PURPOSE "Needed for the night color quick setting"
)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS Auth)
set_package_properties(KF6::AuthCore PROPERTIES
    TYPE REQUIRED
    PURPOSE "Allows the flashlight quick setting to control the flashlight"
)

include(CheckIncludeFiles)


plasma_install_package(lookandfeel org.sonicde.silver.mobile.light look-and-feel lookandfeel)
plasma_install_package(lookandfeel-dark org.sonicde.silver.mobile.dark look-and-feel lookandfeel)
# The dark theme shares the light one's mobile logout screen and layout.
install(DIRECTORY lookandfeel/contents/logout lookandfeel/contents/layouts
        DESTINATION ${KDE_INSTALL_DATADIR}/plasma/look-and-feel/org.sonicde.silver.mobile.dark/contents)
plasma_install_package(shell org.kde.plasma.mobileshell shells)

add_subdirectory(bin)
add_subdirectory(components)
add_subdirectory(containments)
add_subdirectory(devices)
add_subdirectory(quicksettings)
add_subdirectory(kcms)
add_subdirectory(kded)
add_subdirectory(kwin)
add_subdirectory(envmanager)
add_subdirectory(initialstart)
add_subdirectory(layout-templates)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

install(FILES org.kde.plasma.mobileshell.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

find_program(PlasmaOpenSettings plasma-open-settings)
set_package_properties(PlasmaOpenSettings PROPERTIES
                                          URL https://invent.kde.org/plasma/kde-cli-tools/
                                          DESCRIPTION "Opens KCMs properly"
                                          TYPE RUNTIME
                                          PURPOSE "Used on the top panel")

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
list(FILTER ALL_CLANG_FORMAT_SOURCE_FILES EXCLUDE REGEX ".*\\.ccls-cache/.*")
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

ki18n_install(po)

if (INSTALL_SYSTEMD_SERVICE)
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/plasma-mobile.service
        DESTINATION ${KDE_INSTALL_SYSTEMDUNITDIR}/system
    )
endif()

ecm_qt_install_logging_categories(
    EXPORT PLASMA_MOBILE
    FILE plasmamobile.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)
