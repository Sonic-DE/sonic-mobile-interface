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
import "../widgets" as Widgets
import "quicksettings"

/**
 * Root element that contains all of the ActionDrawer's contents, and is anchored to the screen.
 */
PlasmaCore.ColorScope {
    id: root
    
    required property var actionDrawer
    
    readonly property real minimizedQuickSettingsOffset: maximizedQuickSettingsOffset
    readonly property real maximizedQuickSettingsOffset: quickSettings.minimizedHeight + quickSettings.maxAddedHeight
    
    colorGroup: PlasmaCore.Theme.ViewColorGroup
    
    function applyMinMax(val) {
        return Math.max(0, Math.min(1, val));
    }
    
    // fullscreen background
    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(PlasmaCore.Theme.backgroundColor.r, PlasmaCore.Theme.backgroundColor.g, PlasmaCore.Theme.backgroundColor.b, 0.8)
        opacity: Math.max(0, Math.min(1, actionDrawer.offset / root.minimizedQuickSettingsOffset))
        Behavior on opacity { // smooth opacity changes
            NumberAnimation { duration: 70 }
        }
    }
    
    QuickSettingsContainer {
        id: quickSettings
        z: 1 // ensure it's above notifications
        height: Math.min(parent.height, implicitHeight)
        width: Math.min(parent.width * 0.5, intendedWidth)
        
        readonly property real intendedWidth: 360
        
        anchors.top: parent.top
        anchors.left: parent.left
        
        actionDrawer: root.actionDrawer
        
        minimizedToFullProgress: 1
        addedHeight: Math.max(0, Math.min(quickSettings.maxAddedHeight, root.actionDrawer.offset - quickSettings.minimizedHeight));
        
        transform: Translate {
            id: translate
            y: Math.min(root.actionDrawer.offset - quickSettings.minimizedHeight, 0)
        }
    }
    
    Widgets.NotificationsWidget {
        anchors {
            top: parent.top
            right: parent.left
            bottom: parent.bottom
            left: parent.left
        }
        opacity: applyMinMax(root.actionDrawer.offset / root.maximizedQuickSettingsOffset)
    }
}
