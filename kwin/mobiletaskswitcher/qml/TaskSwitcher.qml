/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <notmart@gmail.com>
 *   SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *   SPDX-FileCopyrightText: 2022 ivan tkachenko <me@ratijas.tk>
 *   SPDX-FileCopyrightText: 2021-2022 Devin Lin <devin@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.15

import org.kde.taskmanager 0.1 as TaskManager
import org.kde.plasma.core 2.1 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.plasma.private.nanoshell 2.0 as NanoShell
import org.kde.plasma.private.mobileshell 1.0 as MobileShell
import org.kde.kwin 3.0 as KWinComponents
import org.kde.kwin.private.effects 1.0

import "../components" as Components

/**
 * Component that provides a task switcher.
 */
FocusScope {
    id: root
    focus: true
    
    required property QtObject effect
    required property QtObject targetScreen
    
    function start() {
        
    }
    
    function stop() {
        
    }
    
    Keys.onEscapePressed: {
        // TODO close effects
    }
    
    KWinComponents.DesktopBackgroundItem {
        id: backgroundItem
        activity: KWinComponents.Workspace.currentActivity
        desktop: KWinComponents.Workspace.currentVirtualDesktop
        outputName: targetScreen.name
        property real blurRadius: 50
        
        layer.enabled: effect.blurBackground
        layer.effect: FastBlur {
            radius: backgroundItem.blurRadius
        }
    }

//     FlickContainer {
//         id: flickable
//         anchors.fill: parent
//     }
}
