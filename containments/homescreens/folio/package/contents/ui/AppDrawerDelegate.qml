// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import QtQuick.Effects

import org.kde.kirigami 2.20 as Kirigami

import org.kde.private.mobile.homescreen.folio 1.0 as Folio
import org.kde.plasma.private.mobileshell.shellsettingsplugin as ShellSettings
import org.kde.plasma.private.mobileshell 1.0 as MobileShell

AbstractDelegate {
    id: delegate

    property string icon
    property string storageId
    property bool applicationRunning

    property alias iconItem: icon

    signal launch(int x, int y, var source, string title, string storageId)

    function launchApp() {
        // launch app
        if (applicationRunning) {
            delegate.launch(0, 0, "", delegate.name, delegate.storageId);
        } else {
            delegate.launch(delegate.x + (Kirigami.Units.smallSpacing * 2), delegate.y + (Kirigami.Units.smallSpacing * 2), icon.source, delegate.name, delegate.storageId);
        }
    }

    onAfterClickAnimation: {
        launchApp();
    }

    contentItem: Kirigami.Icon {
        id: icon
        source: delegate.icon

        Rectangle {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
            }
            visible: delegate.applicationRunning
            radius: width
            width: Kirigami.Units.smallSpacing
            height: width
            color: Kirigami.Theme.highlightColor
        }
    }
}



