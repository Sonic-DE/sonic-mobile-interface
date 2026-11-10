# SPDX-FileCopyrightText: 2024 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: GPL-2.0-or-later

add_subdirectory(notificationtest)

find_package(Qt6 REQUIRED COMPONENTS QuickTest Test)

add_executable(maskprovidertest maskprovidertest.cpp)
target_link_libraries(maskprovidertest PRIVATE Qt6::QuickTest Qt6::Test)
add_test(NAME maskprovidertest COMMAND maskprovidertest -input ${CMAKE_CURRENT_SOURCE_DIR}/maskproviders)
set_tests_properties(maskprovidertest PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QSG_RHI_BACKEND=opengl")

add_executable(folioappdrawertest folioappdrawertest.cpp)
target_link_libraries(folioappdrawertest PRIVATE Qt6::QuickTest Qt6::Test Plasma::Plasma KF6::Service KF6::ConfigCore KF6::I18nQml)
target_compile_definitions(folioappdrawertest PRIVATE FOLIO_PLUGIN_PATH="$<TARGET_FILE:org.kde.plasma.mobile.homescreen.folio>")
add_dependencies(folioappdrawertest org.kde.plasma.mobile.homescreen.folio)
add_test(NAME folioappdrawertest COMMAND folioappdrawertest -input ${CMAKE_CURRENT_SOURCE_DIR}/folio)
set_tests_properties(folioappdrawertest PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=xcb;QSG_RHI_BACKEND=opengl")

add_executable(marqueelabeltest marqueelabeltest.cpp)
target_link_libraries(marqueelabeltest PRIVATE Qt6::QuickTest Qt6::Test)
add_test(NAME marqueelabeltest COMMAND marqueelabeltest -input ${CMAKE_CURRENT_SOURCE_DIR}/marqueelabel)
set_tests_properties(marqueelabeltest PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=xcb;QSG_RHI_BACKEND=opengl")

add_executable(qmlimportstest qmlimportstest.cpp)
target_link_libraries(qmlimportstest PRIVATE Qt6::Test)
target_compile_definitions(qmlimportstest PRIVATE SOURCE_ROOT="${CMAKE_SOURCE_DIR}")
add_test(NAME qmlimportstest COMMAND qmlimportstest)

add_executable(overlaywindowtest
    overlaywindowtest.cpp
    ${CMAKE_SOURCE_DIR}/components/mobileshell/overlaywindow.cpp
    ${CMAKE_SOURCE_DIR}/components/mobileshell/shellutil.cpp
)
target_include_directories(overlaywindowtest PRIVATE ${CMAKE_SOURCE_DIR}/components/mobileshell)
target_link_libraries(overlaywindowtest PRIVATE
    Qt6::Test
    Qt6::DBus
    Qt6::Gui
    Qt6::Quick
    KF6::ConfigCore
    KF6::CoreAddons
    KF6::I18n
    KF6::KIOGui
    KF6::KIOWidgets # KNotificationJobUiDelegate
    KF6::Notifications
    KF6::Service
    KF6::WindowSystem
    XCB::XCB
    XCB::SHAPE
)
add_test(NAME overlaywindowtest COMMAND overlaywindowtest)
set_tests_properties(overlaywindowtest PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=xcb")

find_program(XVFB_EXECUTABLE Xvfb REQUIRED)
add_executable(screenrecordertest screenrecordertest.cpp)
target_link_libraries(screenrecordertest PRIVATE Qt6::Test Qt6::Multimedia sonicmobilescreenrecorder Sonic::Logging)
target_compile_definitions(screenrecordertest PRIVATE XVFB_PATH="${XVFB_EXECUTABLE}")
add_test(NAME screenrecordertest COMMAND screenrecordertest)
set_tests_properties(screenrecordertest PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen" TIMEOUT 60)
