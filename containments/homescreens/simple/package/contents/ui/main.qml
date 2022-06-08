// SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.1

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents

import org.kde.plasma.private.mobileshell 1.0 as MobileShell

MobileShell.HomeScreen {
    id: root

    onResetHomeScreenPosition: {
        homescreen.triggerHomescreen();
    }
    
    onHomeTriggered: {
        search.close();
    }
    
    property bool componentComplete: false
    
    Component.onCompleted: {
        MobileShell.ApplicationListModel.loadApplications();
        forceActiveFocus();
    }
    
    Plasmoid.onActivated: {
        console.log("Triggered!", plasmoid.nativeInterface.showingDesktop)
        
        // there's a couple of steps:
        // - minimize windows
        // - open app drawer
        // - restore windows
        if (!plasmoid.nativeInterface.showingDesktop) {
            plasmoid.nativeInterface.showingDesktop = true;
        } else if (homescreen.homeScreenState.currentView === MobileShell.HomeScreenState.PageView) {
            homescreen.homeScreenState.openAppDrawer()
        } else {
            plasmoid.nativeInterface.showingDesktop = false
            homescreen.homeScreenState.closeAppDrawer()
        }
    }
    
    // homescreen component
    contentItem: Item {
        HomeScreen {
            id: homescreen
            anchors.fill: parent
            
            // make the homescreen not interactable when task switcher or startup feedback is on
            //interactive: !root.overlayShown
            searchWidget: search
        }
        
        // search component
        MobileShell.KRunnerWidget {
            id: search
            anchors.fill: parent
            visible: openFactor > 0
        }
    }
}


