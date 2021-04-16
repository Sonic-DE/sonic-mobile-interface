/*
 *  SPDX-FileCopyrightText: 2019 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.draganddrop 2.0 as DragDrop

import org.kde.plasma.private.mobilehomescreencomponents 0.1 as HomeScreenComponents
import org.kde.plasma.private.containmentlayoutmanager 1.0 as ContainmentLayoutManager 

import org.kde.phone.homescreen 1.0

import org.kde.plasma.private.mobileshell 1.0 as MobileShell

FocusScope {
    id: root
    width: 640
    height: 480

    property Item toolBox

//BEGIN functions

    function recalculateMaxFavoriteCount() {
        if (!componentComplete) {
            return;
        }

        plasmoid.nativeInterface.applicationListModel.maxFavoriteCount = Math.max(4, Math.floor(Math.min(width, height) / homeScreenContents.appletsLayout.cellWidth));
    }

//END functions


    property bool componentComplete: false
    onWidthChanged: recalculateMaxFavoriteCount()
    onHeightChanged:recalculateMaxFavoriteCount()
    Component.onCompleted: {
        if (plasmoid.screen == 0) {
            MobileShell.HomeScreenControls.homeScreen = root
            MobileShell.HomeScreenControls.homeScreenWindow = root.Window.window
        }
        componentComplete = true;
        recalculateMaxFavoriteCount()
    }

    Plasmoid.onScreenChanged: {
        if (plasmoid.screen == 0) {
            MobileShell.HomeScreenControls.homeScreen = root
            MobileShell.HomeScreenControls.homeScreenWindow = root.Window.window
        }
    }
    Window.onWindowChanged: {
        if (plasmoid.screen == 0) {
            MobileShell.HomeScreenControls.homeScreenWindow = root.Window.window
        }
    }

    Connections {
        property real lastRequestedPosition: 0
        target: MobileShell.HomeScreenControls
        function onResetHomeScreenPosition() {
            mainFlickable.scrollToPage(0);
            appDrawer.close();
        }
        function onSnapHomeScreenPosition() {
            if (lastRequestedPosition < 0) {
                appDrawer.open();
            } else {
                appDrawer.close();
            }
        }
        function onRequestRelativeScroll(pos) {
            appDrawer.offset -= pos.y;
            lastRequestedPosition = pos.y;
        }
    }

    HomeScreenComponents.FlickablePages {
        id: mainFlickable

        anchors {
            fill: parent
            topMargin: plasmoid.availableScreenRect.y

            bottomMargin: favoriteStrip.height + plasmoid.screenGeometry.height - plasmoid.availableScreenRect.height - plasmoid.availableScreenRect.y
        }

        opacity: 1 - appDrawer.openFactor
        transform: Translate {
            y: -mainFlickable.height/10 * appDrawer.openFactor
        }
        scale: (3 - appDrawer.openFactor) /3

        //bottomMargin: favoriteStrip.height
        contentWidth: appletsLayout.width
        contentHeight: height
        //interactive: !plasmoid.editMode && !launcherDragManager.active
        interactive: false

        signal cancelEditModeForItemsRequested
        onDragStarted: cancelEditModeForItemsRequested()
        onDragEnded: cancelEditModeForItemsRequested()
        onFlickStarted: cancelEditModeForItemsRequested()
        onFlickEnded: cancelEditModeForItemsRequested()

        onContentYChanged: MobileShell.HomeScreenControls.homeScreenPosition = contentY

        LauncherPrivate.DragGestureHandler {
            appDrawer: appDrawer
            mainFlickable: mainFlickable
            enabled: root.focus && appDrawer.status !== Launcher.AppDrawer.Status.Open && !appletsLayout.editMode && !plasmoid.editMode && !launcherDragManager.active
        }
/*
        DragHandler {
            target: mainFlickable
            yAxis.enabled: !appletsLayout.editMode && !plasmoid.editMode && !launcherDragManager.active
            xAxis.enabled: yAxis.enabled
            enabled: root.focus && appDrawer.status !== Launcher.AppDrawer.Status.Open
            property real initialMainFlickableX
            enum ScrollDirection {
                None,
                Horizontal,
                Vertical
            }
            property int scrollDirection: None
            onTranslationChanged: {print(translation.x)
                if (active) {
                    if (appDrawer.offset > PlasmaCore.Units.gridUnit) {
                        scrollDirection = Vertical;
                    } else if (Math.abs(mainFlickable.contentX - initialMainFlickableX) > PlasmaCore.Units.gridUnit) {
                        scrollDirection = Horizontal;
                    }
                    if (scrollDirection !== Horizontal) {
                        appDrawer.offset = -translation.y;
                    }
                    if (scrollDirection !== Vertical) {
                        mainFlickable.contentX = Math.max(0, initialMainFlickableX - translation.x);
                    }
                }
            }
            onActiveChanged: {
                if (active) {
                    initialMainFlickableX = mainFlickable.contentX;
                } else {
                    appDrawer.snapDrawerStatus();
                }
            }
        }
*/
        NumberAnimation {
            id: scrollAnim
            target: mainFlickable
            properties: "contentX"
            duration: units.longDuration
            easing.type: Easing.InOutQuad
        }

        //TODO: favorite strip disappearing with everything else
        footer: favoriteStrip
        appletsLayout: homeScreenContents.appletsLayout


        appDrawer: appDrawer
        contentWidth: Math.max(width, width * Math.ceil(homeScreenContents.itemsBoundingRect.width/width)) + (homeScreenContents.launcherDragManager.active ? width : 0)
        showAddPageIndicator: homeScreenContents.launcherDragManager.active

        dragGestureEnabled: root.focus && appDrawer.status !== HomeScreenComponents.AppDrawer.Status.Open && !appletsLayout.editMode && !plasmoid.editMode && !homeScreenContents.launcherDragManager.active

        HomeScreenComponents.HomeScreenContents {
            id: homeScreenContents
            width: mainFlickable.width * 100
            favoriteStrip: favoriteStrip
        }
    }

    HomeScreenComponents.AppDrawer {
        id: appDrawer
        anchors.fill: parent

        topPadding: plasmoid.availableScreenRect.y
        bottomPadding: plasmoid.screenGeometry.height - plasmoid.availableScreenRect.height - plasmoid.availableScreenRect.y
        closedPositionOffset: favoriteStrip.height
    }

    HomeScreenComponents.FavoriteStrip {
        id: favoriteStrip

        appletsLayout: homeScreenContents.appletsLayout

        visible: flow.children.length > 0 || homeScreenContents.launcherDragManager.active || homeScreenContents.containsDrag

        opacity: homeScreenContents.launcherDragManager.active && plasmoid.nativeInterface.applicationListModel.favoriteCount >= plasmoid.nativeInterface.applicationListModel.maxFavoriteCount ? 0.3 : 1

        TapHandler {
            target: favoriteStrip
            onTapped: {
                //Hides icons close button
                homeScreenContents.appletsLayout.appletsLayoutInteracted();
                homeScreenContents.appletsLayout.editMode = false;
            }
            onLongPressed: homeScreenContents.appletsLayout.editMode = true;
            onPressedChanged: root.focus = true;
        }
    }
}

