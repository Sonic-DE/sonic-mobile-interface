// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Effects

import org.kde.kirigami as Kirigami

pragma ComponentBehavior: Bound

MultiEffect {
    id: root

    property var flickable

    maskEnabled: true
    source: flickable
    maskSource: Rectangle {
        id: mask
        width: root.flickable.width
        height: root.flickable.height
        layer.enabled: true

        property real gradientPct: (Kirigami.Units.gridUnit * 2) / root.flickable.height

        gradient: Gradient {
            GradientStop { position: 0.0; color: root.flickable.atYBeginning ? "white" : "transparent" }
            GradientStop { position: mask.gradientPct; color: "white" }
            GradientStop { position: 1.0 - mask.gradientPct; color: "white" }
            GradientStop { position: 1.0; color: root.flickable.atYEnd ? "white" : "transparent" }
        }
    }
}
