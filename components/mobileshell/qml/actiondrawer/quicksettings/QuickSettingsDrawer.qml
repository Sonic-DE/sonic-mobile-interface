/*
 *   SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.12

import org.kde.kirigami 2.12 as Kirigami

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.private.mobileshell 1.0 as MobileShell

import "../../statusbar" as StatusBar
import "../../components" as Components
import "../../widgets" as Widgets
import "../"

/**
 * Quick settings drawer pulled down from the top (for portrait mode).
 * For the landscape view quicksettings container, see QuickSettingsPanel.
 */
Components.BaseItem {
    id: root
    
    required property var actionDrawer
    
    /**
     * The amount of height to add to the panel (increasing the height of the quick settings area).
     */
    property real addedHeight: 0
    
    /**
     * The maximum amount of added height to snap to the full height of the quick settings panel.
     */
    readonly property real maxAddedHeight: quickSettings.fullHeight - quickSettings.rowHeight // first row is part of minimized height
    
    /**
     * Height of panel when in minimized mode.
     */
    readonly property real minimizedHeight: bottomPadding + topPadding + statusBar.height + quickSettings.rowHeight + handle.fullHeight
    
    /**
     * Progress of showing the full quick settings view from pinned.
     */
    property real minimizedToFullProgress: 1
    
    // we need extra padding if the background side border is enabled
    topPadding: PlasmaCore.Units.smallSpacing 
    leftPadding: PlasmaCore.Units.smallSpacing 
    rightPadding: PlasmaCore.Units.smallSpacing
    bottomPadding: PlasmaCore.Units.smallSpacing * 4
    
    background: PlasmaCore.FrameSvgItem {
        enabledBorders: PlasmaCore.FrameSvg.BottomBorder
        imagePath: "widgets/background"
    }

    contentItem: Item {
        id: containerItem
        implicitHeight: column.implicitHeight
        
        // use container item so that our column doesn't get stretched if base item is anchored
        ColumnLayout {
            id: column
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            spacing: 0
            
            StatusBar.StatusBar {
                id: statusBar
                Layout.fillWidth: true
                Layout.preferredHeight: MobileShell.TopPanelControls.panelHeight + PlasmaCore.Units.gridUnit * 0.8
                
                colorGroup: PlasmaCore.Theme.NormalColorGroup
                backgroundColor: "transparent"
                showSecondRow: true
                showDropShadow: false
            }
            
            QuickSettings {
                id: quickSettings
                Layout.preferredHeight: quickSettings.rowHeight + root.addedHeight
                Layout.topMargin: PlasmaCore.Units.smallSpacing
                Layout.fillWidth: true
                
                actionDrawer: root.actionDrawer
                minimizedViewProgress: 1 - root.minimizedToFullProgress
                fullViewProgress: root.minimizedToFullProgress
                height: quickSettings.rowHeight + root.addedHeight
                width: parent.width
            }
            
            Widgets.MediaPlayerWidget {
                id: mediaWidget
                property real fullHeight: height + Layout.topMargin
                Layout.topMargin: visible ? PlasmaCore.Units.smallSpacing : 0
                Layout.fillWidth: true
            }
            
            Handle {
                id: handle
                property real fullHeight: root.actionDrawer.mode === ActionDrawer.Portrait ? height + Layout.topMargin : 0
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: PlasmaCore.Units.smallSpacing
            }
        }
    }
}
