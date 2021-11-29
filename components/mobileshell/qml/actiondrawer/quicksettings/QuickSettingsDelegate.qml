/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <notmart@gmail.com>
 *   SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.12 as Kirigami

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.private.nanoshell 2.0 as NanoShell
import org.kde.plasma.components 3.0 as PlasmaComponents

import "../../components" as Components

Components.BaseItem {
    id: root
    
    // Model interface
    required property string text
    required property string icon
    required property bool enabled
    required property string settingsCommand
    required property var toggleFunction
    
    
    readonly property color disabledButtonColor: PlasmaCore.Theme.backgroundColor
    readonly property color disabledPressedButtonColor: Qt.darker(disabledButtonColor, 1.1)
    readonly property color enabledButtonColor: Kirigami.ColorUtils.adjustColor(PlasmaCore.ColorScope.highlightColor, {"alpha": 0.4*255})
    readonly property color enabledPressedButtonColor: Kirigami.ColorUtils.adjustColor(PlasmaCore.ColorScope.highlightColor, {"alpha": 0.6*255});
    
    padding: PlasmaCore.Units.smallSpacing * 2
    
    background: Rectangle {
        radius: PlasmaCore.Units.smallSpacing
        border.color: root.enabled ?
            Qt.darker(Kirigami.ColorUtils.adjustColor(PlasmaCore.ColorScope.highlightColor, {}), 1.25) :
            Kirigami.ColorUtils.adjustColor(PlasmaCore.ColorScope.textColor, {"alpha": 0.2*255})
        color: {
            if (root.enabled) {
                return mouseArea.pressed ? enabledPressedButtonColor : enabledButtonColor
            } else {
                return mouseArea.pressed ? disabledPressedButtonColor : disabledButtonColor
            }
        }
    }
    
    contentItem: MouseArea {
        id: mouseArea
        onClicked: {
            if (root.toggle) {
                root.toggle();
            } else if (root.toggleFunction) {
                root.toggleFunction();
            } else if (root.settingsCommand) {
                NanoShell.StartupFeedback.open(
                    root.icon,
                    root.text,
                    icon.Kirigami.ScenePosition.x + icon.width/2,
                    icon.Kirigami.ScenePosition.y + icon.height/2,
                    Math.min(icon.width, icon.height))
                plasmoid.nativeInterface.executeCommand(root.settingsCommand);
                root.closeRequested();
            }
        }
        
        onPressAndHold: {
            if (root.settingsCommand) {
                NanoShell.StartupFeedback.open(
                    root.icon,
                    root.text,
                    icon.Kirigami.ScenePosition.x + icon.width/2,
                    icon.Kirigami.ScenePosition.y + icon.height/2,
                    Math.min(icon.width, icon.height))
                closeRequested();
                plasmoid.nativeInterface.executeCommand(root.settingsCommand);
            } else if (root.toggleFunction) {
                root.toggleFunction();
            }
        }
        
        PlasmaCore.IconItem {
            id: icon
            anchors.top: parent.top
            anchors.left: parent.left
            implicitWidth: PlasmaCore.Units.iconSizes.small
            implicitHeight: width
            source: root.icon
        }
        
        ColumnLayout {
            id: column
            spacing: PlasmaCore.Units.smallSpacing
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            
            PlasmaComponents.Label {
                Layout.fillWidth: true
                elide: Text.ElideRight
                text: root.text
                font.pixelSize: PlasmaCore.Theme.defaultFont.pixelSize * 0.8 // TODO base height off of size of delegate
            }
            PlasmaComponents.Label {
                Layout.fillWidth: true
                elide: Text.ElideRight
                text: root.enabled ? i18n("On") : i18n("Off") // TODO implement descriptive text
                opacity: 0.6
                font.pixelSize: PlasmaCore.Theme.defaultFont.pixelSize * 0.8
            }
        }
    }
}
