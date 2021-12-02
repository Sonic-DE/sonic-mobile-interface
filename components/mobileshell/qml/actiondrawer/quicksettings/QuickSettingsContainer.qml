/*
 *   SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.12

import org.kde.kirigami 2.12 as Kirigami

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.private.mobileshell 1.0 as MobileShell

import "../../statusbar" as StatusBar
import "../../components" as Components
import "../../widgets" as Widgets

/**
 * Quick settings panel for phones.
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
     * Progress of showing the pinned quick settings view.
     */
    property real minimizedViewProgress: 0
    
    /**
     * Progress of showing the full quick settings view (when maximized).
     */
    property real fullViewProgress: 1

    topPadding: PlasmaCore.Units.smallSpacing
    leftPadding: PlasmaCore.Units.smallSpacing
    rightPadding: PlasmaCore.Units.smallSpacing
    bottomPadding: PlasmaCore.Units.smallSpacing * 4
    
    background: PlasmaCore.FrameSvgItem {
        enabledBorders: PlasmaCore.FrameSvg.BottomBorder
        imagePath: "widgets/background"
    }

    contentItem: ColumnLayout {
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
            actionDrawer: root.actionDrawer
            minimizedViewProgress: root.minimizedViewProgress
            fullViewProgress: root.fullViewProgress
            
            readonly property real minimizedHeight: rowHeight * 2 // minimized height of quick settings area
            Layout.topMargin: PlasmaCore.Units.smallSpacing
            Layout.fillWidth: true
            Layout.preferredHeight: quickSettings.rowHeight + root.addedHeight
        }
        
        Widgets.MediaPlayerWidget {
            id: mediaWidget
            property real fullHeight: height + Layout.topMargin
            Layout.topMargin: visible ? PlasmaCore.Units.smallSpacing : 0
            Layout.fillWidth: true
        }
        
        Rectangle {
            id: handle
            property real fullHeight: height + Layout.topMargin
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: PlasmaCore.Units.smallSpacing
            Layout.preferredWidth: PlasmaCore.Units.gridUnit * 3
            Layout.preferredHeight: 3
            radius: height
            color: PlasmaCore.Theme.textColor
            opacity: 0.5
        }
    }
}
