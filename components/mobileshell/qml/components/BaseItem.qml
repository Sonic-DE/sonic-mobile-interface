/*
 *  SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

/**
 * Serves a similar function as a QQC2.Control, but does not 
 * take input events, preventing conflicts with Flickable.
 */

Item {
    id: root
    
    property real topInset: 0
    property real bottomInset: 0
    property real leftInset: 0
    property real rightInset: 0
    
    property real padding: 0
    property real verticalPadding: padding
    property real horizontalPadding: padding
    property real topPadding: verticalPadding
    property real bottomPadding: verticalPadding
    property real leftPadding: horizontalPadding
    property real rightPadding: horizontalPadding
    
    property alias contentItem: contentItemItem
    property alias background: backgroundItem
    
    implicitHeight: topPadding + bottomPadding + contentItem.implicitHeight
    implicitWidth: leftPadding + rightPadding + contentItem.implicitWidth
    
    onContentItemChanged: applyContentItemBounds()
    onBackgroundChanged: applyBackgroundBounds()
    
    function applyBackgroundBounds() {
        background.anchors.fill = root;
        background.anchors.leftMargin = root.leftInset;
        background.anchors.rightMargin = root.rightInset;
        background.anchors.topMargin = root.topInset;
        background.anchors.bottomMargin = root.bottomInset;
    }
    function applyContentItemBounds() {
        contentItem.anchors.fill = root;
        contentItem.anchors.leftMargin = root.leftPadding;
        contentItem.anchors.rightMargin = root.rightPadding;
        contentItem.anchors.topMargin = root.topPadding;
        contentItem.anchors.bottomMargin = root.bottomPadding;
    }
    
    Item {
        id: backgroundItem
    }
    
    Item {
        id: contentItemItem
    }
}
