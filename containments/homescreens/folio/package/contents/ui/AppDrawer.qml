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

    required property var homeScreenState
    property var homeScreen

    property real leftPadding: 0
    property real topPadding: 0
    property real bottomPadding: 0
    property real rightPadding: 0

    required property int headerHeight
    required property var headerItem

    // height from top of screen that the drawer starts
    readonly property real drawerTopMargin: height - topPadding - bottomPadding

    Item {
        anchors.fill: parent

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
            homeScreenState: root.homeScreenState
            homeScreen: root.homeScreen
            height: parent.height - drawerHeader.height
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
    }
}


