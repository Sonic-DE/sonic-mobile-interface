// SPDX-FileCopyrightText: 2024 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick
import org.kde.plasma.private.mobileshell.state as MobileShellState

pragma Singleton

Item {
    Repeater {
        model: MobileShellState.ShellDBusObject.startupFeedbackModel

        delegate: Window {
            id: window
            visbility: Window.FullScreen
            flags: Qt.FramelessWindowHint

            property var startupFeedback: model.delegate

            Item {
                id: backgroundParent
                width: root.width
                height: root.height

                Rectangle {
                    id: background
                    anchors.fill: parent

                    color: Kirigami.Theme.backgroundColor
                }

                Item {
                    id: iconParent
                    anchors.centerIn: background
                    width: Kirigami.Units.iconSizes.enormous
                    height: width

                    Kirigami.Icon {
                        id: icon
                        anchors.fill: parent
                        source: startupFeedback.iconName
                    }

                    MultiEffect {
                        anchors.fill: icon
                        source: icon
                        shadowEnabled: true
                        blurMax: 16
                        shadowColor: "#80000000"
                    }
                }
            }
        }
    }
}