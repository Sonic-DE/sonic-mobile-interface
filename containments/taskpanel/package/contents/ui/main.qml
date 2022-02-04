/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *  SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Window 2.15
import QtGraphicalEffects 1.12

import org.kde.taskmanager 0.1 as TaskManager
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.kquickcontrolsaddons 2.0

import org.kde.plasma.private.nanoshell 2.0 as NanoShell
import org.kde.plasma.private.mobileshell 1.0 as MobileShell
import org.kde.plasma.phone.taskpanel 1.0 as TaskPanel

PlasmaCore.ColorScope {
    id: root
    width: 360
    colorGroup: showingApp ? PlasmaCore.Theme.HeaderColorGroup : PlasmaCore.Theme.ComplementaryColorGroup

    readonly property color backgroundColor: PlasmaCore.ColorScope.backgroundColor
    readonly property bool showingApp: !plasmoid.nativeInterface.allMinimized

    readonly property bool hasTasks: tasksModel.count > 0

    property var taskSwitcher: MobileShell.HomeScreenControls.taskSwitcher

    Plasmoid.backgroundHints: PlasmaCore.Types.NoBackground
    
    Binding {
        target: plasmoid.Window.window // assumed to be plasma-workspace PanelView
        property: "visibilityMode"
        // 0 - VisibilityMode.NormalPanel
        // 3 - VisibilityMode.WindowsGoBelow
        value: MobileShell.MobileShellSettings.navigationPanelEnabled ? 0 : 3
    }
    Binding {
        target: plasmoid.Window.window // assumed to be plasma-workspace PanelView
        property: "height"
        value: MobileShell.MobileShellSettings.navigationPanelEnabled ? PlasmaCore.Units.gridUnit * 2 : 8
    }
    
//BEGIN API implementation

    Binding {
        target: MobileShell.TaskPanelControls
        property: "isPortrait"
        value: Screen.width <= Screen.height
    }
    Binding {
        target: MobileShell.TaskPanelControls
        property: "panelHeight"
        value: MobileShell.MobileShellSettings.navigationPanelEnabled ? root.height : 0
    }
    Binding {
        target: MobileShell.TaskPanelControls
        property: "panelWidth"
        value: MobileShell.MobileShellSettings.navigationPanelEnabled ? root.width : 0
    }

//END API implementation
    
    Connections {
        target: plasmoid.nativeInterface
        function onAllMinimizedChanged() {
            MobileShell.HomeScreenControls.homeScreenVisible = plasmoid.nativeInterface.allMinimized
        }
    }

    TaskManager.TasksModel {
        id: tasksModel
        groupMode: TaskManager.TasksModel.GroupDisabled

        screenGeometry: plasmoid.screenGeometry
        sortMode: TaskManager.TasksModel.SortAlpha

        virtualDesktop: virtualDesktopInfo.currentDesktop
        activity: activityInfo.currentActivity
    }

    TaskManager.VirtualDesktopInfo {
        id: virtualDesktopInfo
    }

    TaskManager.ActivityInfo {
        id: activityInfo
    }
    
    Window.onWindowChanged: {
        if (!Window.window)
            return;

        Window.window.offset = Qt.binding(() => {
            return plasmoid.formFactor === PlasmaCore.Types.Vertical ? MobileShell.TopPanelControls.panelHeight : 0
        });
    }

    // navigation panel actions
    MobileShell.NavigationPanelAction {
        id: taskSwitcherAction
        
        enabled: hasTasks || taskSwitcher.visible
        iconSource: "mobile-task-switcher"
        iconSizeFactor: 0.75
        
        onTriggered: {
            plasmoid.nativeInterface.showDesktop = false;
            
            if (!taskSwitcher.visible) {
                taskSwitcher.show(true);
            } else {
                // when task switcher is open
                if (taskSwitcher.taskSwitcherState.wasInActiveTask) {
                    // restore active window
                    taskSwitcher.activateWindow(taskSwitcher.taskSwitcherState.currentTaskIndex);
                } else {
                    taskSwitcher.hide();
                }
            }
        }
    }
    
    MobileShell.NavigationPanelAction {
        id: homeAction
        
        enabled: true
        iconSource: "start-here-kde"
        iconSizeFactor: 1
        onTriggered: {
            MobileShell.HomeScreenControls.openHomeScreen();
            plasmoid.nativeInterface.allMinimizedChanged();
        }
    }
    
    MobileShell.NavigationPanelAction {
        id: closeAppAction
        
        enabled: MobileShell.KWinVirtualKeyboard.visible || taskSwitcher.visible || plasmoid.nativeInterface.hasCloseableActiveWindow
        // mobile-close-app (from plasma-frameworks) seems to have less margins than icons from breeze-icons
        iconSizeFactor: MobileShell.KWinVirtualKeyboard.visible ? 1 : 0.75
        iconSource: MobileShell.KWinVirtualKeyboard.visible ? "go-down-symbolic" : "mobile-close-app"
        
        onTriggered: {
            if (MobileShell.KWinVirtualKeyboard.active) {
                // close keyboard if it is open
                MobileShell.KWinVirtualKeyboard.active = false;
            } else if (taskSwitcher.visible) { 
                
                // if task switcher is open, close the current window shown
                taskSwitcher.tasksModel.requestClose(taskSwitcher.tasksModel.index(taskSwitcher.currentTaskIndex, 0));
                
            } else if (plasmoid.nativeInterface.hasCloseableActiveWindow) {
                
                // if task switcher is closed, but there is an active window
                var index = taskSwitcher.tasksModel.activeTask;
                if (index) {
                    taskSwitcher.tasksModel.requestClose(index);
                }
            }
        }
    }
    
    // bottom navigation panel
    Component {
        id: navigationPanel 
        MobileShell.NavigationPanel {
            taskSwitcher: root.taskSwitcher
            backgroundColor: {
                if (taskSwitcher.visible) {
                    return Qt.rgba(0, 0, 0, 0.1);
                } else {
                    return root.showingApp ? root.backgroundColor : "transparent";
                }
            }
            foregroundColorGroup: (!taskSwitcher.visible && root.showingApp) ? PlasmaCore.Theme.NormalColorGroup : PlasmaCore.Theme.ComplementaryColorGroup
            
            // do not enable drag gesture when task switcher is already open
            // also don't disable drag gesture mid-drag
            dragGestureEnabled: !taskSwitcher.visible || taskSwitcher.taskSwitcherState.currentlyBeingOpened
            
            leftAction: taskSwitcherAction
            middleAction: homeAction
            rightAction: closeAppAction
        }
    }
    
    // bottom gesture area
    Component {
        id: navigationGesture 
        MobileShell.NavigationGestureArea {
            taskSwitcher: root.taskSwitcher
        }
    }
    
    // load system navigation
    Loader {
        id: navigationLoader
        anchors.fill: parent
        sourceComponent: MobileShell.MobileShellSettings.navigationPanelEnabled ? navigationPanel : navigationGesture
    }
    
    states: [
        State {
            name: "landscape"
            when: MobileShell.Shell.orientation === MobileShell.Shell.Landscape
            PropertyChanges {
                target: plasmoid.nativeInterface
                // only show on right edge if gestures are not enabled
                location: MobileShell.MobileShellSettings.navigationPanelEnabled ? PlasmaCore.Types.RightEdge : PlasmaCore.Types.BottomEdge
            }
            PropertyChanges {
                target: plasmoid
                width: PlasmaCore.Units.gridUnit
                height: PlasmaCore.Units.gridUnit
            }
        }, State {
            name: "portrait"
            when: MobileShell.Shell.orientation === MobileShell.Shell.Portrait
            PropertyChanges {
                target: plasmoid
                height: PlasmaCore.Units.gridUnit
            }
            PropertyChanges {
                target: plasmoid.nativeInterface
                location: PlasmaCore.Types.BottomEdge
            }
        }
    ]
}
