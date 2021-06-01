/*
 *  SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2

import org.kde.plasma.core 2.1 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

// capture presses on the audio applet so it doesn't close the overlay
MouseArea {
    id: content
    implicitWidth: Math.min(PlasmaCore.Units.gridUnit * 20, parent.width - PlasmaCore.Units.largeSpacing * 2)
    implicitHeight: control.implicitHeight
    
    property alias childItem: control.contentItem
    
    PlasmaCore.FrameSvgItem {
        imagePath: "widgets/background"
        anchors.margins: -PlasmaCore.Units.smallSpacing * 2
        anchors.fill: parent
    }
    
    Controls.Control {
        id: control
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        topPadding: PlasmaCore.Units.smallSpacing * 2
        bottomPadding: PlasmaCore.Units.smallSpacing * 2
        leftPadding: PlasmaCore.Units.smallSpacing * 2
        rightPadding: PlasmaCore.Units.smallSpacing * 2
    }
}
