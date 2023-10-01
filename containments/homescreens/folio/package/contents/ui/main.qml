// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.20 as Kirigami

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 3.0 as PlasmaComponents

import org.kde.plasma.private.mobileshell 1.0 as MobileShell
import org.kde.plasma.private.mobileshell.state 1.0 as MobileShellState
import org.kde.private.mobile.homescreen.folio 1.0 as Folio
import org.kde.plasma.private.mobileshell.windowplugin as WindowPlugin

ContainmentItem {
    id: root

    property Folio.HomeScreenState homeScreenState: Folio.HomeScreenState {}

    Component.onCompleted: {
        // ensure the gestures work immediately on load
        forceActiveFocus();
    }

    Plasmoid.onActivated: {
        // // there's a couple of steps:
        // // - minimize windows (only if we are in an app)
        // // - open app drawer
        // // - close app drawer and, if necessary, restore windows
        //
        // // Always close action drawer
        // if (MobileShellState.ShellDBusClient.isActionDrawerOpen) {
        //     MobileShellState.ShellDBusClient.closeActionDrawer();
        // }
        //
        // if (!WindowPlugin.WindowUtil.isShowingDesktop && WindowPlugin.WindowMaximizedTracker.showingWindow
        //     || MobileShellState.ShellDBusClient.isActionDrawerOpen
        //     || searchWidget.isOpen
        // ) {
        //
        //     // Always close the search widget as well
        //     if (searchWidget.isOpen) {
        //         searchWidget.close();
        //     }
        //
        // } else if (folioHomeScreen.homeScreenState.currentView === HomeScreenState.PageView) {
        //     folioHomeScreen.homeScreenState.openAppDrawer();
        // } else {
        //     folioHomeScreen.homeScreenState.closeAppDrawer();
        // }
    }

    Rectangle {
        id: appDrawerBackground
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.6)

        opacity: root.homeScreenState.appDrawerOpenProgress
    }

    Rectangle {
        id: searchWidgetBackground
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.3)

        opacity: root.homeScreenState.searchWidgetOpenProgress
    }

    MobileShell.HomeScreen {
        id: homeScreen
        anchors.fill: parent

        plasmoidItem: root
        onResetHomeScreenPosition: {
            // folioHomeScreen.homeScreenState.animateGoToPageIndex(0, Kirigami.Units.longDuration);
            root.homeScreenState.closeAppDrawer();
        }

        onHomeTriggered: {
            root.homeScreenState.closeSearchWidget();
        }

        contentItem: Item {

            // homescreen component
            HomeScreen {
                id: folioHomeScreen
                homeScreenState: root.homeScreenState
                anchors.fill: parent

                topMargin: homeScreen.topMargin
                bottomMargin: homeScreen.bottomMargin
                leftMargin: homeScreen.leftMargin
                rightMargin: homeScreen.rightMargin

                // make the homescreen not interactable when task switcher or startup feedback is on
                interactive: !homeScreen.overlayShown
            }
        }

        // listen to app launch errors
        Connections {
            target: Folio.ApplicationListModel
            function onLaunchError(msg) {
                MobileShellState.ShellDBusClient.closeAppLaunchAnimation()
            }
        }
    }
}

