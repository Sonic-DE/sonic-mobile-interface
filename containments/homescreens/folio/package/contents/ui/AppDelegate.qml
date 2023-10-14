// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import QtQuick.Effects

import org.kde.kirigami 2.20 as Kirigami

import org.kde.private.mobile.homescreen.folio 1.0 as Folio
import org.kde.plasma.private.mobileshell.state 1.0 as MobileShellState
import org.kde.plasma.private.mobileshell 1.0 as MobileShell

AbstractDelegate {
    id: root
    shadow: true
    name: application.name

    property Folio.FolioApplication application

    property alias iconItem: icon

    function launchApp() {
        if (application.icon !== "") {
            MobileShellState.ShellDBusClient.openAppLaunchAnimation(
                    application.icon,
                    application.name,
                    root.iconItem.Kirigami.ScenePosition.x + root.iconItem.width/2,
                    root.iconItem.Kirigami.ScenePosition.y + root.iconItem.height/2,
                    Math.min(root.iconItem.width, root.iconItem.height));
        }

        application.setMinimizedDelegate(root);
        MobileShell.AppLaunch.launchOrActivateApp(application.storageId);
    }

    onAfterClickAnimation: {
        launchApp();
    }

    contentItem: DelegateAppIcon {
        id: icon
        application: root.application

        Rectangle {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: Kirigami.Units.smallSpacing
            }
            visible: root.application.running
            radius: width
            width: Kirigami.Units.smallSpacing
            height: width
            color: Kirigami.Theme.highlightColor
        }
    }
}


