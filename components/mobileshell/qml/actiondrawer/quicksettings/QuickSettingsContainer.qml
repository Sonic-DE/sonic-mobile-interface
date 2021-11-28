/*
 *   SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.14
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.12

import org.kde.kirigami 2.12 as Kirigami

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.private.mobileshell 1.0 as MobileShell

import "../../statusbar" as StatusBar
import "../../components" as Components

/**
 * Quick settings panel for phones.
 */
Components.BaseItem {
    id: root
    
    /**
     * The amount of height to add to the panel (increasing the height of the quick settings area).
     */
    property real addedHeight: 0
    
    /**
     * The maximum amount of added height to snap to the full height of the quick settings panel.
     */
    readonly property real maxAddedHeight: quickSettings.rowHeight * 2 // TODO don't hardcode this to 3 rows
    
    // TODO implement
    signal expandRequested
    signal closeRequested

    leftPadding: PlasmaCore.Units.largeSpacing
    rightPadding: PlasmaCore.Units.largeSpacing
    
    background: PlasmaCore.FrameSvgItem {
        enabledBorders: PlasmaCore.FrameSvg.BottomBorder
        imagePath: "widgets/background"
    }

    contentItem: ColumnLayout {
        spacing: PlasmaCore.Units.smallSpacing
        
        StatusBar.StatusBar {
            Layout.fillWidth: true
            Layout.preferredHeight: MobileShell.TopPanelControls.panelHeight
            
            colorGroup: PlasmaCore.Theme.NormalColorGroup
            backgroundColor: "transparent"
            showSecondRow: true
            showDropShadow: false
        }
        
        QuickSettings {
            id: quickSettings
            Layout.fillWidth: true
            Layout.preferredHeight: quickSettings.rowHeight
        }
        
        MediaPlayerWidget {
            Layout.fillHeight: true
        }
    }
}
