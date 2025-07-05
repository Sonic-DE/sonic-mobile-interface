# SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
# SPDX-License-Identifier: BSD-2-Clause

ecm_add_qml_module(waydroidintegrationplugin URI org.kde.plasma.private.mobileshell.waydroidintegrationplugin GENERATE_PLUGIN_SOURCE)
target_sources(waydroidintegrationplugin PRIVATE waydroidstate.cpp)

target_link_libraries(waydroidintegrationplugin PRIVATE
    Qt::Gui
    Qt::Qml
    Qt::Quick
    KF6::AuthCore
)

ecm_finalize_qml_module(waydroidintegrationplugin)

add_subdirectory(kauth)