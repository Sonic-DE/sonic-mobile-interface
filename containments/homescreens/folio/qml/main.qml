// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Effects

import org.kde.kirigami as Kirigami

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 3.0 as PlasmaComponents

import org.kde.plasma.private.mobileshell as MobileShell
import org.kde.plasma.private.mobileshell.state as MobileShellState
import org.kde.plasma.private.mobileshell.windowplugin as WindowPlugin
import org.kde.plasma.private.mobileshell.shellsettingsplugin as ShellSettings

import plasma.applet.org.kde.plasma.mobile.homescreen.folio as Folio

import "./private"

pragma ComponentBehavior: Bound

ContainmentItem {
    id: root
    property Folio.HomeScreen folio: root.plasmoid // qmllint disable incompatible-type

    Component.onCompleted: {
        root.folio.FolioSettings.load();
        root.folio.FavouritesModel.load();
        root.folio.PageListModel.load();
    }

    property MobileShell.MaskManager maskManager: MobileShell.MaskManager {
        height: root.height
        width: root.width
    }

    property MobileShell.MaskManager frontMaskManager: MobileShell.MaskManager {
        height: root.height
        width: root.width
    }

    // wallpaper blur layer
    MobileShell.BlurEffect {
        id: wallpaperBlur
        active: root.folio.FolioSettings.wallpaperBlurEffect > 0
        anchors.fill: parent
        sourceLayer: Plasmoid.wallpaperGraphicsObject // qmllint disable missing-property
        maskSourceLayer: root.folio.FolioSettings.wallpaperBlurEffect > 1 ? root.maskManager.maskLayer : null

        fullBlur: Math.min(1,
                           Math.max(
                               1 - homeScreen.contentOpacity,
                               root.folio.HomeScreenState.appDrawerOpenProgress * 2, // blur faster during swipe
                               root.folio.HomeScreenState.searchWidgetOpenProgress * 1.5, // blur faster during swipe
                               root.folio.HomeScreenState.folderOpenProgress
                           )
        )
    }

    WindowPlugin.WindowMaximizedTracker {
        id: windowMaximizedTracker
        screenGeometry: Plasmoid.containment.screenGeometry
    }

    function homeAction() {
        const isInWindow = (!WindowPlugin.WindowUtil.isShowingDesktop && windowMaximizedTracker.showingWindow);

        // Always close action drawer
        if (MobileShellState.ShellDBusClient.isActionDrawerOpen) {
            MobileShellState.ShellDBusClient.closeActionDrawer();
        }

        if (isInWindow) {
            // Only minimize windows and go to homescreen when not in docked mode
            if (!ShellSettings.Settings.convergenceModeEnabled) {
                root.folio.HomeScreenState.closeFolder();
                root.folio.HomeScreenState.closeSearchWidget();
                root.folio.HomeScreenState.closeAppDrawer();
                root.folio.HomeScreenState.goToPage(0, false);

                WindowPlugin.WindowUtil.minimizeAll();
            }

            // Always ensure settings view is closed
            if (root.folio.HomeScreenState.viewState == Folio.HomeScreenState.SettingsView) {
                root.folio.HomeScreenState.closeSettingsView();
            }

        } else { // If we are already on the homescreen
            switch (root.folio.HomeScreenState.viewState) {
                case Folio.HomeScreenState.PageView:
                    if (root.folio.HomeScreenState.currentPage === 0) {
                        root.folio.HomeScreenState.openAppDrawer();
                    } else {
                        root.folio.HomeScreenState.goToPage(0, false);
                    }
                    break;
                case Folio.HomeScreenState.AppDrawerView:
                    root.folio.HomeScreenState.closeAppDrawer();
                    break;
                case Folio.HomeScreenState.SearchWidgetView:
                    root.folio.HomeScreenState.closeSearchWidget();
                    break;
                case Folio.HomeScreenState.FolderView:
                    root.folio.HomeScreenState.closeFolder();
                    break;
                case Folio.HomeScreenState.SettingsView:
                    root.folio.HomeScreenState.closeSettingsView();
                    break;
            }
        }
    }

    Plasmoid.onActivated: homeAction()

    Rectangle {
        id: appDrawerBackground
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.6)

        opacity: root.folio.HomeScreenState.appDrawerOpenProgress
    }

    Rectangle {
        id: searchWidgetBackground
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.3)

        opacity: root.folio.HomeScreenState.searchWidgetOpenProgress
    }

    Rectangle {
        id: settingsViewBackground
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.3)

        opacity: root.folio.HomeScreenState.settingsOpenProgress
    }

    MobileShell.HomeScreen {
        id: homeScreen
        anchors.fill: parent

        plasmoidItem: root
        onResetHomeScreenPosition: {
            // NOTE: empty, because this is handled by homeAction()
        }

        onHomeTriggered: root.homeAction()

        contentItem: Item {

            // homescreen component
            FolioHomeScreen {
                id: folioHomeScreen
                folio: root.folio
                maskManager: root.maskManager
                anchors.fill: parent

                topMargin: homeScreen.topMargin
                bottomMargin: homeScreen.bottomMargin
                leftMargin: homeScreen.leftMargin
                rightMargin: homeScreen.rightMargin

                // Ensure is the focused item at start
                Component.onCompleted: forceActiveFocus()

                onWallpaperSelectorTriggered: wallpaperSelectorLoader.active = true
            }
        }
    }

    // top blur layer for items on top of the base homescreen
    MobileShell.BlurEffect {
        id: homescreenBlur
        anchors.fill: parent
        active: root.folio.FolioSettings.wallpaperBlurEffect > 1 && ((delegateDragItem.visible && root.folio.HomeScreenState.dragState.dropDelegate.type === Folio.FolioDelegate.Folder) || wallpaperSelectorLoader.active)
        visible: active
        fullBlur: 0

        sourceLayer: homeScreenLayer
        maskSourceLayer: root.frontMaskManager.maskLayer

        // stacking both wallpaper and homescreen layers so we can blur them in one pass
        Item {
            id: homeScreenLayer
            anchors.fill: parent
            opacity: 0

            // wallpaper blur
            ShaderEffectSource {
                anchors.fill: parent

                textureSize: homescreenBlur.textureSize
                sourceItem: Plasmoid.wallpaperGraphicsObject // qmllint disable missing-property
                hideSource: false
            }

            // homescreen blur
            ShaderEffectSource {
                anchors.fill: parent

                textureSize: homescreenBlur.textureSize
                sourceItem: homeScreen
                hideSource: false
            }
        }
    }

    // drag and drop component
    DelegateDragItem {
        id: delegateDragItem
        folio: root.folio
        maskManager: root.frontMaskManager
    }

    // drag and drop for widgets
    WidgetDragItem {
        id: widgetDragItem
        folio: root.folio
    }

    // loader for wallpaper selector
    Loader {
        id: wallpaperSelectorLoader
        anchors.fill: parent
        asynchronous: true
        active: false

        onLoaded: {
            wallpaperSelectorLoader.item.open(); // qmllint disable missing-property
        }

        sourceComponent: MobileShell.WallpaperSelector {
            maskManager: root.frontMaskManager
            horizontal: root.width > root.height
            edge: horizontal ? Qt.LeftEdge : Qt.BottomEdge
            bottomMargin: horizontal ? 0 : folioHomeScreen.bottomMargin
            leftMargin: horizontal ? folioHomeScreen.leftMargin : 0
            rightMargin: horizontal ? folioHomeScreen.rightMargin : 0
            onClosed: {
                wallpaperSelectorLoader.active = false;
            }

            onWallpaperSettingsRequested: {
                close();
                folioHomeScreen.openConfigure();
            }
        }
    }
}

