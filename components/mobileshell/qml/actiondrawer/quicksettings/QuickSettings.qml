/*
 *   SPDX-FileCopyrightText: 2014 Marco Martin <notmart@gmail.com>
 *   SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import org.kde.plasma.core 2.0 as PlasmaCore

import "../../components" as Components

/**
 * Quick settings elements layout, change the height to clip.
 */
Item {
    id: root
    clip: true
    
    // readonly property real cellSizeHint: PlasmaCore.Units.iconSizes.large + PlasmaCore.Units.smallSpacing * 6
    readonly property real columns: 3 // Math.floor(width / cellSizeHint)
    readonly property real columnWidth: Math.floor(width / columns)
    readonly property real rowHeight: columnWidth * 0.7
    
    readonly property SettingsModel quickSettingsModel: SettingsModel {}
    
    ColumnLayout {
        spacing: 0
        implicitWidth: root.width
        
        // TODO add pages
        Flow {
            id: flow
            Layout.fillWidth: true
            
            spacing: 0
            
            Repeater {
                model: root.quickSettingsModel
                delegate: Components.BaseItem {
                    required property var modelData
                    
                    height: root.rowHeight
                    width: root.columnWidth
                    padding: PlasmaCore.Units.smallSpacing
                    
                    contentItem: QuickSettingsDelegate {
                        text: modelData.text
                        icon: modelData.icon
                        enabled: modelData.enabled
                        settingsCommand: modelData.settingsCommand
                        toggleFunction: modelData.toggle
                    }
                }
            }
        }
        
        BrightnessItem {
            Layout.leftMargin: PlasmaCore.Units.smallSpacing
            Layout.rightMargin: PlasmaCore.Units.smallSpacing
            Layout.fillWidth: true
        }
    }
}
