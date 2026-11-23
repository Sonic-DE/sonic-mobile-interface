# SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDE-Accepted-GPL

# Needs ModemManager, which FreeBSD doesn't have.
if(NOT CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    add_subdirectory(autodetectapn)
endif()
add_subdirectory(start)
