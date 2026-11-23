# SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: GPL-2.0-or-later

plasma_install_package(finished org.kde.plasma.mobileinitialstart.finished mobileinitialstart)
plasma_install_package(systemnavigation org.kde.plasma.mobileinitialstart.systemnavigation mobileinitialstart)
# Needs ModemManager and sonic-network-manager's cellular module, which FreeBSD doesn't have.
if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    set_property(GLOBAL APPEND PROPERTY MOBILE_UNBUILT_QML initialstart/modules/cellular)
else()
    add_subdirectory(cellular)
endif()
add_subdirectory(prepare)
add_subdirectory(time)
add_subdirectory(wifi)
