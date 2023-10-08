// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.1

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.draganddrop 2.0 as DragDrop
import org.kde.kirigami 2.20 as Kirigami

import org.kde.plasma.private.containmentlayoutmanager 1.0 as ContainmentLayoutManager
import org.kde.plasma.private.mobileshell 1.0 as MobileShell

import org.kde.private.mobile.homescreen.folio 1.0 as Folio

Item {
    id: root

    required property real topMargin
    required property real bottomMargin
    required property real leftMargin
    required property real rightMargin

    property bool interactive: true

    required property Folio.HomeScreenState homeScreenState

    // called by any delegates when starting drag
    // returns the mapped coordinates to be used in the home screen state
    function prepareStartDelegateDrag(item) {
        let mapped = root.mapFromItem(item, 0, 0);
        delegateDragItem.height = item.height;
        delegateDragItem.width = item.width;
        delegateDragItem.source = item;

        mapped.x -= root.leftMargin;
        mapped.y -= root.topMargin;
        return mapped;
    }

    Connections {
        target: homeScreenState
        function onDelegateDragEnded() {
            delegateDragItem.source = null;
        }
    }

    MobileShell.SwipeArea {
        id: swipeArea
        anchors.fill: parent

        interactive: root.interactive

        onSwipeStarted: {
            homeScreenState.swipeStarted();
        }
        onSwipeEnded: {
            homeScreenState.swipeEnded();
        }
        onSwipeMove: (totalDeltaX, totalDeltaY, deltaX, deltaY) => {
            homeScreenState.swipeMoved(totalDeltaX, totalDeltaY, deltaX, deltaY);
        }

        Item {
            id: mainHomeScreen
            anchors.fill: parent

            // we stop showing halfway through the animation
            opacity: 1 - Math.max(homeScreenState.appDrawerOpenProgress, homeScreenState.searchWidgetOpenProgress) * 2
            visible: opacity > 0 // prevent handlers from picking up events

            transform: [
                Scale {
                    origin.x: mainHomeScreen.width / 2
                    origin.y: mainHomeScreen.height / 2
                    yScale: 1 - (homeScreenState.appDrawerOpenProgress * 2) * 0.1
                    xScale: 1 - (homeScreenState.appDrawerOpenProgress * 2) * 0.1
                }
            ]

            HomeScreenPages {
                id: homeScreenPages
                homeScreenState: root.homeScreenState
                homeScreen: root

                anchors.bottom: favouritesBar.top
                anchors.top: parent.top
                anchors.topMargin: root.topMargin
                anchors.left: parent.left
                anchors.leftMargin: root.leftMargin
                anchors.right: parent.right
                anchors.rightMargin: root.rightMargin

                // update the model with page dimensions
                onWidthChanged: {
                    homeScreenState.pageWidth = homeScreenPages.width;
                    homeScreenState.pageContentWidth = homeScreenPages.pageContentWidth;
                }
                onHeightChanged: {
                    homeScreenState.pageHeight = homeScreenPages.height;
                    homeScreenState.pageContentHeight = homeScreenPages.pageContentHeight;
                }
            }

            FavouritesBar {
                id: favouritesBar
                homeScreenState: root.homeScreenState
                homeScreen: root

                anchors.bottom: parent.bottom
                anchors.bottomMargin: root.bottomMargin
                anchors.left: parent.left
                anchors.leftMargin: root.leftMargin
                anchors.right: parent.right
                anchors.rightMargin: root.rightMargin
            }
        }

        // drag and drop component
        DelegateDragItem {
            id: delegateDragItem
            visible: homeScreenState.swipeState === Folio.HomeScreenState.DraggingDelegate

            x: Math.round(homeScreenState.delegateDragX)
            y: Math.round(homeScreenState.delegateDragY)
        }

        AppDrawer {
            id: appDrawer
            anchors.fill: parent

            homeScreenState: root.homeScreenState
            homeScreen: root

            // we only start showing it halfway through
            opacity: homeScreenState.appDrawerOpenProgress < 0.5 ? 0 : (homeScreenState.appDrawerOpenProgress - 0.5) * 2

            // prevent handlers from picking up events
            visible: opacity > 0

            transform: Translate { y: (1 - homeScreenState.appDrawerOpenProgress) * (Kirigami.Units.gridUnit * 2) }

            headerHeight: Math.round(Kirigami.Units.gridUnit * 5)
            headerItem: AppDrawerHeader {}

            // account for panels
            topPadding: root.topMargin
            bottomPadding: root.bottomMargin
            leftPadding: root.leftMargin
            rightPadding: root.rightMargin
        }

        // search component
        MobileShell.KRunnerScreen {
            id: searchWidget
            anchors.fill: parent

            opacity: homeScreenState.searchWidgetOpenProgress
            visible: opacity > 0
            transform: Translate { y: (1 - homeScreenState.searchWidgetOpenProgress) * (-Kirigami.Units.gridUnit * 2) }

            onRequestedClose: {
                homeScreenState.closeSearchWidget();
            }

            anchors.topMargin: root.topMargin
            anchors.bottomMargin: root.bottomMargin
            anchors.leftMargin: root.leftMargin
            anchors.rightMargin: root.rightMargin
        }
    }
}
