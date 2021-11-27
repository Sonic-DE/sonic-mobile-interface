/*
 *  SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */


import QtQuick 2.12

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.plasma.private.mobileshell 1.0 as MobileShell

import "indicators" as Indicators

PlasmaComponents.Label {
    id: clock
    property bool is24HourTime: MobileShell.ShellUtil.isSystem24HourFormat
    Layout.fillHeight: true
    
    text: Qt.formatTime(timeSource.data.Local.DateTime, is24HourTime ? "h:mm" : "h:mm ap")
    color: PlasmaCore.ColorScope.textColor
    verticalAlignment: Qt.AlignVCenter
    font.pixelSize: textPixelSize

    TapHandler {
        onTapped: {
            MobileShell.ShellUtil.launchApp("org.kde.kclock");
        }
    }
}
