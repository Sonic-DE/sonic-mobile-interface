# SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: GPL-2.0-or-later

set(mobiletaskswitcher_SRCS
    main.cpp
    mobiletaskswitchereffect.cpp
)
add_library(kwin_effect_taskswitcher SHARED ${mobiletaskswitcher_SRCS})

install(FILES metadata.json DESTINATION ${KDE_INSTALL_DATADIR}/kwin/builtin-effects/kwin_effect_mobiletaskswitcher/)

target_link_libraries(kwin_effect_taskswitcher
PRIVATE
    kwineffects

    KF5::ConfigGui
    KF5::GlobalAccel
    KF5::I18n
    KF5::CoreAddons
    KF5::WindowSystem

    Qt::Quick
)

install(TARGETS kwin_effect_taskswitcher DESTINATION ${PLUGIN_INSTALL_DIR}/kwin/effects/plugins)
install(DIRECTORY qml DESTINATION ${KDE_INSTALL_DATADIR}/kwin/effects/mobiletaskswitcher)
