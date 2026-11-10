# SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: GPL-2.0-or-later

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_org.kde.plasma.quicksetting.record\")

add_library(sonicmobilescreenrecorder STATIC screenrecorder/x11screenrecorder.cpp)
target_include_directories(sonicmobilescreenrecorder PUBLIC screenrecorder)
target_link_libraries(sonicmobilescreenrecorder PUBLIC Qt::Core Qt::Gui Qt::Multimedia XCB::XCB PRIVATE Sonic::Logging)

add_executable(sonic-mobile-screenrecorder screenrecorder/main.cpp)
target_link_libraries(sonic-mobile-screenrecorder PRIVATE sonicmobilescreenrecorder KF6::CoreAddons XCB::RANDR Sonic::Logging)
install(TARGETS sonic-mobile-screenrecorder DESTINATION ${KDE_INSTALL_LIBEXECDIR})

ecm_add_qml_module(recordplugin URI "org.kde.plasma.quicksetting.record" GENERATE_PLUGIN_SOURCE)

target_sources(recordplugin PRIVATE recordutil.cpp)
target_compile_definitions(recordplugin PRIVATE SCREENRECORDER_PATH="${KDE_INSTALL_FULL_LIBEXECDIR}/sonic-mobile-screenrecorder")

target_link_libraries(recordplugin PRIVATE
    Qt::Core
    Qt::Qml
    Qt::Quick
    KF6::I18n
    KF6::Notifications
    Sonic::Logging
)

ecm_finalize_qml_module(recordplugin)

install(FILES plasma_mobile_quicksetting_record.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR})

plasma_install_package(package org.kde.plasma.quicksetting.record quicksettings)
