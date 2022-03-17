/*
 * Copyright 2022 Devin Lin <devin@kde.org>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami

ColumnLayout {
    id: root
    spacing: 0
    
    property string title: ""
    property string subtitle: ""
    
    ColumnLayout {
        visible: title !== "" || subtitle !== ""
        
        Layout.bottomMargin: Kirigami.Units.largeSpacing
        Layout.topMargin: Kirigami.Units.largeSpacing
        Layout.leftMargin: Kirigami.Units.gridUnit
        Layout.rightMargin: Kirigami.Units.gridUnit
        
        spacing: Kirigami.Units.smallSpacing
        
        Label {
            font.weight: Font.Bold
            text: title
            visible: title !== ""
        }
        
        Label {
            font: Kirigami.Theme.smallFont
            text: subtitle
            visible: subtitle !== ""
        }
    }
    
    Kirigami.Separator { 
        opacity: 0.5
        Layout.fillWidth: true
    }
}
