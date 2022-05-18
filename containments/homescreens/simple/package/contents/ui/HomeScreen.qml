/*
 *  SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.12 as QQC2
import QtQuick.Window 2.12
import QtQuick.Layouts 1.1

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PC3
import org.kde.draganddrop 2.0 as DragDrop

import org.kde.kirigami 2.10 as Kirigami
import org.kde.plasma.private.mobileshell 1.0 as MobileShell

Item {
    id: root
    property var searchWidget
    
    ColumnLayout {
        id: column 
        anchors.fill: parent
        
        opacity: 1 - searchWidget.openFactor
        
        property real horizontalMargin: Math.max(Kirigami.Units.largeSpacing, column.width * 0.1 / 2)
        property real verticalMargin: Math.max(Kirigami.Units.largeSpacing, column.height * 0.1 / 2)
        
        GridAppList {
            Layout.leftMargin: horizontalMargin
            Layout.rightMargin: horizontalMargin
    //         Layout.topMargin: verticalMargin
    //         Layout.bottomMargin: verticalMargin
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        
        QQC2.Button {
            id: searchButton
            Layout.leftMargin: column.horizontalMargin
            Layout.rightMargin: column.horizontalMargin
            Layout.bottomMargin: Kirigami.Units.largeSpacing
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            Layout.maximumWidth: Kirigami.Units.gridUnit * 20
            padding: Kirigami.Units.largeSpacing
            
            background: Rectangle {
                color: PlasmaCore.Theme.backgroundColor
                radius: searchButton.height / 2
                opacity: 0.8
            }
            
            contentItem: Item {
                implicitHeight: rowLayout.implicitHeight
                implicitWidth: rowLayout.implicitWidth
                
                RowLayout {
                    id: rowLayout
                    anchors.centerIn: parent
                    PlasmaCore.IconItem {
                        source: "search"
                    }
                    PC3.Label {
                        text: i18n("Search")
                    }
                }
            }
            
            onClicked: searchWidget.open();
        }
    }
}
