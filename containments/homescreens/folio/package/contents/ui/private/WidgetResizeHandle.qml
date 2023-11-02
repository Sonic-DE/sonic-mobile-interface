// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick

import '../delegate'

MouseArea {
    id: root
    height: 20
    width: 20

    cursorShape: Qt.PointingHandCursor

    property int orientation

    signal dragEvent(real leftEdgeDelta, real rightEdgeDelta, real topEdgeDelta, real bottomEdgeDelta)

    drag {
        target: root
        axis: {
            switch (orientation) {
                case WidgetHandlePosition.TopLeft:
                    return Drag.XAndYAxis;
                case WidgetHandlePosition.TopCenter:
                    return Drag.YAxis;
                case WidgetHandlePosition.TopRight:
                    return Drag.XAndYAxis;
                case WidgetHandlePosition.LeftCenter:
                    return Drag.XAxis;
                case WidgetHandlePosition.RightCenter:
                    return Drag.XAxis;
                case WidgetHandlePosition.BottomLeft:
                    return Drag.XAndYAxis;
                case WidgetHandlePosition.BottomCenter:
                    return Drag.YAxis;
                case WidgetHandlePosition.BottomRight:
                    return Drag.XAndYAxis;
            }
            return Drag.XAndYAxis;
        }
    }

    property real pressX
    property real pressY

    onPressed: {
        pressX = mouseX;
        pressY = mouseY;
    }

    onPositionChanged: {
        updateDrag();
        updateDrag();
    }

    drag { target: root; axis: Drag.XAndYAxis }

    function updateDrag() {
        if (!drag.active) return;

        const dx = mouseX;
        const dy = mouseY;

        switch (orientation) {
            case WidgetHandlePosition.TopLeft:
                root.dragEvent(-dx, 0, -dy, 0);
                break;
            case WidgetHandlePosition.TopCenter:
                root.dragEvent(0, 0, -dy, 0);
                break;
            case WidgetHandlePosition.TopRight:
                root.dragEvent(0, dx, -dy, 0);
                break;
            case WidgetHandlePosition.LeftCenter:
                root.dragEvent(-dx, 0, 0, 0);
                break;
            case WidgetHandlePosition.RightCenter:
                root.dragEvent(0, dx, 0, 0);
                break;
            case WidgetHandlePosition.BottomLeft:
                root.dragEvent(-dx, 0, 0, dy);
                break;
            case WidgetHandlePosition.BottomCenter:
                root.dragEvent(0, 0, 0, dy);
                break;
            case WidgetHandlePosition.BottomRight:
                root.dragEvent(0, dx, 0, dy);
                break;
        }
    }

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
