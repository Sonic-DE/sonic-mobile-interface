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
