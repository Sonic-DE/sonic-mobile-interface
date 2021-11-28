/*
 *   SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

import "../components" as Components
import "quicksettings"

/**
 * Root element that contains all of the ActionDrawer's contents, and is anchored to the screen.
 */
PlasmaCore.ColorScope {
    id: root
    
    required property var actionDrawer
    
    readonly property real minimizedQuickSettingsOffset: quickSettings.height
    readonly property real maximizedQuickSettingsOffset: quickSettings.maxAddedHeight
    
    colorGroup: PlasmaCore.Theme.ViewColorGroup
    
    // fullscreen background
    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.75)
        opacity: actionDrawer.offset / maximizedQuickSettingsOffset
        Behavior on opacity { // smooth opacity changes
            NumberAnimation { duration: 70 }
        }
    }
    
    QuickSettingsContainer {
        id: quickSettings
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        
        transform: Translate {
            y: root.actionDrawer.offset
        }
    }
}
