/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <notmart@gmail.com>
 *   SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import QtQuick.Layouts 1.1

import org.kde.kirigami 2.12 as Kirigami

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.private.nanoshell 2.0 as NanoShell
import org.kde.plasma.private.mobileshell 1.0 as MobileShell
import org.kde.plasma.components 3.0 as PlasmaComponents

import "../../components" as Components

Components.BaseItem {
    id: root
    
    // Model interface
    required property string text
    required property string icon
    required property bool enabled
    required property string settingsCommand
    required property var toggleFunction
    
    // set by children
    property var iconItem
    
    function delegateClick() {
        if (root.toggle) {
            root.toggle();
        } else if (root.toggleFunction) {
            root.toggleFunction();
        } else if (root.settingsCommand) {
            NanoShell.StartupFeedback.open(
                root.icon,
                root.text,
                iconItem.Kirigami.ScenePosition.x + iconItem.width/2,
                iconItem.Kirigami.ScenePosition.y + iconItem.height/2,
                Math.min(iconItem.width, iconItem.height))
            MobileShell.ShellUtil.executeCommand(root.settingsCommand);
            root.closeRequested();
        }
    }
    
    function delegatePressAndHold() {
        if (root.settingsCommand) {
            NanoShell.StartupFeedback.open(
                root.icon,
                root.text,
                iconItem.Kirigami.ScenePosition.x + iconItem.width/2,
                iconItem.Kirigami.ScenePosition.y + iconItem.height/2,
                Math.min(iconItem.width, iconItem.height))
            closeRequested();
            MobileShell.ShellUtil.executeCommand(root.settingsCommand);
        } else if (root.toggleFunction) {
            root.toggleFunction();
        }
    }
}
