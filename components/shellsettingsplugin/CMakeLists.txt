# SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
# SPDX-FileCopyrightText: 2024 Luis Büchi <luis.buechi@server23.cc>
# SPDX-License-Identifier: GPL-2.0-or-later

kconfig_add_kcfg_files(shellsettingsplugin_LIB_SRCS mobileshellsettings.kcfgc GENERATE_MOC)
add_library(shellsettingsplugin ${shellsettingsplugin_LIB_SRCS})
#add_library(PM::LibShellSettingsPlugin ALIAS shellsettingsplugin)
#target_compile_definitions(shellsettingsplugin PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")
generate_export_header(shellsettingsplugin)#EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/shellsettingsplugin_export.h)
target_include_directories(shellsettingsplugin PUBLIC ${CMAKE_BINARY_DIR}/exports)
#target_include_directories(shellsettingsplugin PUBLIC
#    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
#    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
#    "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}>"
#)
target_link_libraries(shellsettingsplugin
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Quick
        KF6::ConfigCore

    PRIVATE
        Qt::DBus
        KF6::ConfigGui
        Qt::Qml
        Plasma::KWaylandClient
        KF6::Service
        KF6::Package
        KF6::KIOGui
        KF6::JobWidgets
)

#install(
#    FILES
#        mobileshellsettings.kcfg
#    DESTINATION
#        ${KDE_INSTALL_KCFGDIR}
#)

ecm_add_qml_module(shellsettingsplugin
    URI org.kde.plasma.private.mobileshell.shellsettingsplugin
    CLASS_NAME MobileShellSettings
    GENERATE_PLUGIN_SOURCE
)
ecm_finalize_qml_module(shellsettingsplugin)
install(TARGETS shellsettingsplugin EXPORT shellsettingspluginLibraryTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )

#set_property(TARGET shellsettingsplugin PROPERTY LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/org/kde/plasma/private/mobileshell/shellsettingsplugin)
#file(COPY qmldir DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/org/kde/plasma/private/mobileshell/shellsettingsplugin)

#install(TARGETS shellsettingsplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/private/mobileshell/shellsettingsplugin)
#install(FILES qmldir ${qml_SRC} DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/private/mobileshell/shellsettingsplugin)
