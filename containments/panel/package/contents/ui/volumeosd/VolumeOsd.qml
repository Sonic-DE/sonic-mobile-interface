/*
 *  SPDX-FileCopyrightText: 2014 Martin Klapetek <mklapetek@kde.org>
 *  SPDX-FileCopyrightText: 2019 Kai Uwe Broulik <kde@broulik.de>
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
    
    color: "transparent"
    width: Screen.width
    height: Screen.height
    
    property int volume: 0
    
    function showOverlay() {
        window.visible = true;
        hideTimer.restart();
    }
    
    Component.onCompleted: {// TODO
        window.visible = true;
    }
    
    Timer {
        id: hideTimer
        interval: 3000
        running: false
        onTriggered: {
            window.close();
        }
    }
    
    MouseArea {
        anchors.fill: parent
        onClicked: {
            hideTimer.stop();
            hideTimer.triggered();
        }
        
        RectangularGlow {
            anchors.topMargin: 1
            anchors.fill: content
            cached: true
            glowRadius: 4
            spread: 0.2
            color: Qt.rgba(0, 0, 0, 0.15)
        }
        
        // capture presses on the audio applet so it doesn't close the overlay
        MouseArea {
            id: content
            anchors.top: parent.top
            anchors.topMargin: PlasmaCore.Units.largeSpacing * 2
            anchors.horizontalCenter: parent.horizontalCenter
            
            implicitWidth: Math.min(PlasmaCore.Units.gridUnit * 20, parent.width - PlasmaCore.Units.largeSpacing * 2)
            implicitHeight: containerLayout.implicitHeight + PlasmaCore.Units.smallSpacing * 4
            
            Rectangle {
                anchors.fill: parent
                radius: PlasmaCore.Units.smallSpacing
                color: PlasmaCore.Theme.backgroundColor
                
                RowLayout {
                    id: containerLayout
                    spacing: PlasmaCore.Units.smallSpacing
                    
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: PlasmaCore.Units.smallSpacing * 2
                    anchors.rightMargin: PlasmaCore.Units.smallSpacing
                    
                    PlasmaCore.IconItem {
                        Layout.alignment: Qt.AlignVCenter
                        Layout.preferredWidth: PlasmaCore.Units.iconSizes.medium
                        Layout.preferredHeight: PlasmaCore.Units.iconSizes.medium
                        Layout.rightMargin: PlasmaCore.Units.smallSpacing
                        source: "audio-volume-high-symbolic"
                    }
                    
                    PlasmaComponents.ProgressBar {
                        id: volumeSlider
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                        Layout.rightMargin: PlasmaCore.Units.smallSpacing
                        value: window.volume
                        from: 0
                        to: 100
                    }
                    
                    // Get the width of a three-digit number so we can size the label
                    // to the maximum width to avoid the progress bar resizing itself
                    TextMetrics {
                        id: widestLabelSize
                        text: i18n("100%")
                        font: percentageLabel.font
                    }

                    PlasmaExtra.Heading {
                        id: percentageLabel
                        Layout.preferredWidth: widestLabelSize.width
                        Layout.alignment: Qt.AlignVCenter
                        Layout.rightMargin: PlasmaCore.Units.smallSpacing
                        level: 3
                        text: i18nc("Percentage value", "%1%", window.volume)
                        
                        // Display a subtle visual indication that the volume might be
                        // dangerously high
                        // ------------------------------------------------
                        // Keep this in sync with the copies in plasma-pa:ListItemBase.qml
                        // and plasma-pa:VolumeSlider.qml
                        color: {
                            if (volumeSlider.value <= 100) {
                                return PlasmaCore.Theme.textColor
                            } else if (volumeSlider.value > 100 && volumeSlider.value <= 125) {
                                return PlasmaCore.Theme.neutralTextColor
                            } else {
                                return PlasmaCore.Theme.negativeTextColor
                            }
                        }
                    }
                    
                    PlasmaComponents.ToolButton {
                        icon.name: "configure"
                        Layout.alignment: Qt.AlignVCenter
                        Layout.preferredWidth: PlasmaCore.Units.iconSizes.medium
                        Layout.preferredHeight: PlasmaCore.Units.iconSizes.medium
                        onClicked: audioApplet.showOverlay()
                    }
                }
            }
        }
    }
}
