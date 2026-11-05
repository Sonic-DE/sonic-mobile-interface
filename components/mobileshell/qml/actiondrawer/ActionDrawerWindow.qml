/*
 *   SPDX-FileCopyrightText: 2022 Devin Lin <devin@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.1
import QtQuick.Window

import org.kde.plasma.private.mobileshell as MobileShell
import org.kde.plasma.components 3.0 as PlasmaComponents

/**
 * Window with the ActionDrawer component embedded in it.
 *
 * Used for overlaying the ActionDrawer if the original window does not cover
 * the whole screen.
 */
MobileShell.OverlayWindow {
    id: window

    x: Screen.virtualX
    y: Screen.virtualY
    width: Screen.width
    height: Screen.height

    /**
     * The ActionDrawer component.
     */
    property alias actionDrawer: drawer
    property alias state: drawer.state

    visible: true

    color: "transparent"

    // Set input to transparent when closing to prevent window from taking unwanted touch inputs
    onStateChanged: MobileShell.ShellUtil.setInputTransparent(window, state === "close")

    // Preload and render content to avoid lag on first open
    Component.onCompleted: {
        preloadTimer.start();
    }

    Timer {
        id: preloadTimer
        interval: 1
        onTriggered: {
            // Set window visibility binding after preloading content to the action drawer state
            window.visible = Qt.binding(() => drawer.intendedToBeVisible);
        }
    }

    onVisibleChanged: {
        if (visible) {
            window.raise();
        }
    }

    onActiveChanged: {
        if (!active) {
            drawer.close();
        }
    }

    ActionDrawer {
        id: drawer
        anchors.fill: parent
    }
}
