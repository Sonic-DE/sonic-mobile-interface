# SPDX-FileCopyrightText: 2017 Marco Martin <mart@kde.org>
# SPDX-FileCopyrightText: 2021 Aleix Pol <apol@kde.org>
# SPDX-FileCopyrightText: 2020-2021 Nicolas Fella <nicolas.fella@kde.org>
# SPDX-FileCopyrightText: 2022 Alexey Andreyev <aa13q@ya.ru>
# SPDX-License-Identifier: GPL-2.0-or-later

set(phonepanel_SRCS
    phonepanel.cpp
)

add_library(plasma_applet_phonepanel MODULE ${phonepanel_SRCS})

target_link_libraries(plasma_applet_phonepanel
                      Qt::Gui
                      Qt::DBus
                      Qt::Quick
                      Qt::WaylandClient
                      Qt::WaylandClientPrivate
                      KF5::Plasma
                      KF5::I18n
                      KF5::Service
                      KF5::WindowSystem
                      Wayland::Client
                     )

if (QT_MAJOR_VERSION EQUAL "5")
    ecm_add_qtwayland_client_protocol(plasma_applet_phonepanel
        PROTOCOL ${PLASMA_WAYLAND_PROTOCOLS_DIR}/kde-lockscreen-overlay-v1.xml
        BASENAME kde-lockscreen-overlay-v1
    )
else()
    qt6_generate_wayland_protocol_client_sources(plasma_applet_phonepanel FILES
        ${PLASMA_WAYLAND_PROTOCOLS_DIR}/kde-lockscreen-overlay-v1.xml)
endif()

install(TARGETS plasma_applet_phonepanel DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/applets)
#install(FILES plasma-phonepanel-default.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

plasma_install_package(package org.kde.phone.panel)

