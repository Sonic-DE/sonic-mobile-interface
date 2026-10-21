/*
    SPDX-FileCopyrightText: 2026 Micah Stanley <stanleymicah@proton.me>

    SPDX-License-Identifier: GPL-2.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import org.kde.kirigami as Kirigami
import plasma.applet.org.kde.plasma.mobile.homescreen.folio as Folio
import "./delegate"
import "./private"

AbstractAppDrawerGrid {
    id: root
    property var categoryAppGrid

    columns: Math.max(baseColumns - 2, 1)

    // maximun amount of space available for a cell, used for calculating the category folder size
    readonly property real rawCellWidth: (width / columns)
    // usually the category folder size is rougly twice the size of the icon, but when space is tight we base the size off of `rawCellWidth`
    readonly property real categoryFolderSize: Math.min(folio.FolioSettings.delegateIconSize * 2 + categoryFolderRadius * 2.75, rawCellWidth - Kirigami.Units.gridUnit - Kirigami.Units.largeSpacing)
    readonly property real categoryFolderRadius: (folio.FolioSettings.delegateIconSize * 2) * 0.125

    cellWidth: effectiveContentWidth / columns
    cellHeight: cellWidth + folio.HomeScreenState.pageDelegateLabelHeight + Math.max(Kirigami.Units.gridUnit * 0.2 + (cellWidth - categoryFolderSize) * 0.4, Kirigami.Units.gridUnit * 0.2)

    topMargin: containerTopMargin + Kirigami.Units.largeSpacing
    bottomMargin: containerBottomMargin
    // the left and right margin are set to make sure the category folder grid is centered and does not exceed the size limit
    leftMargin: Math.floor(Math.max(((width - Math.max((categoryFolderSize * columns), Kirigami.Units.gridUnit * (columns > 1 ? 26 : 0))) * 0.5) - Kirigami.Units.gridUnit, Kirigami.Units.gridUnit))
    rightMargin: leftMargin

    // keyboard navigation for moving to the page on the left
    Keys.onLeftPressed: (event) => {
        if (count > 0 && currentIndex % columns === 0) {
            pageLeftRequested();
            event.accepted = true;
        } else {
            event.accepted = false;
        }
    }

    signal pageLeftRequested()

    // Keyboard focus on category delegate when it is the selected item
    onCurrentItemChanged: (currentItem as CategoryDelegate)?.keyboardFocus()

    signal expandCategory(categoryGrid: var, expandCategoryLayout: var, categoryTitle: string)

    delegate: CategoryDelegate {
        required property var modelData
        required property int index

        delegateIndex: index
        folio: root.folio
        homeScreen: root.homeScreen
        categoryAppGrid: root.categoryAppGrid
        width: root.cellWidth
        height: root.cellHeight
        category: modelData

        folderRadius: root.categoryFolderRadius

        onExpandCategory: (expandCategoryLayout, categoryTitle) => root.expandCategory(root, expandCategoryLayout, categoryTitle)
    }
}
