// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.3 as Controls
import Qt5Compat.GraphicalEffects
import QtQuick.Effects

import org.kde.kirigami 2.20 as Kirigami

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.kquickcontrolsaddons 2.0

import org.kde.private.mobile.homescreen.folio 1.0 as Folio
import org.kde.plasma.private.mobileshell.shellsettingsplugin as ShellSettings

Folio.DelegateTouchArea {
    id: delegate

    property string name
    property string icon
    property string storageId
    property bool applicationRunning

    property bool shadow: false

    property int reservedSpaceForLabel
    property alias iconItem: icon
    property alias delegateItem: delegateWrapper

    property alias labelOpacity: label.opacity

    readonly property real margins: Math.floor(width * 0.2)

    signal launch(int x, int y, var source, string title, string storageId)

    function launchApp() {
        // launch app
        if (applicationRunning) {
            delegate.launch(0, 0, "", delegate.name, delegate.storageId);
        } else {
            delegate.launch(delegate.x + (Kirigami.Units.smallSpacing * 2), delegate.y + (Kirigami.Units.smallSpacing * 2), icon.source, delegate.name, delegate.storageId);
        }
    }

    // grow/shrink animation
    property real zoomScale: 1
    property bool launchAppRequested: false

    NumberAnimation on zoomScale {
        id: shrinkAnim
        running: false
        duration: ShellSettings.Settings.animationsEnabled ? 80 : 1
        to: ShellSettings.Settings.animationsEnabled ? 0.8 : 1
        onFinished: {
            if (!delegate.pressed) {
                growAnim.restart();
            }
        }
    }

    NumberAnimation on zoomScale {
        id: growAnim
        running: false
        duration: ShellSettings.Settings.animationsEnabled ? 80 : 1
        to: 1
        onFinished: {
            if (delegate.launchAppRequested) {
                delegate.launchApp();
                delegate.launchAppRequested = false;
            }
        }
    }

    cursorShape: Qt.PointingHandCursor
    onPressedChanged: (pressed) => {
        if (pressed) {
            growAnim.stop();
            shrinkAnim.restart();
        } else if (!pressed && !shrinkAnim.running) {
            growAnim.restart();
        }
    }
    // launch app handled by press animation
    onClicked: launchAppRequested = true;

    layer.enabled: delegate.shadow
    layer.effect: DelegateShadow {}

    Item {
        id: delegateWrapper
        anchors.fill: parent

        transform: Scale {
            origin.x: delegate.width / 2;
            origin.y: delegate.height / 2;
            xScale: delegate.zoomScale
            yScale: delegate.zoomScale
        }

        ColumnLayout {
            anchors {
                fill: parent
                leftMargin: margins
                topMargin: margins
                rightMargin: margins
                bottomMargin: margins
            }
            spacing: 0

            Kirigami.Icon {
                id: icon

                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                Layout.fillWidth: true
                Layout.minimumHeight: Math.floor(parent.height - delegate.reservedSpaceForLabel)
                Layout.preferredHeight: Layout.minimumHeight

                source: delegate.icon

                Rectangle {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                        bottom: parent.bottom
                    }
                    visible: delegate.applicationRunning
                    radius: width
                    width: Kirigami.Units.smallSpacing
                    height: width
                    color: Kirigami.Theme.highlightColor
                }

                // darken effect when hovered/pressed
                layer {
                    enabled: delegate.pressed || delegate.hovered
                    effect: ColorOverlay {
                        color: Qt.rgba(0, 0, 0, 0.3)
                    }
                }
            }

            PlasmaComponents.Label {
                id: label
                visible: text.length > 0

                Layout.fillWidth: true
                Layout.preferredHeight: delegate.reservedSpaceForLabel
                Layout.topMargin: Kirigami.Units.smallSpacing
                Layout.leftMargin: -parent.anchors.leftMargin + Kirigami.Units.smallSpacing
                Layout.rightMargin: -parent.anchors.rightMargin + Kirigami.Units.smallSpacing

                wrapMode: Text.WordWrap
                maximumLineCount: 2
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignTop
                elide: Text.ElideRight

                text: delegate.name

                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 0.8
                font.weight: Font.Bold
                color: "white"
            }
        }
    }
}


