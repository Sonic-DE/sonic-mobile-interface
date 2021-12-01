/*
 * SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 * 
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.15
import QtGraphicalEffects 1.12

import org.kde.kirigami 2.12 as Kirigami

import org.kde.plasma.core 2.0 as PlasmaCore

Item {
    default property Item contentItem
    onContentItemChanged: {
        contentItem.parent = mainCard;
        contentItem.anchors.fill = mainCard;
        contentItem.anchors.margins = Kirigami.Units.largeSpacing;
        mainCard.children.push(contentItem);
    }
    
    implicitHeight: mainCard.implicitHeight
    
    // glow
    RectangularGlow {
        anchors.topMargin: 1
        anchors.leftMargin: 1
        anchors.fill: mainCard
        cornerRadius: mainCard.radius * 2
        glowRadius: 2
        spread: 0.2
        color: "#616161"
    }

    // shadow
    Rectangle {
        anchors.fill: parent
        anchors.leftMargin: -1
        anchors.rightMargin: -1
        anchors.bottomMargin: -2
        
        color: Qt.darker(PlasmaCore.Theme.backgroundColor, 1.3)
        radius: PlasmaCore.Units.smallSpacing
    }

    // card
    Rectangle {
        id: mainCard
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        
        color: PlasmaCore.Theme.backgroundColor
        radius: PlasmaCore.Units.smallSpacing
        implicitHeight: contentItem.implicitHeight + contentItem.anchors.topMargin + contentItem.anchors.bottomMargin   
    }
}
