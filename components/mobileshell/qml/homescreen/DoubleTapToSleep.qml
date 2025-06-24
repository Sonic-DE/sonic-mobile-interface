// SPDX-FileCopyrightText: 2025 Florian RICHER <florian.richer@protonmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.plasma.private.mobileshell.shellsettingsplugin as ShellSettings
import org.kde.plasma.private.mobileshell.dpmsplugin as DPMS

Item {
    id: root
    enabled: ShellSettings.Settings.doubleTapToSleep

    property real doubleClickInterval: 400
    property int tapCount: 0

    function onDoubleTap() {
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

