// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.1

import org.kde.plasma.components 3.0 as PC3
import org.kde.plasma.private.mobileshell.state 1.0 as MobileShellState
import org.kde.private.mobile.homescreen.folio 1.0 as Folio
import org.kde.plasma.private.mobileshell 1.0 as MobileShell
import org.kde.kirigami 2.10 as Kirigami

Item {
    id: root
    height: rowLayout.height

    property var homeScreenState
    property var homeScreen

    readonly property int reservedSpaceForLabel: metrics.height
    readonly property real effectiveContentWidth: width - leftMargin - rightMargin
    readonly property real horizontalMargin: Math.round(width * 0.05)

    readonly property real bottomMargin: 0
    readonly property real leftMargin: horizontalMargin
    readonly property real rightMargin: horizontalMargin

    readonly property real cellWidth: effectiveContentWidth / Math.min(Math.floor(effectiveContentWidth / (Kirigami.Units.iconSizes.large + Kirigami.Units.gridUnit * 2)), 8)
    readonly property real cellHeight: cellWidth + reservedSpaceForLabel

    signal delegateDragRequested(var item)

    PC3.Label {
        id: metrics
        text: "M\nM"
        visible: false
        font.pointSize: Kirigami.Theme.defaultFont.pointSize * 0.8
        font.weight: Font.Bold
    }

    RowLayout {
        id: rowLayout
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root.bottomMargin
        anchors.leftMargin: root.leftMargin
        anchors.rightMargin: root.rightMargin
        anchors.horizontalCenter: parent.horizontalCenter

        Repeater {
            model: Folio.FavouritesModel

            delegate: AppDelegate {
                id: delegate

                name: Folio.FolioSettings.showFavouritesAppLabels ? model.delegate.application.name : ""
                icon: model.delegate.application.icon
                storageId: model.delegate.application.storageId
                applicationRunning: model.delegate.application.running

                shadow: true

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: root.cellWidth
                Layout.preferredHeight: root.cellHeight

                width: root.cellWidth
                height: root.cellHeight
                reservedSpaceForLabel: root.reservedSpaceForLabel

                // don't show when in drag and drop mode
                property var startPosition: root.homeScreenState.dragState.startPosition
                opacity: (root.homeScreenState.swipeState === Folio.HomeScreenState.DraggingDelegate &&
                            startPosition.location === Folio.DelegateDragPosition.Favourites &&
                            startPosition.favouritesPosition === model.index) ? 0 : 1

                // don't show label in drag and drop mode
                labelOpacity: opacity

                onPressAndHold: {
                    let mappedCoords = root.homeScreen.prepareStartDelegateDrag(delegate.delegateItem);
                    root.homeScreenState.startDelegateFavouritesDrag(
                        mappedCoords.x,
                        mappedCoords.y,
                        model.index
                    );

                    contextMenu.open();
                }
                onPressAndHoldReleased: {
                    // cancel the event if the delegate is not dragged
                    if (root.homeScreenState.swipeState === Folio.HomeScreenState.AwaitingDraggingDelegate) {
                        homeScreen.cancelDelegateDrag();
                    }
                }

                onLaunch: (x, y, icon, title, storageId) => {
                    if (icon !== "") {
                        MobileShellState.ShellDBusClient.openAppLaunchAnimation(
                                icon,
                                title,
                                delegate.iconItem.Kirigami.ScenePosition.x + delegate.iconItem.width/2,
                                delegate.iconItem.Kirigami.ScenePosition.y + delegate.iconItem.height/2,
                                Math.min(delegate.iconItem.width, delegate.iconItem.height));
                    }

                    model.delegate.application.setMinimizedDelegate(delegate);
                    MobileShell.AppLaunch.launchOrActivateApp(storageId);
                }

                ContextMenuLoader {
                    id: contextMenu

                    // close menu when drag starts
                    Connections {
                        target: root.homeScreenState

                        function onSwipeStateChanged() {
                            if (root.homeScreenState.swipeState === Folio.HomeScreenState.DraggingDelegate) {
                                contextMenu.close();
                            }
                        }
                    }

                    actions: [
                        Kirigami.Action {
                            icon.name: "emblem-favorite"
                            text: i18n("Remove")
                            onTriggered: root.pageModel.removeDelegate(delegate.row, delegate.column)
                        }
                    ]
                }
            }
        }
    }
}
