/*
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

QuickSettingsDelegate {
    id: root

    readonly property color disabledButtonColor: PlasmaCore.Theme.backgroundColor
    readonly property color disabledPressedButtonColor: Qt.darker(disabledButtonColor, 1.1)
    readonly property color enabledButtonColor: Kirigami.ColorUtils.adjustColor(PlasmaCore.ColorScope.highlightColor, {"alpha": 0.4*255})
    readonly property color enabledPressedButtonColor: Kirigami.ColorUtils.adjustColor(PlasmaCore.ColorScope.highlightColor, {"alpha": 0.6*255});
    
    iconItem: icon
    
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
        onClicked: root.delegateClick()
        onPressAndHold: root.delegatePressAndHold()
        
        PlasmaCore.IconItem {
            id: icon
            anchors.centerIn: parent
            implicitWidth: PlasmaCore.Units.iconSizes.smallMedium
            implicitHeight: width
            source: root.icon
        }
    }
}

