# SPDX-FileCopyrightText: 2024 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: GPL-2.0-or-later

add_subdirectory(notificationtest)

find_package(Qt6 REQUIRED COMPONENTS QuickTest Test)

add_executable(maskprovidertest maskprovidertest.cpp)
target_link_libraries(maskprovidertest PRIVATE Qt6::QuickTest Qt6::Test)
add_test(NAME maskprovidertest COMMAND maskprovidertest -input ${CMAKE_CURRENT_SOURCE_DIR}/maskproviders)
set_tests_properties(maskprovidertest PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QSG_RHI_BACKEND=opengl")
