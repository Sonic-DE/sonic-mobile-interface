// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts

import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.kirigami 2.20 as Kirigami

import org.kde.plasma.private.mobileshell 1.0 as MobileShell
import org.kde.private.mobile.homescreen.folio 1.0 as Folio

Item {
    id: root

    required property real topMargin
    required property real bottomMargin
    required property real leftMargin
    required property real rightMargin

    property bool interactive: true

    property Folio.HomeScreenState homeScreenState: Folio.HomeScreenState

    // called by any delegates when starting drag
    // returns the mapped coordinates to be used in the home screen state
    function prepareStartDelegateDrag(delegate, item) {
        swipeArea.setSkipSwipeThreshold(true);

        let mapped = root.mapFromItem(item, 0, 0);
        delegateDragItem.delegate = delegate;

        mapped.x -= root.leftMargin;
        mapped.y -= root.topMargin;
        // mapped.x += Folio.FolioSettings.homeScreenIconSize / 2;
        // mapped.y += Folio.FolioSettings.homeScreenIconSize / 2;
        return mapped;
    }

    function cancelDelegateDrag() {
        homeScreenState.cancelDelegateDrag();
    }

    Item {
        id: screenDimensions
        anchors.fill: parent

        onWidthChanged: {
            homeScreenState.viewWidth = width;
        }
        onHeightChanged: {
            homeScreenState.viewHeight = height;
        }
    }

    MobileShell.SwipeArea {
        id: swipeArea
        anchors.fill: parent

        interactive: root.interactive && !(appDrawer.flickable.moving && appDrawer.flickable.contentY > 0)

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
            opacity: 1 - Math.max(homeScreenState.appDrawerOpenProgress, homeScreenState.searchWidgetOpenProgress, homeScreenState.folderOpenProgress) * 2
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
                homeScreen: root

                anchors.bottom: parent.bottom
                anchors.bottomMargin: root.bottomMargin
                anchors.left: parent.left
                anchors.leftMargin: root.leftMargin
                anchors.right: parent.right
                anchors.rightMargin: root.rightMargin
            }
        }

        // folder view
        FolderView {
            id: folderView
            anchors.fill: parent
            anchors.topMargin: root.topMargin
            anchors.leftMargin: root.leftMargin
            anchors.rightMargin: root.rightMargin
            anchors.bottomMargin: root.bottomMargin

            homeScreen: root
            opacity: homeScreenState.folderOpenProgress
            transform: Translate { y: folderView.opacity > 0 ? 0 : folderView.height }
        }

        // drag and drop component
        DelegateDragItem {
            id: delegateDragItem
            visible: homeScreenState.swipeState === Folio.HomeScreenState.DraggingDelegate

            x: Math.round(homeScreenState.delegateDragX)
            y: Math.round(homeScreenState.delegateDragY)
        }

        // bottom app drawer
        AppDrawer {
            id: appDrawer
            width: parent.width
            height: parent.height

            homeScreen: root

            // we only start showing it halfway through
            opacity: homeScreenState.appDrawerOpenProgress < 0.5 ? 0 : (homeScreenState.appDrawerOpenProgress - 0.5) * 2

            // position for animation
            property real animationY: (1 - homeScreenState.appDrawerOpenProgress) * (Kirigami.Units.gridUnit * 2)

            // move the app drawer out of the way if it is not visible
            // HACK: we do this instead of setting visible to false, because
            //       it doesn't mess with app drag and drop from the app drawer
            y: (opacity > 0) ? animationY : parent.height

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
