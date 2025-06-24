// SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick

import org.kde.plasma.private.mobileshell.dpmsplugin as DPMS
import org.kde.plasma.private.mobileshell.state as MobileShellState

Item {
    id: root

    property real doubleClickInterval: 400
    property int tapCount: 0

    function onDoubleTap() {
        MobileShellState.LockscreenDBusClient.lockScreen()
        dpms.turnDpmsOff()
    }

    DPMS.DPMSUtil {
        id: dpms
    }

    // Workaround for double tap detection without capture events for HomeScreen
    Timer {
        id: doubleClickTimer
        interval: root.doubleClickInterval
        onTriggered: {
            root.tapCount = 0
        }
    }

    MouseArea {
        anchors.fill: parent
        propagateComposedEvents: true

        onPressed: {
            if (++root.tapCount === 2) {
                onDoubleTap()
            }
            doubleClickTimer.start()
            mouse.accepted = false
        }
    }
}

