// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.plasma.components 3.0 as PlasmaComponents
import org.kde.kirigami 2.20 as Kirigami

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.components 3.0 as PC3
import org.kde.plasma.private.mobileshell 1.0 as MobileShell
import org.kde.private.mobile.homescreen.folio 1.0 as Folio

import "./delegate"

Item {
    id: root

    property real topMargin: 0
    property real bottomMargin: 0
    property real leftMargin: 0
    property real rightMargin: 0

    property bool interactive: true

    property Folio.HomeScreenState homeScreenState: Folio.HomeScreenState

    readonly property bool dropAnimationRunning: delegateDragItem.dropAnimationRunning

    onTopMarginChanged: Folio.HomeScreenState.viewTopPadding = topMargin
    onBottomMarginChanged: Folio.HomeScreenState.viewBottomPadding = bottomMargin
    onLeftMarginChanged: Folio.HomeScreenState.viewLeftPadding = leftMargin
    onRightMarginChanged: Folio.HomeScreenState.viewRightPadding = rightMargin

    // called by any delegates when starting drag
    // returns the mapped coordinates to be used in the home screen state
    function prepareStartDelegateDrag(delegate, item) {
        swipeArea.setSkipSwipeThreshold(true);

        delegateDragItem.delegate = delegate;
        return root.mapFromItem(item, 0, 0);
    }

    function cancelDelegateDrag() {
        homeScreenState.cancelDelegateDrag();
    }

    function openConfigure() {
        Plasmoid.internalAction("configure").trigger();
    }

    // determine how tall an app label is, for delegate measurements
    DelegateLabel {
        id: appLabelMetrics
        text: "M\nM"
        visible: false

        onHeightChanged: Folio.HomeScreenState.pageDelegateLabelHeight = appLabelMetrics.height

        Component.onCompleted: {
            Folio.HomeScreenState.pageDelegateLabelWidth = Kirigami.Units.smallSpacing;
        }
    }

    Item {
        id: screenDimensions
        anchors.fill: parent

        onWidthChanged: Folio.HomeScreenState.viewWidth = width;
        onHeightChanged: Folio.HomeScreenState.viewHeight = height;
    }

    MobileShell.SwipeArea {
        id: swipeArea
        anchors.fill: parent

        interactive: root.interactive && !appDrawer.flickable.moving && appDrawer.flickable.contentY === 0

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

                anchors.topMargin: root.topMargin
                anchors.leftMargin: root.leftMargin
                anchors.rightMargin: root.rightMargin
                anchors.bottomMargin: root.bottomMargin

                // update the model with page dimensions
                onWidthChanged: {
                    homeScreenState.pageWidth = homeScreenPages.width;
                }
                onHeightChanged: {
                    homeScreenState.pageHeight = homeScreenPages.height;
                }

                states: [
                    State {
                        name: "bottom"
                        when: Folio.HomeScreenState.favouritesBarLocation === Folio.HomeScreenState.Bottom
                        AnchorChanges {
                            target: homeScreenPages
                            anchors.top: parent.top
                            anchors.bottom: favouritesBar.top
                            anchors.left: parent.left
                            anchors.right: parent.right
                        }
                    }, State {
                        name: "left"
                        when: Folio.HomeScreenState.favouritesBarLocation === Folio.HomeScreenState.Left
                        AnchorChanges {
                            target: homeScreenPages
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.left: favouritesBar.right
                            anchors.right: parent.right
                        }
                    }, State {
                        name: "right"
                        when: Folio.HomeScreenState.favouritesBarLocation === Folio.HomeScreenState.Right
                        AnchorChanges {
                            target: homeScreenPages
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: favouritesBar.left
                        }
                    }
                ]
            }

            FavouritesBar {
                id: favouritesBar
                homeScreen: root
                leftMargin: root.leftMargin
                topMargin: root.topMargin

                anchors.topMargin: root.topMargin
                anchors.bottomMargin: root.bottomMargin
                anchors.leftMargin: root.leftMargin
                anchors.rightMargin: root.rightMargin

                states: [
                    State {
                        name: "bottom"
                        when: Folio.HomeScreenState.favouritesBarLocation === Folio.HomeScreenState.Bottom
                        AnchorChanges {
                            target: favouritesBar
                            anchors.top: undefined
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right
                        }
                        PropertyChanges {
                            target: favouritesBar
                            height: Kirigami.Units.gridUnit * 6
                        }
                    }, State {
                        name: "left"
                        when: Folio.HomeScreenState.favouritesBarLocation === Folio.HomeScreenState.Left
                        AnchorChanges {
                            target: favouritesBar
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: undefined
                        }
                        PropertyChanges {
                            target: favouritesBar
                            width: Kirigami.Units.gridUnit * 6
                        }
                    }, State {
                        name: "right"
                        when: Folio.HomeScreenState.favouritesBarLocation === Folio.HomeScreenState.Right
                        AnchorChanges {
                            target: favouritesBar
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.left: undefined
                            anchors.right: parent.right
                        }
                        PropertyChanges {
                            target: favouritesBar
                            width: Kirigami.Units.gridUnit * 6
                        }
                    }
                ]
            }

            QQC2.PageIndicator {
                Kirigami.Theme.inherit: false
                Kirigami.Theme.colorSet: Kirigami.Theme.Complementary

                property bool favouritesBarAtBottom: Folio.HomeScreenState.favouritesBarLocation === Folio.HomeScreenState.Bottom

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: favouritesBarAtBottom ? favouritesBar.top : parent.bottom
                anchors.bottomMargin: favouritesBarAtBottom ? 0 : (root.bottomMargin + Kirigami.Units.largeSpacing)

                currentIndex: Folio.HomeScreenState.currentPage
                count: Folio.PageListModel.length
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

            // focus the search bar if it opens
            Connections {
                target: Folio.HomeScreenState

                function onSearchWidgetOpenProgressChanged() {
                    if (homeScreenState.searchWidgetOpenProgress === 1.0) {
                        searchWidget.requestFocus();
                    } else {
                        // TODO this gets called a lot, can we have a more performant way?
                        root.forceActiveFocus();
                    }
                }
            }

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
