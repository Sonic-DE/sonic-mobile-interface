// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Effects

Item {
    id: root

    property alias source: effect.source

    MultiEffect {
        id: effect
        anchors.fill: parent

    }

    layer.enabled: true
    layer.effect: DelegateShadow {}
}
