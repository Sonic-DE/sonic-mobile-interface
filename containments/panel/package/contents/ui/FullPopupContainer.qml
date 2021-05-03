/*
 *  SPDX-FileCopyrightText: 2019 Marco Martin <mart@kde.org>
 *  SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.12

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtra
import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtra
import org.kde.plasma.private.nanoshell 2.0 as NanoShell

NanoShell.FullScreenOverlay {
    id: window
    property Item applet
    property alias contents: appletItem
    
    color: Qt.rgba(0, 0, 0, 0.25)
    width: Screen.width
    height: Screen.height
    
    function showOverlay() {
        window.visible = true;
        scaleAnimation.to = 1;
        scaleAnimation.restart();
    }
    
    function closeOverlay() {
        scaleAnimation.to = 0.5;
        scaleAnimation.restart();
        window.close();
    }
    
    MouseArea {
        anchors.fill: parent
        onClicked: window.closeOverlay()
        
        scale: 0.5
        ScaleAnimator on scale {
            id: scaleAnimation
            easing.type: Easing.InOutQuad
            duration: PlasmaCore.Units.shortDuration
            onFinished: {
                if (parent.opacity === 0) {
                    window.close();
                }
            }
        }
        
        RectangularGlow {
            anchors.topMargin: 1
            anchors.fill: content
            cached: true
            glowRadius: 4
            spread: 0.2
            color: Qt.rgba(0, 0, 0, 0.15)
        }
        
        MouseArea {
            id: content
            anchors.centerIn: parent
            implicitWidth: Math.min(PlasmaCore.Units.gridUnit * 20, parent.width - PlasmaCore.Units.largeSpacing * 2)
            implicitHeight: PlasmaCore.Units.gridUnit * 20
            
            Rectangle {
                anchors.fill: parent
                radius: PlasmaCore.Units.smallSpacing
                color: PlasmaCore.Theme.backgroundColor
                
                ColumnLayout {
                    id: containerLayout
                    spacing: PlasmaCore.Units.smallSpacing
                    
                    anchors.fill: parent
//                     anchors.margins: PlasmaCore.Units.smallSpacing
                    
                    Controls.Control { // parent of applet
                        id: appletItem
                        Layout.fillHeight: true
//                         Layout.preferredHeight: PlasmaCore.Units.gridUnit * 20
                        Layout.fillWidth: true
                        
                    }
                }
            }
        }
    }
}

