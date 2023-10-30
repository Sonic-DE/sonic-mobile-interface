// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick

import '../delegate'

MouseArea {
    id: root
    height: 20
    width: 20

    cursorShape: Qt.PointingHandCursor

    Rectangle {
        anchors.fill: parent
        color: 'white'
        radius: width / 2

        transform: Scale {
            property real scaleFactor: root.pressed ? 1.2 : 1.0

            Behavior on scaleFactor {
                NumberAnimation { duration: 400; easing.type: Easing.OutExpo }
            }

            xScale: scaleFactor
            yScale: scaleFactor
            origin.x: root.width / 2
            origin.y: root.height / 2
        }
    }

    layer.enabled: true
    layer.effect: DelegateShadow {}
}
