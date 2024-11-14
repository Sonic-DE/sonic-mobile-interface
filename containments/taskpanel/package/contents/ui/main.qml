// SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
// SPDX-FileCopyrightText: 2021-2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Window 2.15
import QtQuick.Shapes 1.8

import org.kde.kirigami 2.20 as Kirigami

import org.kde.taskmanager 0.1 as TaskManager
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core as PlasmaCore
import org.kde.kquickcontrolsaddons 2.0

import org.kde.plasma.shell.panel 0.1 as Panel
import org.kde.plasma.private.mobileshell as MobileShell
import org.kde.plasma.private.mobileshell.shellsettingsplugin as ShellSettings
import org.kde.plasma.private.mobileshell.windowplugin as WindowPlugin
import org.kde.plasma.private.mobileshell.state as MobileShellState
import org.kde.layershell 1.0 as LayerShell

ContainmentItem {
    id: root
    Plasmoid.backgroundHints: PlasmaCore.Types.NoBackground
    Plasmoid.status: PlasmaCore.Types.PassiveStatus // ensure that the panel never takes focus away from the running app

    // filled in by the shell (Panel.qml) with the plasma-workspace PanelView
    property var panel: null
    onPanelChanged: {
        setWindowProperties()
    }

    // filled in by the shell (Panel.qml)
    property var tabBar: null
    onTabBarChanged: {
        if (tabBar) {
            tabBar.visible = false;
        }
    }

    property bool fullscreenExpandTouchArea: false

    readonly property bool inLandscape: MobileShell.Constants.navigationPanelOnSide(Screen.width, Screen.height)

    readonly property real navigationPanelHeight: MobileShell.Constants.navigationPanelThickness

    readonly property real intendedWindowThickness: navigationPanelHeight
    readonly property real intendedWindowLength: inLandscape ? Screen.height : Screen.width
    readonly property real intendedWindowOffset: inLandscape ? MobileShell.Constants.topPanelHeight : 0; // offset for top panel
    readonly property int intendedWindowLocation: inLandscape ? PlasmaCore.Types.RightEdge : PlasmaCore.Types.BottomEdge

    onIntendedWindowLengthChanged: maximizeTimer.restart() // ensure it always takes up the full length of the screen
    onIntendedWindowLocationChanged: setPanelLocationTimer.restart()
    onIntendedWindowOffsetChanged: {
        if (root.panel) {
            root.panel.offset = intendedWindowOffset;
        }
    }

    // HACK: the entire shell seems to crash sometimes if this is applied immediately after a display change (ex. screen rotation)
    // see https://invent.kde.org/plasma/plasma-mobile/-/issues/321
    Timer {
        id: setPanelLocationTimer
        running: false
        interval: 100
        onTriggered: {
            root.panel.location = intendedWindowLocation;
        }
    }

    // use a timer so we don't have to maximize for every single pixel
    // - improves performance if the shell is run in a window, and can be resized
    Timer {
        id: maximizeTimer
        running: false
        interval: 100
        onTriggered: {
            // maximize first, then we can apply offsets (otherwise they are overridden)
            root.panel.maximize()
            root.panel.offset = intendedWindowOffset;
        }
    }


    function setWindowProperties() {
        if (root.panel) {
            root.panel.floating = false;
            root.panel.maximize(); // maximize first, then we can apply offsets (otherwise they are overridden)
            root.panel.offset = intendedWindowOffset;
            root.panel.thickness = currentWindowFullscreen && !fullscreenExpandTouchArea ? Kirigami.Units.gridUnit : navigationPanelHeight;
            root.panel.location = intendedWindowLocation;
            MobileShell.ShellUtil.setWindowLayer(root.panel, LayerShell.Window.LayerOverlay)
        }
    }

    function calculateResistance(value : double, threshold : int) : double {
        if (value > threshold) {
            return threshold + Math.pow(value - threshold + 1, Math.max(0.8 - (value - threshold) / ((Screen.height - threshold) * 2), 0.65));
        } else {
            return value;
        }
    }

    Connections {
        target: root.panel

        // HACK: There seems to be some component that overrides our initial bindings for the panel,
        //   which is particularly problematic on first start (since the panel is misplaced)
        // - We set an event to override any attempts to override our bindings.
        function onLocationChanged() {
            if (root.panel.location !== root.intendedWindowLocation) {
                root.setWindowProperties();
            }
        }

        function onThicknessChanged() {
            if (root.panel.thickness !== root.intendedWindowThickness) {
                root.setWindowProperties();
            }
        }
    }

    Component.onCompleted: setWindowProperties();

    // only opaque if there are no maximized windows on this screen
    readonly property bool showingStartupFeedback: MobileShellState.ShellDBusObject.startupFeedbackModel.activeWindowIsStartupFeedback && windowMaximizedTracker.windowCount === 1
    readonly property bool opaqueBar: (windowMaximizedTracker.showingWindow || currentWindowFullscreen) && !showingStartupFeedback

    readonly property alias currentWindowFullscreen: windowMaximizedTracker.currentWindowFullscreen
    onCurrentWindowFullscreenChanged: {
        swipeArea.state = currentWindowFullscreen ? "hidden" : "default";
    }

    WindowPlugin.WindowMaximizedTracker {
        id: windowMaximizedTracker
        screenGeometry: Plasmoid.containment.screenGeometry
    }

    MobileShell.StartupFeedbackPanelFill {
        id: startupFeedbackColorAnimation
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        fullHeight: root.height
        screen: Plasmoid.screen
        maximizedTracker: windowMaximizedTracker
    }

    MobileShell.SwipeArea {
        id: swipeArea
        mode: MobileShell.SwipeArea.VerticalOnly
        anchors.fill: parent
        interactive: swipeArea.state == "hidden"

        // contrasting colour
        Kirigami.Theme.colorSet: opaqueBar ? Kirigami.Theme.Window : Kirigami.Theme.Complementary
        Kirigami.Theme.inherit: false

        property real offset: 0

        state: "default"
        onStateChanged: {
            if (swipeArea.state != "hidden") {
                root.fullscreenExpandTouchArea = true;
                root.setWindowProperties();
                hiddenTimer.restart();
            }
        }

        Timer {
            id: hiddenTimer
            running: false
            interval: 3000
            onTriggered: {
                if (swipeArea.state == "visible") {
                    swipeArea.state = "hidden";
                }
            }
        }

        states: [
            State {
                name: "default"
                PropertyChanges {
                    target: swipeArea; offset: 0
                }
            },
            State {
                name: "visible"
                PropertyChanges {
                    target: swipeArea; offset: 0
                }
            },
            State {
                name: "hidden"
                PropertyChanges {
                    target: swipeArea; offset: root.navigationPanelHeight
                }
            }
        ]

        transitions: Transition {
            SequentialAnimation {
                ParallelAnimation {
                    PropertyAnimation {
                        properties: "offset"; easing.type: Easing.OutExpo; duration: Kirigami.Units.longDuration
                    }
                }
                ScriptAction {
                    script: {
                        fullscreenExpandTouchArea = swipeArea.state == "visible";
                        root.setWindowProperties();
                    }
                }
            }
        }

        function startSwipeWithPoint(point) {
            fullscreenExpandTouchArea = true;
            root.setWindowProperties();
            resetAn.stop();
            shapepath.startPoint = point.x;
            shapepath.verticalPoint = 0;
        }

        function endSwipe() {
            resetAn.restart()
            if (shapepath.verticalPoint < -Kirigami.Units.gridUnit * 4) {
                swipeArea.state = "visible";
            }
        }

        function updateOffset(offsetX, offsetY) {
            shapepath.horizontalPoint = offsetX;
            shapepath.verticalPoint = offsetY;
        }

        onSwipeStarted: (point) => startSwipeWithPoint(point)
        onSwipeEnded: endSwipe()
        onSwipeMove: (totalDeltaX, totalDeltaY, deltaX, deltaY) => updateOffset(totalDeltaX, totalDeltaY);

        onPressedChanged: {
            if (!pressed && shapepath.verticalPoint == 0) {
                swipeArea.state = "visible";
            }
        }

        NumberAnimation {
            id: resetAn
            running: false
            target: shapepath
            property: "verticalPoint"
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutExpo
            onRunningChanged: {
                if (!running && swipeArea.state == "hidden") {
                    fullscreenExpandTouchArea = false;
                    root.setWindowProperties();
                }
            }
        }

        // load appropriate system navigation component
        NavigationPanelComponent {
            id: navigationPanel
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: root.navigationPanelHeight
            opaqueBar: root.opaqueBar
            isVertical: root.inLandscape
            navbarState: swipeArea.state

            transform: [
                Translate {
                    y: swipeArea.offset
                }
            ]
        }

        Shape {
            id: shape
            anchors.bottom: parent.bottom
            anchors.bottomMargin: -4

            x: shapepath.startPoint - Kirigami.Units.gridUnit * 5
            visible: shapepath.verticalPoint != 0

            ShapePath {
                id: shapepath
                fillColor: "black"
                strokeColor: "black"

                property real startPoint: 0
                property real horizontalPoint: 0
                property real verticalPoint: 0

                readonly property real hp: Math.max(Math.min(horizontalPoint, Kirigami.Units.gridUnit * 10), -Kirigami.Units.gridUnit * 10)
                readonly property real vp: Math.max(Math.min(-root.calculateResistance(-verticalPoint, 0), 0), -swipeArea.height + 3)

                startX: 0; startY: 3
                PathCurve { x: 0; y: 2 }
                PathCurve { x: Kirigami.Units.gridUnit * 2 + shapepath.hp * 0.16; y: 0 }
                PathCurve { x: Kirigami.Units.gridUnit * 5 + shapepath.hp * 0.35; y: shapepath.vp }
                PathCurve { x: Kirigami.Units.gridUnit * 8 + shapepath.hp * 0.16; y: 0 }
                PathCurve { x: Kirigami.Units.gridUnit * 10; y: 2 }
                PathCurve { x: Kirigami.Units.gridUnit * 10; y: 3 }
            }
        }
    }
}
