# SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: GPL-2.0-or-later

set(cellularplugin_SRCS
    cellularplugin.cpp
    cellularutil.cpp
)

add_library(cellularplugin ${cellularplugin_SRCS})

target_link_libraries(cellularplugin
    Qt::Core
    Qt::DBus
    KF5::CoreAddons
    KF5::QuickAddons
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::I18n
    KF5::Notifications
    KF5::NetworkManagerQt
    KF5::ModemManagerQt
)

set_property(TARGET cellularplugin PROPERTY LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/org/kde/plasma/mobileinitialstart/cellular)
file(COPY qmldir DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/org/kde/plasma/mobileinitialstart/cellular)

install(TARGETS cellularplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/mobileinitialstart/cellular)
install(FILES qmldir ${qml_SRC} DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasma/mobileinitialstart/cellular)

plasma_install_package(package org.kde.plasma.mobileinitialstart.cellular mobileinitialstart)




