# SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: GPL-2.0-or-later

set(plasma-mobile-initial-start_SRCS
    main.cpp
    wizard.cpp
    utils.h
)

qt_add_resources(RESOURCES resources.qrc)

add_subdirectory(modules)

add_executable(plasma-mobile-initial-start ${plasma-mobile-initial-start_SRCS} ${RESOURCES})
target_link_libraries(plasma-mobile-initial-start
    Qt::Qml
    Qt::Gui
    Qt::Widgets
    Qt::Quick
    KF5::I18n
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::CoreAddons
    KF5::DBusAddons
    KF5::Package
)

target_include_directories(plasma-mobile-initial-start PRIVATE ${CMAKE_BINARY_DIR})
install(TARGETS plasma-mobile-initial-start ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
