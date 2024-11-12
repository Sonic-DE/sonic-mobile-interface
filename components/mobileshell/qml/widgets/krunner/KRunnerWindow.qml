// SPDX-FileCopyrightText: 2024 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.1
import QtQuick.Window

import org.kde.plasma.private.mobileshell as MobileShell
import org.kde.layershell 1.0 as LayerShell

Window {
    id: root
    visible: false

    LayerShell.Window.scope: "overlay"
    LayerShell.Window.anchors: LayerShell.Window.AnchorTop | LayerShell.Window.AnchorLeft | LayerShell.Window.AnchorRight | LayerShell.Window.AnchorBottom
    LayerShell.Window.layer: LayerShell.Window.LayerOverlay
    LayerShell.Window.exclusionZone: -1

    color: Qt.rgba(0, 0, 0, 0.5)

    onVisibleChanged: {
        if (visible) {
            window.raise();
        }
    }

    MobileShell.KRunnerScreen {
        anchors.fill: parent

        onRequestedClose: root.visible = false
    }
}