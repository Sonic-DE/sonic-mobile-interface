# SPDX-FileCopyrightText: 2025 Micah Stanley <stanleymicah@proton.me>
# SPDX-License-Identifier: BSD-2-Clause

ecm_add_qml_module(masklayerplugin URI org.kde.plasma.private.mobileshell.masklayerplugin GENERATE_PLUGIN_SOURCE)

target_sources(masklayerplugin PRIVATE
    masklayer.cpp
    maskmanager.cpp
)

target_link_libraries(masklayerplugin PRIVATE Plasma::PlasmaQuick)

ecm_finalize_qml_module(masklayerplugin)
