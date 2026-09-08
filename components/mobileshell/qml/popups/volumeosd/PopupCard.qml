/*
 *  SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 *  SPDX-FileCopyrightText: 2024 Micah Stanley <stanleymicah@proton.me>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Window

import org.kde.kirigami as Kirigami
import org.kde.plasma.private.mobileshell as MobileShell

Controls.Control {
    id: content

    property real scaleFactor: 1.0

    implicitWidth: Math.min(Kirigami.Units.gridUnit * 20, Screen.width - Kirigami.Units.gridUnit * 2)
    padding: Kirigami.Units.largeSpacing

    transform: Scale {
        origin.x: Math.round(content.implicitWidth / 2)
        origin.y: Math.round(content.height / 2)
        xScale: content.scaleFactor
        yScale: content.scaleFactor
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    MobileShell.PanelBackground {
        anchors.fill: parent
        panelType: MobileShell.PanelBackground.PanelType.Popup
    }
}
