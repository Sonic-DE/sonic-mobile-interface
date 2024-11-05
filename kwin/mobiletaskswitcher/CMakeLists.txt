# SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: GPL-2.0-or-later

kcoreaddons_add_plugin(mobiletaskswitcher INSTALL_NAMESPACE "kwin/effects/plugins")

target_sources(mobiletaskswitcher PRIVATE
    main.cpp
    mobiletaskswitchereffect.cpp
    effecttouchborder.cpp
    taskfiltermodel.cpp
    taskmodel.cpp
)

target_link_libraries(mobiletaskswitcher
    shellsettingsplugin

    KF6::ConfigGui
    KF6::GlobalAccel
    KF6::I18n
    KF6::CoreAddons
    KF6::WindowSystem

    Qt::Quick
    Qt::Core

    KWin::kwin
    Plasma::Activities
)


target_include_directories(mobiletaskswitcher PRIVATE /home/luis/kde/usr/lib64/qml/org/kde/plasma/private/mobileshell/shellsettingsplugin)

install(DIRECTORY qml DESTINATION ${KDE_INSTALL_DATADIR}/kwin/effects/mobiletaskswitcher)
install(FILES mobiletaskswitcher.json DESTINATION ${KDE_INSTALL_DATADIR}/kwin/effects/mobiletaskswitcher)
