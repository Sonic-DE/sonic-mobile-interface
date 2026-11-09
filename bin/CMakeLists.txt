# SPDX-FileCopyrightText: 2019 Aleix Pol <apol@kde.org>
# SPDX-FileCopyrightText: 2020 Jonah Brüchert <jbb@kaidan.im>
# SPDX-License-Identifier: GPL-2.0-or-later

configure_file(startplasmamobile.in ${CMAKE_CURRENT_BINARY_DIR}/startplasmamobile)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/startplasmamobile DESTINATION ${KDE_INSTALL_BINDIR})

configure_file(sonicde-mobile.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/sonicde-mobile.desktop)
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/sonicde-mobile.desktop
        DESTINATION ${KDE_INSTALL_DATADIR}/xsessions
)

# For Plasma developers
configure_file(startplasmamobile-dev.sh.cmake ${CMAKE_CURRENT_BINARY_DIR}/startplasmamobile-dev.sh)
configure_file(sonicde-mobile-dev.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/sonicde-mobile-dev.desktop)
configure_file(install-sessions.sh.cmake ${CMAKE_CURRENT_BINARY_DIR}/install-sessions.sh)
