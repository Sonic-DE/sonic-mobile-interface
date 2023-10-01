// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.1

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.draganddrop 2.0 as DragDrop
import org.kde.kirigami 2.20 as Kirigami

import org.kde.plasma.private.containmentlayoutmanager 1.0 as ContainmentLayoutManager
import org.kde.plasma.private.mobileshell 1.0 as MobileShell

import org.kde.private.mobile.homescreen.folio 1.0 as Folio

// homescreen:
// - allow whitespace
// app drawer:

Item {
    id: root

    required property real topMargin
    required property real bottomMargin
    required property real leftMargin
    required property real rightMargin

    property bool interactive: true

    required property Folio.HomeScreenState homeScreenState

    MobileShell.SwipeArea {
        id: swipeArea
        anchors.fill: parent

        interactive: root.interactive

        onSwipeStarted: {
            console.log('vertical start');
            homeScreenState.swipeStarted();
        }
        onSwipeEnded: {
            console.log('vertical end');
            homeScreenState.swipeEnded();
        }
        onSwipeMove: (totalDeltaX, totalDeltaY, deltaX, deltaY) => {
            // console.log('vertical swipe');
            homeScreenState.swipeMoved(totalDeltaX, totalDeltaY, deltaX, deltaY);
        }

        AppDrawer {
            id: appDrawer
            anchors.fill: parent
            homeScreenState: root.homeScreenState

            headerHeight: Math.round(Kirigami.Units.gridUnit * 5)
            headerItem: AppDrawerHeader {}

            // account for panels
            topPadding: root.topMargin
            bottomPadding: root.bottomMargin
            leftPadding: root.leftMargin
            rightPadding: root.rightMargin
        }

        // search component
        MobileShell.KRunnerScreen {
            id: searchWidget
            anchors.fill: parent

            opacity: homeScreenState.searchWidgetOpenProgress
            visible: homeScreenState.searchWidgetOpenProgress > 0
            transform: Translate { y: (1 - homeScreenState.searchWidgetOpenProgress) * (-Kirigami.Units.gridUnit * 2) }

            onRequestedClose: {
                homeScreenState.closeSearchWidget();
            }

            anchors.topMargin: root.topMargin
            anchors.bottomMargin: root.bottomMargin
            anchors.leftMargin: root.leftMargin
            anchors.rightMargin: root.rightMargin
        }
    }
}
