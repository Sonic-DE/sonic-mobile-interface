/*
 *   SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15

import org.kde.plasma.core 2.0 as PlasmaCore

Rectangle {
    id: handle
    implicitWidth: PlasmaCore.Units.gridUnit * 3
    implicitHeight: 3
    radius: height
    color: PlasmaCore.Theme.textColor
    opacity: 0.5
}
