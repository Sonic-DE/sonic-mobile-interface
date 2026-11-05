/*
 *  SPDX-FileCopyrightText: 2025 Micah Stanley <stanleymicah@proton.me>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import QtQuick.Effects

import org.kde.kirigami as Kirigami
import org.kde.plasma.private.mobileshell.state as MobileShellState
import org.kde.plasma.private.mobileshell as MobileShell

MobileShell.OverlayWindow {
    id: root

    type: MobileShell.OverlayWindow.Notification

    readonly property int size: Kirigami.Units.gridUnit * 2
    readonly property int margins: Math.round(Kirigami.Units.largeSpace * 0.5)

    property int screenCorner: ActionButton.ScreenCorner.BottomRight
    property int angle: 0
    property string iconSource
    property bool active: false

    signal triggered()

    enum ScreenCorner {
        BottomRight,
        BottomLeft,
        TopLeft,
        TopRight
    }

    // When the button is animating its disappearance, make sure it is transparent to inputs.
    onActiveChanged: {
        ShellUtil.setInputTransparent(root, !active)
        if (active) {
            root.visible = true;
            root.raise();
            hideButton.stop();
            return;
        }
        hideButton.restart();
    }

    x: {
        if (screenCorner === ActionButton.ScreenCorner.TopLeft || screenCorner === ActionButton.ScreenCorner.BottomLeft) {
            return Screen.virtualX + margins;
        }
        return Screen.virtualX + Screen.width - width - margins;
    }
    y: {
        if (screenCorner === ActionButton.ScreenCorner.TopLeft || screenCorner === ActionButton.ScreenCorner.TopRight) {
            return Screen.virtualY + margins;
        }
        return Screen.virtualY + Screen.height - height - margins;
    }

    Kirigami.Theme.colorSet: Kirigami.Theme.View
    Kirigami.Theme.inherit: false

    // Double the set button size to leave room for button scale animation.
    width: size * 2
    height: size * 2

    visible: active

    color: "transparent"

    // Hide the root window after the button disappearing animation finishes.
    Timer {
        id: hideButton
        interval: Kirigami.Units.longDuration
        repeat: false
        onTriggered: if (!active) root.visible = false;
    }

    Component.onCompleted: {
        // Because the window surface area had to be made larger to accommodate the button scale animation,
        // set the input region to the size of the actual button.
        ShellUtil.setInputRegion(root, Qt.rect((root.width - size) / 2, (root.height - size) / 2, size, size));
        ShellUtil.setInputTransparent(root, !active);
    }

    Controls.Control {
        id: content
        anchors.centerIn: parent
        width: root.size
        height: root.size
        opacity: root.active ? 1 : 0

        property double scale: !root.active ? 0.5 : (button.pressed ? 1.5 : 1)

        Behavior on scale {
            NumberAnimation {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.OutBack
            }
        }

        Behavior on opacity {
            NumberAnimation {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.OutCirc
            }
        }

        transform: Scale {
            origin.x: root.size / 2
            origin.y: root.size / 2
            xScale: content.scale
            yScale: content.scale
        }

        MobileShell.PanelBackground {
            anchors.fill: parent
            panelType: MobileShell.PanelBackground.PanelType.Popup
            radius: root.size
        }

        Controls.AbstractButton {
            id: button
            anchors.fill: parent

            MobileShell.HapticsEffect {
                id: haptics
            }

            contentItem: Item {
                Kirigami.Icon {
                    anchors.centerIn: parent
                    width: Kirigami.Units.iconSizes.small
                    height: Kirigami.Units.iconSizes.small
                    transformOrigin: Item.Center
                    rotation: root.angle
                    source: root.iconSource
                }
            }

            onPressed: {
                haptics.buttonVibrate();
            }

            onReleased: {
                if (active) root.triggered();
            }
        }
    }
}
