// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts

import org.kde.plasma.components 3.0 as PC3
import org.kde.kirigami 2.10 as Kirigami
import org.kde.private.mobile.homescreen.folio 1.0 as Folio

Item {
    id: root

    property var homeScreen

    readonly property real verticalMargin: Kirigami.Units.gridUnit
    readonly property real horizontalMargin: Math.round(width * 0.05)
    readonly property real pageContentWidth: width - horizontalMargin * 2
    readonly property real pageContentHeight: height - verticalMargin * 2

    readonly property int reservedSpaceForLabel: metrics.height
    readonly property real cellWidth: Math.round(pageContentWidth / Folio.HomeScreenState.pageColumns)
    readonly property real cellHeight: cellWidth + reservedSpaceForLabel

    onCellWidthChanged: Folio.HomeScreenState.pageCellWidth = cellWidth
    onCellHeightChanged: Folio.HomeScreenState.pageCellHeight = cellHeight

    PC3.Label {
        id: metrics
        text: "M\nM"
        visible: false
        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 0.8
        font.weight: Font.Bold
    }

    Repeater {
        model: Folio.PageListModel

        delegate: HomeScreenPage {
            id: homeScreenPage
            pageNum: model.index
            pageModel: model.delegate
            homeScreen: root.homeScreen

            reservedSpaceForLabel: root.reservedSpaceForLabel
            cellWidth: root.cellWidth
            cellHeight: root.cellHeight

            anchors.fill: root
            anchors.leftMargin: root.horizontalMargin
            anchors.rightMargin: root.horizontalMargin
            anchors.topMargin: root.verticalMargin
            anchors.bottomMargin: root.verticalMargin

            // animation so that full opacity is only when the page is in view
            opacity: 1 - Math.min(1, Math.max(0, Math.abs(-Folio.HomeScreenState.pageViewX - root.width * pageNum) / root.width))

            // x position of page
            transform: Translate {
                x: root.width * index + Folio.HomeScreenState.pageViewX
            }
        }
    }
}
