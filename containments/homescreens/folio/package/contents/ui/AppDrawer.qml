// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.15 as Controls

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 3.0 as PC3
import org.kde.kirigami 2.10 as Kirigami

import org.kde.plasma.private.mobileshell 1.0 as MobileShell

Item {
    id: root
    opacity: homeScreenState.appDrawerOpenProgress
    visible: homeScreenState.appDrawerOpenProgress > 0 // prevent handlers from picking up events

    required property var homeScreenState

    property real leftPadding: 0
    property real topPadding: 0
    property real bottomPadding: 0
    property real rightPadding: 0

    required property int headerHeight
    required property var headerItem

    // height from top of screen that the drawer starts
    readonly property real drawerTopMargin: height - topPadding - bottomPadding

    Drag.dragType: Drag.Automatic

    // // bottom divider
    // GradientBar {
    //     opacity: (homeScreenState.currentView !== HomeScreenState.PageView || homeScreenState.currentSwipeState === HomeScreenState.SwipingAppDrawerVisibility) ? 0.6 : 0
    //     visible: root.bottomPadding > 0
    //     anchors.left: parent.left
    //     anchors.right: parent.right
    //     anchors.bottom: parent.bottom
    //     anchors.bottomMargin: root.bottomPadding - height
    // }

    Item {
        anchors.fill: parent

        transform: Translate { y: (1 - homeScreenState.appDrawerOpenProgress) * (Kirigami.Units.gridUnit * 2) }

        anchors.leftMargin: root.leftPadding
        anchors.topMargin: root.topPadding
        anchors.rightMargin: root.rightPadding
        anchors.bottomMargin: root.bottomPadding

        // drawer header
        MobileShell.BaseItem {
            id: drawerHeader
            height: root.headerHeight

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            contentItem: root.headerItem
        }

        AppDrawerGrid {
            height: parent.height - drawerHeader.height
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
    }
}


