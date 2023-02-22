// SPDX-FileCopyrightText: 2015 Marco Martin <notmart@gmail.com>
// SPDX-FileCopyrightText: 2021-2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.15
import QtGraphicalEffects 1.15

import org.kde.taskmanager 0.1 as TaskManager
import org.kde.plasma.core 2.1 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

import org.kde.kwin 3.0 as KWinComponents
import org.kde.kwin.private.effects 1.0

/**
 * Component that provides a task switcher.
 */
FocusScope {
    id: root
    focus: true

    readonly property QtObject effect: KWinComponents.SceneView.effect
    readonly property QtObject targetScreen: KWinComponents.SceneView.screen

    readonly property real topMargin: 0
    readonly property real bottomMargin: 0
    readonly property real leftMargin: 0
    readonly property real rightMargin: 0

    property var taskSwitcherState: TaskSwitcherState {
        taskSwitcher: root
    }

    KWinComponents.WindowModel {
        id: stackModel
    }

    KWinComponents.VirtualDesktopModel {
        id: desktopModel
    }

    property var tasksModel: KWinComponents.WindowFilterModel {
        activity: KWinComponents.Workspace.currentActivity
        desktop: KWinComponents.Workspace.currentDesktop
        screenName: root.targetScreen.name
        windowModel: stackModel
        minimizedWindows: true
        windowType: ~KWinComponents.WindowFilterModel.Dock &
                    ~KWinComponents.WindowFilterModel.Desktop &
                    ~KWinComponents.WindowFilterModel.Notification &
                    ~KWinComponents.WindowFilterModel.CriticalNotification
    }

    readonly property int tasksCount: taskList.count

    // keep track of task list events
    property int oldTasksCount: tasksCount
    onTasksCountChanged: {
        if (tasksCount === 0 && oldTasksCount !== 0) {
            hide();
        } else if (tasksCount < oldTasksCount && taskSwitcherState.currentTaskIndex >= tasksCount - 1) {
            // if the user is on the last task, and it is closed, scroll left
            taskSwitcherState.animateGoToTaskIndex(tasksCount - 1, PlasmaCore.Units.longDuration);
        }

        oldTasksCount = tasksCount;
    }

    Keys.onEscapePressed: hide();

    Component.onCompleted: {
        taskList.minimizeAll();

        // reset values
        taskSwitcherState.currentlyBeingOpened = true;
        taskSwitcherState.goToTaskIndex(0);

        // fully open the panel (if this is a button press, not gesture)
        taskSwitcherState.open();
    }

    // called by c++ plugin
    function hideAnimation() {
        closeAnim.restart();
    }

    function instantHide() {
        root.effect.deactivate(true);
    }

    function hide() {
        root.effect.deactivate(false);
    }

    // scroll to delegate index, and activate it
    function activateWindow(index, window) {
        KWinComponents.Workspace.activeClient = window;
        taskSwitcherState.openApp(index, window);
    }

    function setSingleActiveWindow(id) {
        instantHide();
    }

    KWinComponents.DesktopBackground {
        id: backgroundItem
        activity: KWinComponents.Workspace.currentActivity
        desktop: KWinComponents.Workspace.currentDesktop
        outputName: targetScreen.name
    }

    FastBlur {
        source: backgroundItem
        anchors.fill: parent
        opacity: container.opacity
        radius: 50
        cached: true
    }

    // background colour
    Rectangle {
        id: backgroundRect
        anchors.fill: parent

        opacity: container.opacity
        color: {
            // animate background colour only if we are *not* opening from the homescreen
            if (taskSwitcherState.wasInActiveTask || !taskSwitcherState.currentlyBeingOpened) {
                return Qt.rgba(0, 0, 0, 0.6);
            } else {
                return Qt.rgba(0, 0, 0, 0.6 * Math.min(1, taskSwitcherState.yPosition / taskSwitcherState.openedYPosition));
            }
        }
    }

    Item {
        id: container

        // provide shell margins
        anchors.fill: parent
        anchors.leftMargin: root.leftMargin
        anchors.rightMargin: root.rightMargin
        anchors.bottomMargin: root.bottomMargin
        anchors.topMargin: root.topMargin

        NumberAnimation on opacity {
            id: closeAnim
            running: false
            to: 0
            duration: 200
            easing.type: Easing.InOutQuad

            onFinished: {
                closeAllButton.closeRequested = false;
            }
        }

        FlickContainer {
            id: flickable

            anchors.fill: parent

            taskSwitcherState: root.taskSwitcherState

            // the item is effectively anchored to the flickable bounds
            TaskList {
                id: taskList
                taskSwitcher: root
                shellTopMargin: root.topMargin
                shellBottomMargin: root.bottomMargin

                opacity: {
                    // animate opacity only if we are *not* opening from the homescreen
                    if (taskSwitcherState.wasInActiveTask || !taskSwitcherState.currentlyBeingOpened) {
                        return 1;
                    } else {
                        return Math.min(1, taskSwitcherState.yPosition / taskSwitcherState.openedYPosition);
                    }
                }

                x: flickable.contentX
                width: flickable.width
                height: flickable.height

                PlasmaComponents.ToolButton {
                    id: closeAllButton

                    property bool closeRequested: false

                    anchors {
                        bottom: parent.bottom
                        bottomMargin: taskList.taskY / 2
                        horizontalCenter: parent.horizontalCenter
                    }

                    PlasmaCore.ColorScope.colorGroup: PlasmaCore.Theme.ComplementaryColorGroup
                    PlasmaCore.ColorScope.inherit: false

                    opacity: (taskSwitcherState.currentlyBeingOpened || taskSwitcherState.currentlyBeingClosed) ? 0.0 : 1.0

                    Behavior on opacity {
                        NumberAnimation {
                            duration: PlasmaCore.Units.shortDuration
                        }
                    }

                    icon.name: "edit-clear-history"
                    font.bold: true

                    text: closeRequested ? i18n("Confirm Close All") : i18n("Close All")

                    onClicked: {
                        if (closeRequested) {
                            taskList.closeAll();
                        } else {
                            closeRequested = true;
                        }
                    }
                }
            }
        }
    }
}

