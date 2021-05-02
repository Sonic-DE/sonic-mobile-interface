/*
 *  SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtra
import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtra
import org.kde.plasma.private.nanoshell 2.0 as NanoShell

NanoShell.FullScreenOverlay {
    id: window
    
    color: "transparent"
    width: Screen.width
    height: Screen.height
    
    property int volume: 0
    property int maxVolume: 0
    
    function showOverlay() {
        window.visible = true;
        hideTimer.restart();
    }
    
    Component.onCompleted: showOverlay()
    
    Timer {
        id: hideTimer
        interval: 3000
        onTriggered: {
            window.visible = false;
        }
    }
    
    MouseArea {
        anchors.fill: parent
        onClicked: {
            hideTimer.stop();
            hideTimer.triggered();
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: PlasmaCore.Units.largeSpacing
        
        PlasmaCore.FrameSvgItem {
            Layout.preferredWidth: parent.width - PlasmaCore.Units.largeSpacing * 2
            Layout.maximumWidth: PlasmaCore.Units.gridUnit * 20
            Layout.preferredHeight: containerLayout.implicitHeight + PlasmaCore.Units.largeSpacing * 2
            Layout.alignment: Qt.AlignHCenter
            imagePath: "widgets/background"
            
            RowLayout {
                id: containerLayout
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: PlasmaCore.Units.largeSpacing
                anchors.rightMargin: PlasmaCore.Units.largeSpacing
                anchors.verticalCenter: parent.verticalCenter
                
                PlasmaCore.IconItem {
                    Layout.alignment: Qt.AlignVCenter
                    Layout.preferredWidth: PlasmaCore.Units.iconSizes.smallMedium
                    Layout.preferredHeight: PlasmaCore.Units.iconSizes.smallMedium
                    source: "audio-volume-high-symbolic"
                }
                
                Controls.Slider {
                    id: volumeSlider
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter
                    value: window.volume
                    from: 0
                    to: window.maxVolume
                }
                
                PlasmaExtra.Heading {
                    level: 3
                    Layout.alignment: Qt.AlignVCenter
                    text: window.volume
                }
            }
        }
    }
}
