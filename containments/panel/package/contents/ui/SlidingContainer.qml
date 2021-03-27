/*
 *   SPDX-FileCopyrightText: 2014 Marco Martin <notmart@gmail.com>
 *   SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQml 2.15
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.plasma.private.nanoshell 2.0 as NanoShell

NanoShell.FullScreenOverlay {
    id: window

    property int offset: 0 // slide progress
    property int openThreshold: PlasmaCore.Units.gridUnit * 2
    property bool userInteracting: false
    
    readonly property bool wideScreen: false // width > height || width > units.gridUnit * 45
    readonly property int drawerWidth: wideScreen ? contentItem.implicitWidth : width
    
    property int drawerX: 0
    property alias fixedArea: mainScope
    property alias flickable: mainFlickable

    color: "transparent"
    property alias contentItem: contentArea.contentItem
    property int topPanelHeight
    property real topEmptyAreaHeight

    signal closed

    width: Screen.width
    height: Screen.height

    // avoids binding loops
    function updateOffset(delta) {
        offset = Math.max(0, Math.min(contentItem.height, offset + delta));
        if (!mainFlickable.moving && !mainFlickable.dragging && !mainFlickable.flicking) {
            mainFlickable.contentY = -window.offset + contentItem.height;
        }
    }
    
    enum MovementDirection {
        None = 0,
        Up,
        Down
    }
    property int direction: SlidingPanel.MovementDirection.None

    function cancelAnimations() {
        closeAnim.stop();
        openAnim.stop();
    }
    function open() {
        cancelAnimations();
        openAnim.restart();
    }
    function close() {
        cancelAnimations();
        closeAnim.restart();
    }
    function updateState() {
        cancelAnimations();
        if (window.offset <= -topPanelHeight) {
            close();
            // close immediately, so that we don't have to wait units.longDuration 
            window.visible = false;
            window.closed();
        } else if (window.direction === SlidingPanel.MovementDirection.None) {
            if (window.offset < openThreshold) {
                close();
            } else {
                open();
            }
        } else if (offset > openThreshold && window.direction === SlidingPanel.MovementDirection.Down) {
            open();
        } else if (mainFlickable.contentY > openThreshold) {
            close();
        } else {
            open();
        }
    }
    Timer {
        id: updateStateTimer
        interval: 0
        onTriggered: updateState()
    }

    onActiveChanged: {
        if (!active) {
            close();
        }
    }

    PropertyAnimation {
        id: closeAnim
        target: mainFlickable
        properties: "contentY"
        duration: PlasmaCore.Units.longDuration
        easing.type: Easing.InOutQuad
        from: mainFlickable.contentY
        to: mainFlickable.contentHeight
        onFinished: {
            window.visible = false;
            window.closed();
        }
    }
    PropertyAnimation {
        id: openAnim
        target: mainFlickable
        properties: "contentY"
        duration: PlasmaCore.Units.longDuration
        easing.type: Easing.InOutQuad
        from: mainFlickable.contentY
        to: -topEmptyAreaHeight
    }

    Rectangle {
        anchors.fill: parent
        color: PlasmaCore.Theme.backgroundColor
        opacity: 0.6 * Math.min(1, offset/(topEmptyAreaHeight + contentItem.height))
    }
    
    PlasmaCore.ColorScope {
        id: mainScope
        colorGroup: PlasmaCore.Theme.ViewColorGroup
        anchors.fill: parent

        Flickable {
            id: mainFlickable
            anchors.fill: parent
            
            property real oldContentY
            contentY: contentHeight

            onContentYChanged: {
                if (contentY === oldContentY) {
                    window.direction = SlidingPanel.MovementDirection.None;
                } else {
                    window.direction = contentY > oldContentY ? SlidingPanel.MovementDirection.Up : SlidingPanel.MovementDirection.Down;
                }
                window.offset = -contentY + contentArea.height;
                oldContentY = contentY;
            }
            
            boundsMovement: Flickable.StopAtBounds
            contentWidth: window.width
            contentHeight: window.height
            bottomMargin: window.height
            onMovementStarted: {
                window.cancelAnimations();
                window.userInteracting = true;
            }
            onFlickStarted: window.userInteracting = true;
            onMovementEnded: {
                window.userInteracting = false;
                window.updateState();
            }
            onFlickEnded: {
                window.userInteracting = true;
                window.updateState();
            }
            
            MouseArea {
                id: dismissArea
                z: 2
                width: parent.width
                height: mainFlickable.contentHeight
                onClicked: window.close();
                PlasmaComponents.Control {
                    id: contentArea
                    z: 1
                    x: drawerX
                    width: drawerWidth
                }
            }
        }
    }
}
