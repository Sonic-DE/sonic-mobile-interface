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
    
    readonly property real minimizedQuickSettingsOffset: quickSettings.minimizedHeight
    readonly property real maximizedQuickSettingsOffset: minimizedQuickSettingsOffset + quickSettings.maxAddedHeight
    
    colorGroup: PlasmaCore.Theme.ViewColorGroup
    
    // fullscreen background
    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(PlasmaCore.Theme.backgroundColor.r, PlasmaCore.Theme.backgroundColor.g, PlasmaCore.Theme.backgroundColor.b, 0.75)
        opacity: Math.max(0, Math.min(1, actionDrawer.offset / root.minimizedQuickSettingsOffset))
        Behavior on opacity { // smooth opacity changes
            NumberAnimation { duration: 70 }
        }
    }
    
    QuickSettingsContainer {
        id: quickSettings
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        
        function applyMinMax(val) {
            return Math.max(0, Math.min(1, val));
        }
        
        // opacity and move animation
        property real dist: (maximizedQuickSettingsOffset - minimizedQuickSettingsOffset)
        minimizedViewProgress: actionDrawer.opened ? applyMinMax(1 - (actionDrawer.offset - minimizedQuickSettingsOffset) / dist) : 1
        fullViewProgress: actionDrawer.opened ? applyMinMax((actionDrawer.offset - minimizedQuickSettingsOffset) / dist) : 0
        
        addedHeight: {
            if (!actionDrawer.opened) {
                // over-scroll effect for initial opening
                let progress = (root.actionDrawer.offset - minimizedQuickSettingsOffset) / quickSettings.maxAddedHeight;
                let effectProgress = Math.atan(Math.max(0, progress));
                return quickSettings.maxAddedHeight * 0.25 * effectProgress;
            } else {
                return Math.max(0, Math.min(quickSettings.maxAddedHeight, root.actionDrawer.offset - minimizedQuickSettingsOffset));
            }
        }
        
        transform: Translate {
            y: Math.min(root.actionDrawer.offset - minimizedQuickSettingsOffset, 0)
        }
    }
}
